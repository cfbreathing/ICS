#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

// #define DEBUG 

int chartonum(char* str, int t){
    int temp = 0;
    while (t--){
        temp = temp * 2 + (*str - '0');
        str++;
    }
    return temp;
}

int chartouns(char* str, int t){
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

char* my_itoa(uint16_t num, char* str, int radix){
    char index[]="0123456789ABCDEF";
    int i=15,j,k=15;
    char temp;
    do{
        str[i--]=index[num%(unsigned)radix];
        num/=radix;
    }while(num);
    for(j=k;j<=(i-1)/2;j--){       
        temp=str[j];
        str[j]=str[i-1+k-j];
        str[i-1+k-j]=temp;
    }
    return str;
}

typedef struct{
    char opcode[4];
    char value[12];
} VM_INST;

typedef struct{
    uint16_t pc;
    int n;
    int z;
    int p;
    char rom[65536][16];
    uint16_t R[8];
} VM_CORE;

VM_CORE core;

//setcc
void setCondition(VM_CORE *c, uint16_t value){
    if(value == 0){
        c->n = 0;  c->p = 0;  c->z = 1;
    }else if(value & (1<<15)){
        c->n = 1;  c->p = 0;  c->z = 0;
    }else{
        c->n = 0;  c->p = 1;  c->z = 0;
    }
}

void do_BR(VM_CORE *c, VM_INST* inst){
    if((inst->value[0]-'0' && c->n) || (inst->value[1]-'0' && c->z) || (inst->value[2]-'0' && c->p)){
        int SEXT = chartouns((char*)inst->value + 3, 9);
        c->pc += SEXT;
    }
}

void do_ADD(VM_CORE *c, VM_INST* inst){   
    int DR = chartonum((char*)inst->value, 3);
    int SR1 = chartonum((char*)inst->value + 3, 3);
    if(inst->value[6] == '1'){
        int SEXT = chartouns((char*)inst->value + 7, 5);
        c->R[DR] = c->R[SR1] + SEXT;
    }else{
        int SR2 = chartonum((char*)inst->value + 9, 3);
        c->R[DR] = c->R[SR1] + c->R[SR2];
    }
    setCondition(c, c->R[DR]);
}

void do_LD(VM_CORE *c, VM_INST* inst){
    int DR = chartonum((char*)inst->value, 3);
    int PCoffset9 = chartouns((char*)inst->value + 3, 9);
    c->R[DR] = chartonum(c->rom[c->pc + PCoffset9], 16);
    setCondition(c, c->R[DR]);
}

void do_ST(VM_CORE *c, VM_INST* inst){
    int SR = chartonum((char*)inst->value, 3);
    int PCoffset9 = chartouns((char*)inst->value + 3, 9);
    char * temp = malloc(sizeof(VM_INST) + 1);
    memset(temp, '0', 16);
    memcpy(c->rom[c->pc + PCoffset9], my_itoa(c->R[SR], temp, 2), 16);
    free(temp);
}

void do_JSR(VM_CORE *c, VM_INST* inst){
    uint16_t temp = c->pc;
    if(inst->value[0] == '1'){
        int PCoffset11 = chartouns((char*)inst->value + 1, 11);
        c->pc = c->pc + PCoffset11;
    }else{
        int BaseR = chartonum((char*)inst->value + 3, 3);
        c->pc = c->R[BaseR];
    }
    c->R[7] = temp;
}

void do_AND(VM_CORE *c, VM_INST* inst){
    int DR = chartonum((char*)inst->value, 3);
    int SR1 = chartonum((char*)inst->value + 3, 3);
    if(inst->value[6] == '1'){
        uint16_t SEXT = (uint16_t)chartouns((char*)inst->value + 7, 5);
        c->R[DR] = c->R[SR1] & SEXT;
    }else{
        int SR2 = chartonum((char*)inst->value + 9, 3);
        c->R[DR] = c->R[SR1] & c->R[SR2];
    }
    setCondition(c, c->R[DR]);
}

void do_LDR(VM_CORE *c, VM_INST* inst){
    int DR = chartonum((char*)inst->value, 3);
    int BaseR = chartonum((char*)inst->value + 3, 3);
    int offset6 = chartouns((char*)inst->value + 6, 6);
    c->R[DR] = chartonum(c->rom[c->R[BaseR] + offset6], 16);
    setCondition(c, c->R[DR]);      
}

void do_STR(VM_CORE *c, VM_INST* inst){
    int SR = chartonum((char*)inst->value, 3);
    int BaseR = chartonum((char*)inst->value + 3, 3);
    int offset6 = chartouns((char*)inst->value + 6, 6);
    char * temp = malloc(sizeof(VM_INST) + 1);
    memset(temp, '0', 16);
    memcpy(c->rom[c->R[BaseR] + offset6], my_itoa(c->R[SR], temp, 2), 16);
    free(temp); 
}

void do_NOT(VM_CORE *c, VM_INST* inst){
    int DR = chartonum((char*)inst->value, 3);
    int SR = chartonum((char*)inst->value + 3, 3);
    c->R[DR] = ~c->R[SR];
    setCondition(c, c->R[DR]);    
}

void do_LDI(VM_CORE *c, VM_INST* inst){
    int DR = chartonum((char*)inst->value, 3);
    int PCoffset9 = chartouns((char*)inst->value + 3, 9);
    int address = chartonum(c->rom[c->pc + PCoffset9], 16);
    c->R[DR] = chartonum(c->rom[address], 16);
    setCondition(c, c->R[DR]);    
}

void do_STI(VM_CORE *c, VM_INST* inst){
    int SR = chartonum((char*)inst->value, 3);
    int PCoffset9 = chartouns((char*)inst->value + 3, 9);
    char* temp = malloc(sizeof(VM_INST) + 1);
    int address = chartonum(c->rom[c->pc + PCoffset9], 16);
    memset(temp, '0', 16);
    memcpy(c->rom[address], my_itoa(c->R[SR], temp, 2), 16);
    free(temp);    
}

void do_JMP(VM_CORE *c, VM_INST* inst){
    int BaseR = chartonum((char*)inst->value + 3, 3);
    c->pc = c->R[BaseR];
}

void do_LEA(VM_CORE *c, VM_INST* inst){
    int DR = chartonum((char*)inst->value, 3);
    int PCoffset9 = chartouns((char*)inst->value + 3, 9);
    c->R[DR] = c->pc + PCoffset9;
}

void do_OUT(VM_CORE *c, VM_INST* inst){
    int t = 0;
    for(t = 0; t<=7 ; t++) printf("R%d = x%04hX\n", t, c->R[t]);

#ifdef DEBUG
    printf("n = %d\n", c->n);
    printf("z = %d\n", c->z);
    printf("p = %d\n", c->p);
#endif
}

//loop to execute
int main_loop(VM_CORE *c){
    VM_INST *inst = malloc(sizeof(VM_INST) + 1);
    ((char*)inst)[16] = '\0';
    while(1){
    // fetch the opcode
        memcpy(inst, c->rom[c->pc], sizeof(VM_INST));

#ifdef DEBUG
        printf("[DEBUG] opcode: %0.4s, value: %0.12s\n", inst->opcode, inst->value);
        printf("[DEBUG] pc: %d\n", c->pc);
#endif

    // increment the pc 
        c->pc += 1;
    // execute
        switch (chartonum(inst->opcode, 4)){
            case 0:{
                do_BR(c, inst);
                break;
            }case 1:{
                do_ADD(c, inst);
                break;
            }case 2:{
                do_LD(c, inst);
                break;
            }case 3:{
                do_ST(c, inst);
                break;
            }case 4:{
                do_JSR(c, inst);
                break;
            }case 5:{
                do_AND(c, inst);
                break;
            }case 6:{
                do_LDR(c, inst);
                break;
            }case 7:{
                do_STR(c, inst);
                break;
            }case 8:{
                do_RTI(c, inst);
                break;
            }case 9:{
                do_NOT(c, inst);
                break;
            }case 10:{
                do_LDI(c, inst);
                break;
            }case 11:{
                do_STI(c, inst);
                break;
            }case 12:{
                do_JMP(c, inst);
                break;
            }case 14:{
                do_LEA(c, inst);
                break;
            }case 15:{   
                if(chartonum(inst->value, 12) == 37){
                    do_OUT(c, inst);
                    return 0;
                }
            }default:
                printf("Error!\n");
                exit(-1);
        }
    }
}
//Initialization
void setup(VM_CORE* core){
    int t = 0;
    char temp[17] = "0111011101110111";
    for(t=0; t<65536; t++){
        memcpy(core->rom[t], temp, 16);
    }
    core->n = 0;  core->z = 0;  core->p = 0;
    core->R[0] = 30583; core->R[1] = 30583; core->R[2] = 30583; core->R[3] = 30583; 
    core->R[4] = 30583; core->R[5] = 30583; core->R[6] = 30583; core->R[7] = 30583;
    t = 0;
    char c = '\0';
    char *address = malloc(sizeof(VM_INST) + 1);
    while (t!=16){
        c=getchar();
        address[t++] = c;
    }
    address[16] = '\0';
    core->pc = (uint16_t)chartonum(address, 16);
    free(address);
    t = 0;
    while ((c=getchar())!= EOF){
        if(c!=10){
            core->rom[core->pc][t++] = c;
        }
    }

#ifdef DEBUG
    printf("ok!\n");
    printf("starting address = %d\n", core->pc);
#endif
}

int main(){
    setup(&core);
    main_loop(&core);
    return 0;
}