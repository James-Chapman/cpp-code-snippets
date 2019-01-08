#include <cstdio>
#include <string>
#include <vector>

int main()
{
    //bool fizz = false;
    //bool buzz = false;
    //for (int i = 1; i <= 100; ++i)
    //{
    //    fizz = false;
    //    buzz = false;
    //    if (i % 3 == 0)
    //        fizz = true;
    //    if (i % 5 == 0)
    //        buzz = true;
    //    if (fizz)
    //        printf("Fizz");
    //    if (buzz)
    //        printf("Buzz");
    //    if (!fizz && !buzz)
    //        printf("%d", i);
    //    printf("\n");

    //}


    std::vector<int> vecJumps;
    printf("vecJumps.size() = %d\n", vecJumps.size());
    printf("vecJumps.capacity() = %d\n", vecJumps.capacity());

    vecJumps.push_back(0);
    vecJumps.push_back(1);
    vecJumps.push_back(2);
    vecJumps.push_back(3);
    vecJumps.push_back(4);
    vecJumps.push_back(5);
    vecJumps.push_back(6);
    vecJumps.push_back(7);
    vecJumps.push_back(8);
    vecJumps.push_back(9);

    printf("vecJumps.size() = %d\n", vecJumps.size());
    printf("vecJumps.capacity() = %d\n", vecJumps.capacity());


    std::vector<int>::iterator vecStart1 = vecJumps.begin();
    std::vector<int>::iterator vecStart2 = vecJumps.begin();
    std::vector<int>::iterator vecIt = vecJumps.begin();

    while (vecIt != vecJumps.end())
    {
        printf("vecIt [%p] = %d\n", vecIt, *vecIt);
        if (*vecIt == 5)
        {
            vecIt = vecJumps.erase(vecIt);
        }
        else
        {
            ++vecIt;
        }
    }

    vecStart2 = vecJumps.begin();
    vecIt = vecJumps.begin();

    while (vecIt != vecJumps.end())
    {
        printf("vecStart1 [%p] = %d\n", vecStart1, *vecStart1);
        ++vecIt;
        ++vecStart1;
    }

    vecIt = vecJumps.begin();

    while (vecIt != vecJumps.end())
    {
        printf("vecStart2 [%p] = %d\n", vecStart2, *vecStart2);
        ++vecIt;
        ++vecStart2;
    }

    return 0;
}