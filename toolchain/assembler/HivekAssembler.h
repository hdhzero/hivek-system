#ifndef HIVEK_ASSEMBLER_H
#define HIVEK_ASSEMBLER_H

#include <map>
#include <vector>
#include <iostream>
#include "HivekAssemblerDefs.h"

class HivekAssembler {
    private:
        std::map<std::string, Opcode> opcode_map;
        std::map<std::string, int> register_map;
        std::map<std::string, int> type_map;
        std::map<std::string, int> size_map;
        std::map<Opcode, std::string> opcode_str_map;
        std::map<std::string, DirectiveType> dir_map;
        std::map<int, int> opcode_mask;
        std::vector<Code> code;
        std::vector<Instruction> instructions;
        std::vector<Label> labels;
        std::vector<Directive> directives;
        std::vector<char> bytes;
        std::map<std::string, std::vector<char> > sections;

    public:
        HivekAssembler();
        void parse(std::string filename);
        void debug();

    public:
        void add_register(std::string str, int reg);
        void add_opcode(std::string str, Opcode op, OpcodeOp opp, int type, int size);
        bool is_label(std::vector<std::string> vec);
        bool is_predicate(std::string str);
        bool is_directive(std::vector<std::string>& vec);
        bool has_predicate(std::vector<std::string> vec);
        bool is_instruction(std::vector<std::string> vec);
        void split(std::string& str, std::vector<std::string>& vec);
        void set_predicate(Instruction& inst, std::vector<std::string>& vec);
        bool add_label(std::vector<std::string>& vec);
        void get_string(std::string& src, std::string& dst);
        bool add_directive(std::vector<std::string>& vec, std::string& str);
        bool add_instruction(std::vector<std::string>& vec);
        void remove_commentary(std::string& str);
        void calculate_addresses();
        bool empty_line(std::string str);
        int get_ilabel(std::string& str);
        int get_ivalue(std::string& str);
        unsigned long long gen_instruction64(Instruction& inst);
        int gen_instruction32(Instruction& inst);
        int gen_instruction24(Instruction& inst);
        int gen_instruction16(Instruction& inst);
        void gen_data_directive(Directive& dir);
        void print_instruction(Instruction& inst);
        void generate_binimg();
        void write_instruction(Instruction& inst);

        void dump_text(std::string filename);
        void dump_bin(std::string filename);

};

#endif
