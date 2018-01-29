#include <iostream>
#include <random>

int main()
{
    std::random_device rd;
    std::mt19937 urng(rd());

    for (int i = 0; i < 10; i++)
    {
        std::cout << urng() << std::endl;
    }

    return 0;
}
