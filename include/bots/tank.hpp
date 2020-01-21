#pragma once

namespace bots {
    class Cpu {
        public:
            Cpu();
            ~Cpu();
    };

    class Tank {
        public:
            Tank();
            ~Tank();

            Cpu cpu;
    };
}
