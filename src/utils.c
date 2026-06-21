#include "utils.h"

void error(const char* msg, int line)
{
    if(line == 0)
    {
        fprintf(stderr,"Can't find file: %s\n",msg);
        exit(EXIT_FAILURE);
    }
    else
    {
        fprintf(stderr, "Error at line %d: %s\n", line, msg);
        exit(EXIT_FAILURE);
    } 
}


char* read_file(const char* filename)
{
    
    FILE* fp = fopen(filename, "r");

    if (!fp)
    {
        error("无法打开文件", 0);
    }
    
    fseek(fp, 0, SEEK_END);
    long file_len = ftell(fp);
    rewind(fp);

    char* buf = malloc(file_len + 1);
    fread(buf, sizeof(char), file_len, fp);
    buf[file_len] = '\0';

    fclose(fp);
    return buf;
}
