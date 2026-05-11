# Demo Commands — JSON to XML Translator

This file contains the commands to be used during the demo. All commands are formatted for **PowerShell**.

---

## 1. Environment Setup
Run this once in your terminal session to ensure the compiler tools are accessible:
```powershell
$env:PATH = "C:\msys64\mingw64\bin;C:\msys64\usr\bin;$env:PATH"
```

---

## 2. Build and Clean
**To build the project:**
```powershell
make
```

**To clean generated files:**
```powershell
make clean
```

---

## 3. Automated Testing
**Run all 7 test cases at once:**
```powershell
make test
```

---

## 4. Manual Demo (JSON to XML)
Run individual test cases to show the translation logic.

**Test 1: Simple Object**
```powershell
Get-Content tests/test1_simple.json | ./json2xml.exe
```

**Test 2: Arrays**
```powershell
Get-Content tests/test2_array.json | ./json2xml.exe
```

**Test 4: Null Values (Self-closing tags)**
```powershell
Get-Content tests/test4_null.json | ./json2xml.exe
```

**Test 7: Special Characters (Escaping & Unicode)**
```powershell
Get-Content tests/test7_special_chars.json | ./json2xml.exe
```

---

## 5. Bonus Feature: AST Visualization
Use the `--ast` flag to show the internal tree structure before the XML output.

```powershell
Get-Content tests/test3_nested.json | ./json2xml.exe --ast
```

---

## 6. Error Handling
Demonstrate how the parser detects lexical and syntax errors.

**Invalid character error:**
```powershell
echo '{ "key": @bad }' | ./json2xml.exe
```

**Syntax error (missing colon):**
```powershell
echo '{ "key" "value" }' | ./json2xml.exe
```

**Unterminated string:**
```powershell
echo '{ "key": "unterminated }' | ./json2xml.exe
```
