#ifndef RUN_MODES_H
#define RUN_MODES_H

#include <stdlib.h>

void noninteractive_run(const char *filename);

void c_option_run(int argc, char *const *argv);

void interactive_mode_loop(void);

size_t input_too_large(const char *input);

#endif
