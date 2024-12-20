#include <iostream>
#include <string>

struct Foo
{
    Foo() : data_{ "foo" }
    {
        std::cout << "  Foo default constructor\n";
    }

    explicit Foo(const std::string& s) : data_{ s }
    {
        std::cout << "  Foo override constructor\n";
    }

    Foo(const Foo& other)
    {
        std::cout << "  Foo copy constructor\n";
        data_ = other.data_;
    }

    Foo(const Foo&& other)
    {
        std::cout << "  Foo move constructor\n";
        data_ = other.data_;
    }

    Foo& operator=(const Foo& other)
    {
        std::cout << "  Foo copy assignment\n";
        data_ = other.data_;
        return *this;
    }

    void PrintData()
    {
        std::cout << "  " << this << ": data_: " << data_ << "\n\n";
    }

    std::string data_;
};

int main()
{
    std::cout << "Foo f0{};\n";
    Foo f0{};
    f0.PrintData();

    std::cout << "Foo f1{ \"f1\" };\n";
    Foo f1{"f1"};
    f1.PrintData();

    std::cout << "auto f2 = Foo{};\n";
    auto f2 = Foo{};
    f2.PrintData();

    std::cout << "auto f3 = Foo{ \"f3\" };\n";
    auto f3 = Foo{"f3"};
    f3.PrintData();

    std::cout << "auto f4 = f3;\n";
    auto f4 = f3;
    f4.PrintData();

    std::cout << "auto f5 = std::move(f3);\n";
    auto f5 = std::move(f3);
    f5.PrintData();
    
    std::cout << "auto f6 = Foo{ \"f6\" };\n";
    auto f6 = Foo{ "f6" };
    f6.PrintData();

    std::cout << "f6 = f5;\n";
    f6 = f5;
    f6.PrintData();
}
