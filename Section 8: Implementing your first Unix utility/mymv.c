#include <stdio.h>

int mv_main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("HERERE: %s <source_file> <destination_file>\n", argv[0]);
        return 1;
    }

    if (rename(argv[1], argv[2]) != 0) {
        printf("Error moving file");
        return 1;
    }

    return 0;
}
