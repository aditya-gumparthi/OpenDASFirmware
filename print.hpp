#ifndef PRINT_HPP
#define PRINT_HPP
#include <stdio.h>
#include <stdarg.h>
#include "fmt/emio.hpp"
#define MAX_MESSAGE_LENGTH 256 // Define a suitable max length for messages

void print_formatted(const char *prefix, const char *format, va_list args) {
    char buffer[MAX_MESSAGE_LENGTH];
    emio::emio_format(buffer, MAX_MESSAGE_LENGTH, format, args); // Using emio for formatting
    printf("%s: %s\n", prefix, buffer);
}
void printi(const char *format, ...) {
    va_list args;
    va_start(args, format);
    print_formatted("I", format, args);
    va_end(args);
}

void printe(const char *format, ...) {
    va_list args;
    va_start(args, format);
    print_formatted("E", format, args);
    va_end(args);
}

void printw(const char *format, ...) {
    va_list args;
    va_start(args, format);
    print_formatted("W", format, args);
    va_end(args);
}

void printr(const char *format, ...) {
    static unsigned int counter = 0; // Static counter to keep track of calls

    va_list args;
    va_start(args, format);

    char prefixWithCounter[MAX_MESSAGE_LENGTH];
    snprintf(prefixWithCounter, sizeof(prefixWithCounter), "R%d", counter++);

    print_formatted(prefixWithCounter, format, args);
    va_end(args);
}

#endif /* PRINT_HPP */
