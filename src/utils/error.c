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
 * @brief Exit the program with return code 1 and perror set
 *
 * @param msg message
 */
noreturn void panic(const char* msg) {
  perror(msg);

  exit(EXIT_FAILURE);
}

/**
 * @brief Exit the program with return code 1
 *
 * @param msg message
 */
noreturn void die(const char* msg) {
  fprintf(stderr, "[ERROR] %s\n", msg);

  exit(EXIT_FAILURE);
}
