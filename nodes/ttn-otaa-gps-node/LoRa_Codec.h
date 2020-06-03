#ifndef __LORA_CODEC_H__
#define __LORA_CODEC_H__

#include "GPS.h"
/********************************************************************************************/
/*                                          DEFINES                                         */
/********************************************************************************************/

typedef uint16_t flag_t;

//struct to hold payload
typedef struct data_frame_t {
    uint32_t frame_counter;
    gps_data_frame_t pos_time_data;
    uint8_t  battery_status;
};

//struct to hold payload with flags
typedef struct lora_data_frame_t {
    flag_t   frame_counter_flag;
    uint32_t frame_counter;
    flag_t   gps_flag;
    gps_data_frame_t pos_time_data;
    flag_t   battery_status_flag;
    uint8_t  battery_status;
};

//union for conversion
//
typedef union lora_payload_t {
    lora_data_frame_t payload;
    uint8_t byte_stream[sizeof(data_frame_t)];
};

typedef struct byte_buffer_t {
  uint8_t byte_stream[sizeof(data_frame_t)];
};

/********************************************************************************************/
/*                                 GLOBALS AND CONSTANTS                                    */
/********************************************************************************************/
enum data_flag{
    dev_id_flag = 0x0001,
    frame_counter_flag,
    gps_flag,
    battery_status_flag,
};

class LoRa_Codec {
    lora_payload_t lora_payload;
public:
    LoRa_Codec();
    void encoder(const data_frame_t data, byte_buffer_t &tx_buffer);
    void decoder(const lora_payload_t rx_buffer, data_frame_t &data);
};
#endif
