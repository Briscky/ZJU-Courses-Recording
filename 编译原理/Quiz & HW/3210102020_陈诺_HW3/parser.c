#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

enum token {
    EOF_TOKEN, ID, INT, PLUS, MINUS, LPAREN, RPAREN, PRINT, COMMA, TIMES, DIV, SEMICOLON, ASSIGN
};
// Define a union to store values associated with tokens
union tokenval {
    char* id;   // For identifiers
    int num;    // For numeric literals
};

enum token current_token;
union tokenval tokval;
char token_buffer[100];  // Buffer to hold string value of tokens
FILE *file;

typedef struct table {
    char* id;
    int value;
    struct table *tail;
} *Table_;

Table_ Table(char* id, int value, Table_ tail) {
    Table_ t = malloc(sizeof(*t));
    t->id = strdup(id);
    t->value = value;
    t->tail = tail;
    return t;
}

Table_ table = NULL;

int lookup(Table_ table, char* id) {
    //printf("id: %s\n", id);
    assert(table != NULL);
    if (strcmp(id, table->id) == 0) return table->value;
    else if (table->tail == NULL) {
        fprintf(stderr, "Error: Identifier not found: %s\n", id);
        return -1;
    }
    else return lookup(table->tail, id);
}

void update(Table_ *tabptr, char* id, int value) {
    *tabptr = Table(id, value, *tabptr);
}

enum token next_token() {
    char ch;
    int i = 0;

    // Skip whitespace
    while ((ch = fgetc(file)) != EOF && isspace(ch));

    if (ch == EOF) {
        current_token = EOF_TOKEN;  // Update current_token
    } else if (isdigit(ch)) {
        // Read a number
        do {
            token_buffer[i++] = ch;
            ch = fgetc(file);
        } while (isdigit(ch));
        ungetc(ch, file);  // Push back the last read character
        token_buffer[i] = '\0';
        tokval.num = atoi(token_buffer);
        current_token = INT;  // Update current_token
    } else if (isalpha(ch)) {
        // Read an identifier
        do {
            token_buffer[i++] = ch;
            ch = fgetc(file);
        } while (isalnum(ch));
        ungetc(ch, file); // Push back the last read character
        token_buffer[i] = '\0';
        tokval.id = strdup(token_buffer);
        if (strcmp(tokval.id, "print") == 0) {
            current_token = PRINT;  // Update current_token if the identifier is "print"
        } else {
            current_token = ID;  // Update current_token
        }
    } else {
        // Handle single character tokens
        switch (ch) {
            case '+': current_token = PLUS; break;
            case '-': current_token = MINUS; break;
            case '*': current_token = TIMES; break;
            case '/': current_token = DIV; break;
            case '(': current_token = LPAREN; break;
            case ')': current_token = RPAREN; break;
            case ',': current_token = COMMA; break;
            case ';': current_token = SEMICOLON; break;
            case '=': current_token = ASSIGN; break;
            default:
                fprintf(stderr, "Unrecognized token %c\n", ch);
                exit(1);
        }
    }

    return current_token;  // Return the current token
}

void match_keywords(enum token expected) {
    if (current_token == expected) {
        current_token = next_token();  // Move to the next token
        return;
    } else {
        fprintf(stderr, "Syntax error: expected %d but found %d\n", expected, current_token);
        exit(1);
    }
}

void match(enum token expected, char *buffer) {
    if (current_token == expected) {
        if (expected == ID) {
            strcpy(buffer, token_buffer);  // Copy the token's value
        }
        current_token = next_token();  // Move to the next token
    } else {
        fprintf(stderr, "Syntax error: expected %d but found %d\n", expected, current_token);
        exit(1);
    }
}


// Forward declarations of parsing functions
void prog();
void stm();
void exps();
int expression();
int term();
int factor();

// Definitions of parsing functions
void prog() {
    stm();
}

void stm() {
    char id[100];
    int val;

    current_token = next_token();
    while (current_token != EOF_TOKEN) {
        switch (current_token) {
            case ID:
                //printf("next token: %d\n", current_token);
                match(ID, id);  // Capture the ID
                //printf("id: %s\n", id);
                match_keywords(ASSIGN);
                //printf("next token: %d\n", current_token);
                val = expression();
                update(&table, id, val);
                break;
            case PRINT:
                //printf("next token: %d\n", current_token);
                match_keywords(PRINT);
                //printf("next token: %d\n", current_token);
                match_keywords(LPAREN);
                exps();
                match_keywords(RPAREN);
                printf("\n");
                break;
            default:
                //printf("next token: %d\n", current_token);
                stm();
                break;
        }
        match_keywords(SEMICOLON);  // Expect a semicolon after each statement
    }
}

void exps() {
    int result = expression();
    printf("%d ", result);
    while (current_token == COMMA) {
        match_keywords(COMMA);
        result = expression();
        printf("%d ", result);
        //current_token = next_token();
    }
}

int expression() {
    int result;
    result = term();  // Process the first term
    while (current_token == PLUS || current_token == MINUS) {  // Continue if the next token is '+' or '-'
        switch (current_token) {
            case PLUS:
                match_keywords(PLUS);  // Consume the '+' token
                result += term();  // Process the next term and add it to the result
                break;
            case MINUS:
                match_keywords(MINUS);  // Consume the '-' token
                result -= term();  // Process the next term and subtract it from the result
                break;
        }
    }
    return result;
}

int term() {
    int result = factor();  // Process the first factor
    while (current_token == TIMES || current_token == DIV) {  // Continue if the next token is '*' or '/'
        switch (current_token) {
            case TIMES:
                match_keywords(TIMES);  // Consume the '*' token
                result *= factor();  // Process the next factor and multiply it with the result
                break;
            case DIV:
                match_keywords(DIV);  // Consume the '/' token
                int divisor = factor();  // Process the next factor
                if (divisor == 0) {
                    fprintf(stderr, "Division by zero\n");
                    exit(1);
                }
                result /= divisor;  // Divide the result by the factor
                break;
        }
    }
    return result;
}

int factor() {
    int result;
    char temp[100];
    //printf("current token: %d\n", current_token);
    switch (current_token) {
        case INT:
            result = tokval.num;
            current_token = next_token();
            break;
        case ID:
            match(ID, temp);
            // printf("temp: %s\n", temp);
            // //打印table
            // Table_ t = table;
            // while (t != NULL) {
            //     printf("id: %s, value: %d\n", t->id, t->value);
            //     t = t->tail;
            // }
            result = lookup(table, temp);
            // printf("result: %d\n", result);
            // printf("current token: %d\n", current_token);
            break;
        case LPAREN:
            match_keywords(LPAREN);
            result = expression();
            match_keywords(RPAREN);
            break;
        default:
            fprintf(stderr, "Unexpected token in expression\n");
            exit(1);
    }
    return result;
}

int main() {
    file = fopen("input.txt", "r");
    if (file == NULL) {
        fprintf(stderr, "Could not open file\n");
        return 1;
    }
    prog();  // Start the parsing process
    fclose(file);
    return 0;
}
