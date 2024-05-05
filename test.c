#include <stdio.h>
#include <stdlib.h>
#include <time.h>

char getRandomChar() {
    char chars[] = {'R', 'G', 'P', 'V'};
    int index = rand() % 4; // Generate a random index between 0 and 3
    return chars[index];
}

int main() {
    srand(time(NULL)); // Seed the random number generator with current time

    char randomChar = getRandomChar();
    printf("Random character: %c\n", randomChar);

    return 0;
}