#include "value.h"
#include "linkedlist.h"
#include "talloc.h"
#include "tokenizer.h"
#include <string.h>
#include <stdio.h>

#ifndef _PARSER
#define _PARSER

// parse
// params: tokens - a pointer to a Value representing a list of tokens
// returns: a pointer to a Value representing a list of parse trees
// parse() creates a series of parse trees based on the provided tokens. It throws a syntax error if it encounters mismatched parentheses.
Value *parse(Value *tokens) {

    Value *parseTrees = makeNull();
    Value *flippedParseTrees;
    Value *currentToken = tokens;
    Value *parseTree;

    while (currentToken -> type != NULL_TYPE) {
        
        // if close paren seen, make a new parse tree and add it to the stack
        if (car(currentToken) -> type == CLOSE_TYPE) {
            parseTree = makeNull();

            while (car(parseTrees) -> type != OPEN_TYPE) {
                // error check for extra close parens
                if (cdr(parseTrees) -> type == NULL_TYPE) {
                    printf("Syntax error: too many close parentheses\n");
                    texit(0);
                } else {
                    parseTree = cons(car(parseTrees), parseTree);
                    parseTrees = cdr(parseTrees);
                }
            }

            // Ensuring parse tree is properly made into a series of cons cells if no tokens present other than parens
            if (parseTree -> type == NULL_TYPE) {
                parseTree = cons(parseTree, makeNull());
            }

            parseTrees = cons(parseTree, cdr(parseTrees));

        } else {
            parseTrees = cons(car(currentToken), parseTrees);
        }

        currentToken = cdr(currentToken);
    }

    // reverse order of the list of parse trees to maintain proper order of parsed tokens
    flippedParseTrees = makeNull();
    Value *currentParseTree = parseTrees;
    while (currentParseTree -> type != NULL_TYPE) {
        // error check for extra open parens
        if (car(currentParseTree) -> type == OPEN_TYPE) {
            printf("Syntax error: not enough close parentheses\n");
            texit(0);
        } else {
            flippedParseTrees = cons(car(currentParseTree), flippedParseTrees);
            currentParseTree = cdr(currentParseTree);
        }
    }

    return flippedParseTrees;
}

// printTree
// params: tree - a pointer to a Value representing a list of parse trees
// returns: Nothing
// printTree uses a mix of iteration and recursion to print the contents of all nodes in each parse tree in the given list.
// Prints all output in one line. Leaves an extra space after the last item in a list.
void printTree(Value *tree) {
    Value *current = tree;
    Value *currentCar;
    // iteration to print contents of multiple parse trees
    while (current -> type != NULL_TYPE) {
        currentCar = car(current);
        switch (currentCar -> type) {
            case INT_TYPE:
                printf("%i ", currentCar -> i);
                break;
            case DOUBLE_TYPE:
                printf("%lf ", currentCar -> d);
                break;
            case STR_TYPE:
                printf("%s ", currentCar -> s);
                break;
            case BOOL_TYPE:
                if (currentCar -> i == 1) {
                    printf("#t ");
                } else {
                    printf("#f ");
                }
                break;
            case SYMBOL_TYPE:
                printf("%s ", currentCar -> s);
                break;
            case CONS_TYPE:
                printf("(");
                // recursion to print contents of subtrees of a parse tree
                printTree(currentCar);
                printf(") ");
                break;
            default:
                break;
        }
        current = cdr(current);
    }
}

// int main() {
//     Value *tokens = tokenize();
//     Value *parseTrees = parse(tokens);
//     printTree(parseTrees);
//     return 1;
// }

#endif