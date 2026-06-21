/**
 * 定义字节码核心功能函数
 * 将AST语法树转化为字节码数组
 * 方便传入VM中执行
 */


#ifndef CODEGEN_H
#define CODEGEN_H

#include "parser.h"


typedef enum {
    OP_PUSH = 0, // 把常量压入栈
    OP_LOAD, // 读取变量压栈
    OP_STORE, // 栈顶值存入变量
    OP_ADD, OP_SUB, OP_MUL, OP_DIV, // 四则运算
    OP_EQ, OP_NEQ, OP_LT, OP_GT, // 比较运算
    OP_PRINT, // 打印栈顶数字
    OP_JMP, // 无条件跳转
    OP_JMPF, // 条件跳转
    OP_HALT // 程序结束
} OpCode;



typedef struct {
    
    int* code;  //虚拟机指令数
    int code_len; // 当前实际指令数量
    int code_cap; // 数组总容量

    // 常量池
    int* constants;
    int const_count;
    int const_cap;

    // 变量名表
    char** vars;
    int var_count;
} Bytecode;



void init_bytecode(Bytecode* bc);

void generate_bytecode(Bytecode* bc, Node* root);

#endif
