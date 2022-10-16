#include <cassert>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

namespace
{
void add(std::mutex& mtx, int& counter, int i)
{
    std::lock_guard<std::mutex> lock(mtx);
    counter += i;
}

class ThreadWorker
{
  public:
    ThreadWorker()
    {
    }

    virtual ~ThreadWorker()
    {
    }

    void Add(std::mutex& mtx, int& counter, int i)
    {
        std::lock_guard<std::mutex> lock(mtx);
        counter += i;
    }
};

} // namespace

int main()
{
    int counter{0};
    std::mutex nMutex;
    ThreadWorker worker;
    std::vector<std::thread> threadVector;

    {
        // Start 10 threads with 'add' function.
        for (int i = 0; i < 10; i++)
        {
            threadVector.push_back(std::thread(add, std::ref(nMutex), std::ref(counter), i));
        }
    }

    {
        // Start 10 threads with 'ThreadWorker::Add' class method.
        for (int i = 0; i < 10; i++)
        {
            threadVector.push_back(
                std::thread(&ThreadWorker::Add, std::ref(worker), std::ref(nMutex), std::ref(counter), i));
        }
    }

    {
        // Start 10 threads with lambda function.
        for (int i = 0; i < 10; i++)
        {
            threadVector.push_back(std::thread([&counter, &nMutex, i]() {
                {
                    std::lock_guard<std::mutex> lock(nMutex);
                    counter += i;
                }
            }));
        }
    }

    {
        // Start 10 threads with a named and better expressed lambda function.
        for (int i = 0; i < 10; i++)
        {
            auto threadFunc = [&counter, &nMutex, i]() {
                {
                    std::lock_guard<std::mutex> lock(nMutex);
                    counter += i;
                }
            };
            threadVector.push_back(std::thread(threadFunc));
        }
    }

    for (auto& thr : threadVector)
    {
        thr.join();
    }

    std::cout << " counter = " << counter << "\n";
    assert(counter == 180);
    return 0;
}
