#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main() {
    char* username = getlogin();
    char test[30] = "media";
    strcat(test,username);

    printf("Hello World %s\n",test);

    return 0;
}