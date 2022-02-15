#include "tokenizer.hpp"
#include "linker.hpp"

void __parseerror(int errcode) {
    static const char* errstr[] = {
        "NUM_EXPECTED", // Number expect, anything >= 2^30 is not a number either
        "SYM_EXPECTED", // Symbol Expected
        "ADDR_EXPECTED", // Addressing Expected which is A/E/I/R
        "SYM_TOO_LONG", // Symbol Name is too long
        "TOO_MANY_DEF_IN_MODULE", // > 16
        "TOO_MANY_USE_IN_MODULE", // > 16
        "TOO_MANY_INSTR", // total num_instr exceeds memory size (512)
    };

    printf("Parse Error line %d offset %d: %s\n", linenum, lineofs, errstr[errcode]);
}

// All warnings and linker error codes are the same as the rule numbers that define them

void __linkererror(int errcode, const char* symbol) {
    switch (errcode)
    {
    case 2: //pass 1, rule 2
        printf(" Error: This variable is multiple times defined; first value used");
        break;
    
    case 3: //pass2, rule 3
        printf(" Error: %s is not defined; zero used", symbol);
        break;

    case 6: //pass2, rule 6
        printf(" Error: External address exceeds length of uselist; treated as immediate");
        break;

    case 8: //pass2, rule 8
        printf(" Error: Absolute address exceeds machine size; zero used");
        break;

    case 9: //pass2, rule 9
        printf(" Error: Relative address exceeds module size; zero used");
        break;

    case 10: //pass2, rule 10
        printf(" Error: Illegal immediate value; treated as 9999");
        break;
    
    case 11: //pass2, rule 11
        printf(" Error: Illegal opcode; treated as 9999");
        break;

    default:
        printf(" Unrecognized errorcode %d", errcode);
        break;
    }
}

void __warning(int warncode, int module_number, const char* symbol, int relative_offset, int max_offset) {
    switch (warncode)
    {
    case 5: //pass 1, rule 5
        printf("Warning: Module %d: %s too big %d (max=%d) assume zero relative\n", 
                module_number, symbol, relative_offset, max_offset);
        break;
    
    case 7: //pass 2, rule 7
        printf("\nWarning: Module %d: %s appeared in the uselist but was not actually used", module_number, symbol);
        break;

    case 4: //pass 2, rule 4
        printf("Warning: Module %d: %s was defined but never used\n", module_number, symbol);
        break;
    
    default:
        printf("Unrecognized warncode %d\n", warncode);
        break;
    }
}
