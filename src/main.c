#include "utils.h"
#include "lexer.h"
#include "parser.h"
#include "codegen.h"
#include "vm.h"


int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <source.mp>\n", argv[0]);
        return 1;
    }
    
    char* src = read_file(argv[1]);

    // Token 生成
    Lexer lex;
    init_lexer(&lex, src);
    
    // 语法分析
    Parser parser;
    init_parser(&parser);

    // 生成语法树
    Token t;
    do {
        t = next_token(&lex);
        add_token(&parser, t);
    } while (t.type != TOK_EOF);

    parser.pos = 0;
    Node* ast = parse_program(&parser);

    Bytecode bc;
    init_bytecode(&bc);
    generate_bytecode(&bc, ast);

    VM vm;
    init_vm(&vm, &bc);
    run_vm(&vm, &bc);

    free(src);
    return 0;
}
