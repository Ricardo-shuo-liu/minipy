/**
 * 将文本转为为一个个单独的Token
 * 方便转化为AST语法树
 * 
 */

#ifndef LEXER_H
#define LEXER_H

#include "utils.h"
#include <ctype.h>

// 定义语法树节点种类
typedef enum {
    TOK_EOF = 0, // 文件结束
    TOK_NUMBER, // 数字
    TOK_IDENT, // 变量名
    TOK_IF, // if 关键字
    TOK_ELSE, // else关键字
    TOK_WHILE, // while 关键字
    TOK_PRINT, // print 关键字
    TOK_PLUS, // +关键字
    TOK_MINUS, // -关键字
    TOK_STAR, // *关键字
    TOK_SLASH, // /关键字
    TOK_ASSIGN, // 赋值= 关键字
    TOK_EQ, // ==关键字
    TOK_NEQ, // !=关键字
    TOK_LT,  // <关键字
    TOK_GT, // >关键字
    TOK_LPAREN, // (
    TOK_RPAREN, // )
    TOK_LBRACE, // {
    TOK_RBRACE,// }
    TOK_SEMI // ;标识语句结束
} TokenType;


// 定义Token对象
typedef struct {
    TokenType type;
    int value;
    char* str;
    int line;
} Token;


typedef struct {
    const char* src;
    int pos; // 当前读取到源代码的下标位置
    int line; // 当前行数 换行时+1
} Lexer;


void init_lexer(Lexer* lex, const char* src);
// 初始化词法分析器
Token next_token(Lexer* lex);
// 读取下一个Token
#endif
