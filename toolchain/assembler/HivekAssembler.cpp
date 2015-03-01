#include <iostream>
#include <sstream>
#include <fstream>

#include "HivekAssembler.h"

bool HivekAssembler::is_label(std::vector<std::string> vec) {
    return vec[0][vec[0].size() - 1] == ':';
}

bool HivekAssembler::is_predicate(std::string str) {
    return str[0] == '(';
}

bool HivekAssembler::is_directive(std::vector<std::string>& vec) {
    return vec[0][0] == '.';
}

bool HivekAssembler::has_predicate(std::vector<std::string> vec) {
    return is_predicate(vec[0]);
}

bool HivekAssembler::is_instruction(std::vector<std::string> vec) {
    return opcode_map.count(vec[0]) > 0 || has_predicate(vec);
}

void HivekAssembler::split(std::string& str, std::vector<std::string>& vec) {
    std::stringstream ss(str);
    std::string tmp;

    vec.clear();

    while (ss >> tmp) {
        vec.push_back(tmp);
    }
}

void HivekAssembler::set_predicate(Instruction& inst, std::vector<std::string>& vec) {
    if (has_predicate(vec)) {
        if (vec[0][1] == '!') {
            inst.predicate_value = 0;
            inst.predicate_register = vec[0][3] - '0';
        } else {
            inst.predicate_value = 1;
            inst.predicate_register = vec[0][2] - '0';
        }

        vec.erase(vec.begin());
    } else {
        inst.predicate_value = 1;
        inst.predicate_register = 0;
    }
}

bool HivekAssembler::add_label(std::vector<std::string>& vec) { 
    Label label;
    Code c;
    bool error = false;

    label.label = vec[0];
    labels.push_back(label);

    c.kind = 1;
    c.index = labels.size() - 1;
    code.push_back(c);

    return error;
}

void HivekAssembler::get_string(std::string& src, std::string& dst) {
    int pos = src.find('"') + 1;

    while (src[pos] != '"') {
        if (src[pos] == '\\') {
            ++pos;
        }

        dst.push_back(src[pos]);
        ++pos;
    }
}

bool HivekAssembler::add_directive(std::vector<std::string>& vec, std::string& str){
    Directive dir;
    Code c;
    bool error = false;

    if (vec[0].compare(".include") == 0) {
std::cout << "ahsduahda\n";
        int pos = str.find('"');
        std::string tmp;
        get_string(str, tmp);
        parse(tmp);
        return error;
    }

    // .asciiz
    if (vec[0].compare(".asciiz") == 0) {
        int pos = str.find('"');
        std::string tmp;

        get_string(str, tmp);
        vec[1] = tmp;
    }
     
    dir.data = vec;
    if (dir_map.count(vec[0]) <= 0) {
        error = true;
        dir.kind = -1;
    } else {
        dir.kind = dir_map[vec[0]];
    }

    directives.push_back(dir);

    c.kind = 2;
    c.index = directives.size() - 1;
    code.push_back(c);

    return error;
}

bool HivekAssembler::add_instruction(std::vector<std::string>& vec) {
    Instruction inst;
    Code c;
    bool error = false;

    set_predicate(inst, vec);

    inst.type = type_map[vec[0]];
    inst.opcode = opcode_map[vec[0]];
    inst.size = size_map[vec[0]];
    inst.parallel = false;

    if (instructions.size() > 0) {
        bool flag = vec[vec.size() - 1][0] == ';';
        instructions[instructions.size() - 1].parallel = flag;
    }

    switch (inst.type) {
        case 1:
            inst.rb = register_map[vec[1]];
            inst.ra = register_map[vec[2]];
            inst.immediate = vec[3];
            break;

        case 2:
            inst.immediate = vec[1];
            break;

        case 3:
            if (vec[0].compare("shadd") == 0) {
                inst.shadd_ammount = get_ivalue(vec[5]);
                inst.rc = register_map[vec[1]];
                inst.ra = register_map[vec[2]];
                inst.rb = register_map[vec[3]];

                if (vec[4].compare("sll") == 0) {
                    inst.shadd_type = SLL;
                } else if (vec[4].compare("srl") == 0) {
                    inst.shadd_type = SRL;
                } else {
                    inst.shadd_type = SRA;
                }
            } else if (vec[0].compare("jr") == 0 || vec[0].compare("jalr") == 0) {
                inst.rc = register_map["r31"];
                inst.ra = register_map[vec[1]];
                inst.rb = register_map["r0"];
            } else {
                inst.rc = register_map[vec[1]];
                inst.ra = register_map[vec[2]];
                inst.rb = register_map[vec[3]];
            }

            break;

        case 4:
            inst.immediate = vec[1];
            break;

        case 5:
            inst.ra = register_map[vec[1]];
            inst.immediate = vec[2];
            break;

        case 6:
            inst.rb = register_map[vec[1]];
            inst.ra = register_map[vec[2]];
            inst.immediate = vec[3];
            break;
            
    }

    instructions.push_back(inst);

    c.kind = 0;
    c.index = instructions.size() - 1;
    code.push_back(c);

    return error;
}

void HivekAssembler::remove_commentary(std::string& str) {
    int i;
    int sz = str.size();
    bool flag = false;

    for (i = 0; i < sz; ++i) {
        if (str[i] == '#') {
            flag = true;
            break;
        }
    }

    if (flag) {
        str.erase(i, sz - i);
    }
}

bool HivekAssembler::empty_line(std::string str) {
    if (str.size() == 0) return true;

    for (int i = 0; i < str.size(); ++i) {
        if (str[i] != ' ' && str[i] != '\t') {
            return false;
        }
    }

    return true;
}

int HivekAssembler::get_ilabel(std::string& str) {
    for (int i = 0; i < labels.size(); ++i) {
        if (str.compare(labels[i].label) == 0) {
            return labels[i].address;
        }
    }

    return 1234567;
}

void HivekAssembler::calculate_addresses() {
    int address = 0;
    int kind;
    int index;
    int tmp;

    for (int i = 0; i < code.size(); ++i) {
        index = code[i].index;

        if (code[i].kind == 0) {
            instructions[index].address = address;
            address += instructions[index].size;
        } else if (code[i].kind == 1) {
            labels[index].address = address;
        } else if (code[i].kind == 2) {
            std::stringstream ss;
            kind = directives[index].kind;
            directives[index].address = address;

            switch (kind) {
                case ASCII:
                    address += directives[index].data[1].size();
                    break;

                case ASCIIZ:
                    address += directives[index].data[1].size() + 1;
                    break;

                case H8:
                case C8:
                case D8:
                case U8:
                    address += (directives[index].data.size() - 1);
                    break;

                case H16:
                case D16:
                case U16:
                    address += (directives[index].data.size() - 1) * 2;
                    break;

                case H32:
                case D32:
                case U32:
                    address += (directives[index].data.size() - 1) * 4;
                    break;

                case R8:
                    ss << directives[index].data[1];
                    ss >> tmp;
                    address += tmp;
                    break; 

                case R16:
                    ss << directives[index].data[1];
                    ss >> tmp;
                    address += tmp * 2;
                    break; 

                case R32:
                    ss << directives[index].data[1];
                    ss >> tmp;
                    address += tmp * 4;
                    break; 

                default:
                    break;
            }
        }
    }
}

int HivekAssembler::get_ivalue(std::string& str) {
    int res = 0;
    int sz = str.size();

    //0x 0b 0o
    if (sz > 2 && str[0] == '0' && str[1] == 'x') {
        for (int i = 2; i < sz; ++i) {
            if (str[i] >= '0' && str[i] <= '9') {
                res = (res << 4) | ((str[i] - '0') & 0xF);
            } else if (str[i] >= 'A' && str[i] <= 'F') {
                res = (res << 4) | ((str[i] - 'A' + 10) & 0xF);
            } else if (str[i] >= 'a' && str[i] <= 'f') {
                res = (res << 4) | ((str[i] - 'a' + 10) & 0xF);
            }
        }
    } else if (sz > 2 && str[0] == '0' && str[1] == 'b') {
        for (int i = 2; i < sz; ++i) {
            res = (res << 1) | ((str[i] - '0') & 0x1);
        }
    } else if (sz > 2 && str[0] == '0' && str[1] == 'o') {
        for (int i = 2; i < sz; ++i) {
            res = (res << 3) | ((str[i] - '0') & 0x7);
        }
    } else if (sz > 0 && str[0] >= '0' && str[0] <= '9') {
        for (int i = 0; i < sz; ++i) {
            res = res * 10 + str[i] - '0';
        }
    } else if (sz > 1 && str[0] == '-') {
        for (int i = 1; i < sz; ++i) {
            res = res * 10 + str[i] - '0';
        }

        res = -res;
    } else if (sz > 2 && str[0] == '%' && str[1] == 'h') { //%hi32(asdad)
        int nbits = (str[3] - '0') * 10 + (str[4] - '0');
        int mask = (~0 >> (32 - nbits)) << (32 - nbits);

        std::string tmp = str.substr(6, str.size() - 7);

        if (tmp[0] >= '0' && tmp[0] <= '9') {
            res = mask & get_ivalue(tmp);
        } else {
            tmp.push_back(':');
            res = mask & get_ilabel(tmp);
        }

        res >>= (32 - nbits);
    } else if (sz > 2 && str[0] == '%' && str[1] == 'l') {
        int nbits = (str[3] - '0') * 10 + (str[4] - '0');
        int mask = (~0 << (32 - nbits)) >> (32 - nbits);
        std::string tmp = str.substr(6, str.size() - 7);

        if (tmp[0] >= '0' && tmp[0] <= '9') {
            res = mask & get_ivalue(tmp);
        } else {
            tmp.push_back(':');
            res = mask & get_ilabel(tmp);
        }
    } else { // it is a label
        std::string tmp = str;
        tmp.push_back(':');

        res = get_ilabel(tmp);
    }

    return res;
}

void HivekAssembler::parse(std::string filename) {
    std::string str;
    std::ifstream ifile;
    std::vector<std::string> vec;
    int line = 0;
    bool error;

    ifile.open(filename.c_str());

    while (ifile.good()) {
        line += 1; 
        getline(ifile, str);
        if (! ifile.good()) break;

        split(str, vec);

        remove_commentary(str);
        if (empty_line(str)) continue;

        if (is_label(vec)) {
            error = add_label(vec);
        } else if (is_instruction(vec)) {
            error = add_instruction(vec);
        } else if (is_directive(vec)) {
            error = add_directive(vec, str);
        }

        if (error) {
            std::cout << filename << ": error on line " << line << std::endl;
        }
    }

    ifile.close();
}

void HivekAssembler::gen_data_directive(Directive& dir) {
    switch (dir.kind) {
        case ASCII:
            for (int i = 0; i < dir.data[1].size(); ++i) {
                bytes.push_back(dir.data[1][i]);
            }

            break;

        case ASCIIZ:
            for (int i = 0; i < dir.data[1].size(); ++i) {
                bytes.push_back(dir.data[1][i]);
            }

            bytes.push_back(0);
            break;

        case H8:
        case C8:
        case D8:
        case U8:
            for (int i = 1; i < dir.data.size(); ++i) { 
                std::string tmp = dir.data[i];
                int tmp2;

                if (tmp[tmp.size() - 1] == ',') {
                    tmp.erase(tmp.size() - 1);
                }

                tmp2 = get_ivalue(tmp);
                bytes.push_back(tmp2 & 0x0FF);
            }

            break;

        case H16:
        case D16:
        case U16:
            for (int i = 1; i < dir.data.size(); ++i) { 
                std::string tmp = dir.data[i];
                int tmp2;

                if (tmp[tmp.size() - 1] == ',') {
                    tmp.erase(tmp.size() - 1);
                }

                tmp2 = get_ivalue(tmp);
                bytes.push_back(tmp2 >> 8);
                bytes.push_back(tmp2 & 0x0FF);
            }

            break;

        case H32:
        case D32:
        case U32:
            for (int i = 1; i < dir.data.size(); ++i) { 
                std::string tmp = dir.data[i];
                int tmp2;

                if (tmp[tmp.size() - 1] == ',') {
                    tmp.erase(tmp.size() - 1);
                }

                tmp2 = get_ivalue(tmp);
                bytes.push_back(tmp2 >> 24);
                bytes.push_back(tmp2 >> 16);
                bytes.push_back(tmp2 >> 8);
                bytes.push_back(tmp2 & 0x0FF);
            }

            break;

        default:
            break;

    }
}

int HivekAssembler::gen_instruction16(Instruction& inst) {
    return 0;
}

int HivekAssembler::gen_instruction24(Instruction& inst) {
    int bin = 0;
    int parallel_mask = inst.parallel ? (1 << 23) : 0;
    int rc = inst.rc << 10;
    int rb = inst.rb << 5;
    int ra = inst.ra;

    bin = parallel_mask | opcode_mask[inst.opcode] | rc | rb | ra;

    return bin;
}

int HivekAssembler::gen_instruction32(Instruction& inst) {
    int bin = 0;
    int parallel_mask = inst.parallel ? (1 << 31) : 0;
    int rc  = inst.rc << 13;
    int rb  = inst.rb << 8;
    int ra  = inst.ra << 3;
    int ra4 = inst.ra << 19;
    int predicate = (inst.predicate_value << 2) | inst.predicate_register;
    int immediate = get_ivalue(inst.immediate);
    int target_addr = immediate - inst.address;
    int immd10 = (immediate & 0x03FF) << 13;
    int immd16 = (immediate & 0x0FFFF) << 3;
    int immd25 = (immediate & 0x01FFFFFF);
    int shamnt = inst.shadd_ammount << 18;
    int sll = 1 << 23;
    int srl = 2 << 23;
    int sra = 3 << 23;

    int typeI;
    int typeII;
    int typeIII;
    int typeIV;
    int typeV;

    switch (inst.type) {
        case 1:
            typeI = parallel_mask | immd10 | rb | ra | predicate;
            bin = typeI | opcode_mask[inst.opcode];
            break;

        case 2:
            if (inst.immediate[0] < '0' || inst.immediate[0] > '9') {
                immd25 = (target_addr & 0x01FFFFFF);
            }

            typeII = parallel_mask | immd25;
            bin = typeII | opcode_mask[inst.opcode];
            break;

        case 3:
            typeIII = parallel_mask | rc | rb | ra | predicate;

            if (inst.opcode != SHADD) {
                bin = typeIII | opcode_mask[inst.opcode];
            } else {
                bin = opcode_mask[SHADD] | typeIII | shamnt;

                if (inst.shadd_type == SLL) {
                    bin |= sll;
                } else if (inst.shadd_type == SRL) {
                    bin |= srl;
                } else if (inst.shadd_type == SRA) {
                    bin |= sra;
                }
            }

            break;

        case 4:
            if (inst.immediate[0] < '0' || inst.immediate[0] > '9') {
                immd16 = (target_addr & 0x0FFFF) << 3;
            }

            typeIV = parallel_mask | immd16 | predicate;
            bin = typeIV | opcode_mask[inst.opcode];
            break;

        case 5:
            typeV = parallel_mask | ra4 | immd16 | predicate;
            bin = typeV | opcode_mask[inst.opcode];
            break;
    }

    return bin;
}

unsigned long long HivekAssembler::gen_instruction64(Instruction& inst) {
    unsigned long long bin = 0;
    int rb = inst.rb << 8;
    int ra = inst.ra << 3;
    int predicate = (inst.predicate_value << 2) | inst.predicate_register;
    int immediate = get_ivalue(inst.immediate);

    bin = opcode_mask[inst.opcode] | rb | ra | predicate;
    bin <<= 32;
    bin |= (immediate & 0x0FFFFFFFF);

    return bin;
}

void HivekAssembler::write_instruction(Instruction& inst) {
    int bin;

    if (inst.size == 8) {
        unsigned long long bin64 = gen_instruction64(inst);

        bytes.push_back(bin64 >> (64 - 8));
        bytes.push_back(bin64 >> (64 - 16));
        bytes.push_back(bin64 >> (64 - 24));
        bytes.push_back(bin64 >> (64 - 32));
        bytes.push_back(bin64 >> (64 - 40));
        bytes.push_back(bin64 >> (64 - 48));
        bytes.push_back(bin64 >> (64 - 56));
        bytes.push_back(bin64 >> (64 - 64));
    } else if (inst.size == 4) {
        bin = gen_instruction32(inst);

        bytes.push_back(bin >> 24);
        bytes.push_back(bin >> 16);
        bytes.push_back(bin >> 8);
        bytes.push_back(bin);
    } else if (inst.size == 3) {
        bin = gen_instruction24(inst);

        bytes.push_back(bin >> 16);
        bytes.push_back(bin >> 8);
        bytes.push_back(bin);
    } else if (inst.size == 2) {
        bin = gen_instruction16(inst);

        bytes.push_back(bin >> 8);
        bytes.push_back(bin);
    }
}

void HivekAssembler::generate_binimg() {
    int index;
    int inst;
    bool flag;
    
    calculate_addresses();

    for (int i = 0; i < code.size(); ++i) {
        index = code[i].index;

        if (code[i].kind == 0) {
            write_instruction(instructions[index]);
        } else if (code[i].kind == 2) {
            gen_data_directive(directives[index]);
        }
    }
}

void HivekAssembler::dump_text(std::string filename) {
    std::ofstream file;
    char c;

    file.open(filename.c_str());

    for (int i = 0; i < bytes.size(); ++i) {
        c = bytes[i];

        for (int j = 7; j >= 0; --j) {
            if (c & (1 << j)) {
                file << "1";
            } else {
                file << "0";
            }
        }

        file << '\n';
    }

    file.close();
}

void HivekAssembler::dump_bin(std::string filename) {
    std::ofstream file;
    char c;

    file.open(filename.c_str());

    for (int i = 0; i < bytes.size(); ++i) {
        file << bytes[i];
    }

    file.close();
}

void HivekAssembler::add_register(std::string str, int reg) {
    register_map[str] = reg;
    str += ",";
    register_map[str] = reg;
}

void HivekAssembler::add_opcode(
    std::string str, Opcode op, OpcodeOp opp, int type, int size) {

    opcode_map[str] = op;
    type_map[str] = type;
    size_map[str] = size;
    opcode_str_map[op] = str;

    switch (type) {
        case 1:
            opcode_mask[op] = TYPE_I_MASK | (opp << 23);
            break;

        case 2:
            if (op == J) {
                opcode_mask[op] = TYPE_II_MASK;
            } else {
                opcode_mask[op] = TYPE_II_MASK | (1 << 25);
            }

            break;

        case 3:
            opcode_mask[op] = TYPE_III_MASK | (opp << 18);
            break;

        case 4:
            if (op == BN) {
                opcode_mask[op] = TYPE_IV_MASK;
            } else {
                opcode_mask[op] = TYPE_IV_MASK | (1 << 19);
            }

            break;

        case 5:
            opcode_mask[op] = TYPE_V_MASK | (opp << 23);
            break;

        case 6:
            opcode_mask[op] = TYPE_VI_MASK | (opp << 13);
            break;
    }
}

HivekAssembler::HivekAssembler() {
    dir_map[".ascii"] = ASCII; dir_map[".asciiz"] = ASCIIZ;
    dir_map[".h8"] = H8; dir_map[".h16"] = H16; dir_map[".h32"] = H32;
    dir_map[".d8"] = D8; dir_map[".d16"] = D16; dir_map[".d32"] = D32;
    dir_map[".u8"] = U8; dir_map[".u16"] = U16; dir_map[".u32"] = U32;
    dir_map[".r8"] = R8; dir_map[".r16"] = R16; dir_map[".r32"] = R32;
    dir_map[".c8"] = C8; dir_map[".incbin"] = INCBIN;
    
    // arith
    add_opcode("add", ADD, OP_ADD, 3, 4); add_opcode("add.m", ADDM, OP_ADD, 5, 3);
    add_opcode("sub", SUB, OP_SUB, 3, 4); add_opcode("sub.m", SUBM, OP_SUB, 5, 3);

    // logical
    add_opcode("and", AND, OP_AND, 3, 4); add_opcode("and.m", ANDM, OP_AND, 5, 3);
    add_opcode("or", OR, OP_OR, 3, 4);    add_opcode("or.m", ORM, OP_OR, 5, 3);
    add_opcode("nor", NOR, OP_NOR, 3, 4); add_opcode("nor.m", NORM, OP_NOR, 5, 3);
    add_opcode("xor", XOR, OP_XOR, 3, 4); add_opcode("xor.m", XORM, OP_XOR, 5, 3);

    // comparison
    add_opcode("eq", EQ, OP_EQ, 3, 4);    add_opcode("eq.m", EQM, OP_EQ, 5, 3);
    add_opcode("lt", LT, OP_LT, 3, 4);    add_opcode("lt.m", LTM, OP_LT, 5, 3);
    add_opcode("ltu", LTU, OP_LTU, 3, 4); add_opcode("ltu.m", LTUM, OP_LTU, 5, 3);

    // shift
    add_opcode("sll", SLL, OP_SLL, 3, 4); add_opcode("sll.m", SLLM, OP_SLL, 5, 3);
    add_opcode("srl", SRL, OP_SRL, 3, 4); add_opcode("srl.m", SRLM, OP_SRL, 5, 3);
    add_opcode("sra", SRA, OP_SRA, 3, 4); add_opcode("sra.m", SRAM, OP_SRA, 5, 3);
    add_opcode("shadd", SHADD, OP_SHADD, 3, 4);

    // jumps
    add_opcode("jr", JR, OP_JR, 3, 4); 
    add_opcode("jr.m", JRM, OP_JR, 5, 3);
    add_opcode("jalr", JALR, OP_JALR, 3, 4); 
    add_opcode("jalr.m", JALRM, OP_JALR, 5, 3);

    //////////
    // Immd //
    //////////

    // lui, lli
    add_opcode("lui", LUI, OP_LUI, 5, 4);
    add_opcode("lli", LLI, OP_LLI, 5, 4);

    // arith
    add_opcode("addi", ADDI, OP_ADDI, 1, 4);
    add_opcode("addi.l", ADDI_L, OP_ADDI, 6, 8);

    // logical
    add_opcode("andi", ANDI, OP_ANDI, 3, 4);
    add_opcode("andi.l", ANDI_L, OP_ANDI, 6, 8);

    add_opcode("ori", ORI, OP_ORI, 3, 4);
    add_opcode("ori.l", ORI_L, OP_ORI, 6, 8);

    add_opcode("xori", XORI, OP_XORI, 3, 4);
    add_opcode("xori.l", XORI_L, OP_XORI, 6, 8);

    // comparison
    add_opcode("eqi", EQI, OP_EQI, 1, 4);
    add_opcode("eqi.l", EQI_L, OP_EQI, 6, 8);

    add_opcode("lti", LTI, OP_LTI, 1, 4);
    add_opcode("lti.l", LTI_L, OP_LTI, 6, 8);

    add_opcode("ltiu", LTIU, OP_LTIU, 1, 4);
    add_opcode("ltiu.l", LTIU_L, OP_LTIU, 6, 8);

    // memory
    add_opcode("lw", LW, OP_LW, 1, 4);
    add_opcode("lw.l", LW_L, OP_LW, 6, 8);

    add_opcode("lh", LH, OP_LH, 1, 4);
    add_opcode("lh.l", LH_L, OP_LH, 6, 8);

    add_opcode("lb", LB, OP_LB, 1, 4);
    add_opcode("lb.l", LB_L, OP_LB, 6, 8);

    add_opcode("lhu", LHU, OP_LHU, 1, 4);
    add_opcode("lhu.l", LHU_L, OP_LHU, 6, 8);

    add_opcode("lbu", LBU, OP_LBU, 1, 4);
    add_opcode("lbu.l", LBU_L, OP_LBU, 6, 8);

    add_opcode("sw", SW, OP_SW, 1, 4);
    add_opcode("sw.l", SW_L, OP_SW, 6, 8);

    add_opcode("sh", SH, OP_SH, 1, 4);
    add_opcode("sh.l", SH_L, OP_SH, 6, 8);

    add_opcode("sb", SB, OP_SB, 1, 4);
    add_opcode("sb.l", SB_L, OP_SB, 6, 8);

    // branches and jumps
    add_opcode("b", B, OP_B, 4, 4);
    add_opcode("bn", BN, OP_BN, 4, 4);
    add_opcode("j", J, OP_J, 2, 4);
    add_opcode("jal", JAL, OP_JAL, 2, 4);

    add_register("r0", 0); add_register("zr", 0);
    add_register("r1", 1);
    add_register("r2", 2);
    add_register("r3", 3);
    add_register("r4", 4);
    add_register("r5", 5);
    add_register("r6", 6);
    add_register("r7", 7);
    add_register("r8", 8);
    add_register("r9", 9);
    add_register("r10", 10);
    add_register("r11", 11);
    add_register("r12", 12);
    add_register("r13", 13);
    add_register("r14", 14);
    add_register("r15", 15);
    add_register("r16", 16);
    add_register("r17", 17);
    add_register("r18", 18);
    add_register("r19", 19);
    add_register("r20", 20);
    add_register("r21", 21);
    add_register("r22", 22);
    add_register("r23", 23);
    add_register("r24", 24);
    add_register("r25", 25);
    add_register("r26", 26);
    add_register("r27", 27);
    add_register("r28", 28);
    add_register("r29", 29);
    add_register("r30", 30);
    add_register("r31", 31);

    add_register("p0", 0);
    add_register("p1", 1);
    add_register("p2", 2);
    add_register("p3", 3);
}

