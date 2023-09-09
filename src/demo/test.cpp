#include <memory>
#include <iostream>

class A {
public:
  A(int a_) : a(a_), ptr(std::make_unique<int> (a)) {}
  int a;
  std::unique_ptr<int> ptr;
};

int main() {
  int x = 1;
  A a(x);
  std::cout << *(a.ptr) << std::endl;
}