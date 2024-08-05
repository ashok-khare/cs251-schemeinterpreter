#include "tokenizer.h"
#include "value.h"
#include "linkedlist.h"
#include "talloc.h"
#include "parser.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#ifndef _INTERPRETER
#define _INTERPRETER

// define eval
Value *eval(Value *, Frame *);

/*
* reverseTopLevel
* params: linkedList - a pointer to a Value representing a linked list
* returns: a pointer to a Value representing the same linked list with its top level elements in reverse order
*/
Value *reverseTopLevel(Value *linkedList) {
    Value *previous = makeNull();
    Value *current = linkedList;
    Value *next = cdr(linkedList);
    while (next->type != NULL_TYPE) {
        current->c.cdr = previous;
        previous = current;
        current = next;
        next = cdr(next);
    }
    current->c.cdr = previous;
    return current;
}

/*
primitivePlus
params: args - a pointer to a Value representing a linked list of arguments
returns: a pointer to a Value containing an integer or double that equals the sum of the arguments
primitivePlus() throws an error if it encounters a non real-number argument.
If no arguments are provided, primitivePlus returns a pointer to a Value containing 0.
*/
Value *primitivePlus(Value *args) {
   // Checks if no arguments were provided, and if so returns a pointer to an integer-type Value containing 0
    if (args -> type == NULL_TYPE) {
    Value *result = talloc(sizeof(Value));
    result -> type = INT_TYPE;
    result -> i = 0;
    }
   
   Value *current = args;
   Value *currentValue;
   int sumAsInt = 0;
   double sumAsDouble = 0;
   bool allInts = true;
   while (current -> type != NULL_TYPE) {
        currentValue = car(current);
        if (currentValue -> type != INT_TYPE && currentValue -> type != DOUBLE_TYPE) {
            printf("Evaluation error: attempting to sum non real-number arguments\n");
            texit(0);
        // If a double type seen in the arguments, switches sum to be stored as a double
        } else if (currentValue -> type == DOUBLE_TYPE && allInts) {
            sumAsDouble = sumAsInt + currentValue -> d;
            allInts = false;      
        } else if (allInts) {
            sumAsInt = sumAsInt + currentValue -> i;
        } else {
            if (currentValue -> type == INT_TYPE) {
                sumAsDouble = sumAsDouble + currentValue -> i;
            } else {
                sumAsDouble = sumAsDouble + currentValue -> d;
            }
        }
        current = cdr(current);
    }

    // make sure result is of the proper type and has its data stored in the proper locations
    Value *result = talloc(sizeof(Value));
    if (allInts) {
        result -> type = INT_TYPE;
        result -> i = sumAsInt;
    } else {
        result -> type = DOUBLE_TYPE;
        result -> d = sumAsDouble;
    }

    return result;
}

/*
* primitiveMinus
* params: args - a pointer to a Value representing a linked list of arguments
* returns: a pointer to a Value containing an integer or double that equals the difference of the arguments
* primitiveMinus() throws an error if it encounters a non real-number argument.
* If no arguments are provided, primitiveMinus() returns a pointer to a Value containing 0.
*/
Value *primitiveMinus(Value *args) {
    if (args -> type == NULL_TYPE) {
        // if no args, return Value containing 0
        Value *returnValue = talloc(sizeof(Value));
        returnValue -> type = INT_TYPE;
        returnValue -> i = 0;
        return returnValue;
    } else {
        Value *current = args;
        Value *currentValue = car(current);
        int differenceAsInt = 0;
        double differenceAsDouble = 0;
        // boolean to keep track of whether to return integer or double result
        bool allInts = true;

        // re-initialize difference as the number in the initial arg for proper behavior
        if (currentValue -> type != INT_TYPE && currentValue -> type != DOUBLE_TYPE) {
            // throw error if initial arg does not contain a valid number
            printf("Evaluation error: attempting to subtract non real-number arguments\n");
            texit(0);
        } else if (currentValue -> type == DOUBLE_TYPE) {
            differenceAsDouble = currentValue -> d;
            allInts = false;
        } else {
            differenceAsInt = currentValue -> i;
        }
        current = cdr(current);

        // calculate the difference of the args, throws an error if an arg is not a valid number
        while (current -> type != NULL_TYPE) {
            currentValue = car(current);
            if (currentValue -> type != INT_TYPE && currentValue -> type != DOUBLE_TYPE) {
                printf("Evaluation error: attempting to subtract non real-number arguments\n");
                texit(0);
            } else if (currentValue -> type == DOUBLE_TYPE && allInts) {
                differenceAsDouble = differenceAsInt - (currentValue -> d);
                allInts = false;
            } else if (allInts) {
                differenceAsInt = differenceAsInt - (currentValue -> i);
            } else {
                if (currentValue -> type == DOUBLE_TYPE) {
                    differenceAsDouble = differenceAsDouble - (currentValue -> d);
                } else {
                    differenceAsDouble = differenceAsDouble - (currentValue -> i);
                }
            }
            current = cdr(current);
        }

        // depending on value of allInts, create and return either an integer or double result
        Value *result = talloc(sizeof(Value));
        if (allInts) {
            result -> type = INT_TYPE;
            result -> i = differenceAsInt;
        } else {
            result -> type = DOUBLE_TYPE;
            result -> d = differenceAsDouble;
        }

        return result;
    }
}

/*
* primitiveEquals
* params: args - a pointer to a Value representing a linked list of arguments
* returns: a pointer to a BOOL_TYPE Value whose value depends on whether the given arguments are equal
*/
Value *primitiveEquals(Value *args) {
    if (args -> type == NULL_TYPE || cdr(args) -> type == NULL_TYPE || cdr(cdr(args)) -> type != NULL_TYPE) {
        // throw an error if the list of arguments does not contain exactly two arguments
        printf("Evaluation error: incorrect number of arguments for =\n");
        texit(0);
    } else if ((car(args) -> type != INT_TYPE && car(args) -> type != DOUBLE_TYPE) ||
                (car(cdr(args)) -> type != INT_TYPE && car(cdr(args)) -> type != DOUBLE_TYPE)
    ) {
        // throw an error if at least one of the args is not a valid number
        printf("Evaluation error: cannot compare a non-number using =\n");
        texit(0);
    } else {
        Value *result = talloc(sizeof(Value));
        result -> type = BOOL_TYPE;

        // account for each possible combination of arg types when determining result
        if (car(args) -> type == INT_TYPE) {
            if (car(cdr(args)) -> type == INT_TYPE) {
                if (car(args) -> i == car(cdr(args)) -> i) {
                    result -> i = 1;
                } else {
                    result -> i = 0;
                }
            } else {
                if (car(args) -> i == car(cdr(args)) -> d) {
                    result -> i = 1;
                } else {
                    result -> i = 0;
                }
            }
        } else {
            if (car(cdr(args)) -> type == INT_TYPE) {
                if (car(args) -> d == car(cdr(args)) -> i) {
                    result -> i = 1;
                } else {
                    result -> i = 0;
                }
            } else {
                if (car(args) -> d == car(cdr(args)) -> d) {
                    result -> i = 1;
                } else {
                    result -> i = 0;
                }
            }
        }

        return result;
    }
    // extra return to prevent compiler warning
    return makeNull();
}

/*
* primitiveLessThan
* params: args - a pointer to a Value representing a linked list of arguments
* returns: a pointer to a BOOL_TYPE Value whose value depends on whether the first argument is less than the second
*/
Value *primitiveLessThan(Value *args) {
    if (args -> type == NULL_TYPE || cdr(args) -> type == NULL_TYPE || cdr(cdr(args)) -> type != NULL_TYPE) {
        // if list of args does not contain exactly two items, throw an error
        printf("Evaluation error: incorrect number of arguments for =\n");
        texit(0);
    } else if ((car(args) -> type != INT_TYPE && car(args) -> type != DOUBLE_TYPE) ||
                (car(cdr(args)) -> type != INT_TYPE && car(cdr(args)) -> type != DOUBLE_TYPE)
    ) {
        // throw an error if at least one of the args is not a valid number
        printf("Evaluation error: cannot compare a non-number using =\n");
        texit(0);
    } else {
        Value *result = talloc(sizeof(Value));
        result -> type = BOOL_TYPE;

        // account for each possible combination of arg types while determining result
        if (car(args) -> type == INT_TYPE) {
            if (car(cdr(args)) -> type == INT_TYPE) {
                if (car(args) -> i < car(cdr(args)) -> i) {
                    result -> i = 1;
                } else {
                    result -> i = 0;
                }
            } else {
                if (car(args) -> i < car(cdr(args)) -> d) {
                    result -> i = 1;
                } else {
                    result -> i = 0;
                }
            }
        } else {
            if (car(cdr(args)) -> type == INT_TYPE) {
                if (car(args) -> d < car(cdr(args)) -> i) {
                    result -> i = 1;
                } else {
                    result -> i = 0;
                }
            } else {
                if (car(args) -> d < car(cdr(args)) -> d) {
                    result -> i = 1;
                } else {
                    result -> i = 0;
                }
            }
        }

        return result;
    }
    // extra return to prevent compiler warning
    return makeNull();
}

/*
* primitiveGreaterThan
* params: args - a pointer to a Value representing a linked list of arguments
* returns: a pointer to a BOOL_TYPE Value whose value depends on whether the first argument is greater than the second
*/
Value *primitiveGreaterThan(Value *args) {
    if (args -> type == NULL_TYPE || cdr(args) -> type == NULL_TYPE || cdr(cdr(args)) -> type != NULL_TYPE) {
        // throw an error if list of arguments does not contain exactly two arguments
        printf("Evaluation error: incorrect number of arguments for =\n");
        texit(0);
    } else if ((car(args) -> type != INT_TYPE && car(args) -> type != DOUBLE_TYPE) ||
                (car(cdr(args)) -> type != INT_TYPE && car(cdr(args)) -> type != DOUBLE_TYPE)
    ) {
        // throw an error if at least one of the args is not a valid number
        printf("Evaluation error: cannot compare a non-number using =\n");
        texit(0);
    } else {
        Value *result = talloc(sizeof(Value));
        result -> type = BOOL_TYPE;

        // account for each possible combination of arg types while determining result
        if (car(args) -> type == INT_TYPE) {
            if (car(cdr(args)) -> type == INT_TYPE) {
                if (car(args) -> i > car(cdr(args)) -> i) {
                    result -> i = 1;
                } else {
                    result -> i = 0;
                }
            } else {
                if (car(args) -> i > car(cdr(args)) -> d) {
                    result -> i = 1;
                } else {
                    result -> i = 0;
                }
            }
        } else {
            if (car(cdr(args)) -> type == INT_TYPE) {
                if (car(args) -> d > car(cdr(args)) -> i) {
                    result -> i = 1;
                } else {
                    result -> i = 0;
                }
            } else {
                if (car(args) -> d > car(cdr(args)) -> d) {
                    result -> i = 1;
                } else {
                    result -> i = 0;
                }
            }
        }

        return result;
    }
    // extra return to prevent compiler warning
    return makeNull();
}

/*
primitiveNull
params: args - a pointer to a Value representing a linked list of arguments
returns: a pointer to a boolean-type Value
primitiveNull() throws an error if greater or fewer than one argument is provided.
The boolean-type Value returned by primitiveNull() will contain true if the argument was an empty list, and false in any other case.
The 
*/
Value *primitiveNull(Value *args) {
    if (args -> type == NULL_TYPE || cdr(args) -> type != NULL_TYPE) {
        printf("Evaluation error: incorrect number of args for 'null?'\n");
        texit(0);
    } else {
        Value *arg = car(args);
        Value *result = talloc(sizeof(Value));
        result -> type = BOOL_TYPE;
        if (args -> type == CONS_TYPE) {
            if (isNull(arg)) {
                result -> i = 1;
            } else {
                result -> i = 0;
            }
        } else {
            result -> i = 0;
        }
        return result;
    }
    return makeNull();
}

/*
primitiveCar
params: args - a pointer to a Value representing a linked list of arguments
returns: a pointer to a Value representing the first item in a given list
primitiveCar() will throw an error if it is given greater or fewer than one argument.
*/
Value *primitiveCar(Value *args) {
    if (args -> type == NULL_TYPE || cdr(args) -> type != NULL_TYPE) {
        printf("Evaluation error: incorrect number of args for 'car'\n");
        texit(0);
    } else {
        Value *arg = car(args);
        if (arg -> type != CONS_TYPE) {
            printf("Evaluation error: argument to car is not a cons cell\n");
            texit(0);
        } else {
            return car(arg);
        }
    }
    return makeNull();
}

/*
primitiveCdr
params: a pointer to a Value representing a linked list of arguments
returns: a pointer to a Value representing everything but the first item in a given list
primitiveCdr() will throw an error if it is given greater or fewer than one argument.
*/
Value *primitiveCdr(Value *args) {
    if (args -> type == NULL_TYPE || cdr(args) -> type != NULL_TYPE) {
        printf("Evaluation error: incorrect number of args for 'cdr'\n");
        texit(0);
    } else {
        Value *arg = car(args);
        if (arg -> type != CONS_TYPE) {
            printf("Evaluation error: argument to cdr is not a cons cell\n");
            texit(0);
        } else {
            return cdr(arg);
        }
    }
    return makeNull();
}

/*
primitiveCons
params: args - a pointer to a Value struct
returns: a pointer to a Value struct
Returns a Cons cell of the two values contained within args. If args does not contain exactly two args, throw an error.
*/
Value *primitiveCons(Value *args) {
    // If args does not contain exactly two args, throw an error.
    if (args -> type == NULL_TYPE || cdr(args) -> type == NULL_TYPE || cdr(cdr(args)) -> type != NULL_TYPE) {
        printf("Evaluation error: incorrect number of args for 'cons'\n");
        texit(0);

    } else {
        return cons(car(args), car(cdr(args)));
    }
    return makeNull();
}

/*
bind
params: name - a pointer to a string, function - a pointer to a function, frame - a pointer to a Frame struct
returns: nothing
bind() adds a definition to the global frame where the given name is the key and the function pointer is its value.
*/
void bind(char *name, Value *(*function)(struct Value *), Frame *frame) {
    Value *functionValue = talloc(sizeof(Value));
    functionValue -> type = PRIMITIVE_TYPE;
    functionValue -> pf = function;
    
    Value *nameValue = talloc(sizeof(Value));
    nameValue -> type = SYMBOL_TYPE;
    nameValue -> s = name;

    Value *binding = cons(nameValue, functionValue);
    
    frame -> bindings = cons(binding, frame -> bindings);
}

/*
evalEach
params: args - a pointer to a Value struct, frame - a pointer to a Frame struct
returns: a Value struct containing the evaluated arguments to be passed into apply()
*/
Value *evalEach(Value *args, Frame *frame, bool needsReversal) {
    Value *evaledArgs = makeNull();
    Value *arg = args;
    while (arg -> type != NULL_TYPE) {
        evaledArgs = cons(eval(car(arg), frame), evaledArgs);
        arg = cdr(arg);
    }
    if (needsReversal && evaledArgs -> type != NULL_TYPE) {
        //reverse the top-level list of arguments
        evaledArgs = reverseTopLevel(evaledArgs);
    }
    return evaledArgs;
}

/*
makeClosure
params: environment - a pointer to a Frame; parameters - a pointer to a Value representing a linked list of parameters; functionBody - a pointer to a Value representing a function's body as a parse tree
returns: a new Value of type CLOSURE_TYPE containing the information provided in the parameters
*/
Value *makeClosure(Frame *environment, Value *parameters, Value *functionBody) {
    Value *closure = talloc(sizeof(Value));
    closure -> type = CLOSURE_TYPE;
    closure -> cl.paramNames = parameters;
    closure -> cl.functionCode = functionBody;
    closure -> cl.frame = environment;
    return closure;
}

/*
makeFrame
params: parent - a pointer to a Frame struct
returns: newFrame - a pointer to a Frame struct
Given a parent frame, allocates a null Frame and sets its parent to point to the parameter.
*/
Frame *makeFrame(Frame *parent) {
   Frame *newFrame = talloc(sizeof(Frame));
   newFrame -> parent = parent;
   newFrame -> bindings = makeNull();
   return newFrame;
}

/*
addBinding
params: binding - a pointer to a Value representing a binding in dotted pair format; frame - a pointer to a Frame
returns: nothing
addBinding() adds the given binding to the given frame's list of bindings.
*/
void addBinding(Value *binding, Frame *frame) {
    Value *current = frame -> bindings;
    // check for multiple bindings for a variable (not allowed)
    while (current -> type != NULL_TYPE) {
        if (!strcmp(car(car(current)) -> s, car(binding) -> s)) {
            printf("Evaluation error: local variable %s already bound\n", car(binding) -> s);
            texit(0);
        }
        current = cdr(current);
    }

    // check to make sure variable to be bound is of symbol type
    if (car(binding) -> type != SYMBOL_TYPE) {
        printf("Evaluation error: variable being bound must be of symbol type\n");
        texit(0);

    } else {
        frame -> bindings = cons(binding, frame -> bindings);
    }
}

/*
* updateBinding
* params: variable - a pointer to a Value specifying the variable whose binding should be changed; newValue - a pointer to a Value specifying the new value for the binding to be changed; frame - a pointer to a Frame in which to search for the binding
* returns: nothing
* updateBinding() recursively searches the given frame and its parents for the binding corresponding to the given variable; if found, the binding's value is updated to be newValue.
* If a binding corresponding to the given variable is not found, updateBinding() will throw an error.
*/
void updateBinding(Value *variable, Value *newValue, Frame *frame) {
    Value *current = frame -> bindings;
    bool updated = false;

    // search bindings in current frame; if desired binding found, update its value
    while (current -> type != NULL_TYPE) {
        if (!strcmp(car(car(current)) -> s, variable -> s)) {
            car(current) -> c.cdr = newValue;
            updated = true;
        }
        current = cdr(current);
    }

    // if desired binding not found in current frame, recursively search parent frames
    if (!updated) {
        if (frame -> parent == NULL) {
            // if desired binding has not been found and current frame does not have a parent, throw an error
            printf("Evaluation error: variable %s never defined\n", variable -> s);
            texit(0);
        } else {
            updateBinding(variable, newValue, frame -> parent);
        }
    }
}

/*
apply
params: evaledOperator - a pointer to a Value, that represents a closure corresponding to a function; evaledArgs - a pointer to a value representing a list of previously evaluated function arguments
returns: the result of evaluating the body contained in the given closure, in the context of evaledArgs and the closure's environment
apply() builds a new frame whose parent is the environment specified in the given closure, and adds bindings to it corresponding to each parameter/argument pair.
apply() then evaluates the function body specified in the given closure in the context of the new frame, and returns the result.
*/
Value *apply(Value *evaledOperator, Value *evaledArgs) {
    // if the given operator is not a function, throw an error.
    if (evaledOperator -> type != CLOSURE_TYPE && evaledOperator -> type != PRIMITIVE_TYPE) {
        printf("Evaluation error: non-function being called as function\n");
        texit(0);
    
    //
    } else if (evaledOperator -> type == PRIMITIVE_TYPE) {
        Value *result = (evaledOperator -> pf)(evaledArgs);
        return result;
        
    // 
    } else {
        Frame *frame = makeFrame(evaledOperator -> cl.frame);
        Value *param = evaledOperator -> cl.paramNames;
        Value *arg = evaledArgs;
        while (param -> type != NULL_TYPE) {
            // if too few arguments are passed, throw an error.
            if (arg -> type == NULL_TYPE) {
                printf("Evaluation error: too few args passed to function\n");
                texit(0);
            }
            Value *binding = cons(car(param), car(arg));
            addBinding(binding, frame);
            arg = cdr(arg);
            param = cdr(param);
        }

        // if too many arguments are passed, throw an error.
        if (arg -> type != NULL_TYPE) {
            printf("Evaluation error: too many args passed to function\n");
            texit(0);
        }

        Value *result;
        Value *body = evaledOperator -> cl.functionCode;
        while (body -> type != NULL_TYPE) {
            result = eval(car(body), frame);
            body = cdr(body);
        }
        // return the final evaluated expression in body
        return result;
    }
    // extra return to prevent compiler warning
    return makeNull();
}

/*
evalDefine
params: args - a pointer to a Value struct, frame - a pointer to a Frame struct
returns: a Value struct of type VOID_TYPE
Adds the bindings requested by args to the global frame.
*/
Value *evalDefine(Value *args, Frame *frame) {
    // if no arguments or body are provided for define or too many arguments are provided, throw an error.
    if (args -> type == NULL_TYPE || cdr(args) -> type == NULL_TYPE  || cdr(cdr(args)) -> type != NULL_TYPE) {
        printf("Evaluation error: incorrect number of args for define\n");
        texit(0);
    // if the given variable for definition is not a symbol, throw an error.
    } else if (car(args) -> type != SYMBOL_TYPE) {
        printf("Evaluation error: trying to define non-variable\n");
        texit(0);
    }
    addBinding(cons(car(args), eval(car(cdr(args)), frame)), frame);

    Value *returnValue = talloc(sizeof(Value));
    returnValue -> type = VOID_TYPE;
    return returnValue;
}

/*
evalLambda
params: args - a pointer to a Value representing lambda's args; frame - a pointer to a Frame
returns: a closure corresponding to the lambda expression being evaluated
evalLambda() checks if the lambda expression's arguments are properly formatted, and if so, creates and returns a closure object corresponding to the expression.
evalLambda() also performs general error checking, including the number of args, type of function parameters, and duplicate function parameters.
*/
Value *evalLambda(Value *args, Frame *frame) {
    // if too few arguments are given for lambda, throw an error.
    if (args -> type == NULL_TYPE || cdr(args) -> type == NULL_TYPE) {
        printf("Evaluation error: incorrect number of args for lambda\n");
        texit(0);
    }
    
    Value *params = car(args);
    Value *param = params;
    Value *visited = makeNull();
    while (param -> type != NULL_TYPE) {
        // if lambda's parameters are not formatted correctly, throw an error.
        if (param -> type != CONS_TYPE) {
            printf("Evaluation error: bad param formatting in lambda\n");
            texit(0);
        // if lambda's paramters are not a symbol, throw an error.
        } else if (car(param) -> type != SYMBOL_TYPE && car(param) -> type != NULL_TYPE) {
            printf("Evaluation error: non-variable param in lambda\n");
            texit(0);
        // if null param (indicating no parameters), set params equal to a NULL_TYPE Value.
        } else if (car(param) -> type == NULL_TYPE) {
            params = makeNull();
            param = cdr(param);
        } else {
            Value *existing = visited;
            while (existing -> type != NULL_TYPE) {
                // if lambda's parameters contain duplicate identifiers, throw an error.
                if (!strcmp(car(existing) -> s, car(param) -> s)) {
                    printf("Evaluation error: duplicate identifier in lambda\n");
                    texit(0);
                }
                existing = cdr(existing);
            }
            visited = cons(car(param), visited);
            param = cdr(param);
        }
    }
    
    return makeClosure(frame, params, cdr(args));
}

/*
evalIf
params: tree - a pointer to a Value struct, frame - a pointer to a Frame struct
returns: a pointer to a Value struct
Function is called in the case that an 'if' symbol is evaluated. Returns the second arg if the first arg evaluates to true and the third arg if false.
*/
Value *evalIf(Value *args, Frame *frame) {
    // if more or less than 3 args provided, throw an error.
    if (cdr(args) -> type == NULL_TYPE || cdr(cdr(args)) -> type == NULL_TYPE || cdr(cdr(cdr(args))) -> type != NULL_TYPE) {
        printf("Evaluation error: incorrect number of args for if statement\n");
        texit(0);
    }

    Value *boolResult = eval(car(args), frame);
    // if the first arg does not evaluate to a boolean, throw an error.
    if (boolResult -> type != BOOL_TYPE) {
        printf("Evaluation error: if statement predicate does not resolve to boolean\n");
        texit(0);

    } else if (boolResult -> i == 1) {
        return eval(car(cdr(args)), frame);

    } else {
        return eval(car(cdr(cdr(args))), frame);
    }
    // extra return to prevent compiler warning
    return makeNull();
}

/*
evalLet
params: args - a pointer to a Value representing the arguments of the let statement; frame - a pointer to a Frame
returns: a pointer to a Value, that is the result of evaluating the body of the let statement within the proper Frame
evalLet() creates a new Frame containing the let statement's bindings, then evaluates the let statement's body in the context of that Frame.
*/
Value *evalLet(Value *args, Frame *frame) {
    // if no arguments or body are provided for let, throw an error.
    if (args -> type == NULL_TYPE || cdr(args) -> type == NULL_TYPE) {
        printf("Evaluation error: incorrect number of args for let\n");
        texit(0);

    }
    // create new frame in which to evaluate let
    Frame *newFrame = makeFrame(frame);
    
    // checks list of bindings to make sure it is a proper list; throws error if not
    if (car(args) -> type != CONS_TYPE && car(args) -> type != NULL_TYPE) {
        printf("Evaluation error: invalid let binding\n");
        texit(0);
    }

    // checks proper nested list formatting for list of bindings; throws error if bindings are incorrectly formatted
    Value *binding = car(args);
    while(binding -> type != NULL_TYPE) {
        // check outer list format
        if (binding -> type != CONS_TYPE) {
            printf("Evaluation error: invalid let binding\n");
            texit(0);

        // check each binding itself
        } else if (car(binding) -> type != CONS_TYPE) {
            printf("Evaluation error: invalid let binding\n");
            texit(0);

        // adds binding to newFrame
        } else {
            addBinding(cons(car(car(binding)), eval(car(cdr(car(binding))), frame)), newFrame);
            binding = cdr(binding);
        }
    }

    // evaluates body of the let statement in the context of newFrame 
    Value *result;
    Value *body = cdr(args);
    while (body -> type != NULL_TYPE) {
        result = eval(car(body), newFrame);
        body = cdr(body);
    }
    // return the final evaluated expression in body
    return result;
}

/*
* evalLetRec
* params: args - a pointer to a Value representing a linked list of arguments; frame - a pointer to the Frame in which to evaluate the statement
* returns: a pointer to a Value representing the result of evaluating the final s-expression in the body of the letrec statement
* evalLetRec() ensures that the local variables created in letrec's new frame do not depend on each other.
* Unlike evalLet(), evalLetRec() evaluates both the local variables and the body in the context of its new frame.
*/
Value *evalLetRec(Value *args, Frame *frame) {
    // ensure two arguments for list of local variables and letrec body
    if (args -> type == NULL_TYPE || cdr(args) -> type == NULL_TYPE) {
        printf("Evaluation error: incorrect number of args for letrec\n");
        texit(0);
    }

    // create new frame in which to evaluate new local variables/letrec body
    Frame *newFrame = makeFrame(frame);

    // check valid formatting for the letrec statement's binding list
    if (car(args) -> type != CONS_TYPE && car(args) -> type != NULL_TYPE) {
        printf("Evaluation error: invalid binding format for letrec\n");
        texit(0);
    }

    // add bindings to the new frame; throw an error if bindings are improperly formatted
    Value *bindings = car(args);
    Value *placeholder;
    while (bindings -> type != NULL_TYPE) {
        if (bindings -> type != CONS_TYPE) {
            printf("Evaluation error: invalid binding format for letrec\n");
            texit(0);
        } else if (car(bindings) -> type != CONS_TYPE) {
            printf("Evaluation error: invalid binding format for letrec\n");
            texit(0);
        } else {
            placeholder = talloc(sizeof(Value));
            placeholder -> type = UNSPECIFIED_TYPE;
            addBinding(cons(car(car(bindings)), placeholder), newFrame);
            bindings = cdr(bindings);
        }
    }

    // evaluate each binding value
    Value *evaluated = makeNull();
    bindings = car(args);
    Value *evaledValue;
    while (bindings -> type != NULL_TYPE) {
        evaledValue = eval(car(cdr(car(bindings))), newFrame);
        evaluated = cons(evaledValue, evaluated);
        bindings = cdr(bindings);
    }

    // reverse the list of evaluated binding values for consistency between variable/value ordering
    if (evaluated -> type != NULL_TYPE) {
        evaluated = reverseTopLevel(evaluated);
    }

    // update each variable binding in the new frame to contain its corresponding evaluated value
    bindings = car(args);
    evaledValue = evaluated;
    while (evaledValue -> type != NULL_TYPE) {
        updateBinding(car(car(bindings)), car(evaledValue), newFrame);
        bindings = cdr(bindings);
        evaledValue = cdr(evaledValue);
    }

    // evaluate the body of the letrec statement and return the result of the final s-expression
    Value *result;
    Value *body = cdr(args);
    while (body -> type != NULL_TYPE) {
        result = eval(car(body), newFrame);
        body = cdr(body);
    }
    return result;
}

/*
* evalSetBang
* params: args - a pointer to a Value representing a linked list of arguments; frame - a pointer to a Frame
* returns: a pointer to a VOID_TYPE Value
* evalSetBang() attempts to access the binding of the variable given as the first argument and replace its existing value with the new value given as the second argument.
* If no binding for the variable given as the first argument exists, evalSetBang() throws an error.
*/
Value *evalSetBang(Value *args, Frame *frame) {
    // check for exactly two args
    if (args -> type == NULL_TYPE || cdr(args) -> type == NULL_TYPE || cdr(cdr(args)) -> type != NULL_TYPE) {
        printf("Evaluation error: incorrect number of args for set!\n");
        texit(0);
    } else {
        // ensure first arg is a variable (must be SYMBOL_TYPE)
        if (car(args) -> type != SYMBOL_TYPE) {
            printf("Evaluation error: first argument for set! must be a variable symbol\n");
            texit(0);
        } else {
            // attempt to access the binding corresponding to the given variable and update its value
            updateBinding(car(args), eval(car(cdr(args)), frame), frame);
            Value *returnValue = talloc(sizeof(Value));
            returnValue -> type = VOID_TYPE;
            return returnValue;
        }
    }
    // extra return to prevent compiler warning
    return makeNull();
}

/*
* evalBegin
* params: args - a pointer to a Value representing a linked list of arguments; frame - a pointer to a Frame
* returns: a pointer to a Value representing the result of evaluating the final s-expression in the body of the begin statement
* evalBegin() evaluates each argument in the given list of arguments as an s-expression, and returns the result of the last one.
* If there are no arguments, evalBegin() returns a VOID_TYPE Value.
*/
Value *evalBegin(Value *args, Frame *frame) {
    if (args -> type == NULL_TYPE) {
        // if no args, return VOID_TYPE Value
        Value *returnValue = talloc(sizeof(Value));
        returnValue -> type = VOID_TYPE;
        return returnValue;
    } else {
        // evaluate each s-expression given in args and return the result of the final evaluation
        Value *returnValue = talloc(sizeof(Value));
        Value *arg = args;
        while (arg -> type != NULL_TYPE) {
            returnValue = eval(car(arg), frame);
            arg = cdr(arg);
        }
        return returnValue;
    }
}

/*
lookUpSymbol
params: symbol - a pointer to a Value struct, frame - a pointer to a Frame struct
returns: a pointer to a Value struct
Given a frame and a symbol, traverse the frame searching for the symbol's assigned value.
*/
Value *lookUpSymbol(Value *symbol, Frame *frame) {
    Value *currentBinding = frame -> bindings;
    while (currentBinding -> type != NULL_TYPE) {
        if (!strcmp(car(car(currentBinding)) -> s, symbol -> s)) {
            return cdr(car(currentBinding));
        } else {
            currentBinding = cdr(currentBinding);
        }
    }

    // if the symbol has not been defined, throw an error.
    if (frame -> parent == NULL) {
        printf("Evaluation error: binding for symbol '%s' not defined in a frame\n", symbol -> s);
        texit(0);
    }

    return lookUpSymbol(symbol, frame -> parent);
}

/*
eval
params: tree - a pointer to a Value struct, frame - a pointer to a Frame struct
returns: a pointer to a Value struct
Given a pointer to a parse tree and a pointer to a frame, evaluate the parse tree in the context of the current frame.
*/
Value *eval(Value *tree, Frame *frame) {
    switch (tree->type)  {
        case INT_TYPE: {
            return tree;
        }
        case DOUBLE_TYPE: {
            return tree;
        }
        case STR_TYPE: {
            return tree;
        }
        case BOOL_TYPE: {
            return tree;
        }
        case SYMBOL_TYPE: {
            Value *result = lookUpSymbol(tree, frame);
            if (result -> type == UNSPECIFIED_TYPE) {
                printf("Evaluation error: local variable depends on local variable in same frame\n");
                texit(0);
                return makeNull();
            } else {
                return result;
            }
        }  
        case CONS_TYPE: {
            Value *first = car(tree);
            Value *args = cdr(tree);

            if (first -> type != SYMBOL_TYPE && first -> type != CONS_TYPE) {
                printf("Evaluation error: given type not a function\n");
                texit(0);

            } else if (!strcmp(first->s, "if")) {
               return evalIf(args, frame);
               
            } else if (!strcmp(first->s, "let")) {
                return evalLet(args, frame);

            } else if (!strcmp(first->s, "quote")) {
                // if there are none or multiple args given to quote, throw an error.
                if (args -> type != CONS_TYPE || cdr(args) -> type != NULL_TYPE) {
                    printf("Evaluation error: incorrect number of args for quote\n");
                    texit(0);
                } else {
                    return car(args);
                }
            
            } else if (!strcmp(first->s, "define")) { 
                return evalDefine(args, frame);  

            } else if (!strcmp(first->s, "lambda")) {
                return evalLambda(args, frame);

            } else if (!strcmp(first->s, "letrec")) {
                return evalLetRec(args, frame);

            } else if (!strcmp(first->s, "set!")) {
                return evalSetBang(args, frame);

            } else if (!strcmp(first->s, "begin")) {
                return evalBegin(args, frame);

            } else {
                // if not special form, evaluate first and args, then try to apply the results as a function
                Value *evaledOperator = eval(first, frame);
                bool needsReversal = true;
                if (!strcmp(first->s, "car") || !strcmp(first->s, "cdr")) {
                    needsReversal = false;
                }
                Value *evaledArgs = evalEach(args, frame, needsReversal);

                return apply(evaledOperator, evaledArgs);
            }
            break;
        }
        default: {
            break;
        }
    }
    return makeNull();    
}

/*
printingHelper
params: tree - a pointer to a Value struct, needsClose - a pointer to an integer
returns: nothing
Given a tree representing the result of evaluating a parse tree, print the contents of the tree.
*/
void printingHelper(Value *tree) {
    Value *current;
    Value *currentCar;
    switch (tree->type) {
        case INT_TYPE: {
            printf("%i ", tree -> i);
            break;
        }
        case DOUBLE_TYPE: {
            printf("%lf ", tree -> d);
            break;
        }
        case STR_TYPE: {
            printf("%s ", tree -> s);
            break;
        }
        case BOOL_TYPE: {
            if (tree -> i == 1) {
                printf("#t");
            } else {
                printf("#f");
            }
            break;
        }
        case SYMBOL_TYPE: {
            printf("%s ", tree -> s);
            break;
        }
        case CONS_TYPE: {
            Value *current = tree;
            printf("(");
            while (current -> type != NULL_TYPE) {
                currentCar = car(current);
                if (cdr(current) -> type != CONS_TYPE && cdr(current) -> type != NULL_TYPE) {
                    printingHelper(currentCar);
                    printf(". ");
                    printingHelper(cdr(current));
                    break;
                } else {
                    printingHelper(currentCar);
                }
                current = cdr(current);
            }
            printf(") ");
            break;
        }
        case NULL_TYPE: {
            printf("()");
            break;
        }
        case CLOSURE_TYPE: {
            printf("#<procedure>");
            break;
        }
        case PRIMITIVE_TYPE: {
            printf("#<procedure>");
            break;
        }
        default:
            break;
    }
}

/*
interpret
params: tree - a pointer to a Value struct
returns: nothing
Given the pointer to a Scheme program, iteratively call eval() on each parse tree and display their respective result.
*/
void interpret(Value *tree) {
    Value *current = tree;
    Frame *global = makeFrame(NULL);
    
    //add primitive functions to the global frame
    bind("+", primitivePlus, global);
    bind("-", primitiveMinus, global);
    bind("=", primitiveEquals, global);
    bind("<", primitiveLessThan, global);
    bind(">", primitiveGreaterThan, global);
    bind("null?", primitiveNull, global);
    bind("car", primitiveCar, global);
    bind("cdr", primitiveCdr, global);
    bind("cons", primitiveCons, global);

    while (current->type != NULL_TYPE) {
        Value *result = eval(car(current), global);
        int needsClose = 0;
        printingHelper(result);
        if (result -> type != VOID_TYPE) {
            printf("\n");
        }
        current = cdr(current);
    }
}

#endif