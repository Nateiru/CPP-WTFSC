#include <iostream>

class Pair {
public:
  Pair() : x(0), y(0) {
    std::cout << "Pair()" << std::endl;
  }

  Pair(int x_, int y_) : x(x_), y(y_) {
    std::cout << "Pair(int, int)" << std::endl;
  }

  // 如果没有 && 那么对于 non-const rvalue reference 
  // 也会被下面函数捕获，由于是 const 并不能实现所有权转移
  Pair(const Pair &rhs) : x(rhs.x), y(rhs.y) {
    std::cout << "Pair(const Pair &)" << std::endl;
  }

  // 为了移动不能是 const 
  Pair(Pair &&rhs) {
    x = std::move(rhs.x);
    y = std::move(rhs.y);
    // 可选择在移动后清空原始对象的状态
    // 注意：被 move 的变量是未定义的
    rhs.x = 0;
    rhs.y = 0;
    std::cout << "Pair(Pair &&)" << std::endl;
  };

  // 用限定符 & 控制哪些对象能够调用特定的成员函数
  Pair& operator=(const Pair&) & = default;
  Pair& operator=(Pair&&) & = default;

  ~Pair() {
    std::cout << "~Pair()" << std::endl;
  }
private:
  int x, y;
};

int main() {

  std::cout << "=========== construct ===========" << std::endl;
  Pair p1;                  // 调用默认构造函数
  Pair p2{1, 2};     // 调用带参数的构造函数
  Pair p3(p2);              // 调用复制构造函数，使用默认生成的实现
  Pair p4(std::move(p3)); // 调用移动构造函数，使用默认生成的实现

  Pair p5 = Pair(-1, -2);
  // Pair p6{std::move(Pair())};
  std::cout << "=========== deconstruct ===========" << std::endl;
  return 0;
}