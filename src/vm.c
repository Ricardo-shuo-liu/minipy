#include "vm.h"

// 初始化虚拟机
void init_vm(VM* vm, Bytecode* bc)
{
    vm->sp = 0;
    vm->ip = 0;
    vm->var_count = bc->var_count;
    vm->vars = calloc(bc->var_count, sizeof(int));
}
// 推入栈
void push(VM* vm, int val)
{
    if (vm->sp >= STACK_SIZE) error("Stack overflow", 0); // 如果推入栈过多报错溢出
    vm->stack[vm->sp++] = val;
}
// 拿出栈
int pop(VM* vm)
{
    if (vm->sp <= 0) error("Stack underflow", 0);
    return vm->stack[--vm->sp];
}
// 虚拟机执行 核心功能端口
void run_vm(VM* vm, Bytecode* bc)
{
    while (1)
    {
        int op = bc->code[vm->ip];
        switch (op)
        {
            case OP_PUSH:
            {
                int idx = bc->code[vm->ip + 1];
                push(vm, bc->constants[idx]);
                vm->ip += 2;
                break;
            }
            case OP_LOAD:
            {
                int idx = bc->code[vm->ip + 1];
                push(vm, vm->vars[idx]);
                vm->ip += 2;
                break;
            }
            case OP_STORE:
            {
                int idx = bc->code[vm->ip + 1];
                vm->vars[idx] = pop(vm);
                vm->ip += 2;
                break;
            }
            case OP_ADD: push(vm, pop(vm)+pop(vm)); vm->ip++; break;
            case OP_SUB:
            {
                int b=pop(vm),a=pop(vm);
                push(vm,a-b);
                vm->ip++;
                break;}
            case OP_MUL: push(vm, pop(vm)*pop(vm)); vm->ip++; break;
            case OP_DIV:
            {
                int b=pop(vm);
                if(b==0)error("Division by zero",0);
                int a=pop(vm);
                push(vm,a/b);
                vm->ip++;
                break;
            }
            case OP_EQ: push(vm, pop(vm)==pop(vm));vm->ip++;break;
            case OP_NEQ:push(vm,pop(vm)!=pop(vm));vm->ip++;break;
            case OP_LT: 
            {
                int b=pop(vm),a=pop(vm);
                push(vm,a<b);
                vm->ip++;break;
            }
            case OP_GT: 
            {
                int b=pop(vm),a=pop(vm);
                push(vm,a>b);
                vm->ip++;
                break;
            }
            case OP_PRINT: printf("%d\n",pop(vm));vm->ip++;break;
            case OP_JMP: vm->ip = bc->code[vm->ip+1]; break;
            case OP_JMPF: 
            {
                int addr=bc->code[vm->ip+1];
                int cond=pop(vm);
                vm->ip = cond ? vm->ip+2 : addr;
                break;
            }
            case OP_HALT: return;
            default: error("Unknown opcode",0);
        }
    }
}