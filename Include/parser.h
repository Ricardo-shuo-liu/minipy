/**
 * 将Token序列转化为AST语法树
 * 方便之后处理成字节码
 */

#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include <string.h>
typedef enum {
    NODE_NUMBER,    // 数字字面量
    NODE_VAR,        // 变量
    NODE_BINARY_OP,  // 二元运算
    NODE_ASSIGN,     // 赋值语句
    NODE_PRINT,      // print
    NODE_IF,         // if
    NODE_WHILE,      // while
    NODE_BLOCK       // 代码块
} NodeType;



typedef struct Node Node;
/**
 *  这里定义核心树的Node节点
 *  用于将代码生成为树方便之后实现字节码生成
 *  这里使用union来存放数据
 *  使用具体结构依赖于type
 *  核心思想是将代码从token转化为NodeType类
 *  减少数量
 */
// 定义节点类型
struct Node {
    NodeType type;
    union {
        int number;
        char* var_name;
        // 二元运算
        struct {
            TokenType op;
            Node* left;
            Node* right;
        } binary;
        // 赋值
        struct {
            char* name;
            Node* expr;
        } assign;
        // 打印
        struct {
            Node* expr;
        } print;
        // if语句
        struct {
            Node* cond; // 逻辑语句
            Node* then_body; // if体代码块
            Node* else_body; // else
        } if_stmt;
        // while 循环
        struct {
            Node* cond;
            Node* body;
        } while_stmt;
        // 代码块
        struct {
            Node** stmts;
            int count; // 数量
        } block;

    } data;
};

typedef struct {
    Token* tokens;
    int pos;
    int count;
    int capacity;
} Parser;

// 初始化空语法分析器
void init_parser(Parser* p);

// 添加token
void add_token(Parser* p, Token t);
// 生成语法树
Node* parse_program(Parser* p);

#endif
