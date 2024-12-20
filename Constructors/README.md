Output:

```C++
Foo f0{};
  Foo default constructor
  0x7ffde0c0dad0: data_: 

Foo f1{ "f1" };
  Foo override constructor
  0x7ffde0c0dab0: data_: f1

auto f2 = Foo{};
  Foo default constructor
  0x7ffde0c0da90: data_: 

auto f3 = Foo{ "f3" };
  Foo override constructor
  0x7ffde0c0da70: data_: f3

auto f4 = f3;
  Foo copy constructor
  0x7ffde0c0da50: data_: f3

auto f5 = std::move(f3);
  Foo move constructor
  0x7ffde0c0da30: data_: f3

auto f6 = Foo{ "f6" };
  Foo override constructor
  0x7ffde0c0da10: data_: f6

f6 = f5;
  Foo copy assignment
  0x7ffde0c0da10: data_: f3
```
