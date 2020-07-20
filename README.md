# arm_MBED_ACN52832_GSRLogger
simple RTT Logger for analog input p26 (for SeedStudio Grove GSR Sensor) 
Use mbed os 6.2 or later

GSR Sensor should be configured to stay below 5V.
To configre use GND and 5V from aconno board, meassure output of OP(Pin8) and set potentiometer.
(Be sure to measure on op, otherwise the measured value will be to low. This is due to a 1M Ohm resistor in the output path and the internal resistance of the voltmeter.)
Afterwards connect analog out to P0.26. 

To log the values start JLinkRTTViewer on the connected PC
