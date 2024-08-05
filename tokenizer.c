#include "value.h"
#include "linkedlist.h"
#include "talloc.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifndef _TOKENIZER
#define _TOKENIZER

// processString
// args: None
// returns: newString - a string
// helper method for tokenize() to parse the body of a string. Throws an error and exits if the string is invalid.
char *processString() {
    char charRead;
    int index = 0;
    char *newString = talloc(301*sizeof(char));  // 301 bytes allocated since max token size of 300, plus null terminator
    newString[0] = '\"';
    index++;
    charRead = (char)fgetc(stdin);

    // continue reading char's from the input stream until end of string or end of file
    while (charRead != '\"') {
        
        // in the event there is no closing double-quote, throw syntax error
        if (charRead == EOF) { 
            printf("Syntax Error: Invalid String\n");
            texit(0);
        }
        newString[index] = charRead;
        index++;
        charRead = (char)fgetc(stdin);
    }
    newString[index] = '\"';
    index++;
    newString[index] = '\0';
    
    return newString;
}

// processNumber
// args: initialChar - a character
// returns: a Value of type INT_TYPE or DOUBLE_TYPE, containing an integer or double respectively
// helper method for tokenize(), to identify and tokenize signed and unsigned integer and double numbers
Value *processNumber(char initialChar) {
    char charRead = initialChar;
    int index = 0;
    char *newNumber = talloc(301*sizeof(char));  // 301 bytes allocated since max token size of 300, plus null terminator
    char *dump;  // dump location for excess string contents when converting strings to longs/doubles

    // if number is signed, include sign
    if (charRead == '+' || charRead == '-') {
        newNumber[index] = charRead;
        index++;
        charRead = (char)fgetc(stdin);
    }

    // build up number digit by digit, while reading consecutive digits
    while (48 <= charRead && charRead <= 57) {
        newNumber[index] = charRead;
        index++;
        charRead = (char)fgetc(stdin);
    }

    // create new INT_TYPE token containing the built-up number
    if (charRead == ' ' || charRead == EOF || charRead == '\n') {
        newNumber[index] = '\0';
        int number = strtol(newNumber, &dump, 10);
        Value *newToken = talloc(sizeof(Value));
        newToken -> type = INT_TYPE;
        newToken -> i = number;
        return newToken;

    // create new INT_TYPE token, but rewind stream by 1 so parentheses will be caught by tokenize()
    } else if (charRead == '(' || charRead == ')') {
        newNumber[index] = '\0';
        int number = strtol(newNumber, &dump, 10);
        Value *newToken = talloc(sizeof(Value));
        newToken -> type = INT_TYPE;
        newToken -> i = number;
        ungetc(charRead, stdin);
        return newToken;
    
    // Recognize . symbol to build double
    } else if (charRead == '.') {

        newNumber[index] = charRead;
        index++;
        charRead = (char)fgetc(stdin);

        // building up double portion of double number
        while (48 <= charRead && charRead <= 57) {
            newNumber[index] = charRead;
            index++;
            charRead = (char)fgetc(stdin);
        }

        // Creating new DOUBLE_TYPE token, similar to integer case
        if (charRead == ' ' || charRead == EOF || charRead == '\n') {
            newNumber[index] = '\0';
            double decimal = strtod(newNumber, &dump);
            Value *newToken = talloc(sizeof(Value));
            newToken -> type = DOUBLE_TYPE;
            newToken -> d = decimal;
            return newToken;
            
        // Create DOUBLE_TYPE token and rewind by one to catch parens
        } else if (charRead == '(' || charRead == ')') {
            newNumber[index] = '\0';
            double decimal = strtod(newNumber, &dump);
            Value *newToken = talloc(sizeof(Value));
            newToken -> type = DOUBLE_TYPE;
            newToken -> d = decimal;
            ungetc(charRead, stdin);
            return newToken;
            
    // if character read not in the language for numbers, throw syntax error and exit the program

        } else {
            printf("Syntax Error: Invalid double\n");
            texit(0);
        }

    } else {
        printf("Syntax Error: Invalid number\n");
        texit(0);
    }

    // return case will never trigger, but placed to avoid compiler warning
    return makeNull();
}

// processSymbol
// args: initialChar - a char
// returns: a Value of SYMBOL_TYPE containing the read symbol
// helper function for tokenize(), to identify and tokenize symbols
Value *processSymbol(char initialChar) {
    char charRead = initialChar;
    char *symbol = talloc(301*sizeof(char));
    int index = 0;
    symbol[0] = charRead;
    index++;
    charRead = (char)fgetc(stdin);

    // continue reading char's until charRead does not equal a suitable character to be contained in a symbol
    while ((65 <= charRead && charRead <= 90) || (97 <= charRead && charRead <= 122)
            || charRead == '!' || charRead == '$' || charRead == '%' || charRead == '&'
            || charRead == '*' || charRead == '/' || charRead == ':' || charRead == '<'
            || charRead == '=' || charRead == '>' || charRead == '?' || charRead == '~'
            || charRead == '_' || charRead == '^' || charRead == '+' || charRead == '-'
            || charRead == '.' || (48 <= charRead && charRead <= 57)) {

        symbol[index] = charRead;
        index++;
        charRead = (char)fgetc(stdin);

    }

    // create new SYMBOL_TYPE token containing the built-up symbol
    if (charRead == ' ' || charRead == EOF || charRead == '\n') {
        symbol[index] = '\0';
        Value *newToken = talloc(sizeof(Value));
        newToken -> type = SYMBOL_TYPE;
        newToken -> s = symbol;
        return newToken;

    // create new SYMBOL_TYPE token and rewind by one to catch parens
    } else if (charRead == '(' || charRead == ')') {
        symbol[index] = '\0';
        Value *newToken = talloc(sizeof(Value));
        newToken -> type = SYMBOL_TYPE;
        newToken -> s = symbol;
        ungetc(charRead, stdin);
        return newToken;
    
    // if character read not in the grammar for symbol, throw an error and exit the program
    } else {
        printf("Syntax Error: Invalid Symbol\n");
        texit(0);
    }

    return makeNull();
}

// tokenize
// args: None
// returns: a Value containing the first element in a linked-list of tokens
// reads characters from stdin, and creates the appropriate tokens (or throws a syntax error if it reads an unexpected character)
Value *tokenize() {
    char charRead;
    Value *list = makeNull();
    charRead = (char)fgetc(stdin);

    while (charRead != EOF) {

        // case: open parenthesis
        if (charRead == '(') {
            
            Value *newToken = talloc(sizeof(Value));
            newToken -> type = OPEN_TYPE;
            char *newString = talloc(2*sizeof(char));
            strcpy(newString, "(");
            newToken -> s = newString;
            list = cons(newToken, list);

        // case: close parenthesis
        } else if (charRead == ')') {

            Value *newToken = talloc(sizeof(Value));
            newToken -> type = CLOSE_TYPE;
            char *newString = talloc(2*sizeof(char));
            strcpy(newString, ")");
            newToken -> s = newString;
            list = cons(newToken, list);

        // case: string
        } else if (charRead == '\"') {

            char *processedString = processString();
            Value *newToken = talloc(sizeof(Value));
            newToken -> type = STR_TYPE;
            newToken -> s = processedString;
            list = cons(newToken, list);

        // case: unsigned integer
        } else if (48 <= charRead && charRead <= 57) {

            Value *newToken = processNumber(charRead);
            list = cons(newToken, list);

        // case: signed number, or + - symbol
        } else if (charRead == '+' || charRead == '-') {

            char sign = charRead;
            charRead = (char)fgetc(stdin);

            // subcase: +/- read as a symbol
            if (charRead == ' ' || charRead == EOF || charRead == '\n'
                || charRead == '(' || charRead == ')') {
                ungetc(charRead, stdin);
                Value *newToken = processSymbol(sign);
                list = cons(newToken, list);
            
            // subcase: +/- read as part of a number
            } else if (48 <= charRead && charRead <= 57) {
                ungetc(charRead, stdin);
                Value *newToken = processNumber(sign);
                list = cons(newToken, list);
                
            } else if (charRead == '.') {
                
                char point = charRead;
                charRead = (char)fgetc(stdin);
                if (48 <= charRead && charRead <= 57) {
                    ungetc(charRead, stdin);
                    ungetc(point, stdin);
                    Value *newToken = processNumber(sign);
                    list = cons(newToken, list);
                } else {
                    printf("Syntax Error: Invalid Symbol\n");
                    texit(0);
                }

            } else {
                printf("Syntax Error: Invalid Symbol\n");
                texit(0);
            }

        // case: unsigned double
        } else if (charRead == '.') {
            
            char point = charRead;
            charRead = (char)fgetc(stdin);
            
            if (48 <= charRead && charRead <= 57) {
                ungetc(charRead, stdin);
                Value *newToken = processNumber('.');
                list = cons(newToken, list);
            } else {
                printf("Syntax Error: Invalid double\n");
                texit(0);
            }

        // case: boolean
        } else if (charRead == '#') {
            charRead = (char)fgetc(stdin);
            if (charRead == 't') {
                Value *newToken = talloc(sizeof(Value));
                newToken -> type = BOOL_TYPE;
                newToken -> i = 1;
                list = cons(newToken, list);
            } else if (charRead == 'f') {
                Value *newToken = talloc(sizeof(Value));
                newToken -> type = BOOL_TYPE;
                newToken -> i = 0;
                list = cons(newToken, list);
            } else {
                printf("Syntax Error: Invalid Boolean\n");
                texit(0);
            }

        // case: symbol (omitting the singular + - case)
        } else if ((65 <= charRead && charRead <= 90) || (97 <= charRead && charRead <= 122)
                    || charRead == '!' || charRead == '$' || charRead == '%' || charRead == '&'
                    || charRead == '*' || charRead == '/' || charRead == ':' || charRead == '<'
                    || charRead == '=' || charRead == '>' || charRead == '?' || charRead == '~'
                    || charRead == '_' || charRead == '^') {

            Value* newToken = processSymbol(charRead);
            list = cons(newToken, list);
        
        // case: comment
        } else if (charRead == ';') {
            while (charRead != '\n' && charRead != EOF) {
                charRead = (char)fgetc(stdin);
            }

        // case: invalid non-whitespace or EOF read
        } else if (charRead != ' ' && charRead != '\n' && charRead != EOF) {
            printf("Syntax Error: Bad Syntax\n");
            texit(0);
        
        // case: space/newline/EOF    
        } else {
            // continue
        }
        
        charRead = (char)fgetc(stdin);
    }

    // reverse the list to put tokens in order
    Value *revList = reverse(list);
    return revList;
}

// displayTokens
// args: list - a pointer to a Value
// returns: nothing
// displayTokens() prints out each of the items in the linked list pointed to by list, along with their types.
void displayTokens(Value *list) {
    Value *currentItem = list;
    Value *currentCar;
    while (currentItem -> type != NULL_TYPE) {
        currentCar = car(currentItem);
        switch (currentCar -> type) {
            case INT_TYPE:
                printf("%i:integer\n", currentCar -> i);
                break;
            case DOUBLE_TYPE:
                printf("%lf:double\n", currentCar -> d);
                break;
            case STR_TYPE:
                printf("%s:string\n", currentCar -> s);
                break;
            case PTR_TYPE:
                printf("Pointer\n");
                break;
            case OPEN_TYPE:
                printf("%s:open\n", currentCar -> s);
                break;
            case CLOSE_TYPE:
                printf("%s:close\n", currentCar -> s);
                break;
            case BOOL_TYPE:
                if (currentCar -> i == 1) {
                    printf("#t:boolean\n");
                } else {
                    printf("#f:boolean\n");
                }
                break;
            case SYMBOL_TYPE:
                printf("%s:symbol\n", currentCar -> s);
                break;
            case CONS_TYPE:
                break;
            case NULL_TYPE:
                break; 
            default:
                break;
        }
        currentItem = cdr(currentItem);
    }
}

#endif