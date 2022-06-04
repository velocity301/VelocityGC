#pragma once
#include "VelocityGC.h"
#include "ButtonDefines.h"
#include "BackendMacros.h"

void VelocityGC::init(){
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

// sets up cpu cycle counter register
void VelocityGC::initCounter(){
    {
        ARM_DEMCR |= ARM_DEMCR_TRCENA;
        ARM_DWT_CTRL |= ARM_DWT_CTRL_CYCCTENA;
    }
}

void VelocityGC::initBtns()
            {
                //initialize A, B, X, Y, S, Dl, Dr, Dd, Du, Z, R, L
                for (int i = 2; i <= 13; i++){
                    pinMode(i, INPUT_PULLUP);
                } 

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
            }

// sets pin modes for gamecube controller data line
void VelocityGC::initLines(){ //TODO: check this shit
                pinMode(LINE, INPUT);
}    
//TODO: Rewrite this to work with analogReadFast if the pin number matches an analog read pin
bool VelocityGC::readBit(){
    while (digitalReadFast(LINE)) { 
        //start of bit starts on falling edge
    }
    markCycle(); //record CPU cycle of falling edge
    while(!digitalReadFast(LINE)) {
        //bit value is determined by how long LINE is held LOW by the console
    }
    return elapsedCycles() < _halfBit;
}

// writes 1 bit to LINE
// (assumes LINE is already OUTPUT)
void VelocityGC::writeBit(bool bt /* bit to be written */){
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

    // read analog buttons
    btn.La = readAnalog(btn_La); 
    btn.Ra = readAnalog(btn_Ra);
    btn.Ax = readAnalog(btn_Ax);
    btn.Ay = readAnalog(btn_Ay);
    btn.Cx = readAnalog(btn_Cx);
    btn.Cy = readAnalog(btn_Cy);

    
}