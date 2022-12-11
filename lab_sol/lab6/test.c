#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#define DEBUG 
typedef struct
{
    uint16_t pc;
    int n;
    int z;
    int p;
    char rom[65536][16];
    uint16_t R[8];
} TINYVM_CORE;

TINYVM_CORE core;
int chartohex(char* str, int t){
    int temp = 0;
    int flag = 0;
    int max = 1<<t;
    if(*str == '1') flag = 1;
    while (t--){
        temp = temp * 2 + (*str - '0');
        str++;
    }
    if(flag){
        temp -= max; 
    }
    return temp;
}
char* myitoa(uint16_t num, char* str, int radix){
    char index[]="0123456789ABCDEF";
    unsigned unum;
    int i=15,j,k=15;
    unum=(unsigned)num;
    do{
        str[i--]=index[unum%(unsigned)radix];
        unum/=radix;
    }while(unum);
    for(j=k;j<=(i-1)/2;j--){       
        char temp;
        temp=str[j];
        str[j]=str[i-1+k-j];
        str[i-1+k-j]=temp;
    }
    return str;
}
int main(){
    uint16_t value = 32767;
    if(value==0){
        printf("z\n");
    }else if(value & (1<<15)){
        printf("n\n");
    }else{
        printf("p\n");
    }
    printf("%d\n", chartohex("10000000", 8));
    core.R[0] = 1;
    core.R[1] = ~core.R[0];
    printf("%d\n", core.R[1]);
    char a[16];
    memset(a, '0', 16);
    a[16] = '\0';
    printf("itoa = %s\n", myitoa(16, a, 2));
    uint16_t SEXT = (uint16_t)chartohex("10001", 5);
    printf("%u",SEXT);
}