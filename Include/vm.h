/**
 * 虚拟机
 * 用于执行字节码
 */

#ifndef VM_H
#define VM_H

#include "codegen.h"

#define STACK_SIZE 256


typedef struct {
    int stack[STACK_SIZE]; // 运算栈
    int sp;                // 栈指针
    int ip;                // 指令指针
    int* vars;             // 变量存储数组
    int var_count;         // 一共有多少个变量
} VM;


// 初始化虚拟机
void init_vm(VM* vm, Bytecode* bc);
// 运行虚拟机
void run_vm(VM* vm, Bytecode* bc);

#endif
