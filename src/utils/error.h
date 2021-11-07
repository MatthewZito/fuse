#ifndef ERROR_H
#define ERROR_H

#include <stdnoreturn.h>

noreturn void panic(const char* s);

noreturn void die(const char* s);

#endif /* ERROR_H */
