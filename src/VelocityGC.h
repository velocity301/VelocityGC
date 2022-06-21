#pragma once
#include "stdint.h"
#include <digitalWriteFast.h>
#include "BackendMacros.h"
#include "ButtonDefines.h"

union Buttons{
    uint8_t arr[10];
    struct {
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
        uint8_t Dl : 1;
        uint8_t Dr : 1;
        uint8_t Dd : 1;
        uint8_t Du : 1;
        uint8_t Z : 1;
        uint8_t R : 1;
        uint8_t L : 1;
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
    };
}btn;

union ID { 
    uint8_t arr3[3] = { 0x09,0x00,0x00}; //last one might be 0x03???? 
    struct {
        uint8_t byt1 : 8;
        uint8_t byt2 : 8;
        uint8_t byt3 : 8;
    };
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
            
            static void initCounter();
            void initBtns();
            static void initLines();
            
            static bool readBit();
            static void writeBit(bool bt /* bit to be written */);
             
            // writes one controller stop bit to LINE
            // (assumes LINE is already OUTPUT)
            static void writeStop();

            // writes one byte to LINE (MSB order)
            // (assumes LINE is already OUTPUT)
            static void writeByte(uint8_t byt /* byte to be written*/);
            static void align();

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