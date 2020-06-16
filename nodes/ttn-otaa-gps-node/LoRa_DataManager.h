/****************************************************************************
 * @file LoRa_DataManager.h
 * 
 * @author Timon Tscheulin
 *
 * @brief <b> Discription: </b> LoRa node data manager 
 *
 * BSD license, all text above, and the splash screen header file,
 * must be included in any redistribution.
 *
 ****************************************************************************/
#ifndef __LORA_DATA_MANAGER_H__
#define __LORA_DATA_MANAGER_H__

/****************************************************************************
 * INCLUDES                                         
 ****************************************************************************/
#include "GPS.h"

/****************************************************************************
 * DEFINITION                                         
 ****************************************************************************/
typedef uint8_t flag_t;

//struct hold payload with flags
typedef struct lora_data_frame_t {
    flag_t   frame_counter_flag;
    uint32_t frame_counter;
    flag_t   gps_flag;
    uint32_t gps_longitude;
    uint32_t gps_latitude;
    uint32_t gps_altitude;
    uint16_t gps_year;
    uint8_t  gps_month;
    uint8_t  gps_day;
    uint8_t  gps_hour;
    uint8_t  gps_minute;
    uint8_t  gps_second; 
    flag_t   battery_status_flag;
    uint8_t  battery_level;
};

typedef struct byte_buffer_t {
  uint8_t byte_stream[sizeof(lora_data_frame_t)];
};

/****************************************************************************
 * CLASS DEFINITION
 ****************************************************************************/
class LoRa_DataManager {
    lora_data_frame_t lora_payload;
    bool gps_active;
    bool frame_counter_active;
    bool battery_status_active;
public:
    LoRa_DataManager(bool enable_frame_counter=true);
    void set_gps_data(const gps_data_frame_t &data);
    void set_battery_level(uint8_t  battery_level_percent);
    void encoder(byte_buffer_t &tx_buffer);
    //void decoder(const lora_payload_t rx_buffer, data_frame_t &data);
private:
    static uint32_t correct_endianness_uint32(uint32_t value);
    static uint16_t correct_endianness_uint16(uint16_t value);
    static uint32_t convert_float_to_uint32(float value, int precission=0);
    void encode_frame_counter(uint8_t* &p_buffer);
    void encode_gps(uint8_t* &p_buffer);
    void encode_battery_status(uint8_t* &p_buffer);
};
#endif
