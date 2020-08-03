/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mbed.h"

// To send debug/logging messages to Seggers JLinkRTTViewer 
#include "SEGGER_RTT.h"
#include "SEGGER_RTT.c"
#include "SEGGER_RTT_printf.c"
#include "SparkFun_Bio_Sensor_Hub_Library.h"
#include "BMA253.h"

// To use milliseconds without calling std::chrono
using namespace std::chrono;

// Blinking rate in milliseconds
#define BLINKING_RATE     100ms


// --- I2C ---
I2C i2c(I2C_SDA0,I2C_SCL0);
    // HR
    DigitalOut   resetPin(p26,0);
    DigitalInOut mfioPin(p25,PIN_OUTPUT,PullUp,0);
    SparkFun_Bio_Sensor_Hub bioHub(resetPin, mfioPin);

    bioData body;
    // --- ACC ---
    DigitalIn   ACC_Int1(p27);
    BMA253 bma(ADDRESS_ONE);
    bmaData accData;
//I2C end

// Initialise the digital pin LD1 as an output
DigitalOut led(p24);
// Read from p0.28 - use jumper to switch from Pot to J4
AnalogIn adc_gsr1(p28);
AnalogIn adc_gsr2(p29);

int main()
{
    //ACC start
    int result = bma.begin(i2c);
    result = bma.setElIntBehaviour(INTERRUPT_PIN_INT1,INTERRUPT_EL_BEHAVIOUR_PUSHPULL,INTERRUPT_EL_BEHAVIOUR_LVL_NORMAL);
    #ifdef _debug
		SEGGER_RTT_printf(0,"#setElIntBehaviour returned: %X\n",result);
	#endif
    result = bma.moveIntSetThreashold(0x10,0b10);
    #ifdef _debug
		SEGGER_RTT_printf(0,"#moveIntSetThreashold returned: %X\n",result);
	#endif
    result = bma.moveInt(true);
    #ifdef _debug
		SEGGER_RTT_printf(0,"#moveInt returned: %X\n",result);
	#endif
    result = bma.knockOnInt(true);
    #ifdef _debug
   	    SEGGER_RTT_printf(0,"#knockOnInt returned: %X\n",result);
    #endif
   ThisThread::sleep_for(1s);
   //HR start
    result = bioHub.begin(i2c);
    SEGGER_RTT_printf(0,"#bioHub returned %d\n",result);
    ThisThread::sleep_for(1s);
    result = bioHub.configBpm(MODE_ONE);
    SEGGER_RTT_printf(0,"#bioHub.configBpm returned %d\n",result);
    ThisThread::sleep_for(4s);
    body = bioHub.readBpm();
    SEGGER_RTT_printf(0,"#Heartrate: %d\n",body.heartRate);
    SEGGER_RTT_printf(0,"#Confidence: %d\n",body.confidence);
    SEGGER_RTT_printf(0,"#Oxygen: %d\n",body.oxygen);
    SEGGER_RTT_printf(0,"#Status: %d\n",body.status);

    ThisThread::sleep_for(1s);



	// get start time
	auto start = Kernel::Clock::now(); // type will be Kernel::Clock::time_point

	SEGGER_RTT_printf(0,"#times since start in ms, adc value between 0xFFFF and 0x0\n");
	SEGGER_RTT_printf(0,"#outputs in decimals \n");
    SEGGER_RTT_printf(0,"time;gsr1;gsr2;acc_x;y;z;movement_detected;double_tap_detected;HR;HR_Confidence\n");
    
    while (true) {
	// get elapsed time in milliseconds
	milliseconds elapsed_time = Kernel::Clock::now() - start;
	// print/log timestamp and adc value to RTT
	//SEGGER_RTT_printf(0,"%d adc_value: %d\n",int(elapsed_time.count()),adc_gsr.read_u16());
    accData = bma.read();
	//SEGGER_RTT_printf(0,"%d x_value: %07d\n",int(elapsed_time.count()),accData.x);
	//SEGGER_RTT_printf(0,"%d y_value: %07d\n",int(elapsed_time.count()),accData.y);
	//SEGGER_RTT_printf(0,"%d z_value: %07d\n",int(elapsed_time.count()),accData.z);
    result = bma.readByte(REG_INTERRUPT_ACTIVE);
	//SEGGER_RTT_printf(0,"%d movement_triggered: %01X\n",int(elapsed_time.count()),(result&0x04)?1:0);
	//SEGGER_RTT_printf(0,"%d double_tap_triggered: %01X\n",int(elapsed_time.count()),(result&0x10)?1:0);

    body = bioHub.readBpm();
    //SEGGER_RTT_printf(0,"%d Heartrate: %d\n",int(elapsed_time.count()),body.heartRate);
    //SEGGER_RTT_printf(0,"%d Confidence: %d\n",int(elapsed_time.count()),body.confidence);
    SEGGER_RTT_printf(0, "%d;%d;%d;%d;%d;%d;%01d;%01d;%02d;%02d\n",int(elapsed_time.count()),adc_gsr1.read_u16(),adc_gsr2.read_u16(),accData.x,accData.y,accData.z,(result&0x04)?1:0,(result&0x10)?1:0,body.heartRate,body.confidence);

        led = !led;
        ThisThread::sleep_for(BLINKING_RATE);
    }
}
