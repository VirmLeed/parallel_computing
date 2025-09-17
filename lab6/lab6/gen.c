#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define LINES 10000
#define LENGTH 100000
#define PROGRESS_INTERVAL 100

int main() {
    srand(time(NULL));
    const char *ascii_letters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    FILE *f = fopen("input.txt", "w");

    for (int line = 0; line < LINES; line++) {
        for (int letter = 0; letter < LENGTH; letter++) {
            fputc(ascii_letters[rand() % 52], f);
        }
        fputc('\n', f);

        if (line % PROGRESS_INTERVAL == 0) {
            printf("Progress: %d/%d lines generated\n", line, LINES);
        }
    }

    fclose(f);
    return 0;
}
