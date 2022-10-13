#include "main.h"

#define WII_NUNCHUCK_DEV_ID ((0x52)<<1) // I2C slave address

//nunchaku structure declaration 
 typedef struct {
	 		/*I2C Handle */
		I2C_HandleTypeDef *i2cHandle;
	 
    uint8_t joy_x;
    uint8_t joy_y;
    uint8_t button_c;
    uint8_t button_z;
    uint16_t accel_x;
    uint16_t accel_y;
    uint16_t accel_z;
 }wiiNunchuck;
 
  void WiiNunchuck_init(wiiNunchuck *dev,I2C_HandleTypeDef *i2cHandle);
	void WiiNunchuck_ReadData(wiiNunchuck * dev, I2C_HandleTypeDef * i2cHandle);
  static void send_request(I2C_HandleTypeDef * i2cHandle);
	static uint8_t decode_byte(uint8_t x);
 