#pragma once
#include "stdint.h"
#include "BackendMacros.h"
#include "ButtonDefines.h"

struct Buttons{
    // TODO: uint8_t arr[10]; ask about this line probably just for union
    // byte 0
    uint8_t A : 1;
    uint8_t B : 1;
    uint8_t X : 1;
    uint8_t Y : 1;
    uint8_t S : 1;
    uint8_t orig : 1;
    uint8_t errL : 1;
    uint8_t errS : 1;

    //byte 1
    uint8_t Du : 1;
    uint8_t Dd : 1;
    uint8_t Dl : 1;
    uint8_t Dr : 1;
    uint8_t L : 1;
    uint8_t R : 1;
    uint8_t Z : 1;
    uint8_t high : 1;

    //byte 2-7
    uint8_t Ax : 8;
    uint8_t Ay : 8;
    uint8_t Cx : 8;
    uint8_t Cy : 8;
    uint8_t La : 8;
    uint8_t Ra : 8;

    //these bytes are needed to make a response work
    uint8_t magic1 : 8;
    uint8_t magic2 : 8;
}btn;

struct ID {
    uint8_t byt1 : 8;
    uint8_t byt2 : 8;
    uint8_t byt3 : 8;
}id;

struct CMD {
    uint8_t command : 8;
    uint8_t readmode : 8;
    uint8_t rumbleInfo : 8;
}cmd;

class VelocityGC {
    public: 
            void init();
            void readWrite();
    private:
            // sets up cpu cycle counter register make sure to check this for teensy 3.2
            static void initCounter()
            {
                ARM_DEMCR |= ARM_DEMCR_TRCENA;
                ARM_DWT_CTRL |= ARM_DWT_CTRL_CYCCTENA;
            }

           static void initGroup(uint8_t group)
            {
                for (uint8_t i = 0; i < 5; i++) {
                    pinMode((i+group), INPUT_PULLUP); // this needs to be rewritten without groups
                }   
            }
            void initBtns()
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
            static void initLines()
            {
                pinMode(LINE, INPUT);   
            }
            
            static bool readBit()
            {
                while (digitalReadFast(LINE)) {
                    //start of bit starts on falling edge
                }
                markCycle();//record CPU cycle of falling edge
                while(!digitalReadFast(LINE)) {
                    //bit value is determined by how long LINE is held LOW by the console
                    }
                // 1 if LINE was LOW for < halfBit
                // 0 if LINE was LOW for > halfBit
                return elapsedCycles() < _halfBit;
            }
            // writes 1 bit to LINE
            // (assumes LINE is already OUTPUT)
            static void writeBit(bool bt /* bit to be written */)
                {
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
            // writes one controller stop bit to LINE
            // (assumes LINE is already OUTPUT)
            static void writeStop(){
                markCycle();
                digitalWriteFast(LINE,LOW);
                while (elapsedCycles() < _halfBit) {
                    //hold LINE LOW for 1/2 (2us) of the bit
                }
                digitalWriteFast(LINE,HIGH);
                while (elapsedCycles() < _halfBit) {
                    // hold LINE HIGH for 1/2 (2us) of the bit
                }
                return;
                }
            // writes one byte to LINE (MSB order)
            // (assumes LINE is already OUTPUT)
            static void writeByte(uint8_t byt /* byte to be written*/) {
                for (uint8_t i = 0; i < 8; i++) 
                {
			        writeBit(byt & 0x80u);
			        byt <<= 1;
                }
            }
            // waits for almost 2 init cmds + stop bits to ensure
            // the next command is aligned with th efirst command read
            static void align()
            {
                uint16_t buffer=0xffff;
                delay(100);
                while (buffer!=0b0000001000000001) {
                    buffer = (buffer << 1) | readBit();
                }
            }
    public:
        // attempts to read a console command from LINE
        // one cmd will either be 8bits or 24bits (not including stop bit)
        // returns the first byte of the command
        // stores entire command in the cmd struct
        // sets LINE to pinmode INPUT
        static void readCmd();
        static void write();
        static void readBtns();
            };