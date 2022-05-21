#pragma once

// #include other files here

struct Buttons{
    //uint8_t arr[10]; ask about this line probably just for union
    //byte 0
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