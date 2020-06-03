#include "LoRa_Codec.h"

LoRa_Codec::LoRa_Codec() {
    lora_payload.payload.frame_counter_flag = frame_counter_flag;
    lora_payload.payload.gps_flag = gps_flag;
    lora_payload.payload.battery_status_flag = battery_status_flag;
}

void LoRa_Codec::encoder(const data_frame_t data, byte_buffer_t &tx_buffer) {
    lora_payload.payload.frame_counter = data.frame_counter;
    lora_payload.payload.pos_time_data.satellites = data.pos_time_data.satellites;
    lora_payload.payload.pos_time_data.gps_longitude = data.pos_time_data.gps_longitude;
    lora_payload.payload.pos_time_data.gps_latitude = data.pos_time_data.gps_latitude;
    lora_payload.payload.pos_time_data.gps_altitude = data.pos_time_data.gps_altitude;
    lora_payload.payload.pos_time_data.year = data.pos_time_data.year;
    lora_payload.payload.pos_time_data.month = data.pos_time_data.month;
    lora_payload.payload.pos_time_data.day = data.pos_time_data.day;
    lora_payload.payload.pos_time_data.hour = data.pos_time_data.hour;
    lora_payload.payload.pos_time_data.minute = data.pos_time_data.minute;
    lora_payload.payload.pos_time_data.second = data.pos_time_data.second;
    lora_payload.payload.pos_time_data.hundredths = data.pos_time_data.hundredths;
    lora_payload.payload.pos_time_data.fix_age = data.pos_time_data.fix_age;
    lora_payload.payload.pos_time_data.date_age = data.pos_time_data.date_age;
    lora_payload.payload.battery_status = data.battery_status;
    
    memcpy(lora_payload.byte_stream, tx_buffer.byte_stream, sizeof(tx_buffer.byte_stream));
    
}

void LoRa_Codec::decoder(const lora_payload_t rx_buffer, data_frame_t &data) {
    
}
