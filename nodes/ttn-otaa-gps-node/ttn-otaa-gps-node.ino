/*******************************************************************************
 * Copyright (c) 2015 Thomas Telkamp and Matthijs Kooijman
 *
 * Permission is hereby granted, free of charge, to anyone
 * obtaining a copy of this document and accompanying files,
 * to do whatever they want with them without any restriction,
 * including, but not limited to, copying, modification and redistribution.
 * NO WARRANTY OF ANY KIND IS PROVIDED.
 *
 * This example sends a valid LoRaWAN packet with payload "Hello,
 * world!", using frequency and encryption settings matching those of
 * the The Things Network.
 *
 * This uses OTAA (Over-the-air activation), where where a DevEUI and
 * application key is configured, which are used in an over-the-air
 * activation procedure where a DevAddr and session keys are
 * assigned/generated for use with all further communication.
 *
 * Note: LoRaWAN per sub-band duty-cycle limitation is enforced (1% in
 * g1, 0.1% in g2), but not the TTN fair usage policy (which is probably
 * violated by this sketch when left running for longer)!

 * To use this sketch, first register your application and device with
 * the things network, to set or generate an AppEUI, DevEUI and AppKey.
 * Multiple devices can use the same AppEUI, but each device has its own
 * DevEUI and AppKey.
 *
 * Do not forget to define the radio type correctly in config.h.
 *
 *******************************************************************************/
/********************************************************************************************/
/*                                       INCLUDES                                           */
/********************************************************************************************/
#include <lmic.h>
#include <hal/hal.h>
#include "GPS.h"
#include "LoRa_DataManager.h"
#include <SPI.h>

/********************************************************************************************/
/*                                       DEFINES                                            */
/********************************************************************************************/
#define VBATPIN A7
#define TX_LED 13



#if defined(ARDUINO_SAMD_ZERO) && defined(SERIAL_PORT_USBVIRTUAL)
  // Required for Serial on Zero based boards
  #define Serial SERIAL_PORT_USBVIRTUAL
#endif

/********************************************************************************************/
/*                                 GLOBALS AND CONSTANTS                                    */
/********************************************************************************************/

const float bat_max_v = 4.2;

/********************************************************************************************/
/*                                       TYPEDEFS                                           */
/********************************************************************************************/


/********************************************************************************************/
/*                                 VARIABLE DECLARATION                                     */
/********************************************************************************************/
// This EUI must be in little-endian format, so least-significant-byte
// first. When copying an EUI from ttnctl output, this means to reverse
// the bytes. For TTN issued EUIs the last bytes should be 0xD5, 0xB3,
// 0x70.
//static const u1_t PROGMEM APPEUI[8]={0x0a, 0x73, 0xf2, 0x5a, 0xb7, 0x30, 0xe5, 0x3a };
static const u1_t PROGMEM APPEUI[8]={0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
void os_getArtEui (u1_t* buf) { memcpy_P(buf, APPEUI, 8);}

// This should also be in little endian format, see above.
static const u1_t PROGMEM DEVEUI[8]={0x0a, 0x73, 0xf2, 0x5a, 0xb7, 0x30, 0xe5, 0x3a };
//static const u1_t PROGMEM DEVEUI[8]={0x3a, 0xe5, 0x30, 0xb7, 0x5a, 0xf2, 0x73, 0x0a };
void os_getDevEui (u1_t* buf) { memcpy_P(buf, DEVEUI, 8);}

// This key should be in big endian format (or, since it is not really a
// number but a block of memory, endianness does not really apply). In
// practice, a key taken from ttnctl can be copied as-is.
// The key shown here is the semtech default key.
static const u1_t PROGMEM APPKEY[16] = {0xac, 0x28, 0x7e, 0xdf, 0x33, 0xca, 0x66, 0x6b, 0xfc, 0x64, 0xdb, 0x13, 0x89, 0x8f, 0x9c, 0x6a};
void os_getDevKey (u1_t* buf) {  memcpy_P(buf, APPKEY, 16);}

//data_frame_t is deprecated;
//data_frame_t node_data;
static osjob_t sendjob;
LoRa_DataManager codec_obj;

// Schedule TX every this many seconds (might become longer due to duty
// cycle limitations).
const unsigned TX_INTERVAL = 10;

// Pin mapping
const lmic_pinmap lmic_pins = {
    .nss = 8,
    .rxtx = LMIC_UNUSED_PIN,
    .rst = 4,
    .dio = {3, 6, LMIC_UNUSED_PIN},
};

/********************************************************************************************/
/*                                  FUNCTION DECLARATION                                    */
/********************************************************************************************/
void onEvent (ev_t ev) {
    Serial.print(os_getTime());
    Serial.print(": ");
    switch(ev) {
        case EV_SCAN_TIMEOUT:
            Serial.println(F("EV_SCAN_TIMEOUT"));
            break;
        case EV_BEACON_FOUND:
            Serial.println(F("EV_BEACON_FOUND"));
            break;
        case EV_BEACON_MISSED:
            Serial.println(F("EV_BEACON_MISSED"));
            break;
        case EV_BEACON_TRACKED:
            Serial.println(F("EV_BEACON_TRACKED"));
            break;
        case EV_JOINING:
            Serial.println(F("EV_JOINING"));
            break;
        case EV_JOINED:
            Serial.println(F("EV_JOINED"));

            // Disable link check validation (automatically enabled
            // during join, but not supported by TTN at this time).
            LMIC_setLinkCheckMode(0);
            break;
        case EV_RFU1:
            Serial.println(F("EV_RFU1"));
            break;
        case EV_JOIN_FAILED:
            Serial.println(F("EV_JOIN_FAILED"));
            break;
        case EV_REJOIN_FAILED:
            Serial.println(F("EV_REJOIN_FAILED"));
            break;
            break;
        case EV_TXCOMPLETE:
            Serial.println(F("EV_TXCOMPLETE (includes waiting for RX windows)"));
            if (LMIC.txrxFlags & TXRX_ACK)
              Serial.println(F("Received ack"));
            if (LMIC.dataLen) {
              Serial.println(F("Received "));
              Serial.println(LMIC.dataLen);
              Serial.println(F(" bytes of payload"));
            }
            digitalWrite(TX_LED, LOW);
            // Schedule next transmission
            os_setTimedCallback(&sendjob, os_getTime()+sec2osticks(TX_INTERVAL), do_send);
            break;
        case EV_LOST_TSYNC:
            Serial.println(F("EV_LOST_TSYNC"));
            break;
        case EV_RESET:
            Serial.println(F("EV_RESET"));
            break;
        case EV_RXCOMPLETE:
            // data received in ping slot
            Serial.println(F("EV_RXCOMPLETE"));
            break;
        case EV_LINK_DEAD:
            Serial.println(F("EV_LINK_DEAD"));
            break;
        case EV_LINK_ALIVE:
            Serial.println(F("EV_LINK_ALIVE"));
            break;
         default:
            Serial.println(F("Unknown event"));
            break;
    }
}

void do_send(osjob_t* j){
    digitalWrite(TX_LED, HIGH);
    byte_buffer_t tx_buffer;
    codec_obj.encoder(tx_buffer);
   

    // Check if there is not a current TX/RX job running
    if (LMIC.opmode & OP_TXRXPEND) {
        Serial.println(F("OP_TXRXPEND, not sending"));
    } else {
        // Prepare upstream data transmission at the next possible time.
        LMIC_setTxData2(1, tx_buffer.byte_stream, sizeof(tx_buffer.byte_stream), 0);
        Serial.println(F("Packet queued"));
    }
    // Next TX is scheduled after TX_COMPLETE event.
}

void init_board( void ) {
    Serial.begin(9600);
    pinMode(TX_LED, OUTPUT);
}

static float read_battery_voltage( void ) {
    float measuredvbat = analogRead(VBATPIN);
    measuredvbat *= 2;    // we divided by 2, so multiply back
    measuredvbat *= 3.3;  // Multiply by 3.3V, our reference voltage
    measuredvbat /= 1024; // convert to voltage
    return measuredvbat;
}

static uint8_t get_battery_level_percent() {
    uint8_t measuredvbat_percent = 0;

    float measuredvbat = read_battery_voltage();
    double upper_barrier_bat_max_v = bat_max_v*100;
    Serial.print("VBat: " ); 
    Serial.println(measuredvbat);

        
    measuredvbat_percent = map((long)(measuredvbat*100), 0, 
                                  upper_barrier_bat_max_v, 0, 100);
    measuredvbat_percent = measuredvbat_percent >= 100 ? 100 : measuredvbat_percent;
    Serial.println(measuredvbat_percent);
    return measuredvbat_percent;
}

void setup() {
    init_board();
    Serial.println(F("Starting"));
    // LMIC init
    os_init();
    // Reset the MAC state. Session and pending data transfers will be discarded.
    LMIC_reset();

    // Start job (sending automatically starts OTAA too)
    do_send(&sendjob);
}

void loop() {
    GPS gps_obj(&Serial1);
    gps_data_frame_t gps_data;
    unsigned long old_time = 0;
    while(true) {
        gps_obj.get_data(gps_data);
        
        if(old_time+1000 < millis()) {
            gps_obj.print_date(gps_data);
            Serial.println();
            gps_obj.print_position(gps_data);
            Serial.println();
            old_time = millis();
            codec_obj.set_gps_data(gps_data);
            codec_obj.set_battery_level(get_battery_level_percent());
            
        }
        os_runloop_once();
    }
}
