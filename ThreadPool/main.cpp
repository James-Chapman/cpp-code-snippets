 /*******************************************************************************
  * Copyright (c) 2014 James Chapman
  *
  * Permission is hereby granted, free of charge, to any person obtaining a copy
  * of this software and associated documentation files (the "Software"), to deal
  * in the Software without restriction, including without limitation the rights
  * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  * copies of the Software, and to permit persons to whom the Software is
  * furnished to do so, subject to the following conditions:
  *
  * 1. The above copyright notice and this permission notice shall be included in
  *    all copies or substantial portions of the Software.
  *
  * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  * SOFTWARE.
  *
  ********************************************************************************/

 #include <functional>
 #include <iostream>
 #include <random>

 #include "commonthreadpool.hpp"

 void functionForThreadToRun(Uplinkzero::Common::FunctionParams &p)
 {
     std::cout << "Thread ID: " << std::this_thread::get_id() << " running with parameter: " << p.i << std::endl;
     std::this_thread::sleep_for(std::chrono::milliseconds(100));
 }

 int main()
 {
     // random number generator
     std::random_device rd;
     std::mt19937 urng(rd());

     // Create a threadpool with 5 threads
     ::Uplinkzero::Common::ThreadPool<Uplinkzero::Common::ThreadData> threadPool(5);

     // Pointer to the function we want the threads in the pool to execute
     auto pFunctionForThreadToRun = std::function<void(Uplinkzero::Common::FunctionParams &)>(functionForThreadToRun);

     // enque tasks into the threadpool
     for (int i = 0; i < 1000; i++)
     {
         Uplinkzero::Common::FunctionParams params;
         params.i = urng();
         Uplinkzero::Common::ThreadData td;
         td.setThreadFunction(pFunctionForThreadToRun);
         td.setThreadFunctionParams(params);
         threadPool.enqueue(td);
     }

     // sleep for 10 seconds
     std::this_thread::sleep_for(std::chrono::milliseconds(10000));

     std::cout << "Main thread finished!" << std::endl;

     return 0;
 }
