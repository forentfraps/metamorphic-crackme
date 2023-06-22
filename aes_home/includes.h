#ifndef _INCLUDES_H_
#define _INCLUDES_H_

#include <stdio.h>

union bl128{
    unsigned long ints[4];
    unsigned char bytes[16];
}typedef bl128;

union bl32{
    unsigned long i;
    unsigned char bytes[4];
} typedef bl32;


void PrintBlock(bl128 b);
void Encrypt(bl128* restrict block, bl128* KeyList);
void KeyScheduler(bl128 MasterKey, bl128* dest);


#endif