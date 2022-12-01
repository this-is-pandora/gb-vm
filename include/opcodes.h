#ifndef OPCODE_H
#define OPCODE_H

//TODO: implement opcodes w/ computed GOTO's

enum Opcodes {
    /* 8-bit operations */
    OP_LD = 0x06,
    OP_PUSH,
    OP_POP,
    OP_ADD,
    OP_ADC,
    OP_SUB,
    OP_SBC,
    OP_AND,
    OP_OR,
    OP_XOR,
    OP_CP,
    OP_INC,
    OP_DEC,
    /* 16-bit operations */
    MAX_OPCODES = 0 /* max. opcode value */
};
#endif