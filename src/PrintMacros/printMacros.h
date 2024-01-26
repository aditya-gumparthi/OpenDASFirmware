#ifndef PRINT_MACRO_H
#define PRINT_MACRO_H
#include <pico/stdlib.h>
#include <stdio.h>

#define VERBOSE_PRINT

#ifdef VERBOSE_PRINT

// extern bool g_OI_info_print_en_flag;
#define print_hb(...)                               \
    do                                              \
    {                                               \
                                                    \
        printf(__FILE_NAME__ " [HB]: " __VA_ARGS__); \
    } while (0)

#define print_error(...)                               \
    do                                                 \
    {                                                  \
                                                       \
        printf(__FILE_NAME__ " [ERROR]: " __VA_ARGS__); \
    } while (0)

#define print_warning(...)                            \
    do                                                \
    {                                                 \
                                                      \
        printf(__FILE_NAME__ " [WARNING]: " __VA_ARGS__); \
    } while (0)

#define print_info(...)                               \
    do                                                \
    {                                                 \
                                                      \
        printf(__FILE_NAME__ " [INFO]: " __VA_ARGS__); \
    } while (0)

// #define print_OIinfo(...)          \
//     if (1)                         \
//     {                              \
//                                    \
//         printf("OI:" __VA_ARGS__); \
//     }

#else
// extern bool g_OI_info_print_en_flag;
#define print_hb(...)               \
    do                              \
    {                               \
                                    \
        printf("[HB]" __VA_ARGS__); \
    } while (0)

#define print_error(...)           \
    do                             \
    {                              \
                                   \
        printf("[E]" __VA_ARGS__); \
    } while (0)

#define print_warning(...)         \
    do                             \
    {                              \
                                   \
        printf("[W]" __VA_ARGS__); \
    } while (0)

#define print_info(...)            \
    do                             \
    {                              \
                                   \
        printf("[I]" __VA_ARGS__); \
    } while (0)

// #define print_OIinfo(...)          \
//     if (1)                         \
//     {                              \
//                                    \
//         printf("OI:" __VA_ARGS__); \
//     }

#endif /* VERBOSE_PRINT */

#endif /* PRINT_MACRO_H */
