#include <iostream>
#include <vector>
#include <memory>
#include <cstring>
#include <functional>

class Widget {
public:
  Widget() = default;

  Widget(const std::vector<int> &vec, const std::string &str) : vec(vec), str(str) {}

  Widget(const Widget&) = default;

  Widget& operator = (const Widget&) = default;

  friend void Swap(Widget& a, Widget& b) {
    std::swap(a.vec, b.vec);
    std::swap(a.str, b.str);
  }

  Widget(Widget &&rhs) {
    Swap(*this, rhs);
  }

  Widget& operator = (Widget &&rhs) {
    Swap(*this, rhs);
    return *this;
  }

private:
  std::vector<int> vec;
  std::string str;
};

// 使用模板不定长参数来保存数据
template<int ...Args>
struct List;

// 特化一个Null来表示结尾
using Null = List<>;

// 特化链表的结构
template<int head, int ...Args>
struct List<head, Args...> {
    static constexpr int value = head;
    using next = List<Args...>;
};

template<int head>
struct List<head> {
    static constexpr int value = head;
    using next = Null;
};

template <typename T> void foo(T t) { std::cout << t << std::endl; }
// primary template
template <int N>  // non-type parameter N
struct binary {
  // an template instantiation inside the template itself, which contructs a recursion
  static constexpr int value = binary<N / 10>::value << 1 | N % 10;
};
// full specialization when N == 0
template <> struct binary<0> {
  static constexpr int value = 0;
};

struct Base {
  using type = Base;
};

struct Derive : public Base {
  type B;
};

template <typename T>
struct rank : std::integral_constant<std::size_t, 0> {};                          // #1

template <typename T>
struct rank<T[]> : std::integral_constant<std::size_t, rank<T>::value + 1> {};    // #2

template <typename T, std::size_t N>
struct rank<T[N]> : std::integral_constant<std::size_t, rank<T>::value + 1> { };  // #3

template <typename T>
using rank_t = typename rank<T>::value_type;

template <typename T>
inline constexpr rank_t<T> rank_v = rank<T>::value;



void f() {
}
#include <typeinfo>

class Test {
public:
  void print() {
    std::cout << "Hello World" << std::endl;
  }
};

int main() {

  using list = List<1, 1, 4, 5, 1, 4>;
  // std::cout << list::value << std::endl;
  // std::cout << list::next::value << std::endl;
  // std::cout << list::next::next::value << std::endl;
  std::cout << binary<101>::value << std::endl;    // instantiation
  

  std::cout << std::rank<int>::value << std::endl;           // 0
  std::cout << std::rank<int[5]>::value << std::endl;        // 1
  std::cout << std::rank<int[5][4]>::value << std::endl;     // 2
  std::cout << std::rank<int[][5][6]>::value << std::endl;   // 3


  const int a = 1;
  int *ptr = (int *)&a;
  *ptr = 2;
  std::cout << "a: " << a << std::endl;
  
  Test *nptr = static_cast<Test *>(nullptr);
  nptr->print();
  return 0;
}
