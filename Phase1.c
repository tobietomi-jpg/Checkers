#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

unsigned int SetBit32(unsigned int value, int pos){
    if(pos < 0 || pos >= 32) return value;
    return value | (1u << pos);
}

unsigned int ClearBit32(unsigned int value, int pos){
    if (pos < 0 || pos >= 32) return value;
    return value & (~ (1u << pos));
}

unsigned int ToggleBit32(unsigned int value, int pos){
    if(pos < 0 || pos >= 32) return value;
    return value ^ (1u << pos);
}

int GetBit32(unsigned int value, int pos){
    if(pos < 0 || pos >= 32) return 0;
    return (value >> pos) & 1u;
}

int CountBits32(unsigned int value){
    int count = 0;
    while(value){
        value &= (value - 1);
        count++;
    }
    return count;
}

unsigned int ShiftRight32(unsigned int value, int positions){
    if (positions < 0) return ShiftLeft32(value, -positions);
    if (positions >= 32) return 0u;
    return value >> positions;
}

unsigned int ShiftLeft32(unsigned int value, int positions){
    if (positions < 0) return ShiftLeft32(value, -positions);
    if (positions >= 32) return 0u;
    return value << positions;
}


void PrintBinary(unsigned int value){
    for (int i = 31; i >= 0; i--){
        putchar(((value >> i) & 1u) ? '1' : '0');
        if (i % 4 == 0 && i != 0) putchar(' ');
    }
    putchar('\n');
}

void PrintHex32(unsigned int value) {
    printf("0x%08X\n", value);
}

uint64_t SetBit64(uint64_t value1, int pos1){
    if (pos1 < 0 || pos1 >= 64) return value1;
    return value1 | (1ull << pos1);    
}

uint64_t ClearBit64(uint64_t value1, int pos1){
    if (pos1 < 0 || pos1 >= 64) return value1;
    return value1 & ~(1ull << pos1);    
}

uint64_t ToggleBit64(uint64_t value1, int pos1){
    if (pos1 < 0 || pos1 >= 64) return value1;
    return value1 ^ (1ull << pos1);
}

int main(){
    uint32_t test = 0;
    printf("===== TESTING 32-bit BIT FUNCTIONS =====\n\n");

    // --- Test SetBit32 ---
    printf("Initial value: ");
    PrintBinary(test);
    test = SetBit32(test, 3); // set bit 3
    printf("After SetBit32(pos=3): ");
    PrintBinary(test);
    printf("Expected binary: 0000 0000 0000 1000 (bit 3 set)\n\n");

    // --- Test GetBit32 ---
    printf("GetBit32(pos=3): %d (expected 1)\n", GetBit32(test, 3));
    printf("GetBit32(pos=2): %d (expected 0)\n\n", GetBit32(test, 2));

    // --- Test CountBits32 ---
    printf("CountBits32: %d (expected 1)\n\n", CountBits32(test));

    // --- Test ClearBit32 ---
    test = ClearBit32(test, 3);
    printf("After ClearBit32(pos=3): ");
    PrintBinary(test);
    printf("Expected binary: 0000 0000 0000 0000\n");
    printf("CountBits32: %d (expected 0)\n\n", CountBits32(test));

    // --- Test ToggleBit32 ---
    test = ToggleBit32(test, 1);
    printf("After ToggleBit32(pos=1): ");
    PrintBinary(test);
    printf("Expected binary: 0000 0000 0000 0010\n");

    test = ToggleBit32(test, 1); // toggle again
    printf("After ToggleBit32(pos=1) again: ");
    PrintBinary(test);
    printf("Expected binary: 0000 0000 0000 0000\n\n");

    // --- Test ShiftLeft32 & ShiftRight32 ---
    test = 5; // binary 0000 0000 0000 0101
    printf("Original value (5): ");
    PrintBinary(test);
    printf("ShiftLeft32(5, 2): ");
    PrintBinary(ShiftLeft32(test, 2));
    printf("Expected binary: 0000 0000 0001 0100 (value 20)\n");

    printf("ShiftRight32(5, 1): ");
    PrintBinary(ShiftRight32(test, 1));
    printf("Expected binary: 0000 0000 0000 0010 (value 2)\n\n");

    // --- Test PrintHex32 ---
    printf("PrintHex32(255): ");
    PrintHex32(255); // Expect 0x000000FF
    printf("PrintHex32(305419896): ");
    PrintHex32(305419896); // Expect 0x12345678

    printf("\n===== END OF TESTS =====\n");

    test = SetBit32 ( test , 3) ;
    printf("After setting bit 3: ");
    PrintBinary(test);

    printf (" Number of 1s: %d\n", CountBits32 ( test ) ) ;

    return 0;
}
