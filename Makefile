# Makefile — JSON to XML Translator
# CS-4031 Compiler Construction — Assignment 04
# Author: Naveed Ahmed (i220889)
#
# Build: make
# Clean: make clean
# Test:  make test
#
# Note: Requires flex, bison, and gcc in PATH.
#       On Windows with MSYS2, add C:\msys64\mingw64\bin and
#       C:\msys64\usr\bin to your PATH before running make.

# ---- Tool settings ------------------------------------------------
FLEX   ?= flex
BISON  ?= bison
CC     ?= gcc

# ---- Compiler flags -----------------------------------------------
CFLAGS = -Wall -Wno-unused-function -g -I.

# ---- Targets -------------------------------------------------------
TARGET = json2xml

OBJS   = lex.yy.o parser.tab.o main.o

all: $(TARGET)

# Step 1: Generate parser from Bison grammar
parser.tab.c parser.tab.h: parser.y
	$(BISON) -d -v parser.y

# Step 2: Generate lexer from Flex specification
lex.yy.c: scanner.l parser.tab.h
	$(FLEX) scanner.l

# Step 3: Compile object files
lex.yy.o: lex.yy.c ast.h parser.tab.h
	$(CC) $(CFLAGS) -c lex.yy.c -o lex.yy.o

parser.tab.o: parser.tab.c ast.h
	$(CC) $(CFLAGS) -c parser.tab.c -o parser.tab.o

main.o: main.c ast.h xmlgen.h
	$(CC) $(CFLAGS) -c main.c -o main.o

# Step 4: Link
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) -lm

# ---- Test all JSON files -------------------------------------------
test: $(TARGET)
	@echo "===== Test 1: Simple Object ====="
	./$(TARGET) < tests/test1_simple.json
	@echo ""
	@echo "===== Test 2: Array of Scalars ====="
	./$(TARGET) < tests/test2_array.json
	@echo ""
	@echo "===== Test 3: Nested Object ====="
	./$(TARGET) < tests/test3_nested.json
	@echo ""
	@echo "===== Test 4: Null Values ====="
	./$(TARGET) < tests/test4_null.json
	@echo ""
	@echo "===== Test 5: Complex Nested ====="
	./$(TARGET) < tests/test5_complex.json
	@echo ""
	@echo "===== Test 6: Booleans and Mixed ====="
	./$(TARGET) < tests/test6_booleans.json
	@echo ""
	@echo "===== Test 7: Special Characters ====="
	./$(TARGET) < tests/test7_special_chars.json
	@echo ""

# ---- Clean generated files -----------------------------------------
clean:
	rm -f lex.yy.c parser.tab.c parser.tab.h parser.output
	rm -f lex.yy.o parser.tab.o main.o
	rm -f $(TARGET) $(TARGET).exe

.PHONY: all clean test
