#ifndef ARGPARSE_H
#define ARGPARSE_H
/**
 * Command-line arguments parsing library.
 *
 * This module is inspired by parse-options.c (git) and python's argparse
 * module.
 *
 * Arguments parsing is common task in cli program, but traditional `getopt`
 * libraries are not easy to use. This library provides high-level arguments
 * parsing solutions.
 *
 * The program defines what arguments it requires, and `argparse` will figure
 * out how to parse those out of `argc` and `argv`, it also automatically
 * generates help and usage messages and issues errors when users give the
 * program invalid arguments.
 *
 * Reserved namespaces:
 *  argparse
 *  OPT
 * Author: Yecheng Fu <cofyc.jackson@gmail.com>
 */

#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct argparse;
struct argparse_option;

typedef int argparse_callback(struct argparse *this,
                              const struct argparse_option *option);

enum argparse_flag {
    ARGPARSE_STOP_AT_NON_OPTION = 1,
};

enum argparse_option_type {
    /* special */
    ARGPARSE_OPT_END,
    ARGPARSE_OPT_GROUP,
    /* options with no arguments */
    ARGPARSE_OPT_BOOLEAN,
    ARGPARSE_OPT_BIT,
    /* options with arguments (optional or required) */
    ARGPARSE_OPT_INTEGER,
    ARGPARSE_OPT_STRING,
};

enum argparse_option_flags {
    OPT_NONEG = 1,              /* Negation disabled. */
};

/*
 *  Argparse option struct.
 *
 *  `type`:
 *    holds the type of the option, you must have an ARGPARSE_OPT_END last in your
 *    array.
 *
 *  `short_name`:
 *    the character to use as a short option name, '\0' if none.
 *
 *  `long_name`:
 *    the long option name, without the leading dash, NULL if none.
 *
 *  `value`:
 *    stores pointer to the value to be filled.
 *
 *  `help`:
 *    the short help message associated to what the option does.
 *    Must never be NULL (except for ARGPARSE_OPT_END).
 *
 *  `callback`:
 *    function is called when corresponding argument is parsed.
 *
 *  `data`:
 *    associated data. Callbacks can use it like they want.
 *
 *  `flags`:
 *    option flags.
 *
 */
struct argparse_option {
    enum argparse_option_type type;
    const char short_name;
    const char *long_name;
    void *value;
    const char *help;
    argparse_callback *callback;
    intptr_t data;
    int flags;
};

/*
 * argpparse
 */
struct argparse {
    // user supplied
    const struct argparse_option *options;
    const char *const *usage;
    int flags;
    // internal context
    int argc;
    const char **argv;
    const char **out;
    int cpidx;
    const char *optvalue;       // current option value
};

// builtin callbacks
int argparse_help_cb(struct argparse *this,
                     const struct argparse_option *option);

// builtin option macros
#define OPT_END()          { ARGPARSE_OPT_END }
#define OPT_BOOLEAN(...)   { ARGPARSE_OPT_BOOLEAN, __VA_ARGS__ }
#define OPT_BIT(...)       { ARGPARSE_OPT_BIT, __VA_ARGS__ }
#define OPT_INTEGER(...)   { ARGPARSE_OPT_INTEGER, __VA_ARGS__ }
#define OPT_STRING(...)    { ARGPARSE_OPT_STRING, __VA_ARGS__ }
#define OPT_GROUP(h)     { ARGPARSE_OPT_GROUP, 0, NULL, NULL, h, NULL }
#define OPT_HELP()         OPT_BOOLEAN('h', "help", NULL, "show this help message and exit", argparse_help_cb)

int argparse_init(struct argparse *this, struct argparse_option *options,
                  const char *const *usage, int flags);
int argparse_parse(struct argparse *this, int argc, const char **argv);
void argparse_usage(struct argparse *this);

#endif
