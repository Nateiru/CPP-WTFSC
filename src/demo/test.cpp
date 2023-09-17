#include <iostream>
#include <vector>
#include <memory>
#include <cstring>

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



int main() {

  using list = List<1, 1, 4, 5, 1, 4>;
  std::cout << list::value << std::endl;
  std::cout << list::next::value << std::endl;
  std::cout << list::next::next::value << std::endl;
  return 0;
}