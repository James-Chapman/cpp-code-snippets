Output:


```C++
Foo f0{};
  Foo default constructor
  0x7fffc79ac7d0: data_: foo

Foo f1{ "f1" };
  Foo override constructor
  0x7fffc79ac7b0: data_: f1

auto f2 = Foo{};
  Foo default constructor
  0x7fffc79ac790: data_: foo

auto f3 = Foo{ "f3" };
  Foo override constructor
  0x7fffc79ac770: data_: f3

auto f4 = f3;
  Foo copy constructor
  0x7fffc79ac750: data_: f3

auto f5 = std::move(f3);
  Foo move constructor
  0x7fffc79ac730: data_: f3

auto f6 = Foo{ "f6" };
  Foo override constructor
  0x7fffc79ac710: data_: f6

f6 = f5;
  Foo copy assignment
  0x7fffc79ac710: data_: f3
```