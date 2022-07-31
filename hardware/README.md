# Schematic and Board Layout

The schematic and board layout was developer using DipTrace.

## TODO

Fix some hardware bugs:
1) GND to MC VSS pin 16
2) Smaller pad for inductor L1
3) MC NRESET connect to SWD header pin 10
4) Diode D4 must protect MC from Analog Switch chips, which feedback voltage from their input to VCC
	There is over 1V there, so MC tends to start in BOOT mode. 
