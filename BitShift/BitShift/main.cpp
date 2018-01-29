#include <iostream>
#include <bitset>
//#include <Windows.h>

typedef unsigned char    BYTE;
typedef unsigned short   WORD;
typedef signed short     SWORD;
typedef unsigned long    DWORD;
typedef signed long      SDWORD;
typedef unsigned __int64 QWORD;
typedef __int64          SQWORD;

#define LIT64(x) x##i64

int main()
{
    SDWORD short11 = -1;
    std::bitset<32> bitset11{ static_cast<DWORD>(short11) };
    std::cout << bitset11 << std::endl;     // 0000000000000001

    short11 <<= 1;
    std::bitset<32> bitset12{ static_cast<DWORD>(short11) };
    std::cout << bitset12 << std::endl;     // 0000000000000010

    short11 <<= 1;
    std::bitset<32> bitset13{ static_cast<DWORD>(short11) };
    std::cout << bitset13 << std::endl;     // 0000000000000100

    short11 <<= 1;
    std::bitset<32> bitset14{ static_cast<DWORD>(short11) };
    std::cout << bitset14 << std::endl;     // 0000000000001000

    short11 <<= 1;
    std::bitset<32> bitset15{ static_cast<DWORD>(short11) };
    std::cout << bitset15 << std::endl;     // 0000000000010000

    short11 <<= 1;
    std::bitset<32> bitset16{ static_cast<DWORD>(short11) };
    std::cout << bitset16 << std::endl;     // 0000000000100000

    short11 <<= 1;
    std::bitset<32> bitset17{ static_cast<DWORD>(short11) };
    std::cout << bitset17 << std::endl;     // 0000000001000000

    short11 <<= 1;
    std::bitset<32> bitset18{ static_cast<DWORD>(short11) };
    std::cout << bitset18 << std::endl;     // 0000000010000000

    short11 <<= 1;
    std::bitset<32> bitset19{ static_cast<DWORD>(short11) };
    std::cout << bitset19 << std::endl;     // 0000000100000000

    short11 <<= 2;
    std::bitset<32> bitset21{ static_cast<DWORD>(short11) };
    std::cout << bitset21 << std::endl;     // 0000010000000000

    short11 <<= 2;
    std::bitset<32> bitset22{ static_cast<DWORD>(short11) };
    std::cout << bitset22 << std::endl;     // 0001000000000000

    short11 <<= 3;
    std::bitset<32> bitset31{ static_cast<DWORD>(short11) };
    std::cout << bitset31 << std::endl;     // 1000000000000000

    printf("short11 = 0x%lx\n", short11);

    short11 = -1;
    short11 <<= 8;
    std::bitset<32> bitset44{ static_cast<DWORD>(short11) };
    std::cout << bitset44 << std::endl;     // 0000000000000010

    printf("short11 = 0x%lx\n", short11);

    QWORD qw1 = 0x0001000000000000 & LIT64(0x0000ffffffffffff);
    WORD qw2 = (WORD)0x3fff;
    WORD qw3 = (WORD)(0x0 >> 48);
    WORD qw4 = (WORD)0x3fff + (WORD)(qw1 >> 48);

    printf("qw1 = 0x%x\n", qw1);
    printf("qw2 = 0x%x\n", qw2);
    printf("qw3 = 0x%x\n", qw3);
    printf("qw4 = 0x%x\n", qw4);

    std::bitset<128> bitset51{ static_cast<QWORD>(qw1) };
    std::bitset<128> bitset52{ static_cast<QWORD>(qw2) };

    std::cout << bitset51 << std::endl;     // 0000000000000010
    std::cout << bitset52 << std::endl;     // 0000000000000010

    printf("sizeof(double) = %d\n", sizeof(double));
    printf("sizeof(QWORD) = %d\n", sizeof(QWORD));
    printf("sizeof(DWORD) = %d\n", sizeof(DWORD));
    printf("sizeof(WORD) = %d\n", sizeof(WORD));
    printf("sizeof(long) = %d\n", sizeof(long));
    printf("sizeof(int) = %d\n", sizeof(int));
    printf("sizeof(float) = %d\n", sizeof(float));

    return 0;
}