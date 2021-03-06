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
LIBS:tp4055
LIBS:tprobe-cache
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
L ATTINY85-P IC1
U 1 1 585E6BD7
P 5800 3850
F 0 "IC1" H 4650 4250 50  0000 C CNN
F 1 "ATTINY85-P" H 6800 3450 50  0000 C CNN
F 2 "Power_Integrations:PDIP-8" H 6800 3850 50  0000 C CIN
F 3 "" H 5800 3850 50  0000 C CNN
	1    5800 3850
	1    0    0    -1  
$EndComp
$Comp
L CONN_02X03 P4
U 1 1 585E6C7E
P 4900 4550
F 0 "P4" H 4900 4750 50  0000 C CNN
F 1 "AVR_ISP" H 4900 4350 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_2x03" H 4900 3350 50  0001 C CNN
F 3 "" H 4900 3350 50  0000 C CNN
	1    4900 4550
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR01
U 1 1 585E6D61
P 7300 3500
F 0 "#PWR01" H 7300 3350 50  0001 C CNN
F 1 "VCC" H 7300 3650 50  0000 C CNN
F 2 "" H 7300 3500 50  0000 C CNN
F 3 "" H 7300 3500 50  0000 C CNN
	1    7300 3500
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR02
U 1 1 585E6D7B
P 7300 4150
F 0 "#PWR02" H 7300 3900 50  0001 C CNN
F 1 "GND" H 7300 4000 50  0000 C CNN
F 2 "" H 7300 4150 50  0000 C CNN
F 3 "" H 7300 4150 50  0000 C CNN
	1    7300 4150
	1    0    0    -1  
$EndComp
Wire Wire Line
	7300 3500 7300 3750
Wire Wire Line
	7300 3950 7300 4150
$Comp
L VCC #PWR03
U 1 1 585E6E34
P 5300 4450
F 0 "#PWR03" H 5300 4300 50  0001 C CNN
F 1 "VCC" H 5300 4600 50  0000 C CNN
F 2 "" H 5300 4450 50  0000 C CNN
F 3 "" H 5300 4450 50  0000 C CNN
	1    5300 4450
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR04
U 1 1 585E6E84
P 5250 4700
F 0 "#PWR04" H 5250 4450 50  0001 C CNN
F 1 "GND" H 5250 4550 50  0000 C CNN
F 2 "" H 5250 4700 50  0000 C CNN
F 3 "" H 5250 4700 50  0000 C CNN
	1    5250 4700
	1    0    0    -1  
$EndComp
Wire Wire Line
	4650 4650 4400 4650
Wire Wire Line
	4400 4650 4400 4100
Wire Wire Line
	4400 4100 4450 4100
Wire Wire Line
	4450 3800 4300 3800
Wire Wire Line
	4300 4550 4650 4550
Wire Wire Line
	4450 3700 4200 3700
Wire Wire Line
	4200 3350 4200 4450
Wire Wire Line
	4200 4450 4650 4450
Wire Wire Line
	5150 4650 5250 4650
Wire Wire Line
	5250 4650 5250 4700
Wire Wire Line
	5150 4450 5300 4450
Wire Wire Line
	5150 4550 5200 4550
Wire Wire Line
	5200 4550 5200 4300
Wire Wire Line
	5200 4300 4100 4300
Wire Wire Line
	4100 3600 4450 3600
$Comp
L Led_Small D2
U 1 1 585E71DF
P 4300 3250
F 0 "D2" H 4350 3300 50  0000 L CNN
F 1 "RED" H 4100 3300 50  0000 L CNN
F 2 "LEDs:LED-3MM" V 4300 3250 50  0001 C CNN
F 3 "" V 4300 3250 50  0000 C CNN
	1    4300 3250
	0    -1   -1   0   
$EndComp
$Comp
L Led_Small D3
U 1 1 585E7209
P 4200 3250
F 0 "D3" H 4250 3300 50  0000 L CNN
F 1 "GRN" H 4000 3300 50  0000 L CNN
F 2 "LEDs:LED-3MM" V 4200 3250 50  0001 C CNN
F 3 "" V 4200 3250 50  0000 C CNN
	1    4200 3250
	0    -1   -1   0   
$EndComp
$Comp
L Led_Small D4
U 1 1 585E7225
P 4100 3250
F 0 "D4" H 4150 3300 50  0000 L CNN
F 1 "YEL" H 3900 3300 50  0000 L CNN
F 2 "LEDs:LED-3MM" V 4100 3250 50  0001 C CNN
F 3 "" V 4100 3250 50  0000 C CNN
	1    4100 3250
	0    -1   -1   0   
$EndComp
Connection ~ 4100 3600
Connection ~ 4200 3700
Connection ~ 4300 3800
$Comp
L R_Small R3
U 1 1 585E7354
P 4300 2850
F 0 "R3" V 4250 2950 50  0000 L CNN
F 1 "1k" V 4250 2650 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" H 4300 2850 50  0001 C CNN
F 3 "" H 4300 2850 50  0000 C CNN
	1    4300 2850
	1    0    0    -1  
$EndComp
$Comp
L R_Small R4
U 1 1 585E73DF
P 4200 2850
F 0 "R4" V 4150 2950 50  0000 L CNN
F 1 "560" V 4150 2600 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" H 4200 2850 50  0001 C CNN
F 3 "" H 4200 2850 50  0000 C CNN
	1    4200 2850
	1    0    0    -1  
$EndComp
$Comp
L R_Small R5
U 1 1 585E7404
P 4100 2850
F 0 "R5" V 4050 2950 50  0000 L CNN
F 1 "560" V 4050 2600 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" H 4100 2850 50  0001 C CNN
F 3 "" H 4100 2850 50  0000 C CNN
	1    4100 2850
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR05
U 1 1 585E7724
P 4100 2550
F 0 "#PWR05" H 4100 2400 50  0001 C CNN
F 1 "VCC" H 4100 2700 50  0000 C CNN
F 2 "" H 4100 2550 50  0000 C CNN
F 3 "" H 4100 2550 50  0000 C CNN
	1    4100 2550
	1    0    0    -1  
$EndComp
Wire Wire Line
	4100 2600 4300 2600
Wire Wire Line
	4200 2600 4200 2750
Connection ~ 4100 2600
Connection ~ 4200 2600
Wire Wire Line
	4200 2950 4200 3150
Wire Wire Line
	3600 4000 4450 4000
$Comp
L R_Small R2
U 1 1 585E7A33
P 3800 4200
F 0 "R2" V 3750 4300 50  0000 L CNN
F 1 "1M" V 3750 4000 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" H 3800 4200 50  0001 C CNN
F 3 "" H 3800 4200 50  0000 C CNN
	1    3800 4200
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR06
U 1 1 585E7B4A
P 3800 4450
F 0 "#PWR06" H 3800 4200 50  0001 C CNN
F 1 "GND" H 3800 4300 50  0000 C CNN
F 2 "" H 3800 4450 50  0000 C CNN
F 3 "" H 3800 4450 50  0000 C CNN
	1    3800 4450
	1    0    0    -1  
$EndComp
Wire Wire Line
	3800 4300 3800 4450
Wire Wire Line
	3800 4100 3800 4000
Connection ~ 3800 4000
$Comp
L R_Small R1
U 1 1 585E7CA4
P 3500 4000
F 0 "R1" V 3450 4100 50  0000 L CNN
F 1 "1k" V 3450 3800 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" H 3500 4000 50  0001 C CNN
F 3 "" H 3500 4000 50  0000 C CNN
	1    3500 4000
	0    1    1    0   
$EndComp
Wire Wire Line
	3250 4000 3400 4000
$Comp
L C_Small C1
U 1 1 585E8548
P 7300 3850
F 0 "C1" H 7310 3920 50  0000 L CNN
F 1 "1u" H 7310 3770 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603_HandSoldering" H 7300 3850 50  0001 C CNN
F 3 "" H 7300 3850 50  0000 C CNN
	1    7300 3850
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X01 P1
U 1 1 585E8B2B
P 3050 4000
F 0 "P1" H 3050 4100 50  0000 C CNN
F 1 "INPUT" V 3150 4000 50  0000 C CNN
F 2 "Wire_Pads:SolderWirePad_single_2mmDrill" H 3050 4000 50  0001 C CNN
F 3 "" H 3050 4000 50  0000 C CNN
	1    3050 4000
	-1   0    0    1   
$EndComp
Wire Wire Line
	4450 3900 3950 3900
Text Label 3950 3900 0    60   ~ 0
DTXRX
Text Label 7800 3750 0    60   ~ 0
DTXRX
$Comp
L CONN_01X06 P5
U 1 1 585E9039
P 8750 3900
F 0 "P5" H 8750 4250 50  0000 C CNN
F 1 "FTDI_CN" V 8850 3900 50  0000 C CNN
F 2 "Socket_Strips:Socket_Strip_Angled_1x06" H 8750 3900 50  0001 C CNN
F 3 "" H 8750 3900 50  0000 C CNN
	1    8750 3900
	1    0    0    -1  
$EndComp
$Comp
L R_Small R7
U 1 1 585E9377
P 8350 3850
F 0 "R7" V 8300 3950 50  0000 L CNN
F 1 "1k" V 8300 3650 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" H 8350 3850 50  0001 C CNN
F 3 "" H 8350 3850 50  0000 C CNN
	1    8350 3850
	0    1    1    0   
$EndComp
Wire Wire Line
	8450 3850 8550 3850
Wire Wire Line
	8250 3850 8100 3850
Wire Wire Line
	8100 3850 8100 3750
Connection ~ 8100 3750
Text Label 4400 4650 0    60   ~ 0
RST
Text Label 7800 3650 0    60   ~ 0
RST
$Comp
L C_Small C2
U 1 1 585E9899
P 8350 3650
F 0 "C2" H 8360 3720 50  0000 L CNN
F 1 "1u" H 8360 3570 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603_HandSoldering" H 8350 3650 50  0001 C CNN
F 3 "" H 8350 3650 50  0000 C CNN
	1    8350 3650
	0    1    1    0   
$EndComp
Wire Wire Line
	8550 3650 8450 3650
Wire Wire Line
	7800 3650 8250 3650
$Comp
L R_Small R6
U 1 1 585E9A00
P 8100 3500
F 0 "R6" V 8050 3600 50  0000 L CNN
F 1 "10k" V 8050 3300 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" H 8100 3500 50  0001 C CNN
F 3 "" H 8100 3500 50  0000 C CNN
	1    8100 3500
	-1   0    0    1   
$EndComp
$Comp
L VCC #PWR07
U 1 1 585E9A92
P 8100 3350
F 0 "#PWR07" H 8100 3200 50  0001 C CNN
F 1 "VCC" H 8100 3500 50  0000 C CNN
F 2 "" H 8100 3350 50  0000 C CNN
F 3 "" H 8100 3350 50  0000 C CNN
	1    8100 3350
	1    0    0    -1  
$EndComp
Wire Wire Line
	8100 3600 8100 3650
Connection ~ 8100 3650
Wire Wire Line
	8100 3350 8100 3400
$Comp
L VCC #PWR08
U 1 1 585E9C31
P 7950 3950
F 0 "#PWR08" H 7950 3800 50  0001 C CNN
F 1 "VCC" H 7950 4100 50  0000 C CNN
F 2 "" H 7950 3950 50  0000 C CNN
F 3 "" H 7950 3950 50  0000 C CNN
	1    7950 3950
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR09
U 1 1 585E9D87
P 8450 4200
F 0 "#PWR09" H 8450 3950 50  0001 C CNN
F 1 "GND" H 8450 4050 50  0000 C CNN
F 2 "" H 8450 4200 50  0000 C CNN
F 3 "" H 8450 4200 50  0000 C CNN
	1    8450 4200
	1    0    0    -1  
$EndComp
Wire Wire Line
	8000 4150 8550 4150
Wire Wire Line
	8450 4050 8450 4200
Wire Wire Line
	8550 4050 8450 4050
Connection ~ 8450 4150
$Comp
L CONN_01X01 P2
U 1 1 585EA418
P 3050 4400
F 0 "P2" H 3050 4500 50  0000 C CNN
F 1 "PRB_GND" V 3150 4400 50  0000 C CNN
F 2 "Wire_Pads:SolderWirePad_single_2mmDrill" H 3050 4400 50  0001 C CNN
F 3 "" H 3050 4400 50  0000 C CNN
	1    3050 4400
	-1   0    0    1   
$EndComp
Wire Wire Line
	3800 4400 3250 4400
Connection ~ 3800 4400
Wire Wire Line
	7300 4100 7150 4100
Connection ~ 7300 4100
Wire Wire Line
	7150 3600 7300 3600
Connection ~ 7300 3600
$Comp
L CONN_01X01 P3
U 1 1 585EC605
P 3050 4750
F 0 "P3" H 3050 4850 50  0000 C CNN
F 1 "PRB_VCC" V 3150 4750 50  0000 C CNN
F 2 "Wire_Pads:SolderWirePad_single_2mmDrill" H 3050 4750 50  0001 C CNN
F 3 "" H 3050 4750 50  0000 C CNN
	1    3050 4750
	-1   0    0    1   
$EndComp
$Comp
L VCC #PWR010
U 1 1 585EC736
P 3550 4700
F 0 "#PWR010" H 3550 4550 50  0001 C CNN
F 1 "VCC" H 3550 4850 50  0000 C CNN
F 2 "" H 3550 4700 50  0000 C CNN
F 3 "" H 3550 4700 50  0000 C CNN
	1    3550 4700
	1    0    0    -1  
$EndComp
$Comp
L D_Schottky_Small D5
U 1 1 585ECFF5
P 8250 3950
F 0 "D5" H 8200 4030 50  0000 L CNN
F 1 "SMD" H 8200 3850 50  0000 L CNN
F 2 "Diodes_SMD:SOD-323_HandSoldering" V 8250 3950 50  0001 C CNN
F 3 "" V 8250 3950 50  0000 C CNN
	1    8250 3950
	1    0    0    -1  
$EndComp
Wire Wire Line
	8550 3950 8350 3950
Wire Wire Line
	8150 3950 7950 3950
$Comp
L D_Schottky_Small D1
U 1 1 585ED38D
P 3400 4750
F 0 "D1" H 3350 4830 50  0000 L CNN
F 1 "SMD" H 3350 4650 50  0000 L CNN
F 2 "Diodes_SMD:SOD-323_HandSoldering" V 3400 4750 50  0001 C CNN
F 3 "" V 3400 4750 50  0000 C CNN
	1    3400 4750
	-1   0    0    1   
$EndComp
Wire Wire Line
	3250 4750 3300 4750
Wire Wire Line
	3500 4750 3550 4750
Wire Wire Line
	3550 4750 3550 4700
$Comp
L R_Small R8
U 1 1 585EE32A
P 8350 3750
F 0 "R8" V 8300 3850 50  0000 L CNN
F 1 "1k" V 8300 3550 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" H 8350 3750 50  0001 C CNN
F 3 "" H 8350 3750 50  0000 C CNN
	1    8350 3750
	0    1    1    0   
$EndComp
Wire Wire Line
	8550 3750 8450 3750
Wire Wire Line
	7800 3750 8250 3750
Wire Wire Line
	4100 2550 4100 2750
Wire Wire Line
	4300 2600 4300 2750
Wire Wire Line
	4300 2950 4300 3150
Wire Wire Line
	4100 2950 4100 3150
Wire Wire Line
	4100 4300 4100 3350
Wire Wire Line
	4300 3350 4300 4550
$Comp
L SW_PUSH_SMALL_H SW1
U 1 1 585EF904
P 8650 2450
F 0 "SW1" H 8730 2560 50  0000 C CNN
F 1 "SW_PUSH_SMALL_H" H 9010 2390 50  0000 C CNN
F 2 "Buttons_Switches_ThroughHole:SW_PUSH_6mm" H 8650 2650 50  0001 C CNN
F 3 "" H 8650 2650 50  0000 C CNN
	1    8650 2450
	0    1    1    0   
$EndComp
$Comp
L D_Schottky_Small D6
U 1 1 585EFA53
P 8300 2250
F 0 "D6" H 8250 2330 50  0000 L CNN
F 1 "SMD" H 8250 2150 50  0000 L CNN
F 2 "Diodes_SMD:SOD-323_HandSoldering" V 8300 2250 50  0001 C CNN
F 3 "" V 8300 2250 50  0000 C CNN
	1    8300 2250
	-1   0    0    1   
$EndComp
$Comp
L GND #PWR011
U 1 1 585EFAF0
P 8650 2700
F 0 "#PWR011" H 8650 2450 50  0001 C CNN
F 1 "GND" H 8650 2550 50  0000 C CNN
F 2 "" H 8650 2700 50  0000 C CNN
F 3 "" H 8650 2700 50  0000 C CNN
	1    8650 2700
	1    0    0    -1  
$EndComp
$Comp
L R_Small R9
U 1 1 585EFDF7
P 8900 2250
F 0 "R9" V 8850 2350 50  0000 L CNN
F 1 "1M" V 8850 2050 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" H 8900 2250 50  0001 C CNN
F 3 "" H 8900 2250 50  0000 C CNN
	1    8900 2250
	0    -1   -1   0   
$EndComp
$Comp
L C_Small C3
U 1 1 585EFF1E
P 8500 2450
F 0 "C3" H 8510 2520 50  0000 L CNN
F 1 "1u" H 8510 2370 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603_HandSoldering" H 8500 2450 50  0001 C CNN
F 3 "" H 8500 2450 50  0000 C CNN
	1    8500 2450
	-1   0    0    1   
$EndComp
$Comp
L VCC #PWR012
U 1 1 585F02E3
P 9150 2250
F 0 "#PWR012" H 9150 2100 50  0001 C CNN
F 1 "VCC" H 9150 2400 50  0000 C CNN
F 2 "" H 9150 2250 50  0000 C CNN
F 3 "" H 9150 2250 50  0000 C CNN
	1    9150 2250
	1    0    0    -1  
$EndComp
Wire Wire Line
	9150 2250 9000 2250
Wire Wire Line
	8650 2600 8650 2700
Wire Wire Line
	8650 2650 8500 2650
Wire Wire Line
	8500 2650 8500 2550
Connection ~ 8650 2650
Wire Wire Line
	8400 2250 8800 2250
Wire Wire Line
	8650 2250 8650 2300
Connection ~ 8650 2250
Wire Wire Line
	8500 2350 8500 2250
Connection ~ 8500 2250
Wire Wire Line
	7850 2250 8200 2250
Text Label 7850 2250 0    60   ~ 0
BTN_OUT
Text Label 8550 3400 0    60   ~ 0
BTN_OUT
Wire Wire Line
	8500 3850 8500 3400
Wire Wire Line
	8500 3400 8550 3400
Connection ~ 8500 3850
Text Label 4450 3600 2    60   ~ 0
MOSI_Y
Text Label 4450 3700 2    60   ~ 0
MISO_G
Text Label 4450 3800 2    60   ~ 0
SCK_R
Text Label 3950 4000 0    60   ~ 0
PROBE
Text Label 3300 4750 1    60   ~ 0
PVCC
Text Label 3300 4000 1    60   ~ 0
PRBR
Text Label 8350 3950 0    60   ~ 0
FVCC
Text Label 8550 3650 0    60   ~ 0
DTR
Text Label 8550 3750 0    60   ~ 0
TX
$Comp
L CONN_01X02 P6
U 1 1 585FD3AE
P 8700 5000
F 0 "P6" H 8700 5150 50  0000 C CNN
F 1 "MLIPO" V 8800 5000 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x02" H 8700 5000 50  0001 C CNN
F 3 "" H 8700 5000 50  0000 C CNN
	1    8700 5000
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR013
U 1 1 585FD4CE
P 8200 5500
F 0 "#PWR013" H 8200 5250 50  0001 C CNN
F 1 "GND" H 8200 5350 50  0000 C CNN
F 2 "" H 8200 5500 50  0000 C CNN
F 3 "" H 8200 5500 50  0000 C CNN
	1    8200 5500
	1    0    0    -1  
$EndComp
$Comp
L D_Schottky_Small D7
U 1 1 585FD612
P 7500 4650
F 0 "D7" H 7350 4700 50  0000 L CNN
F 1 "SMD" H 7450 4550 50  0000 L CNN
F 2 "Diodes_SMD:SOD-323_HandSoldering" V 7500 4650 50  0001 C CNN
F 3 "" V 7500 4650 50  0000 C CNN
	1    7500 4650
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR014
U 1 1 585FD775
P 7050 4650
F 0 "#PWR014" H 7050 4500 50  0001 C CNN
F 1 "VCC" H 7050 4800 50  0000 C CNN
F 2 "" H 7050 4650 50  0000 C CNN
F 3 "" H 7050 4650 50  0000 C CNN
	1    7050 4650
	1    0    0    -1  
$EndComp
$Comp
L R_Small R10
U 1 1 585FD9D3
P 8050 5250
F 0 "R10" V 8000 5350 50  0000 L CNN
F 1 "10k" V 8000 5050 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" H 8050 5250 50  0001 C CNN
F 3 "" H 8050 5250 50  0000 C CNN
	1    8050 5250
	-1   0    0    1   
$EndComp
$Comp
L TP4055 U1
U 1 1 58D7CC94
P 7600 5050
F 0 "U1" H 7300 5300 50  0000 L CNN
F 1 "TP4055" H 7900 5300 50  0000 R CNN
F 2 "TO_SOT_Packages_SMD:SOT-23-5" H 7600 4650 50  0001 C CNN
F 3 "" H 7600 5050 50  0000 C CNN
	1    7600 5050
	1    0    0    -1  
$EndComp
$Comp
L C_Small C4
U 1 1 58D7D41D
P 8350 5250
F 0 "C4" H 8360 5320 50  0000 L CNN
F 1 "1u" H 8360 5170 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603_HandSoldering" H 8350 5250 50  0001 C CNN
F 3 "" H 8350 5250 50  0000 C CNN
	1    8350 5250
	1    0    0    -1  
$EndComp
Wire Wire Line
	8050 5350 8050 5450
Wire Wire Line
	7600 5450 8500 5450
Wire Wire Line
	8350 5450 8350 5350
Wire Wire Line
	8200 5450 8200 5500
Connection ~ 8200 5450
Wire Wire Line
	7600 5450 7600 5350
Connection ~ 8050 5450
Wire Wire Line
	8000 5050 8050 5050
Wire Wire Line
	8050 5050 8050 5150
Wire Wire Line
	8500 5450 8500 5050
Connection ~ 8350 5450
Wire Wire Line
	8500 4950 8000 4950
Wire Wire Line
	8350 5150 8350 4950
Connection ~ 8350 4950
Wire Wire Line
	7050 4950 7200 4950
Wire Wire Line
	7050 4650 7050 4950
Wire Wire Line
	7400 4650 7250 4650
Wire Wire Line
	7250 4650 7250 4750
Wire Wire Line
	7250 4750 7050 4750
Connection ~ 7050 4750
Wire Wire Line
	7600 4650 8050 4650
Wire Wire Line
	8050 4650 8050 4950
Connection ~ 8050 4950
$Comp
L PWR_FLAG #FLG015
U 1 1 58D821B4
P 7700 4050
F 0 "#FLG015" H 7700 4145 50  0001 C CNN
F 1 "PWR_FLAG" H 7700 4230 50  0000 C CNN
F 2 "" H 7700 4050 50  0000 C CNN
F 3 "" H 7700 4050 50  0000 C CNN
	1    7700 4050
	1    0    0    -1  
$EndComp
Wire Wire Line
	7700 4050 8000 4050
Wire Wire Line
	8000 4050 8000 3950
Connection ~ 8000 3950
$Comp
L PWR_FLAG #FLG016
U 1 1 58D828E7
P 7700 4300
F 0 "#FLG016" H 7700 4395 50  0001 C CNN
F 1 "PWR_FLAG" H 7700 4480 50  0000 C CNN
F 2 "" H 7700 4300 50  0000 C CNN
F 3 "" H 7700 4300 50  0000 C CNN
	1    7700 4300
	1    0    0    -1  
$EndComp
Wire Wire Line
	7700 4300 8000 4300
Wire Wire Line
	8000 4300 8000 4150
NoConn ~ 7200 5050
$EndSCHEMATC
