// C++模板元编程（三）：从简单案例中学习: https://zhuanlan.zhihu.com/p/384826036

#include <type_traits>

using std::integral_constant;

/// ----------------------------------------
// rank: array dimensions
template <typename T>
struct rank : integral_constant<std::size_t, 0> {};                          // #1

template <typename T>
struct rank<T[]> : integral_constant<std::size_t, rank<T>::value + 1> {};    // #2

template <typename T, std::size_t N>
struct rank<T[N]> : integral_constant<std::size_t, rank<T>::value + 1> { };  // #3

/// ----------------------------------------
/// extent: the length of array N dimensions
template<typename T, unsigned N = 0>
struct extent : integral_constant<std::size_t, 0> {};

template<typename T>
struct extent<T[], 0> : integral_constant<std::size_t, 0> {};

template<typename T, unsigned N>
struct extent<T[], N> : extent<T, N-1> {};

template<typename T, std::size_t I>
struct extent<T[I], 0> : integral_constant<std::size_t, I> {};

template<typename T, std::size_t I, unsigned N>
struct extent<T[I], N> : extent<T, N-1> {};

int main() {

}