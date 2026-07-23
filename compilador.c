#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <math.h>

#define MEM_SIZE 1000
#define MAX_LINE 512
#define MAX_SYMBOLS 500
#define MAX_PATCHES 500
#define MAX_TOKENS 256
#define MAX_STACK 256
#define MAX_NAME 64
#define MAX_SOURCE_LINES 1000

/* Opcodes compatibles con el Simpletron proporcionado */
#define READ 10
#define WRITE 11
#define READ_STRING 12
#define WRITE_STRING 13
#define NEWLINE 14
#define READ_FLOAT 15
#define WRITE_FLOAT 16
#define LOAD 20
#define STORE 21
#define ADD 30
#define SUBTRACT 31
#define DIVIDE 32
#define MULTIPLY 33
#define MODULO 34
#define POWER 35
#define BRANCH 40
#define BRANCHNEG 41
#define BRANCHZERO 42
#define HALT 43

typedef enum {
    SYM_LINE,
    SYM_VAR,
    SYM_CONST
} SymbolType;

typedef struct {
    SymbolType type;
    int lineNumber;
    char name[MAX_NAME];
    double value;
    int address;
} Symbol;

typedef struct {
    int instructionIndex;
    int targetLine;
} Patch;

typedef enum {
    TK_NUMBER,
    TK_IDENTIFIER,
    TK_OPERATOR,
    TK_LPAREN,
    TK_RPAREN
} TokenType;

typedef struct {
    TokenType type;
    char text[MAX_NAME];
    double number;
    char op;
} Token;

typedef struct {
    int number;
    char text[MAX_LINE];
} SourceLine;

typedef struct {
    double memory[MEM_SIZE];
    int instructionCounter;
    int dataCounter;
    Symbol symbols[MAX_SYMBOLS];
    int symbolCount;
    Patch patches[MAX_PATCHES];
    int patchCount;
    int tempAddresses[MAX_STACK];
    int tempCount;
    int errors;
} Compiler;

static void trim_newline(char *s)
{
    size_t n = strlen(s);
    while (n > 0 && (s[n - 1] == '\n' || s[n - 1] == '\r')) {
        s[--n] = '\0';
    }
}

static char *skip_spaces(char *p)
{
    while (*p && isspace((unsigned char)*p)) {
        p++;
    }
    return p;
}

static void rtrim(char *s)
{
    size_t n = strlen(s);
    while (n > 0 && isspace((unsigned char)s[n - 1])) {
        s[--n] = '\0';
    }
}

static int starts_with_word(const char *text, const char *word)
{
    size_t n = strlen(word);
    if (strncmp(text, word, n) != 0) {
        return 0;
    }
    return text[n] == '\0' || isspace((unsigned char)text[n]);
}

static void compiler_error(Compiler *c, int sourceLine, const char *message)
{
    if (sourceLine > 0) {
        fprintf(stderr, "Error en linea Simple %d: %s\n", sourceLine, message);
    } else {
        fprintf(stderr, "Error: %s\n", message);
    }
    c->errors++;
}

static int ensure_space(Compiler *c, int sourceLine)
{
    if (c->instructionCounter > c->dataCounter) {
        compiler_error(c, sourceLine, "la zona de instrucciones alcanzo la zona de datos");
        return 0;
    }
    return 1;
}

static int emit(Compiler *c, int opcode, int operand, int sourceLine)
{
    if (opcode < 0 || opcode > 99 || operand < 0 || operand >= MEM_SIZE) {
        compiler_error(c, sourceLine, "instruccion SML fuera de rango");
        return -1;
    }
    if (!ensure_space(c, sourceLine)) {
        return -1;
    }
    c->memory[c->instructionCounter] = opcode * 1000 + operand;
    return c->instructionCounter++;
}

static int find_line_symbol(const Compiler *c, int lineNumber)
{
    int i;
    for (i = 0; i < c->symbolCount; i++) {
        if (c->symbols[i].type == SYM_LINE && c->symbols[i].lineNumber == lineNumber) {
            return i;
        }
    }
    return -1;
}

static int find_var_symbol(const Compiler *c, const char *name)
{
    int i;
    for (i = 0; i < c->symbolCount; i++) {
        if (c->symbols[i].type == SYM_VAR && strcmp(c->symbols[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

static int find_const_symbol(const Compiler *c, double value)
{
    int i;
    for (i = 0; i < c->symbolCount; i++) {
        if (c->symbols[i].type == SYM_CONST && fabs(c->symbols[i].value - value) < 1e-9) {
            return i;
        }
    }
    return -1;
}

static int add_symbol(Compiler *c, Symbol symbol, int sourceLine)
{
    if (c->symbolCount >= MAX_SYMBOLS) {
        compiler_error(c, sourceLine, "tabla de simbolos llena");
        return -1;
    }
    c->symbols[c->symbolCount] = symbol;
    return c->symbolCount++;
}

static int add_line_symbol(Compiler *c, int lineNumber, int address)
{
    Symbol s;
    if (find_line_symbol(c, lineNumber) >= 0) {
        compiler_error(c, lineNumber, "numero de linea repetido");
        return -1;
    }
    memset(&s, 0, sizeof(s));
    s.type = SYM_LINE;
    s.lineNumber = lineNumber;
    s.address = address;
    return add_symbol(c, s, lineNumber);
}

static int allocate_data(Compiler *c, double initialValue, int sourceLine)
{
    int address;
    if (!ensure_space(c, sourceLine)) {
        return -1;
    }
    address = c->dataCounter--;
    c->memory[address] = initialValue;
    return address;
}

static int get_or_add_var(Compiler *c, const char *name, int sourceLine)
{
    int index = find_var_symbol(c, name);
    Symbol s;
    int address;
    if (index >= 0) {
        return c->symbols[index].address;
    }
    address = allocate_data(c, 0.0, sourceLine);
    if (address < 0) {
        return -1;
    }
    memset(&s, 0, sizeof(s));
    s.type = SYM_VAR;
    strncpy(s.name, name, sizeof(s.name) - 1);
    s.address = address;
    if (add_symbol(c, s, sourceLine) < 0) {
        return -1;
    }
    return address;
}

static int get_or_add_const(Compiler *c, double value, int sourceLine)
{
    int index = find_const_symbol(c, value);
    Symbol s;
    int address;
    if (index >= 0) {
        return c->symbols[index].address;
    }
    address = allocate_data(c, value, sourceLine);
    if (address < 0) {
        return -1;
    }
    memset(&s, 0, sizeof(s));
    s.type = SYM_CONST;
    s.value = value;
    s.address = address;
    if (add_symbol(c, s, sourceLine) < 0) {
        return -1;
    }
    return address;
}

static int alloc_temp(Compiler *c, int sourceLine)
{
    int address = allocate_data(c, 0.0, sourceLine);
    if (address >= 0 && c->tempCount < MAX_STACK) {
        c->tempAddresses[c->tempCount++] = address;
    }
    return address;
}

static void add_patch(Compiler *c, int instructionIndex, int targetLine, int sourceLine)
{
    if (instructionIndex < 0) {
        return;
    }
    if (c->patchCount >= MAX_PATCHES) {
        compiler_error(c, sourceLine, "demasiados saltos pendientes");
        return;
    }
    c->patches[c->patchCount].instructionIndex = instructionIndex;
    c->patches[c->patchCount].targetLine = targetLine;
    c->patchCount++;
}

static int emit_branch_to_line(Compiler *c, int opcode, int targetLine, int sourceLine)
{
    int sym = find_line_symbol(c, targetLine);
    int pos;
    if (sym >= 0) {
        return emit(c, opcode, c->symbols[sym].address, sourceLine);
    }
    pos = emit(c, opcode, 0, sourceLine);
    add_patch(c, pos, targetLine, sourceLine);
    return pos;
}

static int valid_identifier(const char *s)
{
    int i;
    if (!isalpha((unsigned char)s[0]) && s[0] != '_') {
        return 0;
    }
    for (i = 1; s[i]; i++) {
        if (!isalnum((unsigned char)s[i]) && s[i] != '_') {
            return 0;
        }
    }
    return 1;
}

static int tokenize_expression(Compiler *c, const char *expr, Token tokens[], int *count, int sourceLine)
{
    const char *p = expr;
    int n = 0;
    int expectOperand = 1;

    while (*p) {
        Token t;
        memset(&t, 0, sizeof(t));
        while (*p && isspace((unsigned char)*p)) {
            p++;
        }
        if (!*p) {
            break;
        }
        if (n >= MAX_TOKENS) {
            compiler_error(c, sourceLine, "expresion demasiado larga");
            return 0;
        }

        if (isdigit((unsigned char)*p) || *p == '.' ||
            ((*p == '+' || *p == '-') && expectOperand &&
             (isdigit((unsigned char)p[1]) || p[1] == '.'))) {
            char *end;
            errno = 0;
            t.number = strtod(p, &end);
            if (p == end || errno == ERANGE || !isfinite(t.number)) {
                compiler_error(c, sourceLine, "numero invalido en expresion");
                return 0;
            }
            t.type = TK_NUMBER;
            snprintf(t.text, sizeof(t.text), "%.15g", t.number);
            tokens[n++] = t;
            p = end;
            expectOperand = 0;
            continue;
        }

        if (isalpha((unsigned char)*p) || *p == '_') {
            int len = 0;
            t.type = TK_IDENTIFIER;
            while ((isalnum((unsigned char)*p) || *p == '_') && len < MAX_NAME - 1) {
                t.text[len++] = *p++;
            }
            t.text[len] = '\0';
            tokens[n++] = t;
            expectOperand = 0;
            continue;
        }

        if (strchr("+-*/%^", *p)) {
            t.type = TK_OPERATOR;
            t.op = *p++;
            tokens[n++] = t;
            expectOperand = 1;
            continue;
        }

        if (*p == '(') {
            t.type = TK_LPAREN;
            t.op = *p++;
            tokens[n++] = t;
            expectOperand = 1;
            continue;
        }

        if (*p == ')') {
            t.type = TK_RPAREN;
            t.op = *p++;
            tokens[n++] = t;
            expectOperand = 0;
            continue;
        }

        compiler_error(c, sourceLine, "caracter invalido en expresion");
        return 0;
    }

    if (n == 0) {
        compiler_error(c, sourceLine, "expresion vacia");
        return 0;
    }
    *count = n;
    return 1;
}

static int precedence(char op)
{
    switch (op) {
        case '^': return 4;
        case '*':
        case '/':
        case '%': return 3;
        case '+':
        case '-': return 2;
        default: return 0;
    }
}

static int right_associative(char op)
{
    return op == '^';
}

static int to_postfix(Compiler *c, Token input[], int inputCount,
                      Token output[], int *outputCount, int sourceLine)
{
    Token stack[MAX_STACK];
    int top = -1;
    int out = 0;
    int i;

    for (i = 0; i < inputCount; i++) {
        Token t = input[i];
        if (t.type == TK_NUMBER || t.type == TK_IDENTIFIER) {
            output[out++] = t;
        } else if (t.type == TK_OPERATOR) {
            while (top >= 0 && stack[top].type == TK_OPERATOR &&
                   (precedence(stack[top].op) > precedence(t.op) ||
                    (precedence(stack[top].op) == precedence(t.op) &&
                     !right_associative(t.op)))) {
                output[out++] = stack[top--];
            }
            stack[++top] = t;
        } else if (t.type == TK_LPAREN) {
            stack[++top] = t;
        } else if (t.type == TK_RPAREN) {
            int found = 0;
            while (top >= 0) {
                if (stack[top].type == TK_LPAREN) {
                    top--;
                    found = 1;
                    break;
                }
                output[out++] = stack[top--];
            }
            if (!found) {
                compiler_error(c, sourceLine, "parentesis desbalanceados");
                return 0;
            }
        }
        if (top >= MAX_STACK - 1 || out >= MAX_TOKENS) {
            compiler_error(c, sourceLine, "expresion demasiado compleja");
            return 0;
        }
    }

    while (top >= 0) {
        if (stack[top].type == TK_LPAREN || stack[top].type == TK_RPAREN) {
            compiler_error(c, sourceLine, "parentesis desbalanceados");
            return 0;
        }
        output[out++] = stack[top--];
    }

    *outputCount = out;
    return 1;
}

static int token_address(Compiler *c, const Token *t, int sourceLine)
{
    if (t->type == TK_NUMBER) {
        return get_or_add_const(c, t->number, sourceLine);
    }
    if (t->type == TK_IDENTIFIER) {
        if (!valid_identifier(t->text)) {
            compiler_error(c, sourceLine, "identificador invalido");
            return -1;
        }
        return get_or_add_var(c, t->text, sourceLine);
    }
    compiler_error(c, sourceLine, "operando invalido");
    return -1;
}

static int compile_expression(Compiler *c, const char *expr, int sourceLine)
{
    Token infix[MAX_TOKENS];
    Token postfix[MAX_TOKENS];
    int infixCount = 0;
    int postfixCount = 0;
    int stack[MAX_STACK];
    int top = -1;
    int i;

    if (!tokenize_expression(c, expr, infix, &infixCount, sourceLine)) {
        return -1;
    }
    if (!to_postfix(c, infix, infixCount, postfix, &postfixCount, sourceLine)) {
        return -1;
    }

    for (i = 0; i < postfixCount; i++) {
        Token *t = &postfix[i];
        if (t->type == TK_NUMBER || t->type == TK_IDENTIFIER) {
            int addr = token_address(c, t, sourceLine);
            if (addr < 0) {
                return -1;
            }
            if (top >= MAX_STACK - 1) {
                compiler_error(c, sourceLine, "pila de expresion llena");
                return -1;
            }
            stack[++top] = addr;
        } else if (t->type == TK_OPERATOR) {
            int left, right, temp, opcode;
            if (top < 1) {
                compiler_error(c, sourceLine, "faltan operandos en la expresion");
                return -1;
            }
            right = stack[top--];
            left = stack[top--];
            switch (t->op) {
                case '+': opcode = ADD; break;
                case '-': opcode = SUBTRACT; break;
                case '*': opcode = MULTIPLY; break;
                case '/': opcode = DIVIDE; break;
                case '%': opcode = MODULO; break;
                case '^': opcode = POWER; break;
                default:
                    compiler_error(c, sourceLine, "operador desconocido");
                    return -1;
            }
            temp = alloc_temp(c, sourceLine);
            if (temp < 0) {
                return -1;
            }
            if (emit(c, LOAD, left, sourceLine) < 0 ||
                emit(c, opcode, right, sourceLine) < 0 ||
                emit(c, STORE, temp, sourceLine) < 0) {
                return -1;
            }
            stack[++top] = temp;
        }
    }

    if (top != 0) {
        compiler_error(c, sourceLine, "expresion invalida");
        return -1;
    }
    return stack[0];
}

static int parse_target_line(const char *text, int *target)
{
    char extra;
    return sscanf(text, "%d %c", target, &extra) == 1 && *target >= 0;
}

static int find_relop(const char *s, int *pos, char relop[3])
{
    int i;
    int depth = 0;
    for (i = 0; s[i]; i++) {
        if (s[i] == '(') depth++;
        else if (s[i] == ')') depth--;
        if (depth != 0) continue;
        if ((s[i] == '<' || s[i] == '>' || s[i] == '=' || s[i] == '!') && s[i + 1] == '=') {
            relop[0] = s[i];
            relop[1] = '=';
            relop[2] = '\0';
            *pos = i;
            return 1;
        }
        if (s[i] == '<' || s[i] == '>') {
            relop[0] = s[i];
            relop[1] = '\0';
            *pos = i;
            return 1;
        }
    }
    return 0;
}

static int compile_condition(Compiler *c, const char *condition,
                             int targetLine, int sourceLine)
{
    int pos;
    char relop[3];
    char leftExpr[MAX_LINE];
    char rightExpr[MAX_LINE];
    int leftAddr, rightAddr;

    if (!find_relop(condition, &pos, relop)) {
        compiler_error(c, sourceLine, "condicion sin operador relacional");
        return 0;
    }

    if ((size_t)pos >= sizeof(leftExpr)) {
        compiler_error(c, sourceLine, "condicion demasiado larga");
        return 0;
    }
    memcpy(leftExpr, condition, (size_t)pos);
    leftExpr[pos] = '\0';
    strcpy(rightExpr, condition + pos + (int)strlen(relop));
    rtrim(leftExpr);
    memmove(rightExpr, skip_spaces(rightExpr), strlen(skip_spaces(rightExpr)) + 1);

    leftAddr = compile_expression(c, leftExpr, sourceLine);
    rightAddr = compile_expression(c, rightExpr, sourceLine);
    if (leftAddr < 0 || rightAddr < 0) {
        return 0;
    }

    if (strcmp(relop, "==") == 0) {
        emit(c, LOAD, leftAddr, sourceLine);
        emit(c, SUBTRACT, rightAddr, sourceLine);
        emit_branch_to_line(c, BRANCHZERO, targetLine, sourceLine);
    } else if (strcmp(relop, "!=") == 0) {
        int skip = c->instructionCounter + 4;
        emit(c, LOAD, leftAddr, sourceLine);
        emit(c, SUBTRACT, rightAddr, sourceLine);
        emit(c, BRANCHZERO, skip, sourceLine);
        emit_branch_to_line(c, BRANCH, targetLine, sourceLine);
    } else if (strcmp(relop, "<") == 0) {
        emit(c, LOAD, leftAddr, sourceLine);
        emit(c, SUBTRACT, rightAddr, sourceLine);
        emit_branch_to_line(c, BRANCHNEG, targetLine, sourceLine);
    } else if (strcmp(relop, "<=") == 0) {
        emit(c, LOAD, leftAddr, sourceLine);
        emit(c, SUBTRACT, rightAddr, sourceLine);
        emit_branch_to_line(c, BRANCHNEG, targetLine, sourceLine);
        emit_branch_to_line(c, BRANCHZERO, targetLine, sourceLine);
    } else if (strcmp(relop, ">") == 0) {
        emit(c, LOAD, rightAddr, sourceLine);
        emit(c, SUBTRACT, leftAddr, sourceLine);
        emit_branch_to_line(c, BRANCHNEG, targetLine, sourceLine);
    } else if (strcmp(relop, ">=") == 0) {
        emit(c, LOAD, rightAddr, sourceLine);
        emit(c, SUBTRACT, leftAddr, sourceLine);
        emit_branch_to_line(c, BRANCHNEG, targetLine, sourceLine);
        emit_branch_to_line(c, BRANCHZERO, targetLine, sourceLine);
    } else {
        compiler_error(c, sourceLine, "operador relacional no soportado");
        return 0;
    }
    return 1;
}

static int parse_if_statement(Compiler *c, char *rest, int sourceLine)
{
    char *gotoPos = NULL;
    char *p;
    int targetLine;

    for (p = rest; *p; p++) {
        if ((p == rest || isspace((unsigned char)p[-1])) &&
            strncmp(p, "goto", 4) == 0 &&
            (p[4] == '\0' || isspace((unsigned char)p[4]))) {
            gotoPos = p;
        }
    }
    if (!gotoPos) {
        compiler_error(c, sourceLine, "if debe terminar con goto <linea>");
        return 0;
    }
    *gotoPos = '\0';
    rtrim(rest);
    gotoPos = skip_spaces(gotoPos + 4);
    if (!parse_target_line(gotoPos, &targetLine)) {
        compiler_error(c, sourceLine, "linea destino invalida en if");
        return 0;
    }
    return compile_condition(c, rest, targetLine, sourceLine);
}

static int compile_statement(Compiler *c, int sourceLine, char *statement)
{
    char *p = skip_spaces(statement);

    if (*p == '\0' || starts_with_word(p, "rem")) {
        return 1;
    }

    if (starts_with_word(p, "inputfloat")) {
        char name[MAX_NAME];
        int addr;
        if (sscanf(p + 10, "%63s", name) != 1 || !valid_identifier(name)) {
            compiler_error(c, sourceLine, "uso: inputfloat variable");
            return 0;
        }
        addr = get_or_add_var(c, name, sourceLine);
        return emit(c, READ_FLOAT, addr, sourceLine) >= 0;
    }

    if (starts_with_word(p, "input")) {
        char name[MAX_NAME];
        int addr;
        if (sscanf(p + 5, "%63s", name) != 1 || !valid_identifier(name)) {
            compiler_error(c, sourceLine, "uso: input variable");
            return 0;
        }
        addr = get_or_add_var(c, name, sourceLine);
        return emit(c, READ, addr, sourceLine) >= 0;
    }

    if (starts_with_word(p, "printfloat")) {
        char *expr = skip_spaces(p + 10);
        int addr = compile_expression(c, expr, sourceLine);
        return addr >= 0 && emit(c, WRITE_FLOAT, addr, sourceLine) >= 0;
    }

    if (starts_with_word(p, "print")) {
        char *expr = skip_spaces(p + 5);
        int addr = compile_expression(c, expr, sourceLine);
        return addr >= 0 && emit(c, WRITE, addr, sourceLine) >= 0;
    }

    if (strcmp(p, "newline") == 0) {
        return emit(c, NEWLINE, 0, sourceLine) >= 0;
    }

    if (starts_with_word(p, "goto")) {
        int targetLine;
        char *rest = skip_spaces(p + 4);
        if (!parse_target_line(rest, &targetLine)) {
            compiler_error(c, sourceLine, "uso: goto numeroDeLinea");
            return 0;
        }
        return emit_branch_to_line(c, BRANCH, targetLine, sourceLine) >= 0;
    }

    if (starts_with_word(p, "if")) {
        char buffer[MAX_LINE];
        strncpy(buffer, skip_spaces(p + 2), sizeof(buffer) - 1);
        buffer[sizeof(buffer) - 1] = '\0';
        return parse_if_statement(c, buffer, sourceLine);
    }

    if (starts_with_word(p, "let")) {
        char *rest = skip_spaces(p + 3);
        char *eq = strchr(rest, '=');
        char name[MAX_NAME];
        int addr, resultAddr;
        size_t len;
        if (!eq) {
            compiler_error(c, sourceLine, "let requiere el simbolo =");
            return 0;
        }
        len = (size_t)(eq - rest);
        while (len > 0 && isspace((unsigned char)rest[len - 1])) len--;
        if (len == 0 || len >= sizeof(name)) {
            compiler_error(c, sourceLine, "nombre de variable invalido en let");
            return 0;
        }
        memcpy(name, rest, len);
        name[len] = '\0';
        if (!valid_identifier(name)) {
            compiler_error(c, sourceLine, "nombre de variable invalido en let");
            return 0;
        }
        addr = get_or_add_var(c, name, sourceLine);
        resultAddr = compile_expression(c, skip_spaces(eq + 1), sourceLine);
        if (addr < 0 || resultAddr < 0) {
            return 0;
        }
        return emit(c, LOAD, resultAddr, sourceLine) >= 0 &&
               emit(c, STORE, addr, sourceLine) >= 0;
    }

    if (strcmp(p, "end") == 0) {
        return emit(c, HALT, 0, sourceLine) >= 0;
    }

    compiler_error(c, sourceLine, "instruccion Simple desconocida");
    return 0;
}

static int compare_source_lines(const void *a, const void *b)
{
    const SourceLine *x = (const SourceLine *)a;
    const SourceLine *y = (const SourceLine *)b;
    return (x->number > y->number) - (x->number < y->number);
}

static int load_source(const char *filename, SourceLine lines[], int *count)
{
    FILE *f = fopen(filename, "r");
    char buffer[MAX_LINE];
    int n = 0;
    int physical = 0;

    if (!f) {
        fprintf(stderr, "No se pudo abrir %s\n", filename);
        return 0;
    }

    while (fgets(buffer, sizeof(buffer), f)) {
        char *p;
        char *end;
        long number;
        physical++;
        trim_newline(buffer);
        p = skip_spaces(buffer);
        if (*p == '\0') continue;
        errno = 0;
        number = strtol(p, &end, 10);
        if (p == end || errno == ERANGE || number < 0 || number > 999999) {
            fprintf(stderr, "Linea fisica %d: falta un numero de linea Simple valido\n", physical);
            fclose(f);
            return 0;
        }
        if (n >= MAX_SOURCE_LINES) {
            fprintf(stderr, "El programa excede %d lineas\n", MAX_SOURCE_LINES);
            fclose(f);
            return 0;
        }
        lines[n].number = (int)number;
        strncpy(lines[n].text, skip_spaces(end), sizeof(lines[n].text) - 1);
        lines[n].text[sizeof(lines[n].text) - 1] = '\0';
        n++;
    }
    fclose(f);
    qsort(lines, (size_t)n, sizeof(SourceLine), compare_source_lines);
    *count = n;
    return 1;
}

static void resolve_patches(Compiler *c)
{
    int i;
    for (i = 0; i < c->patchCount; i++) {
        int sym = find_line_symbol(c, c->patches[i].targetLine);
        int pos = c->patches[i].instructionIndex;
        int opcode;
        if (sym < 0) {
            char msg[128];
            snprintf(msg, sizeof(msg), "salto hacia linea inexistente %d", c->patches[i].targetLine);
            compiler_error(c, 0, msg);
            continue;
        }
        opcode = (int)llround(c->memory[pos]) / 1000;
        c->memory[pos] = opcode * 1000 + c->symbols[sym].address;
    }
}

static int write_output(const Compiler *c, const char *filename)
{
    FILE *f;
    int i;
    if (c->errors != 0) {
        return 0;
    }
    f = fopen(filename, "w");
    if (!f) {
        fprintf(stderr, "No se pudo crear %s\n", filename);
        return 0;
    }

    for (i = 0; i < MEM_SIZE; i++) {
        double value = c->memory[i];
        if (fabs(value - round(value)) < 1e-9) {
            fprintf(f, "%.0f\n", value);
        } else {
            fprintf(f, "%.15g\n", value);
        }
    }
    fclose(f);
    return 1;
}

static void print_summary(const Compiler *c, const char *output)
{
    printf("\nCompilacion terminada correctamente.\n");
    printf("Instrucciones generadas: %d\n", c->instructionCounter);
    printf("Primera direccion de datos utilizada: %d\n", c->dataCounter + 1);
    printf("Archivo SML: %s\n", output);
}

int main(int argc, char *argv[])
{
    const char *input = argc > 1 ? argv[1] : "programa.simple";
    const char *output = argc > 2 ? argv[2] : "programa.simp";
    SourceLine lines[MAX_SOURCE_LINES];
    int lineCount = 0;
    int i;
    Compiler c;

    memset(&c, 0, sizeof(c));
    c.dataCounter = MEM_SIZE - 1;

    if (!load_source(input, lines, &lineCount)) {
        return EXIT_FAILURE;
    }

    for (i = 0; i < lineCount; i++) {
        if (add_line_symbol(&c, lines[i].number, c.instructionCounter) < 0) {
            continue;
        }
        compile_statement(&c, lines[i].number, lines[i].text);
    }

    resolve_patches(&c);

    if (c.errors > 0) {
        fprintf(stderr, "\nLa compilacion termino con %d error(es).\n", c.errors);
        return EXIT_FAILURE;
    }

    if (!write_output(&c, output)) {
        return EXIT_FAILURE;
    }

    print_summary(&c, output);
    return EXIT_SUCCESS;
}
