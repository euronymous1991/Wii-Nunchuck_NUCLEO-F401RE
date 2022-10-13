#include "main.h"
#include "Wii_Nunchuk.h"

uint8_t rxData[6], decData[6];

void WiiNunchuck_init(wiiNunchuck * dev, I2C_HandleTypeDef * i2cHandle) {
  uint8_t pData[2] = {
    0x40,
    0x00
  };
  dev -> i2cHandle = i2cHandle;
  dev -> joy_x = 0;
  dev -> joy_y = 0;
  dev -> button_c = 0;
  dev -> button_z = 0;
  dev -> accel_x = 0;
  dev -> accel_y = 0;
  dev -> accel_z = 0;

  HAL_I2C_Master_Transmit(i2cHandle, WII_NUNCHUCK_DEV_ID, pData, 2, 3000);
}

static void send_request(I2C_HandleTypeDef * i2cHandle) {
  uint8_t pData[2] = {
    0x00,
    0x00
  };
  HAL_I2C_Master_Transmit(i2cHandle, WII_NUNCHUCK_DEV_ID, pData, 1, 3000);
}

static uint8_t decode_byte(uint8_t x) {
  return ((x ^ 0x17) + 0x17);
}

void WiiNunchuck_ReadData(wiiNunchuck * dev, I2C_HandleTypeDef * i2cHandle) {
  send_request(i2cHandle);
  HAL_Delay(2);

  char cnt;
  HAL_I2C_Master_Receive(i2cHandle, WII_NUNCHUCK_DEV_ID | 0x01, rxData, 6, 3000);

  //Data composite 
  for (cnt = 0; cnt < 6; cnt++) {
    decData[cnt] = decode_byte(rxData[cnt]);
  }
  //assignment to structure 
  dev -> joy_x = decData[0];
  dev -> joy_y = decData[1];
  dev -> button_c = (decData[5] >> 1) & 0x01;
  dev -> button_z = (decData[5] >> 0) & 0x01;
  dev -> accel_x = (((uint16_t) decData[2]) << 2) | ((decData[5] >> 2) & 0x03);
  dev -> accel_y = (((uint16_t) decData[3]) << 2) | ((decData[5] >> 4) & 0x03);
  dev -> accel_z = (((uint16_t) decData[4]) << 2) | ((decData[5] >> 6) & 0x03);
}
