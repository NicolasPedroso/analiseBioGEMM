#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <diretório>\n", argv[0]);
        return 1;
    }

    readBase(argv[1]);

    return 0;
}