#include "lexer.h"

// 初始化文本
void init_lexer(Lexer* lex, const char* src)
{
    lex->src = src;
    lex->pos = 0;
    lex->line = 1;
}
// 获得当前的字符
char peek(Lexer* lex)
{ 
    return lex->src[lex->pos];
}
// 取出当前字符
char advance(Lexer* lex)
{
    char c = peek(lex);
    lex->pos++;
    if (c == '\n') lex->line++;
    return c;
}
// 跳过空白符
void skip_whitespace(Lexer* lex)
{
    while (peek(lex) != '\0' && isspace((unsigned char)peek(lex)))
    {
        char c = advance(lex);
        if (c == '\r') continue;
    }
}
// 跳过注释
void skip_comment(Lexer* lex)
{
    if (peek(lex) == '#')
    {
        while (peek(lex) != '\0' && peek(lex) != '\n')advance(lex);
        if (peek(lex) == '\n') advance(lex);
    }
}
    

// 生成token
Token make_token(TokenType type, int line)
{
    Token t = {type, 0, NULL, line};
    return t;
}
// 生成number的token
Token number_token(Lexer* lex)
{
    int val = 0;
    int line = lex->line;
    while (isdigit((unsigned char)peek(lex)))
    {
        val = val * 10 + (advance(lex) - '0');
    }
    Token t = make_token(TOK_NUMBER, line);
    t.value = val;
    return t;
}

Token ident_token(Lexer* lex)
{
    int start = lex->pos;
    int line = lex->line;
    while (isalnum((unsigned char)peek(lex)) || peek(lex) == '_') advance(lex);
    
    int len = lex->pos - start;
    char* s = malloc(len + 1);
    strncpy(s, lex->src + start, len);
    s[len] = '\0';
    // 判断为关键字
    if (strcmp(s, "if") == 0)
    { 
        free(s); 
        return make_token(TOK_IF, line);
    }
    if (strcmp(s, "else") == 0)  
    { 
        free(s); 
        return make_token(TOK_ELSE, line); 
    }
    if (strcmp(s, "while") == 0) 
    { 
        free(s); 
        return make_token(TOK_WHILE, line); 
    }
    if (strcmp(s, "print") == 0) 
    { 
        free(s); 
        return make_token(TOK_PRINT, line); 
    }

    Token t = make_token(TOK_IDENT, line);
    t.str = s;
    return t;
}

// 获取下一个token
Token next_token(Lexer* lex)
{
    while (peek(lex) != '\0')
    {
        int redo;
        do
        {
            redo = 0;
            if (isspace((unsigned char)peek(lex)))
            {
                skip_whitespace(lex);
                redo = 1;
            }
            if (peek(lex) == '#')
            {
                skip_comment(lex);
                redo = 1;
            }
        } while (redo);

        if (peek(lex) == '\0') break;

        char c = peek(lex);
        // 判断数字
        if (isdigit((unsigned char)c)) return number_token(lex);
        // 判断字符串的token情况
        if (isalpha((unsigned char)c) || c == '_') return ident_token(lex);

        advance(lex);
        switch (c) {
            case '+': return make_token(TOK_PLUS, lex->line);
            case '-': return make_token(TOK_MINUS, lex->line);
            case '*': return make_token(TOK_STAR, lex->line);
            case '/': return make_token(TOK_SLASH, lex->line);
            case '(': return make_token(TOK_LPAREN, lex->line);
            case ')': return make_token(TOK_RPAREN, lex->line);
            case '{': return make_token(TOK_LBRACE, lex->line);
            case '}': return make_token(TOK_RBRACE, lex->line);
            case ';': return make_token(TOK_SEMI, lex->line);
            case '=':
                if (peek(lex) == '=') 
                { 
                    advance(lex); 
                    return make_token(TOK_EQ, lex->line); 
                }
                return make_token(TOK_ASSIGN, lex->line);
            case '!':
                if (peek(lex) == '=') 
                { 
                    advance(lex); 
                    return make_token(TOK_NEQ, lex->line); 
                }
                error("Unexpected character '!'", lex->line);
                return make_token(TOK_EOF, lex->line);
            case '<': return make_token(TOK_LT, lex->line);
            case '>': return make_token(TOK_GT, lex->line);
            default: 
            {
                char buf[64];
                snprintf(buf, sizeof(buf), "Unexpected character '%c' (ascii %d)", c, (unsigned char)c);
                error(buf, lex->line);
                return make_token(TOK_EOF, lex->line);
            }
        }
    }
    return make_token(TOK_EOF, lex->line);
}