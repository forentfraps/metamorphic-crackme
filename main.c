#include "aes_home/includes.h"
#include "funcs_enc.c"
#include <stdlib.h>
#include <string.h>
#include <windows.h>
typedef int (*FuncPtr)(unsigned char*, unsigned char*);

extern void* GetRip();



int func_wrapper(bl128 KeyList[], unsigned long long params){

    unsigned int sz = sizeof(hello_enc);
    unsigned char f[sizeof(hello_enc)];
    memcpy(&f, &hello_enc, sz);
    unsigned long long rip = GetRip();
    // printf("%llu is the key\n", rip ^ 0x0011223344332211ULL);
    unsigned long long ptr = (unsigned long long) params;
    for (int i = 0; i<sz; i +=16){
        bl128* hello_bl = (bl128*)(f + i);
        Encrypt(hello_bl, KeyList);
    }
    FuncPtr rf;
    PDWORD junk = malloc(sizeof(PDWORD));
    VirtualProtect(&f, sz, PAGE_EXECUTE_READWRITE, junk);
    rf = (FuncPtr)(f);
    unsigned long long ans = ptr ^ 0x0011223344332211ULL;
    return rf(ans,  rip);
}


int main(){

    bl128 MasterKey;
    bl128 KeyList[11];
    unsigned long long p;
    printf("Input Key:\n");
    for (int i = 0; i< 16; ++i){
        MasterKey.bytes[i] = i*16 + i;
    }
    // PrintBlock(MasterKey);
    KeyScheduler(MasterKey, KeyList);
    scanf("%llu", &p);
    if (func_wrapper(KeyList, p)){
        printf("Success!\n");
        return 0;
    }
    printf("Fail\n");
    return 0;
}