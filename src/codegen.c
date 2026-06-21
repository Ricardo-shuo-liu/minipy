#include "codegen.h"


void emit(Bytecode* bc, int val);
int add_constant(Bytecode* bc, int num);
int add_var(Bytecode* bc, const char* name);

void gen_node(Bytecode* bc, Node* node);
void gen_block(Bytecode* bc, Node* block);
void gen_binary(Bytecode* bc, Node* node);
void gen_if(Bytecode* bc, Node* node);
void gen_while(Bytecode* bc, Node* node);


// 初始化字节码机
void init_bytecode(Bytecode* bc)
{
    bc->code = NULL;
    bc->code_len = 0;
    bc->code_cap = 0;
    bc->constants = NULL;
    bc->const_count = 0;
    bc->const_cap = 0;
    bc->vars = NULL;
    bc->var_count = 0;
}
// 添加字节码
void emit(Bytecode* bc, int val)
{
    if (bc->code_len >= bc->code_cap)
    {
        bc->code_cap = bc->code_cap == 0 ? 64 : bc->code_cap * 2;
        bc->code = realloc(bc->code, bc->code_cap * sizeof(int));
    }
    bc->code[bc->code_len++] = val;
}
// 添加常量栈
int add_constant(Bytecode* bc, int val)
{
    for (int i = 0; i < bc->const_count; i++)
    {
        if (bc->constants[i] == val) return i;
    }
    if (bc->const_count >= bc->const_cap) 
    {
        bc->const_cap = bc->const_cap == 0 ? 32 : bc->const_cap * 2;
        bc->constants = realloc(bc->constants, bc->const_cap * sizeof(int));
    }
    bc->constants[bc->const_count] = val;
    return bc->const_count++;
}
// 添加变量
int add_var(Bytecode* bc, const char* name)
{
    for (int i = 0; i < bc->var_count; i++)
    {
        if (strcmp(bc->vars[i], name) == 0) return i;
    }
    bc->var_count++;
    bc->vars = realloc(bc->vars, bc->var_count * sizeof(char*));
    bc->vars[bc->var_count - 1] = strdup(name);
    return bc->var_count - 1;
}
// 二元操作码
void gen_binary(Bytecode* bc, Node* node)
{
    gen_node(bc, node->data.binary.left);
    gen_node(bc, node->data.binary.right);
    switch (node->data.binary.op) {
        case TOK_PLUS: emit(bc, OP_ADD); break;
        case TOK_MINUS: emit(bc, OP_SUB); break;
        case TOK_STAR: emit(bc, OP_MUL); break;
        case TOK_SLASH: emit(bc, OP_DIV); break;
        case TOK_EQ: emit(bc, OP_EQ); break;
        case TOK_NEQ: emit(bc, OP_NEQ); break;
        case TOK_LT: emit(bc, OP_LT); break;
        case TOK_GT: emit(bc, OP_GT); break;
        default: break;
    }
}
// if
void gen_if(Bytecode* bc, Node* node) {
    gen_node(bc, node->data.if_stmt.cond);
    int jmpf_pos = bc->code_len;
    emit(bc, OP_JMPF);
    emit(bc, 0);

    gen_block(bc, node->data.if_stmt.then_body);

    if (node->data.if_stmt.else_body)
    {
        int jmp_pos = bc->code_len;
        emit(bc, OP_JMP);
        emit(bc, 0);
        bc->code[jmpf_pos + 1] = bc->code_len;
        gen_block(bc, node->data.if_stmt.else_body);
        bc->code[jmp_pos + 1] = bc->code_len;
    }
    else 
    {
        bc->code[jmpf_pos + 1] = bc->code_len;
    }
}
// while
void gen_while(Bytecode* bc, Node* node) {
    int loop_start = bc->code_len;
    gen_node(bc, node->data.while_stmt.cond);
    
    int jmpf_pos = bc->code_len;
    emit(bc, OP_JMPF);
    emit(bc, 0);

    gen_block(bc, node->data.while_stmt.body);
    emit(bc, OP_JMP);
    emit(bc, loop_start);

    bc->code[jmpf_pos + 1] = bc->code_len;
}
// 对于语法树的每一个节点实现操作数的生成
void gen_node(Bytecode* bc, Node* node) {
    switch (node->type) 
    {
        case NODE_NUMBER: 
        {
            int idx = add_constant(bc, node->data.number);
            emit(bc, OP_PUSH); 
            emit(bc, idx);
            break;
        }
        case NODE_VAR:
        {
            int idx = add_var(bc, node->data.var_name);
            emit(bc, OP_LOAD);
            emit(bc, idx);
            break;
        }
        case NODE_BINARY_OP: gen_binary(bc, node); break;
        case NODE_ASSIGN:
        {
            int idx = add_var(bc, node->data.assign.name);
            gen_node(bc, node->data.assign.expr);
            emit(bc, OP_STORE);
            emit(bc, idx);
            break;
        }
        case NODE_PRINT:
            gen_node(bc, node->data.print.expr);
            emit(bc, OP_PRINT);
            break;
        case NODE_IF: gen_if(bc, node); break;
        case NODE_WHILE: gen_while(bc, node); break;
        case NODE_BLOCK: gen_block(bc, node); break;
    }
}
// 将语法树谱写成字节码
void gen_block(Bytecode* bc, Node* block)
{
    for (int i = 0; i < block->data.block.count; i++)
    {
        gen_node(bc, block->data.block.stmts[i]);
    }
}
void generate_bytecode(Bytecode* bc, Node* root) {
    gen_block(bc, root);
    emit(bc, OP_HALT);
}