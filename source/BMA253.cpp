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
//Interrupts
// chose Int Pin (INTERRUPT_PIN_INT*), and interrupt (INTERRUPT_PIN_*) to be set.
// optional set the interrupt mode (INTERRUPT_MODE_*)
uint8_t BMA253::activateInt(uint8_t pin, uint16_t interrupt){
	char _readData;
	switch (pin){
		case INTERRUPT_PIN_INT1:
			if(	interrupt <= 0xFF){
				_readData = readByte(REG_INTERRUPT_PIN_1);
				return writeByte(REG_INTERRUPT_PIN_1,_readData|interrupt);
			}
			else{
				_readData = readByte(REG_INTERRUPT_PIN_12);
				return writeByte(REG_INTERRUPT_PIN_12,_readData|uint8_t(interrupt>>8));
			}
		break;
		case INTERRUPT_PIN_INT2:
			if(	interrupt <= 0xFF){
				_readData = readByte(REG_INTERRUPT_PIN_2);
				return writeByte(REG_INTERRUPT_PIN_2,_readData|interrupt);
			}
			else{
				_readData = readByte(REG_INTERRUPT_PIN_12);
				//shift the bit to the bits for pin Int2
				uint8_t _writeData;
				_writeData = (uint8_t(interrupt>>1)|0x80);
				_writeData = _writeData|(uint8_t(interrupt>>3)|0x40);
				_writeData = _writeData|(uint8_t(interrupt>>5)|0x20);
				return writeByte(REG_INTERRUPT_PIN_12,_readData|_writeData);
			}
		break;
		default:
			return 0;
		break;
	}
}

uint8_t BMA253::activateInt(uint8_t pin, uint16_t interrupt, uint8_t mode){
	char _readData;
	writeByte(REG_INTERRUPT_MODE,mode);
  ThisThread::sleep_for(12ms);
	switch (pin){
		case INTERRUPT_PIN_INT1:
			if(	interrupt <= 0xFF){
				_readData = readByte(REG_INTERRUPT_PIN_1);
				return writeByte(REG_INTERRUPT_PIN_1,_readData|interrupt);
			}
			else{
				_readData = readByte(REG_INTERRUPT_PIN_12);
				return writeByte(REG_INTERRUPT_PIN_12,_readData|uint8_t(interrupt>>8));
			}
		break;
		case INTERRUPT_PIN_INT2:
			if(	interrupt <= 0xFF){
				_readData = readByte(REG_INTERRUPT_PIN_2);
				return writeByte(REG_INTERRUPT_PIN_2,_readData|interrupt);
			}
			else{
				_readData = readByte(REG_INTERRUPT_PIN_12);
				//shift the bit to the bits for pin Int2
				uint8_t _writeData;
				_writeData = (uint8_t(interrupt>>1)|0x80);
				_writeData = _writeData|(uint8_t(interrupt>>3)|0x40);
				_writeData = _writeData|(uint8_t(interrupt>>5)|0x20);
				return writeByte(REG_INTERRUPT_PIN_12,_readData|_writeData);
			}
		break;
		default:
			return 1;
		break;
	}
}

// chose Int Pin (INTERRUPT_PIN_INT*), and interrupt (INTERRUPT_PIN_*) to be shutoff.
uint8_t BMA253::deactivateInt(uint8_t pin, uint16_t interrupt){
	char _readData;
	switch (pin){
		case INTERRUPT_PIN_INT1:
			if(	interrupt <= 0xFF){
				_readData = readByte(REG_INTERRUPT_PIN_1);
				return writeByte(REG_INTERRUPT_PIN_1,_readData&!interrupt);
			}
			else{
				_readData = readByte(REG_INTERRUPT_PIN_12);
				return writeByte(REG_INTERRUPT_PIN_12,_readData&!uint8_t(interrupt>>8));
			}
		break;
		case INTERRUPT_PIN_INT2:
			if(	interrupt <= 0xFF){
				_readData = readByte(REG_INTERRUPT_PIN_2);
				return writeByte(REG_INTERRUPT_PIN_2,_readData&!interrupt);
			}
			else{
				_readData = readByte(REG_INTERRUPT_PIN_12);
				//shift the bit to the bits for pin Int2
				uint8_t _writeData;
				_writeData = (uint8_t(interrupt>>1)|0x80);
				_writeData = _writeData|(uint8_t(interrupt>>3)|0x40);
				_writeData = _writeData|(uint8_t(interrupt>>5)|0x20);
				return writeByte(REG_INTERRUPT_PIN_12,_readData&!_writeData);
			}
		break;
		default:
			return 0;
		break;
	}
}

//KnockOn | sets the double tap interrupt
uint8_t BMA253::knockOnInt(bool set, uint8_t pin){
	if(set) return activateInt(pin,INTERRUPT_PIN_D_TAP);
	else return deactivateInt(pin,INTERRUPT_PIN_D_TAP);
}
//Movementdetection
uint8_t BMA253::moveInt(bool set, uint8_t pin){
	if(set) return activateInt(pin,INTERRUPT_PIN_SLOPE);
	else return deactivateInt(pin,INTERRUPT_PIN_SLOPE);
}

// In terms of scaling 1 LSB of (thr) corresponds to 3.91 mg in 2g-range 
// (7.81 mg in 4g-range, 15.6 mg in 8g-range and 31.3 mg in 16g-range). 
// Therefore the maximum value is 996 mg in 2g-range 
// (1.99g in 4g-range, 3.98g in 8g-range and 7.97g in 16g-range).
uint8_t BMA253::moveIntSetThreashold(uint8_t thr){
	uint8_t _activeInts;
	//check if Pin Int1 was connected to slope interrupt before
	// if so deactivate and safe the information
	if(readByte(REG_INTERRUPT_PIN_1)&INTERRUPT_PIN_SLOPE){
		deactivateInt(INTERRUPT_PIN_INT1,INTERRUPT_PIN_SLOPE);
		_activeInts = 0x01;
	}
	//check if Pin Int2 was connected to slope interrupt before
	// if so deactivate and safe the information
	if(readByte(REG_INTERRUPT_PIN_2)&INTERRUPT_PIN_SLOPE){
		deactivateInt(INTERRUPT_PIN_INT2,INTERRUPT_PIN_SLOPE);
		_activeInts = _activeInts|0x02;
	}
	// return 1 (error) if writeByte returned error, otherwise continue
	// leaves interrupts deaktivated even if they where active before
 	if(writeByte(REG_INTERRUPT_SLOPE_THREASHOLD,thr)) return 1;
  ThisThread::sleep_for(12ms);
	switch (_activeInts){
		case 0x01:
			return activateInt(REG_INTERRUPT_PIN_1,INTERRUPT_PIN_SLOPE);
		break;
		case 0x02:
			return activateInt(REG_INTERRUPT_PIN_2,INTERRUPT_PIN_SLOPE);
		break;
		case 0x03:
			//only return 0 (SUCCESS) if both activations returned success
			return activateInt(REG_INTERRUPT_PIN_1,INTERRUPT_PIN_SLOPE)|activateInt(REG_INTERRUPT_PIN_2,INTERRUPT_PIN_SLOPE);
		break;
		default:
			//return 0 (SUCCESS) if writeByte was a success and no reg needs activation
			return 0;
		break;
	}
}
// The time difference between the successive acceleration signals 
// depends on the selected bandwidth and equates to 1/(2*bandwidth) (t=1/(2*bw)). 
// In order to suppress false triggers, the interrupt is only generated (cleared) 
// if a certain number N of consecutive slope data points is larger (smaller) 
// than the slope threshold given by thr. 
// This number is set by the slope_dur bits. 
// It is N = slope_dur + 1.
// Example: slope_dur = 00b, ..., 11b = 1decimal, ..., 4decimal.
uint8_t BMA253::moveIntSetThreashold(uint8_t thr, uint8_t slope_dur){
	uint8_t _activeInts;
	if(slope_dur <= 0x04) return 1;// only 2 bits for slope_dur!
	//check if Pin Int1 was connected to slope interrupt before
	// if so deactivate and safe the information
	if(readByte(REG_INTERRUPT_PIN_1)&INTERRUPT_PIN_SLOPE){
		deactivateInt(INTERRUPT_PIN_INT1,INTERRUPT_PIN_SLOPE);
		_activeInts = 0x01;
	}
	//check if Pin Int2 was connected to slope interrupt before
	// if so deactivate and safe the information
	if(readByte(REG_INTERRUPT_PIN_2)&INTERRUPT_PIN_SLOPE){
		deactivateInt(INTERRUPT_PIN_INT2,INTERRUPT_PIN_SLOPE);
		_activeInts = _activeInts|0x02;
	}
	//---- slope duration ----//
	//read old register data
	uint8_t _readData = readByte(REG_INTERRUPT_SLOPE_DUR);
	//set only slope_dur and keep old values for rest of register
	// return 1 (error) if writeByte returned error, otherwise continue
	// leaves interrupts deaktivated even if they where active before
 	if(writeByte(REG_INTERRUPT_SLOPE_DUR,(_readData&0xFC)|slope_dur)) return 1;
	//---- slope dur end -----//
	// return 1 (error) if writeByte returned error, otherwise continue
	// leaves interrupts deaktivated even if they where active before
 	if(writeByte(REG_INTERRUPT_SLOPE_THREASHOLD,thr)) return 1;
  ThisThread::sleep_for(12ms);
	switch (_activeInts){
		case 0x01:
			return activateInt(REG_INTERRUPT_PIN_1,INTERRUPT_PIN_SLOPE);
		break;
		case 0x02:
			return activateInt(REG_INTERRUPT_PIN_2,INTERRUPT_PIN_SLOPE);
		break;
		case 0x03:
			//only return 0 (SUCCESS) if both activations returned success
			return activateInt(REG_INTERRUPT_PIN_1,INTERRUPT_PIN_SLOPE)|activateInt(REG_INTERRUPT_PIN_2,INTERRUPT_PIN_SLOPE);
		break;
		default:
			//return 0 (SUCCESS) if writeByte was a success and no reg needs activation
			return 0;
		break;
	}
}

//Accelerations
bmaData BMA253::read(){
	char data[6];
	bmaData bdat;
	data[0] = REG_ACC_X;
	_i2c->write(_address,data,1,true);
	_i2c->read(_address,data,6,false);
	bdat.x = (data[0] | (data[1]<<8));
	bdat.y = (data[2] | (data[3]<<8));
	bdat.z = (data[4] | (data[5]<<8));
	/*
	switch (_range){
		case RANGE_2G:
			bdat.xf = (bdat.x>>5)*0.98;
			bdat.yf = (bdat.y>>5)*0.98;
			bdat.zf = (bdat.z>>5)*0.98;
		break;
		case RANGE_4G:
			bdat.xf = (bdat.x>>5)*1.95;
			bdat.yf = (bdat.y>>5)*1.95;
			bdat.zf = (bdat.z>>5)*1.95;
		break;
		case RANGE_8G:
			bdat.xf = (bdat.x>>5)*3.91;
			bdat.yf = (bdat.y>>5)*3.91;
			bdat.zf = (bdat.z>>5)*3.91;
		break;
		case RANGE_16G:
			bdat.xf = (bdat.x>>5)*7.81;
			bdat.yf = (bdat.y>>5)*7.81;
			bdat.zf = (bdat.z>>5)*7.81;
		break;
		default:
			bdat.xf = 0.0;
			bdat.yf = 0.0;
			bdat.zf = 0.0;
		break;
	}
	*/
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
