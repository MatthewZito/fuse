/**
 * @file error.c
 * @author goldmund
 * @brief Program error handlers
 * @version 0.1
 * @date 2021-11-06
 *
 * @copyright Copyright (c) 2021 Matthew Zito (goldmund)
 *
 */

#include "error.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdnoreturn.h>

/**
 * @brief Exit the program with return code 1
 *
 * @param s message
 */
noreturn void panic(const char* s) {
  perror(s);

  exit(EXIT_FAILURE);
}
