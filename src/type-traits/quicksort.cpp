// https://www.aneureka.cn/type-traits-practice-quick-sort-2023-07-31/
#include <type_traits>

// --------------------------------------
// Nums
template <int... Ns>
struct Nums {};

template <typename T1, typename T2>
struct Concat {};

template <int... Ns1, int... Ns2>
struct Concat<Nums<Ns1...>, Nums<Ns2...>> {
  using type = Nums<Ns1..., Ns2...>;
};
// --------------------------------------


// --------------------------------------
// Divide
template <int Pivot, typename T>
struct Divide {};

template <int Pivot, int... Ns>
struct Divide<Pivot, Nums<Ns...>> {};

template <int Pivot>
struct Divide<Pivot, Nums<>> {
  using left = Nums<>;
  using mid = Nums<>;
  using right = Nums<>;
};

template <int Pivot, int N0, int... Ns>
struct Divide<Pivot, Nums<N0, Ns...>> {
  using left =
      std::conditional_t <
      N0<Pivot,
         typename Concat<Nums<N0>,
                         typename Divide<Pivot, Nums<Ns...>>::left>::type,
         typename Divide<Pivot, Nums<Ns...>>::left>;
  using mid = std::conditional_t<
      N0 == Pivot,
      typename Concat<Nums<N0>, typename Divide<Pivot, Nums<Ns...>>::mid>::type,
      typename Divide<Pivot, Nums<Ns...>>::mid>;
  using right =
      std::conditional_t <
      Pivot<N0,
            typename Concat<Nums<N0>,
                            typename Divide<Pivot, Nums<Ns...>>::right>::type,
            typename Divide<Pivot, Nums<Ns...>>::right>;
};
// --------------------------------------


// --------------------------------------
// DivideInplace
template <typename T>
struct DivideInplace {};

template <>
struct DivideInplace<Nums<>> : Divide<0, Nums<>> {};

template <int N0, int... Ns>
struct DivideInplace<Nums<N0, Ns...>> : Divide<N0, Nums<N0, Ns...>> {};
// --------------------------------------


// --------------------------------------
// QuickSort
template <typename T>
struct QuickSort {};

template <>
struct QuickSort<Nums<>> {
  using type = Nums<>;
};

template <int N>
struct QuickSort<Nums<N>> {
  using type = Nums<N>;
};

template <int... Ns>
struct QuickSort<Nums<Ns...>> {
  using divide = DivideInplace<Nums<Ns...>>;
  using left = typename QuickSort<typename divide::left>::type;
  using mid = typename divide::mid;
  using right = typename QuickSort<typename divide::right>::type;
  using type = typename Concat<typename Concat<left, mid>::type, right>::type;
};
// --------------------------------------


int main() {

  return 0;
}