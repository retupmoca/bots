#pragma once

#include <cstdint>
#include <map>
#include <memory>

namespace bots {
    class Peripheral {
        public:
            virtual void write_word(uint16_t address, uint16_t value) = 0;
            virtual void write_byte(uint16_t address, uint8_t value) = 0;
            virtual uint16_t read_word(uint16_t address) = 0;
            virtual uint8_t read_byte(uint16_t address) = 0;

            virtual void tick() {}

            virtual uint16_t size() = 0;
    };

    class MappedPeripherals {
        public:
            void mount(uint16_t base_address, std::unique_ptr<Peripheral>);

            void write_word(uint16_t address, uint16_t value);
            void write_byte(uint16_t address, uint8_t value);
            uint16_t read_word(uint16_t address);
            uint8_t read_byte(uint16_t address);


            std::map<uint16_t, std::unique_ptr<Peripheral>> peripherals;
    };

    class Cpu {
        public:
            void cycle();
            
            uint16_t registers[12];
            uint8_t memory[65536];
            MappedPeripherals peripherals;
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
}
