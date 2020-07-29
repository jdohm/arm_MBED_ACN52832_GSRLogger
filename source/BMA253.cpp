/* BMA253 I2C library - offers a class to handle the accelerometer
 * created by j.dohm
 * date: 2020-07-29
 */

#include <mbed.h>
#include <BMA253.h>

//----------- constructor -----------//
BMA253::BMA253(uint8_t address){
	_address = address;
}
//----------- functiones ------------//
uint8_t BMA253::begin(I2C &i2c){
  _i2c = &i2c;
	if (writeByte(REG_RANGE,RANGE_2G)) return 1;
	if (writeByte(REG_ACC_FILTER_BW,ACC_FILTER_BW_250HZ)) return 1;
	if (writeByte(REG_ACC_FILTER,ACC_FILTER_ON)) return 1;
	return 0;
}
//KnockOn
uint8_t BMA253::knockOnInt(bool set, uint8_t Int){
 // TODO
	return 0;
}
//Movementdetection
uint8_t BMA253::moveInt(bool set, uint8_t Int){
 // TODO
	return 0;
}

uint8_t BMA253::moveIntSetThreashold(uint8_t thr){
 // TODO
	return 0;
}

//Accelerations
bmaData BMA253::read(){
	char data[6];
	bmaData bdat;
	data[0] = REG_ACC_X;
	_i2c->write(_address,data,1,true);
	_i2c->read(_address,data,6,false);
	bdat.x = (data[0]<<8|data[1]);
	bdat.y = (data[2]<<8|data[3]);
	bdat.z = (data[4]<<8|data[5]);
	return bdat;
}

uint8_t BMA253::setFilter(uint8_t set){
	return writeByte(REG_ACC_FILTER,set);
}

uint8_t BMA253::setFilterBW(uint8_t bw){
	return writeByte(REG_ACC_FILTER_BW,bw);
}

uint8_t BMA253::setRange(uint8_t range){
	return writeByte(REG_RANGE,range);
}

//Chip temp
bmaData BMA253::readTemp(){
	char data[1];
	bmaData bdat;
	data[0] = REG_TEMP;
	_i2c->write(_address,data,1,true);
	_i2c->read(_address,data,1,false);
	bdat.temp = (data[0]);
	return bdat;
}

//private
uint8_t BMA253::writeByte(char reg, char dat){
	char data[2];
	data[0] = reg;
	data[1] = dat;
 // write data
	_i2c->write(_address,data,2,false);
  ThisThread::sleep_for(2ms);
 // readByte
	if(dat == readByte(reg)) return 0;
	return 1;
}

uint8_t BMA253::readByte(char reg){
	char data;
	_i2c->write(_address,&reg,1,true);
	_i2c->read(_address,&data,1,false);
	return data;
}
