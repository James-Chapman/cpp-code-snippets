#include <cstdio>

int main()
{
    bool fizz = false;
    bool buzz = false;
    for (int i = 1; i <= 100; ++i)
    {
        fizz = false;
        buzz = false;
        if (i % 3 == 0)
            fizz = true;
        if (i % 5 == 0)
            buzz = true;
        if (fizz)
            printf("Fizz");
        if (buzz)
            printf("Buzz");
        if (!fizz && !buzz)
            printf("%d", i);
        printf("\n");

    }


    return 0;
}