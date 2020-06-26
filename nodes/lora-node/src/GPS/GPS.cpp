/****************************************************************************
 * @file GPS.cpp
 *
 * @author Timon Tscheulin
 * 
 * BSD license, all text above, and the splash screen header file,
 * must be included in any redistribution.
 *
 ****************************************************************************/

 /****************************************************************************
 * INCLUDES                                         
 ****************************************************************************/
#include "GPS.h"

/****************************************************************************
 * METHOD IMPLEMENTATION                                        
 ****************************************************************************/
GPS::GPS(HardwareSerial* gps_uart) {
    if(gps_uart != nullptr) {
        this->gps_uart = gps_uart;
        this->gps_uart->begin(9600);
    }
}

//This method must be called to get new gps data. min. one call per second. Serial buffer overflow! 
int GPS::get_data(gps_data_frame_t  &data) {
    bool new_data_recived = false;
    while (this->gps_uart->available()){
      //Serial.println("GPS: recived Data");
      new_data_recived = true;
      gps.encode(this->gps_uart->read());
    }

    //if new data recived. Update structure.
    if(new_data_recived == true) {
        data.satellites = this->gps.satellites();
        this->gps.f_get_position(&data.gps_latitude,
                             &data.gps_longitude,
                             &data.fix_age);
        data.gps_altitude = this->gps.f_altitude();
        this->gps.crack_datetime(&data.year,
                             &data.month,
                             &data.day,
                             &data.hour,
                             &data.minute,
                             &data.second,
                             &data.hundredths,
                             &data.date_age);
    }
}

TinyGPS* GPS::get_gps() {
    return &this->gps;
}

void GPS::print_position(const gps_data_frame_t &data) {
    Serial.println("Latitude  Longitude  Altitude Fix  ");
    Serial.println("(deg)     (deg)      (m)      Age  ");
    Serial.println("-----------------------------------");

    print_float(data.gps_latitude, TinyGPS::GPS_INVALID_F_ANGLE, 10, 6);
    print_float(data.gps_longitude, TinyGPS::GPS_INVALID_F_ANGLE, 11, 6);
    print_float(data.gps_altitude, TinyGPS::GPS_INVALID_F_ALTITUDE, 7, 2);
    Serial.print("  ");
    print_int(data.fix_age, TinyGPS::GPS_INVALID_AGE, 5);

}

void GPS::print_date(const gps_data_frame_t &data) {
    Serial.println("Date       Time     Date");
    Serial.println("                    Age ");
    Serial.println("------------------------");
    if (data.date_age == TinyGPS::GPS_INVALID_AGE) {
      Serial.print("********** ******** ");
    }
    else {
        char sz[32];
        sprintf(sz, "%02d/%02d/%02d %02d:%02d:%02d ",
                data.month,
                data.day,
                data.year,
                data.hour,
                data.minute,
                data.second);
        Serial.print(sz);
    }
    this->print_int(data.date_age, TinyGPS::GPS_INVALID_AGE, 5);
    this->smartdelay(0);
}

void GPS::print_str(const char *str, int len) {
    int slen = strlen(str);
    for (int i=0; i<len; ++i) {
        Serial.print(i<slen ? str[i] : ' ');
    }
    this->smartdelay(0);
}

void GPS::print_float(float val, float invalid, int len, int prec)
{
    if (val == invalid) {
        while (len-- > 1) {
            Serial.print('*');
        }
        Serial.print(' ');
    }
    else
    {
        Serial.print(val, prec);
        int vi = abs((int)val);
        int flen = prec + (val < 0.0 ? 2 : 1); // . and -
        flen += vi >= 1000 ? 4 : vi >= 100 ? 3 : vi >= 10 ? 2 : 1;
        for (int i=flen; i<len; ++i) {
            Serial.print(' ');
        }
    }
    this->smartdelay(0);
}

void GPS::print_int(unsigned long val, unsigned long invalid, int len)
{
    char sz[32];
    if (val == invalid) {
        strcpy(sz, "*******");
    }
    else {
        sprintf(sz, "%ld", val);
    }
    sz[len] = 0;
    for (int i=strlen(sz); i<len; ++i) {
        sz[i] = ' ';
    }
    if (len > 0) {
        sz[len-1] = ' ';
    }
    Serial.print(sz);
    this->smartdelay(0);
}

void GPS::smartdelay(unsigned long ms)
{
    unsigned long start = millis();
    do {
        while (this->gps_uart->available()) {
            gps.encode(this->gps_uart->read());
        }
    } while (millis() - start < ms);
}

    
