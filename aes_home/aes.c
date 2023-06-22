#include "constants.c"
#include "includes.h"
extern int _ShiftRows(unsigned char*[]);
extern int _MixColumn(unsigned long* operand, void* table);
extern int _Sbox(unsigned char* list, void* table);
extern int _KeyAdd(unsigned char* block, unsigned char* key);

void PrintBlock(bl128 b){
    for(int i =0; i < 16; ++i){
        printf("%x ", b.bytes[i]);
    }
    printf("\n");
}

unsigned char GaloisMul_slow(unsigned char op1, unsigned char op2){
    return MultiplicationTable[op1][op2];
}


void SBox_slow(bl128* block){
    for (int i = 0; i < 16; i++){
        block->bytes[i] = sbox[block->bytes[i]];
    }
}

void SBox_wrapper(bl128* block){
    _Sbox(&(block->bytes[0]), sbox);
}


void MixColumn_wrapper(unsigned long* val){
    _MixColumn(val, MultiplicationTable);
}

// xor in xmm's rather than int wise
void KeyAdd_slow(bl128* block, bl128 key){
    for(int i = 0; i < 4; ++i){
        block->ints[i] ^= key.ints[i];
    }
}

void KeyAdd_wrapper(bl128* block, bl128 key){
    _KeyAdd(&(block->bytes[0]), &(key.bytes[0]));
}



void CipherRound(bl128* restrict block, bl128 key){
    unsigned char* ptr = &(block->bytes[0]);
    _Sbox(ptr, sbox);
    _ShiftRows(ptr);
    for (int i = 0; i < 4; ++i){
        _MixColumn(&(block->ints[i]), MultiplicationTable);
    }
    _KeyAdd(ptr, &(key.bytes[0]));
}



void Encrypt(bl128* restrict block, bl128* KeyList){
    unsigned char* ptr = &(block->bytes[0]);
    _KeyAdd(ptr, &((*KeyList).bytes[0]));
    for (int i = 0; i < 9; ++i){
        CipherRound(block, KeyList[i+1]);
    }
    _Sbox(ptr, sbox);
    _ShiftRows((unsigned char**)block->bytes);
    _KeyAdd(ptr, &((KeyList[10]).bytes[0]));
}

void CopyBlock(bl128 src, bl128* dest){
    for (int i = 0; i< 4; ++i){
        dest->ints[i]=src.ints[i];
    }
}

unsigned long G(unsigned long i32, int i){
    bl32 a;
    a.i = i32;
    unsigned char temp = a.bytes[0];
    a.bytes[0] = sbox[a.bytes[1]] ^ RC[i+1 % 10];
    a.bytes[1] = sbox[a.bytes[2]];
    a.bytes[2] = sbox[a.bytes[3]];
    a.bytes[3] = sbox[temp];
    return a.i;
}


void KeyScheduler(bl128 MasterKey, bl128* dest){
    bl128 tmp = MasterKey;
    bl128 scratch;
    CopyBlock(tmp, dest);
    for(int i = 0; i<10;++i){
        scratch.ints[0]  = G(tmp.ints[3], i) ^ tmp.ints[0];
        scratch.ints[1] = scratch.ints[0] ^ tmp.ints[1];
        scratch.ints[2] = scratch.ints[1] ^ tmp.ints[2];
        scratch.ints[3] = scratch.ints[2] ^ tmp.ints[3];
        CopyBlock(scratch, &(dest[i+1]));
        CopyBlock(scratch, &tmp);
    }
}

