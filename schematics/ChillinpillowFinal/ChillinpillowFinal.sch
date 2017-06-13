EESchema Schematic File Version 2
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:ChillinpillowFinal-cache
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L Screw_Terminal_1x02 J1
U 1 1 58F9336E
P 3700 2900
F 0 "J1" H 3700 3150 50  0000 C TNN
F 1 "Screw_Terminal_1x02" V 3550 2900 50  0000 C TNN
F 2 "" H 3700 2675 50  0001 C CNN
F 3 "" H 3675 2900 50  0001 C CNN
	1    3700 2900
	1    0    0    -1  
$EndComp
$Comp
L TL081 U1
U 1 1 58F93396
P 5700 2900
F 0 "U1" H 5700 3150 50  0000 L CNN
F 1 "TL081" H 5700 3050 50  0000 L CNN
F 2 "" H 5750 2950 50  0001 C CNN
F 3 "" H 5850 3050 50  0001 C CNN
	1    5700 2900
	1    0    0    -1  
$EndComp
Wire Wire Line
	5600 2600 3900 2600
Wire Wire Line
	3900 2600 3900 2800
Wire Wire Line
	3900 3000 3900 3300
Wire Wire Line
	3900 3300 5600 3300
Wire Wire Line
	5600 3300 5600 3200
$Comp
L Earth #PWR01
U 1 1 58F933DE
P 5300 3100
F 0 "#PWR01" H 5300 2850 50  0001 C CNN
F 1 "Earth" H 5300 2950 50  0001 C CNN
F 2 "" H 5300 3100 50  0001 C CNN
F 3 "" H 5300 3100 50  0001 C CNN
	1    5300 3100
	1    0    0    -1  
$EndComp
Wire Wire Line
	5400 3000 5300 3000
Wire Wire Line
	5300 3000 5300 3100
$Comp
L Screw_Terminal_1x01 J2
U 1 1 58F93401
P 5200 2800
F 0 "J2" H 5200 2950 50  0000 C TNN
F 1 "Screw_Terminal_1x01" V 5050 2800 50  0000 C TNN
F 2 "" H 5200 2675 50  0001 C CNN
F 3 "" H 5200 2700 50  0001 C CNN
	1    5200 2800
	1    0    0    -1  
$EndComp
$Comp
L Screw_Terminal_1x01 J3
U 1 1 58F93430
P 6200 2900
F 0 "J3" H 6200 3050 50  0000 C TNN
F 1 "Screw_Terminal_1x01" V 6050 2900 50  0000 C TNN
F 2 "" H 6200 2775 50  0001 C CNN
F 3 "" H 6200 2800 50  0001 C CNN
	1    6200 2900
	-1   0    0    -1  
$EndComp
$EndSCHEMATC
