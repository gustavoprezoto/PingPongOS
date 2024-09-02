#include "../include/logging.h"

// define DEBUG definido no arquivo logging.c ou passando a flag -DDEBUG como parametro pelo GCC.

int test_main() {
    log_debug("Debug is working!");
}