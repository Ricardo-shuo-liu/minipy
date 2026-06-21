#include "parser.h"


Node* parse_expr(Parser* p);
Node* parse_primary(Parser* p);
Node* parse_stmt(Parser* p);
Node* parse_block(Parser* p);
Node* parse_if(Parser* p);
int match(Parser* p, TokenType tok);


// 初始化token槽
void init_parser(Parser* p)
{
    p->tokens = NULL;
    p->pos = 0;
    p->count = 0;
    p->capacity = 0;
}

// 添加token
void add_token(Parser* p, Token t) 
{
    // 为初始化或者动态开辟更多空间
    if (p->count >= p->capacity)
    {
        p->capacity = p->capacity == 0 ? 64 : p->capacity * 2;
        p->tokens = realloc(p->tokens, p->capacity * sizeof(Token));
    }
    p->tokens[p->count++] = t;
}


// 建立一个新的节点
Node* new_node(NodeType type)
{
    Node* n = malloc(sizeof(Node));
    n->type = type;
    memset(&n->data, 0, sizeof(n->data)); // 将初始化的数据清洗
    return n;
}
// 获取当前的token
Token current(Parser* p) 
{ 
    return p->tokens[p->pos]; 
}

Token consume(Parser* p, TokenType type, const char* msg) 
{
    Token t = current(p);
    if (t.type != type) error(msg, t.line);
    p->pos++;
    return t;
}
// 解析对象
Node* parse_primary(Parser* p)
{
    Token t = current(p);
    if (t.type == TOK_NUMBER)
    {
        p->pos++;
        Node* n = new_node(NODE_NUMBER);
        n->data.number = t.value;
        return n;
    }
    if (t.type == TOK_IDENT)
    {
        p->pos++;
        Node* n = new_node(NODE_VAR);
        n->data.var_name = t.str;
        return n;
    }
    if (t.type == TOK_LPAREN)
    {
        p->pos++;
        Node* expr = parse_expr(p);
        consume(p, TOK_RPAREN, "Expected ')'");
        return expr;
    }
    error("Expected expression", t.line);
    return NULL;
}
// 乘法解析
Node* parse_mul(Parser* p)
{
    Node* left = parse_primary(p);
    while (current(p).type == TOK_STAR || current(p).type == TOK_SLASH)
    {
        TokenType op = current(p).type;
        p->pos++;
        Node* right = parse_primary(p);
        Node* n = new_node(NODE_BINARY_OP);
        n->data.binary.op = op;
        n->data.binary.left = left;
        n->data.binary.right = right;
        left = n;
    }
    return left;
}
// 加法解析
Node* parse_add(Parser* p)
{
    Node* left = parse_mul(p);
    while (current(p).type == TOK_PLUS || current(p).type == TOK_MINUS)
    {
        TokenType op = current(p).type;
        p->pos++;
        Node* right = parse_mul(p);
        Node* n = new_node(NODE_BINARY_OP);
        n->data.binary.op = op;
        n->data.binary.left = left;
        n->data.binary.right = right;
        left = n;
    }
    return left;
}

// 解析p用于获得后续代码体
Node* parse_compare(Parser* p)
{
    Node* left = parse_add(p);
    while (current(p).type == TOK_EQ || current(p).type == TOK_NEQ || current(p).type == TOK_LT || current(p).type == TOK_GT)
    {
        TokenType op = current(p).type;
        p->pos++;
        Node* right = parse_add(p);
        Node* n = new_node(NODE_BINARY_OP);
        n->data.binary.op = op;
        n->data.binary.left = left;
        n->data.binary.right = right;
        left = n;
    }
    return left;
}

// 获得后续代码体
Node* parse_expr(Parser* p)
{
    return parse_compare(p);
}

// 生成print Node
Node* parse_print(Parser* p) {
    p->pos++;
    Node* n = new_node(NODE_PRINT);
    n->data.print.expr = parse_expr(p);
    consume(p, TOK_SEMI, "Expected ';' after print");
    return n;
}
// 解析{}
Node* parse_block(Parser* p) {
    consume(p, TOK_LBRACE, "Expected '{'");
    Node* block = new_node(NODE_BLOCK);
    block->data.block.stmts = NULL;
    block->data.block.count = 0;

    while (current(p).type != TOK_RBRACE && current(p).type != TOK_EOF)
    {
        Node* stmt = parse_stmt(p);
        block->data.block.count++;
        block->data.block.stmts = realloc(block->data.block.stmts, block->data.block.count * sizeof(Node*));
        block->data.block.stmts[block->data.block.count - 1] = stmt;
    }
    consume(p, TOK_RBRACE, "Expected '}'");
    return block;
}
int match(Parser* p, TokenType type) {
    if (current(p).type == type) {
        p->pos++;
        return 1;
    }
    return 0;
}
// if节点
Node* parse_if(Parser* p)
{
    p->pos++;
    consume(p, TOK_LPAREN, "Expected '(' after if");
    Node* n = new_node(NODE_IF);
    n->data.if_stmt.cond = parse_expr(p);
    consume(p, TOK_RPAREN, "Expected ')' after condition");
    n->data.if_stmt.then_body = parse_block(p);

    if (match(p, TOK_ELSE))
    {
        n->data.if_stmt.else_body = parse_block(p);
    }
    else 
    {
        n->data.if_stmt.else_body = NULL;
    }
    return n;
}
// 循环体
Node* parse_while(Parser* p) {
    p->pos++;
    consume(p, TOK_LPAREN, "Expected '(' after while");
    Node* n = new_node(NODE_WHILE);
    n->data.while_stmt.cond = parse_expr(p);
    consume(p, TOK_RPAREN, "Expected ')' after condition");
    n->data.while_stmt.body = parse_block(p);
    return n;
}
// 赋值Node
Node* parse_assign(Parser* p)
{
    Token name = current(p);
    p->pos++;
    consume(p, TOK_ASSIGN, "Expected '='");
    Node* n = new_node(NODE_ASSIGN);
    n->data.assign.name = name.str;
    n->data.assign.expr = parse_expr(p);
    consume(p, TOK_SEMI, "Expected ';' after assignment");
    return n;
}

// 生成Node
Node* parse_stmt(Parser* p)
{
    Token t = current(p);
    switch (t.type)
    {
        case TOK_PRINT:return parse_print(p);
        case TOK_IF: return parse_if(p);
        case TOK_WHILE: return parse_while(p);
        case TOK_IDENT: return parse_assign(p);
        default: error("Unexpected statement", t.line);
    }
    return NULL;
}

// 生成AST语法树
Node* parse_program(Parser* p)
{
    Node* program = new_node(NODE_BLOCK);
    program->data.block.stmts = NULL;
    program->data.block.count = 0;

    while (current(p).type != TOK_EOF)
    {
        Node* stmt = parse_stmt(p);
        program->data.block.count++;
        program->data.block.stmts = realloc(program->data.block.stmts, program->data.block.count * sizeof(Node*));
        program->data.block.stmts[program->data.block.count - 1] = stmt;
    }
    
    return program;
}