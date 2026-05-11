# JSON to XML Translator

**CS-4031 Compiler Construction — Assignment 04**  

**Language:** C (with Flex & Bison)

### Run All Tests 
```bash
$env:PATH = "C:\msys64\mingw64\bin;C:\msys64\usr\bin;$env:PATH"; make test
```

---
<img width="551" height="232" alt="image" src="https://github.com/user-attachments/assets/0030b9d1-d476-4723-8c33-880d805d10cd" />





## Overview

A compiler-style translator that reads valid JSON input, parses it using **Flex** (lexer) and **Bison** (parser), constructs an **Abstract Syntax Tree (AST)**, and converts the input into well-formed **XML** output.

### Pipeline

```
JSON Input → Lexer (Flex) → Parser (Bison) → AST → XML Generator → XML Output
```

---
<img width="616" height="281" alt="image" src="https://github.com/user-attachments/assets/30936396-3111-4b05-b3da-fcb9bcddda46" />
## Build Instructions

### Prerequisites

- **Flex** (≥ 2.6)
- **Bison** (≥ 3.0)
- **GCC** (any recent version)
- **Make**

On Windows with MSYS2:
```bash
pacman -S mingw-w64-x86_64-gcc flex bison make
```

### Building

```bash
make
```

This will:
1. Run Bison on `parser.y` → generates `parser.tab.c` and `parser.tab.h`
2. Run Flex on `scanner.l` → generates `lex.yy.c`
3. Compile all C files and link into `json2xml`

### Cleaning

```bash
make clean
```

---

## Run Instructions

```bash
./json2xml < input.json
```

The program reads JSON from **stdin** and prints XML to **stdout**.

### Optional: Display AST (Bonus)

```bash
./json2xml --ast < input.json
```

This prints the AST structure before the XML output.

---

## AST Structure

The AST uses a tagged-node design with the following node types:

| Node Type     | Description                          | Payload                        |
|---------------|--------------------------------------|--------------------------------|
| `NODE_OBJECT` | JSON object `{ ... }`               | Children = list of PAIR nodes  |
| `NODE_ARRAY`  | JSON array `[ ... ]`                | Children = list of value nodes |
| `NODE_STRING` | String literal `"text"`             | `string_val`                   |
| `NODE_NUMBER` | Number (int, float, scientific)     | `number_val`                   |
| `NODE_BOOL`   | `true` / `false`                    | `bool_val` (1 or 0)           |
| `NODE_NULL`   | `null`                              | (no payload)                   |
| `NODE_PAIR`   | Key-value pair `"key": value`       | `string_val` = key, child = value |

Each node has a dynamic array of children, allowing arbitrary nesting depth.

### Example AST

For the input:
```json
{"author": {"uid": "u1", "name": "Sara"}, "published": true}
```
<img width="688" height="433" alt="image" src="https://github.com/user-attachments/assets/e714f5b8-3238-4f14-a78a-d6c11fb06a18" />
<img width="391" height="161" alt="image" src="https://github.com/user-attachments/assets/f3f92101-8c2a-4164-bfb4-a475bbe465b7" />

The AST is:
```
OBJECT (2 pairs)
  PAIR: "author" =>
    OBJECT (2 pairs)
      PAIR: "uid" =>
        STRING: "u1"
      PAIR: "name" =>
        STRING: "Sara"
  PAIR: "published" =>
    BOOL: true
```

---
<img width="401" height="358" alt="image" src="https://github.com/user-attachments/assets/7b50e079-7b08-42b8-a5c4-5cff75bf9900" />

## Conversion Rules

| JSON Construct | XML Output                                  |
|----------------|---------------------------------------------|
| Root value     | Wrapped in `<root>...</root>`              |
| Object key     | Becomes XML tag name                        |
| String value   | Text content inside tag                     |
| Number value   | Text content (integers printed without `.0`)|
| Boolean        | `true` or `false` as text                   |
| Null           | Self-closing tag `<tag/>`                   |
| Array elements | Wrapped in `<item>` tags                    |
| Nested objects | Preserved as nested XML elements            |

---

## Test Files

| Test File               | Description                                   |
|-------------------------|-----------------------------------------------|
| `test1_simple.json`     | Simple object with id, name, age              |
| `test2_array.json`      | Object with an array of strings               |
| `test3_nested.json`     | Nested object with boolean                    |
| `test4_null.json`       | Object with null value                        |
| `test5_complex.json`    | Deep nesting, arrays of objects, mixed types  |
| `test6_booleans.json`   | Booleans, floats, arrays, null combined       |
| `test7_special_chars.json` | XML escaping, unicode, escape sequences    |

Run all tests:
```bash
make test
```

Expected outputs are in the `expected_output/` directory.

---

## Error Handling

The translator reports errors at the first lexical or syntax error and exits with code 1:

- **Lexical errors:** Invalid characters, invalid escape sequences, unterminated strings
- **Syntax errors:** Unexpected tokens, missing colons, trailing commas, etc.

Error messages include **line and column numbers** (bonus feature):
```
Error: unexpected character '@' at line 1, column 10
Error: syntax error at line 1, column 11
```

---
<img width="713" height="43" alt="image" src="https://github.com/user-attachments/assets/ff1f4f70-a01f-43c1-9744-19814ffee9bd" />

## Bonus Features Implemented

| Feature                        | Status |
|--------------------------------|--------|
| Pretty-printed XML             |  (+2) |
| AST printing (`--ast` flag)    |  (+2) |
| Column-based error detail      |  (+2) |
| Unicode escape support (`\uXXXX`) |  (+2) |
| Scientific notation support    |  (+2) |

**Total Bonus: +10 marks**

---
<img width="539" height="145" alt="image" src="https://github.com/user-attachments/assets/d8e95140-93f6-465e-8bd7-8972ed3e2b65" />

## File Structure

```
├── scanner.l              # Flex lexer specification
├── parser.y               # Bison parser grammar
├── ast.h                  # AST node definitions and utilities
├── xmlgen.h               # XML generation from AST
├── main.c                 # Program entry point
├── Makefile               # Build system
├── README.md              # This file
├── tests/                 # JSON test input files
│   ├── test1_simple.json
│   ├── test2_array.json
│   ├── test3_nested.json
│   ├── test4_null.json
│   ├── test5_complex.json
│   ├── test6_booleans.json
│   └── test7_special_chars.json
└── expected_output/       # Expected XML outputs
    ├── test1_expected.xml
    ├── test2_expected.xml
    ├── test3_expected.xml
    ├── test4_expected.xml
    ├── test5_expected.xml
    ├── test6_expected.xml
    └── test7_expected.xml
```

---

## Assumptions & Limitations

- JSON keys are assumed to be valid XML tag names (as stated in assignment).
- The program reads from **stdin only** (no file path argument).
- Maximum string literal length: 65,536 characters.
- Surrogate pairs (`\uD800`–`\uDFFF`) are not explicitly handled.
