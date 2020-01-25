#include <bots/cpu.hpp>
#include <bots/ops.hpp>

namespace bots {
    void Cpu::fetch() {
        if(fetch_pc > user_mem_max - 3)
            fetch_pc = 0;

        if(fetch_flag == 0) {
            fetch_flag = 1;

            fetched_pc = fetch_pc;
            fetched_instruction  = memory[(fetch_pc)++] << 24;
            fetched_instruction |= memory[(fetch_pc)++] << 16;
            fetched_instruction |= memory[(fetch_pc)++] << 8;
            fetched_instruction |= memory[(fetch_pc)++];
        }
    }

    void Cpu::decode() {
        if(fetch_flag == 0)
            return;

        if(decode_flag == 0) {
            decode_flag = 1;
            fetch_flag = 0;
            decoded_pc = fetched_pc;
            decoded_opcode = (fetched_instruction >> 28) & 0x0f;
            decoded_flags = (fetched_instruction >> 24) & 0x0f;
            decoded_ra = (fetched_instruction >> 20) & 0x0f;
            decoded_rb = (fetched_instruction >> 16) & 0x0f;
            decoded_imm = fetched_instruction & 0xffff;

            /* sanity check register numbers */
            if(decoded_ra > 11) decoded_ra = 0;
            if(decoded_rb > 11) decoded_rb = 0;
             /* if the instruction uses immediate as register value, check it */
            if(decoded_flags & 0x08)
                if(decoded_imm > 11)
                    decoded_imm = 0;
        }
    }

    void Cpu::execute() {
        if(decode_flag == 0)
            return;

        registers[0] = 0;
        registers[1] = 1;
     
        uint8_t done = (*bots_cpu_oplist[decoded_opcode])(
                *this,
                execute_cycle++,
                decoded_flags,
                decoded_ra,
                decoded_rb,
                decoded_imm
        );

        if(done) {
            decode_flag = 0;
            execute_cycle = 0;
        }
    }

    void Cpu::cycle() {
        execute();
        decode();
        fetch();
    }
}
