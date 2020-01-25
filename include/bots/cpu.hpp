#ifndef BOTS_CPU_H
#define BOTS_CPU_H

#include <cstdint>

namespace bots {
    class MappedPeripherals;

    class Cpu {
        public:
            //void cycle(MappedPeripherals&);
            void cycle();
            
            uint16_t registers[12];
            uint8_t memory[65536];
            uint16_t user_mem_max;

            uint8_t fetch_flag;
            uint16_t fetch_pc;
            uint16_t fetched_pc;
            uint32_t fetched_instruction;

            uint8_t decode_flag;
            uint8_t decoded_pc;
            uint8_t decoded_opcode;
            uint8_t decoded_flags;
            uint8_t decoded_ra;
            uint8_t decoded_rb;
            uint16_t decoded_imm;

            uint8_t execute_cycle;

            uint8_t bot_id;

        private:
            void fetch();
            void decode();
            void execute();
    };

    class Peripheral {

    };

    class MappedPeripherals {

    };
}

#endif
