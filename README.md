# Attiny13 FAN controller

3 work modes with individual parameters of working time (in seconds) and relaxing time (in seconds too);

When changing the parameters of the operating modes, they are saved in the internal memory(EEPROM), therefore, after the device is rebooted -> data on the operating modes are not lost.

PINOUT:

PB3 -> LED;
PB4 -> Button+ Work time;
PB0 -> Button+ Relax time;
PB1 -> Button Change work mode;
PB2 -> Output for motor.
