#pragma once

#include <bots/cpu.hpp>

namespace bots {
    class World;

    class BotPeripheral : public Peripheral {
        public:
            Bot &bot;

            BotPeripheral(Bot &bot): bot(bot) {}
    };

    class ResetPeripheral : public BotPeripheral {
        void write_byte(uint16_t address, uint8_t value);
        void write_word(uint16_t address, uint16_t value) { write_word(address, value & 0xff); }
        uint16_t read_word(uint16_t address) { return 0; }
        uint8_t read_byte(uint16_t address) { return 0; }

        uint16_t size() { return 1; }

        using BotPeripheral::BotPeripheral;
    };
    class RadarPeripheral : public BotPeripheral {
        void write_byte(uint16_t address, uint8_t value);
        void write_word(uint16_t address, uint16_t value);
        uint16_t read_word(uint16_t address);
        uint8_t read_byte(uint16_t address);

        void tick();

        uint16_t size() { return 0x13; }

        using BotPeripheral::BotPeripheral;

        enum class Mem {
            result_offset = 0x00,
            result_range = 0x02,
            keepshift = 0x04,
            arc = 0x05,
            range = 0x06,
            scan = 0x08,
            steering = 0x09,
            target_offset = 0x11
        };

        uint16_t arc;
        uint16_t range;
        uint8_t scan;

        uint16_t result_range;
        uint16_t result_offset;
    };
    class TurretPeripheral : public BotPeripheral {
        void write_byte(uint16_t address, uint8_t value);
        void write_word(uint16_t address, uint16_t value);
        uint16_t read_word(uint16_t address);
        uint8_t read_byte(uint16_t address);

        void tick();

        uint16_t size() { return 0x06; }

        using BotPeripheral::BotPeripheral;

        enum class Mem {
            fire = 0x00,
            keepshift = 0x01,
            steering = 0x02,
            target_offset = 0x04
        };

        uint8_t fire;
    };
    class HullPeripheral : public BotPeripheral {
        void write_byte(uint16_t address, uint8_t value);
        void write_word(uint16_t address, uint16_t value);
        uint16_t read_word(uint16_t address);
        uint8_t read_byte(uint16_t address);

        uint16_t size() { return 0x03; }

        using BotPeripheral::BotPeripheral;

        enum class Mem {
            throttle = 0x00,
            steering = 0x01,
        };
    };
}
