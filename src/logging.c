#include <stdio.h>

// DEBUG definido neste arquivo ou passando a flag -DDEBUG como parametro pelo GCC.
#define DEBUG

void log_debug(char* msg) {
#ifdef DEBUG
    printf("%s\n", msg);
#endif
}
