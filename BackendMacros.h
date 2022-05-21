#pragma once
// one bit takes 4us to read/write
// quarterBit = number of CPU cycles in 1us

constexpr uint32_t _quarterBit = F_CPU / 1000000;	// 1/4 bit takes this number of
							                    // cycles to read/write
constexpr uint32_t _halfBit = _quarterBit * 2;	    // 1/2 bit takes this number of
                                                // cycles to read/write
constexpr uint32_t _threeqBit = _quarterBit * 3;	    // 3/4 bit takes this number of
                                                // cycles to read/write
constexpr uint32_t _oneBit = _quarterBit * 4;	    // 1 bit takes this number of
                                                // cycles to read/write
constexpr uint32_t _twoBit = _oneBit * 2;		    // 2 bits take this number of
                                                // cycles to read/write
uint32_t _tempCycle;					                // temp var to store CPU cycle

							                    // record the current CPU cycle
							                    // (ARM_DWT_CYCCNT) = CPU Cycle count Register
#define markCycle() (_tempCycle = (ARM_DWT_CYCCNT))

							                    // number of CPU cycles elapsed
							                    // since markCycle() was recorded
#define elapsedCycles() ((ARM_DWT_CYCCNT) - _tempCycle)