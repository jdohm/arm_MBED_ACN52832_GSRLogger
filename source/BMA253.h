/* BMA253 I2C library - offers a class to handle the accelerometer
 * created by j.dohm
 * date: 2020-07-29
 */
#ifndef _BMA253_Library
#define _BMA253_Library

//----------- includes --------------//
#include <mbed.h>

//----------- defines ---------------//
// 7-Bit address 0x18, 8-bit address 0x30
#define ADDRESS_ONE 	0x30
// 7-Bit address 0x19, 8-bit address 0x32
#define ADDRESS_TWO 	0x32
// register addresses
#define REG_ACC_X 	0x02 //read including 0x03 (12 bit data) | 0x02 LSB, 0x03 MSB
#define REG_ACC_Y 	0x04
#define REG_ACC_Z 	0x06
#define REG_TEMP 		0x08
#define REG_RANGE		0x0F
// 0011	| 2g	| 0.98mg/LSB
// 0101 | 4g	| 1.95mg/LSB
// 1000 | 8g	| 3.91mg/LSB
// 1100 | 16g | 7.81mg/LSB
#define RANGE_2G	0b0011
#define RANGE_4G	0b0101
#define RANGE_8G	0b1000
#define RANGE_16G	0b1100

#define REG_ACC_FILTER_BW	0x10 //Bandwidth 
// bits		|	f				| Update Time
// 01000 	| 7.81Hz	| 64ms
// 01001	| 15.63Hz	| 32ms
// 01010 	| 31.25Hz | 16ms
// 01011 	| 62.5Hz	| 8ms
// 01100 	| 125Hz		| 4ms
// 01101 	| 250Hz		| 2ms
// 01110 	| 500Hz		| 1ms
// 01111 	| 1000Hz	| 0.5ms
#define ACC_FILTER_BW_8HZ 		0b01000
#define ACC_FILTER_BW_16HZ 		0b01001
#define ACC_FILTER_BW_31HZ 		0b01010
#define ACC_FILTER_BW_63HZ 		0b01011
#define ACC_FILTER_BW_125HZ 	0b01100
#define ACC_FILTER_BW_250HZ 	0b01101
#define ACC_FILTER_BW_500HZ 	0b01110
#define ACC_FILTER_BW_1000HZ 	0b01111

#define REG_ACC_FILTER		0x13 //activate/deactivate Filter
#define ACC_FILTER_ON 		0x01 
#define ACC_FILTER_OFF 		0x00
struct bmaData {
	int16_t x;   //12bit starting at MSB
	int16_t y;
	int16_t z;
	int8_t temp; //23 grad C +0.5K/LSB
	bool int1;
	bool int2;
};

//----------- class -----------------//
class BMA253
{
	public:
  //----------- variables -------------//
	//----------- constructor -----------//
	BMA253(uint8_t address = ADDRESS_ONE);
	//----------- functiones ------------//
	uint8_t begin(I2C &i2c);
	//KnockOn
	uint8_t knockOnInt(bool set = true, uint8_t Int = 2);
	//Movementdetection
	uint8_t moveInt(bool set = true, uint8_t Int = 1);
	uint8_t moveIntSetThreashold(uint8_t thr);
	//Accelerations
	bmaData read();
	uint8_t setFilter(uint8_t set = ACC_FILTER_ON);
	uint8_t setFilterBW(uint8_t bw = ACC_FILTER_BW_250HZ);
	uint8_t setRange(uint8_t range = RANGE_2G);
	//Chip temp
	bmaData readTemp();
	private:
  	//----------- variables -------------//
    uint8_t _address;
    I2C *_i2c;
		uint8_t _range = RANGE_2G;
		//----------- functiones ------------//
		uint8_t writeByte(char reg, char dat);
		uint8_t readByte(char reg);
};
#endif
