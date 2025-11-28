#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char *feelings = malloc(100);
    strcpy(feelings, "I love you");

    printf("%s\n", feelings);
    free(feelings);

    // Still trying to feel something after it's gone
    printf("%s\n", feelings); // 💔 Segmentation fault (core dumped)

    return 0;
}
