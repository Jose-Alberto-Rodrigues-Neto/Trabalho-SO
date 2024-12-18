#include <stdio.h>

int main(){
    char input;
    while(1){
        input = getchar();
        if(input == 's'){
            break;
        }
        printf("loop\n");
    }
    return 0;
}