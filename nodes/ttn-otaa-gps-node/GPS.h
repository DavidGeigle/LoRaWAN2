#ifndef __GPS_H__
#define __GPS_H__

/********************************************************************************************/
/*                                       INCLUDES                                           */
/********************************************************************************************/
#include <Arduino.h>
#include <TinyGPS.h>
#include <HardwareSerial.h>

/********************************************************************************************/
/*                                       TYPEDEFS                                           */
/********************************************************************************************/
typedef struct gps_data_frame_t {
    float gps_latitude;
    float gps_longitude;
    float gps_altitude;
    int satellites;
    int year;
    byte month;
    byte day;
    byte hour;
    byte minute;
    byte second;
    byte hundredths;
    unsigned long fix_age;
    unsigned long date_age;
};

/********************************************************************************************/
/*                                     CLASS DECLARATION                                    */
/********************************************************************************************/
class GPS {
    TinyGPS gps;
    HardwareSerial* gps_uart = nullptr;
    
public:
    GPS(HardwareSerial* gps_uart);
    int get_data(gps_data_frame_t  &data);
    TinyGPS* get_gps();
    void smartdelay(unsigned long ms);
    void print_date(const gps_data_frame_t &data);
    void print_position(const gps_data_frame_t &data);
private:
    void print_int(unsigned long val, unsigned long invalid, int len);
    void print_float(float val, float invalid, int len, int prec);
    void print_str(const char *str, int len);
};

#endif
