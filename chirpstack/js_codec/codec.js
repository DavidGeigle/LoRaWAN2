// Decode decodes an array of bytes into an object.
//  - fPort contains the LoRaWAN fPort number
//  - bytes is an array of bytes, e.g. [225, 230, 255, 0]
// The function must return an object, e.g. {"temperature": 22.5}
function Decode(fPort, bytes) {
  var decoded = {};
  var hexString=bin2HexStr(bytes);
  return rakSensorDataDecode(hexString);
}

// convert array of bytes to hex string.
// e.g: 0188053797109D5900DC140802017A0768580673256D0267011D040214AF0371FFFFFFDDFC2E
function bin2HexStr(bytesArr) {
  var str = "";
  for(var i=0; i<bytesArr.length; i++) {
    var tmp = (bytesArr[i] & 0xff).toString(16);
    if(tmp.length == 1) {
      tmp = "0" + tmp;
    }
    str += tmp;
  }
  return str;
}

// convert string to short integer
function parseShort(str, base) {
  var n = parseInt(str, base);
  return (n << 16) >> 16;
}

function parseUint8_t(str, base) {
  var n = parseInt(str, base);
  return (n << 32) >> 32;
}

// convert string to triple bytes integer
function parseTriple(str, base) {
  var n = parseInt(str, base);
  return (n << 8) >> 8;
}

// convert string to Quadruple bytes integer
function parseQuadruple(str, base) {
  var n = parseInt(str, base);
  return (n << 32) >> 32;
}

// decode Hex sensor string data to object
function rakSensorDataDecode(hexStr) {
  var str = hexStr;
  var myObj = {};
  
  //myObj.payload_length = str.length;
  //myObj.str = str;
  //myObj.flag = 

  while (str.length > 4) {
    var flag = parseInt(str.substring(0, 2), 16);
    switch (flag) {
      case 0x03:// GPS
        var object_location = {}
        var object_time = {}
        object_location.latitude = parseFloat((parseInt(str.substring(2, 10), 16) * 0.000001).toFixed(6));//unit:°
        object_location.longitude = parseFloat((parseInt(str.substring(10, 18), 16) * 0.000001).toFixed(6));//unit:°
        object_location.altitude = parseFloat((parseInt(str.substring(18, 26), 16) * 0.01).toFixed(1));//unit:m
        object_time.year = parseInt(str.substring(26, 30), 16);
        object_time.month = parseInt(str.substring(30, 32), 16);
        object_time.day = parseInt(str.substring(32, 34), 16);
        object_time.hour = parseInt(str.substring(34, 36), 16);
        object_time.minute = parseInt(str.substring(36, 38), 16);
        object_time.second = parseInt(str.substring(38, 40), 16);
        myObj.gps_location = object_location;
        myObj.gps_time = object_time;
        str = str.substring(40);
        break;
      case 0x02://Frame counter
        myObj.frame_counter = parseInt(str.substring(2, 10), 16);
        str = str.substring(10);
        break;
      case 0x04://battery status
        myObj.device_status_battery_level = parseInt(str.substring(2, 4), 16);
        str = str.substring(4);
        break;
      case 0x05:// mq135
        myObj.mq135_co2_ppm = parseInt(str.substring(2, 6), 16);
        str = str.substring(6);
        break;
      case 0x06://scd30
        var scd30 = {};
        //convert this values to float?
        scd30.scd30_co2_ppm = parseInt(str.substring(2, 6), 16);
        scd30.scd30_temperature = parseInt(str.substring(6, 10), 16);
        scd30.scd30_humidity = parseInt(str.substring(10, 14), 16);
        myObj.scd30 = scd30;
        str = str.substring(14);
        break;
      case 0x07://tsl2591
        myObj.tsl2591_lux = parseInt(str.substring(2,10),16);
        str = str.substring(10);
        break;
      default:
        str = str.substring(2);
        break;
    }
  }

  return myObj;
}
