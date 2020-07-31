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
#define BLINKING_RATE     1500ms


// --- I2C ---
I2C i2c(I2C_SDA0,I2C_SCL0);
    // HR
    DigitalOut   resetPin(p26,0);
    DigitalInOut mfioPin(p25,PIN_OUTPUT,PullUp,0);
    //SparkFun_Bio_Sensor_Hub bioHub(p26, p25);//resPin, mfioPin
    SparkFun_Bio_Sensor_Hub bioHub(resetPin, mfioPin);//resPin, mfioPin

    bioData body;
    // --- ACC ---
    DigitalIn   ACC_Int1(p27);
    BMA253 bma(ADDRESS_ONE);
    bmaData accData;
//I2C end

// Initialise the digital pin LD1 as an output
DigitalOut led(p26);
// Read from p0.28 - use jumper to switch from Pot to J4
AnalogIn adc_gsr(p28);

int main()
{
    //ACC start
    int result = bma.begin(i2c);
    bma.setElIntBehaviour(INTERRUPT_PIN_INT1,INTERRUPT_EL_BEHAVIOUR_PUSHPULL);
    result = bma.moveIntSetThreashold(100,0b11);
    result = bma.moveInt(true);

	// get start time
	auto start = Kernel::Clock::now(); // type will be Kernel::Clock::time_point

	SEGGER_RTT_printf(0,"times since start in ms, adc value between 0xFFFF and 0x0\n");
	SEGGER_RTT_printf(0,"outputs in decimals \n");

    while (true) {
	// get elapsed time in milliseconds
	milliseconds elapsed_time = Kernel::Clock::now() - start;
	// print/log timestamp and adc value to RTT
	SEGGER_RTT_printf(0,"%d adc value: %d\n",int(elapsed_time.count()),adc_gsr.read_u16());
    accData = bma.read();
	SEGGER_RTT_printf(0,"%d x value: %d\n",int(elapsed_time.count()),accData.x);
	SEGGER_RTT_printf(0,"%d y value: %d\n",int(elapsed_time.count()),accData.y);
	SEGGER_RTT_printf(0,"%d z value: %d\n",int(elapsed_time.count()),accData.z);
        led = !led;
        ThisThread::sleep_for(BLINKING_RATE);
    }
}
