/**
 * 定义文件读取和报错
 * 
 * 零散工具
 */


#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// 文件错位类
void error(const char* msg, int line);
// 读取文件内容
char* read_file(const char* filename);



#endif