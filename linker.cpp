#include "linker.hpp"
#include "tokenizer.hpp"

using namespace std;

vector<pair<string, int>> symbol_table;
unordered_map<string, int> symbol_to_index; // stores index of symbol in symbol_table
unordered_map<string, int> symbol_to_module; // stores the 0 indexed module number where symbol was defined
vector<int> module_base_addr; // stores base address of each module in order of appearance

void pass1(ifstream& f) {
    int base_addr = 0; // base address of current module
    int module = 0; // current module number - 1

    unordered_set<string> multiple_defs;

    while(1) {
        //TODO: createModule()

        int defcount = readInt(f);
        if(defcount == -1)
            break;
        
        if(defcount > MAX_USE_DEF) {
            __parseerror(4);
            exit(EXIT_FAILURE);
        }

        vector<string> module_defs(defcount, "");

        for(int i = 0; i < defcount; i++) {
            Symbol sym = string(readSymbol(f));
            int val = readInt(f);

            if(val == -1) {
                __parseerror(0);
                exit(EXIT_FAILURE);
            }

            // see rule 2
            if(symbol_to_index.find(sym) == symbol_to_index.end()) {
                symbol_to_index[sym] = symbol_table.size();
                symbol_table.push_back({sym, val+base_addr});
                symbol_to_module[sym] = module;
            }
            else { // if the symbol already exists, add it to multiple defs set
                multiple_defs.insert(sym);
            }
            module_defs[i] = sym;
        }

        int usecount = readInt(f);
        if(usecount > MAX_USE_DEF) {
            __parseerror(5);
            exit(EXIT_FAILURE);
        }
        
        if(usecount == -1) {
            __parseerror(0);
            exit(EXIT_FAILURE);
        }

        for(int i = 0; i < usecount; i++) {
            Symbol sym = string(readSymbol(f));
        }

        int instcount = readInt(f);
        if(instcount == -1) {
            __parseerror(0);
            exit(EXIT_FAILURE);
        }
        if(base_addr + instcount > MACHINE_SIZE) {
            __parseerror(6);
            exit(EXIT_FAILURE);
        }

        // cout << "pass1 module " << module+1 << " instcount " << instcount << endl;

        for(int i = 0; i < instcount; i++) {
            char addrmode = readIAER(f);
            int instr = readInt(f);
            if(instr == -1) {
                __parseerror(0);
                exit(EXIT_FAILURE);
            }
            //TODO: error check in pass2
        }

        module++;
        module_base_addr.push_back(base_addr);

        // last defcount symbols are defined in this module
        // see rule 5; relative address of a symbol is given by symbtable.second - instcount
        // for(int i = symbol_table.size() - defcount; i < symbol_table.size(); i++) {
        //     if(symbol_table[i].second - base_addr >= instcount) {
        //         __warning(5, module, symbol_table[i].first.c_str(), symbol_table[i].second - base_addr, instcount-1);
        //         symbol_table[i].second = base_addr;
        //     }
        // }
        for(string s : module_defs) {
            int idx = symbol_to_index[s];
            if(symbol_table[idx].second - base_addr >= instcount) {
                __warning(5, module, symbol_table[idx].first.c_str(), symbol_table[idx].second - base_addr, instcount - 1);
                symbol_table[idx].second = base_addr;
            }
        }
        base_addr += instcount;
    }

    cout << "Symbol Table\n";
    for(auto p : symbol_table) {
        // cout << p.first << '=' << p.second << " symbol to index " << symbol_to_index[p.first];
        cout << p.first << '=' << p.second;
        //TODO: add space after symbol
        if(multiple_defs.find(p.first) != multiple_defs.end())
            __linkererror(2, "");
        cout << endl;
    }
    // cout << "pass1 done\n";
}

void __print_memory(int abs_addr, int instr) {
    printf("\n%03d: %04d", abs_addr, instr);
}

void __illegal_opcode(int* opcode, int* operand, int addr) {
    *opcode = MAX_OPCODE; *operand = MAX_OPERAND;
    __print_memory(addr, *opcode*MOD_OPCODE + *operand);
    __linkererror(11, "");
}

void pass2(ifstream& f) {
    if(parse_error)
        return;
    eof = false;
    int module = 0;
    unordered_set<string> used_set_global;

    cout << "\nMemory Map";
    while(!eof) {
        //TODO: createModule()

        int defcount = readInt(f);
        if(eof)
            break;

        for(int i = 0; i < defcount; i++) {
            Symbol sym = string(readSymbol(f));
            int val = readInt(f);
        }

        int usecount = readInt(f);
        vector<string> uselist(usecount);
        unordered_map<string, unordered_set<int>> used_set_module;

        for(int i = 0; i < usecount; i++) {
            Symbol sym = string(readSymbol(f));
            uselist[i] = sym;
        }

        int instcount = readInt(f);
        if(instcount > MACHINE_SIZE) {
            __parseerror(6);
            exit(EXIT_FAILURE);
        }

        for(int i = 0; i < instcount; i++) {
            char addrmode = readIAER(f);
            int instr = readInt(f);
            //TODO: error check in pass2
            //TODO: rule 7, rule 4 keep track of variables defined and used
            int opcode = instr / MOD_OPCODE;
            int operand = instr % MOD_OPCODE, corr_operand;
            int base_addr = module_base_addr[module];
            string used_sym;

            switch (addrmode)
            {
            case 'I':
                //error check rule 10
                if(instr >= MAX_IMMEDIATE) {
                    opcode = MAX_OPCODE; operand = MAX_OPERAND;
                    __print_memory(base_addr+i, opcode*MOD_OPCODE + operand);
                    __linkererror(10, "");
                    break;
                }
                __print_memory(base_addr+i, opcode*MOD_OPCODE + operand);
                break;

            case 'A':
                //error check rule 11
                if(opcode > MAX_OPCODE) {
                    __illegal_opcode(&opcode, &operand, base_addr+i);
                    break;
                }
                //error check rule 8
                if(operand >= MACHINE_SIZE) {
                    operand = 0;
                    __print_memory(base_addr+i, opcode*MOD_OPCODE + operand);
                    __linkererror(8, "");
                    break;
                }
                __print_memory(base_addr+i, opcode*MOD_OPCODE + operand);
                break;

            case 'E':
                //error check rule 11
                if(opcode > MAX_OPCODE) {
                    __illegal_opcode(&opcode, &operand, base_addr+i);
                    break;
                }

                //error check rule 3, 6
                if(operand >= uselist.size()) {
                    // cout << "\naddr " << base_addr+i << " operand " << operand << " uselist size " << uselist.size() << endl;
                    
                    //code below same as case 'I'
                    //error check rule 10
                    if(instr > MAX_IMMEDIATE) {
                        opcode = MAX_OPCODE; operand = MAX_OPERAND;
                        __print_memory(base_addr+i, opcode*MOD_OPCODE + operand);
                        __linkererror(10, "");
                        break;
                    }
                    __print_memory(base_addr+i, opcode*MOD_OPCODE + operand);
                    __linkererror(6, "");
                    break;
                }

                used_sym = uselist[operand];
                // if symbol was not defined anywhere
                if(symbol_to_index.find(used_sym) == symbol_to_index.end()) {
                    // used_set_module.insert({used_sym, operand});
                    used_set_module[used_sym].insert(operand);
                    operand = 0;
                    __print_memory(base_addr+i, opcode*MOD_OPCODE + operand);
                    __linkererror(3, used_sym.c_str());
                    break;
                }
                // used_set_module.insert({used_sym, operand});
                used_set_module[used_sym].insert(operand);
                used_set_global.insert(used_sym);
                operand = symbol_table[symbol_to_index[used_sym]].second;
                __print_memory(base_addr+i, opcode*MOD_OPCODE + operand);
                break;

            case 'R':
                //error check rule 9 >= or > ?
                if(opcode > MAX_OPCODE) {
                    __illegal_opcode(&opcode, &operand, base_addr+i);
                    break;
                }

                if(operand >= instcount) {
                    operand = 0;
                    operand += base_addr;
                    __print_memory(base_addr+i, opcode*MOD_OPCODE + operand);
                    __linkererror(9, "");
                    break;
                }
                operand += base_addr;
                __print_memory(base_addr+i, opcode*MOD_OPCODE + operand);
                break;
            
            default:
                cout << "Unrecognized addressing mode " << addrmode << " should not be encountered\n";
                break;
            }
        }

        module++;

        //warning check rule 7
        for(int i = 0; i < uselist.size(); i++) {
            string s = uselist[i];
            // if(used_set_module.find({s, i}) == used_set_module.end()) {
            //     __warning(7, module, s.c_str(), 0, 0);
            // }
            if(used_set_module[s].find(i) == used_set_module[s].end()) {
                __warning(7, module, s.c_str(), 0, 0);
            }
        }
    }

    cout << endl << endl;
    //warning check rule 4
    for(pair<string, int> s : symbol_table) {
        if(used_set_global.find(s.first) == used_set_global.end()) {
            //TODO: keep track of module number where each symbol is defined
            __warning(4, symbol_to_module[s.first]+1, s.first.c_str(), 0, 0);
        }
    }
    cout << endl;
}
