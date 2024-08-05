#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "value.h"
#include <string.h>
#include <assert.h>
#include "talloc.h"

#ifndef _LINKEDLIST
#define _LINKEDLIST

// Looked up documentation for assert() at the following: https://www.geeksforgeeks.org/assertions-cc/
// Functions appear in the order before they are called to prevent implicit definitions(?) from being created.

// makeNull
// params: None
// returns: A new Value with type NULL_TYPE
Value *makeNull() {
    Value *nullValue = talloc(sizeof(Value));
    nullValue -> type = NULL_TYPE;
    return nullValue;
}

// Cons
// params: newCar - a pointer to a Value; newCdr - a pointer to a Value
// returns: a new Value with type CONS_TYPE.
// Sets the car and cdr of the value to point to the given parameters.
Value *cons(Value *newCar, Value *newCdr) {
    Value *consCell = talloc(sizeof(Value));
    consCell -> type = CONS_TYPE;
    consCell -> c.car = newCar;
    consCell -> c.cdr = newCdr;
    return consCell;
}

// car
// params: list - a pointer to a Value
// returns: a pointer to a Value
// car returns the car of the given Value. Throws an error if list is not of type CONS_TYPE.
Value *car(Value *list) {
    assert(list -> type == CONS_TYPE);
    return list -> c.car;
}

// cdr
// params: list - a pointer to a Value
// returns: a pointer to a Value
// cdr returns the cdr of the given Value. Throws an error if list is not of type CONS_TYPE.
Value *cdr(Value *list) {
    assert(list -> type == CONS_TYPE);
    return list -> c.cdr;
}

// displayHelper
// params: list - a pointer to a Value; index - an integer
// returns: Nothing
// Helper method called by display() to print the items in list.
void displayHelper(Value *list, int index) {
    Value *currentList = list;
    switch (currentList -> type) {
        case INT_TYPE:
            printf("Integer at index %i: %i\n", index, currentList -> i);
            break;
        case DOUBLE_TYPE:
            printf("Double at index %i: %lf\n", index, currentList -> d);
            break;
        case STR_TYPE:
            printf("String at index %i: %s\n", index, currentList -> s);
            break;
        case PTR_TYPE:
            printf("Pointer at index %i\n", index);
            break;
        case OPEN_TYPE:
            printf("Open parenthesis at index %i: %s\n", index, currentList -> s);
            break;
        case CLOSE_TYPE:
            printf("Close parenthesis at index %i: %s\n", index, currentList -> s);
            break;
        case BOOL_TYPE:
            printf("Boolean at index %i: %i\n", index, currentList -> i);
            break;
        case SYMBOL_TYPE:
            printf("Symbol at index %i: %s\n", index, currentList -> s);
            break;
        case CONS_TYPE:
            displayHelper(currentList -> c.car, index);
            displayHelper(currentList -> c.cdr, index + 1);
            break;
        case NULL_TYPE:
            printf("Null at index %i\n", index);
            break;
        default:
            break;
    }
}

// display
// params: list - a pointer to a Value
// returns: Nothing
// Prints out the values contained in list in a readable format
void display(Value *list) {
    displayHelper(list, 0);
}

// reverseHelper
// params: list - a pointer to a Value; reversed - a pointer to a Value
// returns: a pointer to a Value
// helper method called by reverse() to create a reversed version of list.
Value *reverseHelper(Value *list, Value *reversed) {
    switch (list -> type) {
        case NULL_TYPE:
            return reversed;
        case INT_TYPE:
            return list;
        case DOUBLE_TYPE:
            return list;
        case STR_TYPE:
            return list;
        case PTR_TYPE:
            return list;
        case OPEN_TYPE:
            return list;
        case CLOSE_TYPE:
            return list;
        case BOOL_TYPE:
            return list;
        case SYMBOL_TYPE:
            return list;
        case CONS_TYPE:
            reversed = cons(reverseHelper(list -> c.car, reversed), reversed);
            if (cdr(list) -> type == NULL_TYPE || cdr(list) -> type == CONS_TYPE) {
                return reverseHelper(list -> c.cdr, reversed);
            } else {
                return cons(reverseHelper(list -> c.cdr, reversed), reversed);
            }
        default:
            return list;
    }
}

// reverse
// params: list - a pointer to a Value
// returns: a pointer to a Value
// reverse uses reverseHelper() to recursively run through the items in the linked list pointed to by list.
// Creates and returns a pointer to a new linked list containing the items in list, in reverse order.
Value *reverse(Value *list) {
    Value *reversed = makeNull();
    return reverseHelper(list, reversed);
}
    

// isNull
// params: value - a poier to a Value
// returns: true or false
// Returns true if the type of the give Value is of NULL_TYPE, and false otherwise.
bool isNull(Value *value) {
    if (value -> type == NULL_TYPE) {
        return true;
    } else {
        return false;
    }
}

// length
// params: value - a pointer to a value
// returns: an integer
// length iteratively finds the number of items in the linked list pointed to by value.
int length(Value *value) {
    if (value -> type == NULL_TYPE) {
        return 0;
    } else if (value -> type == CONS_TYPE) {
        int count = 1;
        Value *current = value;
        Value *next = cdr(value);

        while (next -> type != NULL_TYPE) {
            current = next;
            next = cdr(next);
            count++;
        }

        return count;
    } else {
        return 1;
    }
}

#endif