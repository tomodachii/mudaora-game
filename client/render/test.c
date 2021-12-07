#include <stdio.h>
#include<string.h>

int main()
{
    // for(int i = 0; i < 256; i++){
    //     printf("%d %c\n", i, i);
    // }
    // printf("\xE2\x98\xA0");
    // printf("😀😀😀😀😀😀\n"); 
    // printf("👍👍👍\n");
    // printf("😠😠😠😠😠\n");
    char username[10] = "long";
    char str[10][10];
    strcpy(str[0], username);
    printf("%s", str[0]);
}