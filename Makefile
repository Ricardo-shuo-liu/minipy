# 编译器与编译参数
CC = gcc
CFLAGS = -Wall -Wextra -g -I./Include

# 最终可执行文件名
TARGET = minipy

# 自动抓取src下全部c文件，新增源文件不用改这里
SRC := $(wildcard src/*.c)
# src/xxx.c 转为 obj/xxx.o
OBJ := $(patsubst src/%.c,obj/%.o,$(SRC))

# 所有头文件依赖
HEADERS := $(wildcard Include/*.h)

# 默认目标all依赖test_all，编译完自动跑全部测试
all: test_all

# 链接所有目标文件生成程序
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ)

# 编译规则：src/xxx.c -> obj/xxx.o
obj/%.o: src/%.c $(HEADERS)
	@mkdir -p obj
	$(CC) $(CFLAGS) -c $< -o $@

# 批量运行全部测试脚本
test_all: $(TARGET)
	@echo "======== 开始执行全部测试脚本 ========"
	@for file in scripts/*.mp; do \
		echo; \
		echo "---------------- $$file ----------------"; \
		./$(TARGET) $$file; \
	done
	@echo;
	@echo "======== 所有测试执行完毕 ========"


# 清理编译产物
clean:
	rm -rf obj $(TARGET)

# gdb调试
debug: $(TARGET)
	gdb ./$(TARGET)

# 发布优化版本
release: CFLAGS += -O2
release: clean all

# 声明伪目标
.PHONY: all clean run debug release test_all
