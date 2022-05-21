#pragma once
#include "VelocityGC.h"

void VelocityGC::init(){
    //cli();  //idk what this is
    initCounter();
    initBtns();
    initLines();
    align();
}
void VelocityGC::readWrite(){
    readBtns();
    readCmd();
    write();
}

// sets up cpu cycle counter register make sure to check this for teensy 3.2
void VelocityGC::initCounter(){
    {
        ARM_DEMCR |= ARM_DEMCR_TRCENA;
        ARM_DWT_CTRL |= ARM_DWT_CTRL_CYCCTENA;
    }
}

void VelocityGC::initGroup(uint8_t group){
    {
        for (uint8_t i = 0; i < 5; i++) {
            pinMode((i+group), INPUT_PULLUP); // this needs to be rewritten without groups
        }   
    }
}
void VelocityGC::initBtns()
            {
                initGroup(GL); //needs to be rewritten without groups
                initGroup(GM);
                initGroup(GR);
                btn.high = 0b1u;
                btn.orig = 0b0u;
                btn.errL = 0b0u;
                btn.errS = 0b0u;
                btn.Ax = 0x80u;
                btn.Ay = 0x80u;
                btn.Cx = 0x80u;
                btn.Cy = 0x80u;
                btn.magic1 = 0x02u;
                btn.magic2 = 0x02u;
                pinMode(HP, INPUT_PULLUP);

            }

// sets pin modes for gamecube controller data line
void VelocityGC::initLines(){
                pinMode(LINE, INPUT);   
}    

bool VelocityGC::readBit(){
    while (digitalReadFast(LINE)) { 
        //start of bit starts on falling edge
    }
    markCycle(); //record CPU cycle of falling edge
    while(!digitalReadFast(LINE)) {
        //bit value is determined by how long LINE is held LOW by the console
    }
    return elapsedCycles() < _halfbit;
}

// writes 1 bit to LINE
// (assumes LINE is already OUTPUT)
void VelocityGC::writeBit(bool /* bit to be written */){
    markCycle();// record CPU cycle of the start of the bit
    if (bt) {
        digitalWriteFast(LINE, LOW);
        while (elapsedCycles() < _quarterBit){
            //hold Line Low for 1/4 (1us) of the bit
        }
        digitalWriteFast(LINE, HIGH);
        while (elapsedCycles()< _oneBit) {
            // hold LINE HIGH for the rest of the bit
        }
        return;
        }
    else{
        digitalWriteFast(LINE,LOW);
        while(elapsedCycles() < _threeqBit) {
            // hold LINE LOW for 3/4 (3us) of the bit
        }
        digitalWriteFast(LINE,HIGH);
        while (elapsedCycles() < _oneBit){
            // hold LINE HIGH for the rest of the bit
        }
        return;
    }
}

void VelocityGC::writeStop(){
    markCycle();
    digitalWriteFast(LINE,LOW);
    while (elapsedCycles() < _halfBit) {
        //hold LINE LOW for 1/2 (2us) of the bit
    }
    digitalWriteFast(LINE,HIGH);
    while (elapsedCycles() < _halfBit){
        //hold LINE HIGH for 1/2 (2us) of the bit
    }
    return;
}

// writes one byte to LINE (MSB order)
// (assumes LINE is already OUTPUT)
void VelocityGC::writeByte(uint8_t byt /* byte to be written*/) {
    for (uint8_t i = 0; i < 8; i++) 
    {
        writeBit(byt & 0x80u);
        byt <<= 1;
    }
}

void VelocityGC::align(){
    uint16_t buffer=0xffff;
    delay(100);
    while (buffer!=0b0000001000000001) {
        buffer = (buffer << 1) | readBit();
    }
}

// attempts to read a console command from LINE
// one cmd will either be 8bits or 24bits (not including stop bit)
// returns the first byte of the command
// stores entire command in the cmd struct
// sets LINE to pinmode INPUT
void VelocityGC::readCmd(){
    pinMode(LINE, INPUT);
		for (uint8_t i = 0; i < 8; i++) {
			cmd.command = (cmd.command << 1) | readBit();
		}
		if (cmd.command==0x41||cmd.command==0x00||cmd.command == 0xff) {			// stop bit
			readBit();			// stop bit
			return;
		}
		else {
			for (uint8_t i = 0; i < 8; i++) {
				cmd.readmode = (cmd.readmode << 1) | readBit();
			}
			for (uint8_t i = 0; i < 8; i++) {
				cmd.rumbleInfo = (cmd.rumbleInfo << 1) | readBit();
			}
			readBit();			// stop bit
			return;
		}
}

// writes a response to LINE based on the contents of cmd
void VelocityGC::write(){
    pinMode(LINE, OUTPUT);

		// read cmd
		if (cmd.command == 0x40) {
			for (uint8_t i = 0; i < 8; i++) {
				writeByte(btn.arr[i]);
			}
			writeStop();
		}

		// origin cmd
		else if (cmd.command == 0x41) {
			btn.orig = 0b0u;
			for (uint8_t i = 0; i < 10; i++) {
				writeByte(btn.arr[i]);
			}
			writeStop();
		}

		// init / reset cmd
		else if (cmd.command == 0x00 || cmd.command == 0xff) {
			for (uint8_t i = 0; i < 3; i++) {
				writeByte(id.arr3[i]);
			}
			writeStop();
		}
		pinMode(LINE, INPUT);
	}

// reads button state and stores in btn struct/array
// assumes all button pinmodes are INPUT_PULLUP
void VelocityGC::readBtns(){
    // digital buttons
    btn.A = readBtn(btn_A);
    btn.B = readBtn(btn_B);
    btn.X = readBtn(btn_X);
    btn.Y = readBtn(btn_Y);
    btn.L = readBtn(btn_L);
    btn.R = readBtn(btn_R);
    btn.Z = readBtn(btn_Z);
    btn.S = readBtn(btn_S);
    btn.Du = readBtn(btn_Du);
    btn.Dd = readBtn(btn_Dd);
    btn.Dl = readBtn(btn_Dl);
    btn.Dr = readBtn(btn_Dr);

    // trigger light press DAC
    btn.La = readBtn(btn_La) ? 0x80u : 0x00u;
    btn.Ra = readBtn(btn_Ra) ? 0x80u : 0x00u;

    // Analog stick X axis
    if (readBtn(btn_Al)) {
        btn.Ax = readBtn(btn_Ar) ? 0x80u : 0x00u;
    }
    else if (readBtn(btn_Ar)) {
        btn.Ax = 0xffu;
    }
    else {
        btn.Ax = 0x80u;
    }
    
    // Analog stick Y axis
    if (readBtn(btn_Ad)) {
        btn.Ay = readBtn(btn_Au) ? 0x80u : 0x00u;
    }
    else if (readBtn(btn_Au)) {
        btn.Ay = 0xffu;
    }
    else {
        btn.Ay = 0x80u;
    }

    // C stick X axis
    if (readBtn(btn_Cl)) {
        btn.Cx = readBtn(btn_Cr) ? 0x80u : 0x00u;
    }
    else if (readBtn(btn_Cr)) {
        btn.Cx = 0xffu;
    }
    else {
        btn.Cx = 0x80u;
    }

    // C stick Y axis
    if (readBtn(btn_Cd)) {
        btn.Cy = readBtn(btn_Cu) ? 0x80u : 0x00u;
    }
    else if (readBtn(btn_Cu)) {
        btn.Cy = 0xffu;
    }
    else {
        btn.Cy = 0x80u;
    }
}