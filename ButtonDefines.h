#pragma once

// data line pin
#define LINE 23

#define readBtn(button) (!digitalReadFast(button))

// screen receive and transmit pins
#define SRX 0
#define STX 1

// 3 groups of 5 buttons
// first pin of each
//#define GL 2	// left group
//#define GM 7	// middle group
//#define GR 14	// right group

// always high pin for unused action binds
//#define HP 19

// BIND YOUR BUTTONS HERE

#define btn_A	(2)
#define btn_B	(3)
#define btn_X	(4)
#define btn_Y	(5)
#define btn_S	(6)
#define btn_Dl	(7) 
#define btn_Dr	(8) 
#define btn_Dd	(9) 
#define btn_Du	(10) 
#define btn_Z	(11)
#define btn_R	(12)
#define btn_L	(13)
#define btn_Ax	(14)  //Analog
#define btn_Ay	(15)  //Analog
#define btn_Cx	(16)  //Analog
#define btn_Cy	(17)  //Analog
#define btn_La	(20)  //Analog
#define btn_Ra	(21)  //Analog
