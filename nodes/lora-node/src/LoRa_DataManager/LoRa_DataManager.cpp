/****************************************************************************
 * @file LoRa_DataManager.cpp
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
#include "LoRa_DataManager.h"

/****************************************************************************
 * DEFINITION FOR MODULE INTERNAL USE                                        
 ****************************************************************************/

enum data_flag{
    dev_id_flag = 0x01,
    frame_counter_flag,
    gps_flag,
    battery_status_flag,
};

/****************************************************************************
 * METHOD IMPLEMENTATION                                        
 ****************************************************************************/

/**
 * This method corrects the endianness for lora transfer of uint32_t values.
 * 
 * @param [in] value uint32_t to convert 
 *        
 * @return the converted uint32_t
 */
uint32_t LoRa_DataManager::correct_endianness_uint32(uint32_t value) {
    uint32_t buffer = 0x00000000;
    buffer |=  0x000000FF & (value >> 24);
    buffer |=  0x0000FF00 & (value >> 8);
    buffer |=  0x00FF0000 & (value << 8);
    buffer |=  0xFF000000 & (value << 24);
    return buffer;
}

/**
 * This method corrects the endianness for lora transfer of uint16_t values.
 * 
 * @param [in] value uint16_t to convert 
 *        
 * @return the converted uint16_t
 */
uint16_t LoRa_DataManager::correct_endianness_uint16(uint16_t value) {
    uint32_t buffer = 0x0000;
    buffer |=  0x00FF & (value >> 8);
    buffer |=  0xFF00 & (value << 8);
    return buffer;
}

/**
 * This method converts a float to an int and corrects his
 * endianness for lora transfer.
 * old: float (2345.23345)
 * new: uint32_t (234523345)
 * 
 * @param [in] value float to convert 
 * @param [in] precission Tells the method, how many decimal 
 *        places be noted
 *        
 * @return the converted value as uint32_t
 */
uint32_t LoRa_DataManager::convert_float_to_uint32(float value, int precission) {
    uint32_t buffer = 0;
    buffer = (uint32_t)(value * pow(10, precission));
    buffer = correct_endianness_uint32(buffer);
    return buffer;
}

/**
 * This the default constructor.
 * 
 * @param [in] enable_frame_counter default is ative.
 */
LoRa_DataManager::LoRa_DataManager(bool enable_frame_counter) {
    //status flags for package encodeing
    gps_active = false;
    frame_counter_active = enable_frame_counter;
    battery_status_active = false;
    /*Set all frame-flags in this constructor. They are allways the same!*/
    lora_payload.frame_counter = 0;
    lora_payload.frame_counter_flag = (int)frame_counter_flag;
    lora_payload.gps_flag = gps_flag;
    lora_payload.battery_status_flag = battery_status_flag;
}

/**
 * Set GPS data for bytestream convertion.
 * !!! Activates gps data package !!!
 * 
 * @param [in] data referenz of gps data structure 
 */
void LoRa_DataManager::set_gps_data(const gps_data_frame_t &data) {
    gps_active = true;
    lora_payload.gps_latitude = convert_float_to_uint32(data.gps_latitude, 6);
    lora_payload.gps_longitude = convert_float_to_uint32(data.gps_longitude, 6);
    lora_payload.gps_altitude = convert_float_to_uint32(data.gps_altitude, 2);
    lora_payload.gps_year = correct_endianness_uint16(data.year);
    lora_payload.gps_month = data.month;
    lora_payload.gps_day = data.day;
    lora_payload.gps_hour = data.hour;
    lora_payload.gps_minute = data.minute;
    lora_payload.gps_second = data.second;
}

/**
 * Set set battery level for bytestream convertion.
 * !!! Activates battery status package !!!
 * 
 * note: If value is not in the intvall [0, 100] the 
 * battery_value field holds the error value 200.
 * 
 * @param [in] battery_level_percent holds battery level in percent 
 */
void LoRa_DataManager::set_battery_level(uint8_t battery_level_percent) {
    battery_status_active = true;
    if(battery_level_percent <= 100) {
        lora_payload.battery_level = battery_level_percent;
    }
    else {
        //error value
        lora_payload.battery_level = 200;
    }
}

/**
 * Helper function to encode the frame counter package.
 * 
 * @param [out] p_buffer pointer referenz to current buffer address.
 */
void LoRa_DataManager::encode_frame_counter(uint8_t* &p_buffer) {
    if(frame_counter_active) {
        //increas frame counter if a new frame is requested!
        ++lora_payload.frame_counter;
        uint32_t frame_counter = correct_endianness_uint32(lora_payload.frame_counter);
        //encode framecounter package
        memmove(p_buffer, &lora_payload.frame_counter_flag, sizeof(lora_payload.frame_counter_flag));
        p_buffer += sizeof(lora_payload.frame_counter_flag);
        memmove(p_buffer, &frame_counter, sizeof(frame_counter));
        p_buffer += sizeof(frame_counter);
    }
}

/**
 * Helper function to encode the gps package.
 * 
 * @param [out] p_buffer pointer referenz to current buffer address.
 */
void LoRa_DataManager::encode_gps(uint8_t* &p_buffer) {
    if(gps_active) {
        //encode gps package
        memmove(p_buffer, &lora_payload.gps_flag, sizeof(lora_payload.gps_flag));
        p_buffer += sizeof(lora_payload.gps_flag);
        //gps position
        memmove(p_buffer, &lora_payload.gps_latitude, sizeof(lora_payload.gps_latitude));
        p_buffer += sizeof(lora_payload.gps_latitude);
        memmove(p_buffer, &lora_payload.gps_longitude, sizeof(lora_payload.gps_longitude));
        p_buffer += sizeof(lora_payload.gps_longitude);
        memmove(p_buffer, &lora_payload.gps_altitude, sizeof(lora_payload.gps_altitude));
        p_buffer += sizeof(lora_payload.gps_altitude);
        
        //gps time
        memmove(p_buffer, &lora_payload.gps_year, sizeof(lora_payload.gps_year));
        p_buffer += sizeof(lora_payload.gps_year);
        memmove(p_buffer, &lora_payload.gps_month, sizeof(lora_payload.gps_month));
        p_buffer += sizeof(lora_payload.gps_month);
        memmove(p_buffer, &lora_payload.gps_day, sizeof(lora_payload.gps_day));
        p_buffer += sizeof(lora_payload.gps_day);
        memmove(p_buffer, &lora_payload.gps_hour, sizeof(lora_payload.gps_hour));
        p_buffer += sizeof(lora_payload.gps_hour);
        memmove(p_buffer, &lora_payload.gps_minute, sizeof(lora_payload.gps_minute));
        p_buffer += sizeof(lora_payload.gps_minute);
        memmove(p_buffer, &lora_payload.gps_second, sizeof(lora_payload.gps_second));
        p_buffer += sizeof(lora_payload.gps_second);
    }
}

/**
 * Helper function to encode the battery status package.
 * 
 * @param [out] p_buffer pointer referenz to current buffer address.
 */
void LoRa_DataManager::encode_battery_status(uint8_t* &p_buffer) {
    if(battery_status_active) {
        //encode battery status package
        memmove(p_buffer, &lora_payload.battery_status_flag, sizeof(lora_payload.battery_status_flag));
        p_buffer += sizeof(lora_payload.battery_status_flag);
        memmove(p_buffer, &lora_payload.battery_level, sizeof(lora_payload.battery_level));
        p_buffer += sizeof(lora_payload.battery_level);
    }
}

/**
 * Encodes the member value lora_payload into compact bytestream.
 * The bytestream is used as lora payload.
 * 
 * @param [out] referenz to byte stream buffer.
 */
void LoRa_DataManager::encoder(byte_buffer_t &tx_buffer) {
    uint8_t buffer [sizeof(tx_buffer)] = {0};
    //actual memory index
    uint8_t *p_buffer = buffer;

    //encode data
    this->encode_frame_counter(p_buffer);
    this->encode_gps(p_buffer);
    this->encode_battery_status(p_buffer);

    //copy buffer to tx_stream
    memcpy( tx_buffer.byte_stream, buffer, sizeof(tx_buffer.byte_stream));
    tx_buffer.byte_stream_size = (size_t)(p_buffer + 1 - buffer);
}

/*void LoRa_Codec::decoder(const lora_payload_t rx_buffer, data_frame_t &data) {
    
}*/
