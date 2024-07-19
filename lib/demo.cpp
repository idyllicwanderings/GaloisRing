#include <type_traits>
#include <concepts>
#include <array>
#include <cstdint>
#include <stdexcept>
#include <tuple>
#include <iostream>

// template<int k, int d>
// class Ring {
//    public:
//     using BaseType = void; //WHY?

//     std::array<uint8_t, k> polys_; // 无符号整数数组成员
//     static constexpr int d_ = d;

//     // 定义 operator-=
//     Ring<k, d>& operator-=(const  Ring<k, d>& o) {
//         for (int i = 0; i < k; ++i) {
//             polys_[i] -= o.polys_[i];
//         }
//         return *this;
//     }

//     // 定义 operator-
//     Ring<k, d> operator-(const  Ring<k, d>& o) const {
//         Ring<k, d> result = *this;
//         result -= o;
//         return result;
//     }
// };


// template<typename R, int k>
// class GR;


// template<typename T>
// struct is_br_template : std::false_type {};
// template<int k, int d>
// struct is_br_template<Ring<k, d>> : std::true_type {};


// template<typename T>
// struct is_gr_template : std::false_type {};
// template<typename R, int k>
// struct is_gr_template<GR<R, k>> : std::true_type {};


// template<typename T>
// concept IsBaseRing = is_br_template<T>::value && !is_gr_template<T>::value;

// template<typename T>
// concept ValidR = requires {
//     requires IsBaseRing<T> || (is_gr_template<T>::value && requires {
//         typename T::BaseType;
//         requires IsBaseRing<typename T::BaseType>;
//     });
// };

// // more generic requires to include Z2K
// // template <typename R, auto X>
// // concept is_base_ring = requires(R r)
// // {
// //     std::array<X> val;
// //     []<R ring>(const std::optional<R>&) { } (r.operator+());
// // };



// template<ValidR R, int k> 
// class GR<R,k> {
//    public:
//     // 将 R 的最终 Ring 类型通过别名暴露出来
//     using BaseType = std::conditional_t<is_br_template<R>::value, R, typename R::BaseType>;
//     //using F = uint8_t;

// };


// int main() {

//     GR<Ring<1, 2>, 3> gr1; 
//     GR<GR<Ring<1, 2>, 5>, 3> gr2; 

//     //GR<GR<1, 2>, 3> gr_wrong;  

//     return 0;
// }





//=======================================exmaple 2===================

// template <typename T>
// concept ring = requires(T t, const T& o) {
//     // check *, + 
//     { t.operator-=(o) } -> std::same_as<T&>;
//     { t.operator-(o) } -> std::same_as<T>;
//     { t.operator+=(o) } -> std::same_as<T&>;
//     { t.operator+(o) } -> std::same_as<T>;
//     { t.operator*=(o) } -> std::same_as<T&>;
//     { t.operator*(o) } -> std::same_as<T>;
//     { t.operator==(o) } -> std::same_as<std::bool>;
//     { t.operator!=(o) } -> std::same_as<std::bool>;

//     // // check if there's a ring polynomial stored in an array
//     std::convertible_to<decltype(T::polys_ ), std::array<typename T::F, T::d>>;
//     std::unsigned_integral<typename decltype(T::polys_)::value_type>;
// };

// template <typename R, int k>
// struct GR {
//     std::array<uint8_t, k> polys_; 
//     static constexpr int d = k;
//     using F = uint8_t;

//     GR<R, k>& operator-=(const GR<R, k>& o) {
//         for (int i = 0; i < k; ++i) {
//             polys_[i] -= o.polys_[i];
//         }
//         return *this;
//     }

//     GR<R, k> operator-(const GR<R, k>& o) const {
//         GR<R, k> result = *this;
//         result -= o;
//         return result;
//     }
// };

// static_assert(ring<GR<int, 5>>, "GR<int, 5> is ring");

// int main() {
//     GR<int, 5> a, b;
//     a -= b; 
//     return 0;
// }



//=======================================exmaple 3===================

// // test initilization list
// template <int k>
// constexpr int type_idx() {
//         static_assert(k > 0, "Sane values?");
//         if (k <= 8) return 0;
//         else if (k <= 16) return 1;
//         else if (k <= 32) return 2;
//         else return 3;
// };

// template <int idx> struct datatype;
// template <> struct datatype<0> {using type = uint8_t;};
// template <> struct datatype<1> {using type = uint16_t;};
// template <> struct datatype<2> {using type = uint32_t;};
// template <> struct datatype<3> {using type = uint64_t;};


// template <typename T, int k>
// constexpr T make_mask() {
//     return (T(1) << (k % (8 * sizeof(T)))) - 1;
// };

// template<int k>
// requires( 1 <= k && k <= 64 )
// class Z 
// {
//     public:
//         using F = typename datatype<type_idx<k>()>::type;
//         static_assert(2 <= k && k <= 32, "Unsupported Base Ring F");
//         static inline F MASK = make_mask<F, k>();

//         template <typename T>
//         explicit Z<k>(const T& ele): val_(F(ele) & MASK) {}

//         explicit Z<k>(F f, bool /*skip mask*/) : val_(std::move(f & MASK)) {}

//         Z() : val_(0) {}
//     private:
//         F val_;
// };



// template<int k, int d>
// requires ( 1 <= k && k <= 64 && 1 <= d && d <= 32)
// class Ring 
// {
//     public: 
//         template <typename T>
//         explicit Ring<k, d>(const std::array<T, d>& eles): polys_(eles) {}

//         explicit Ring<k, d>(const std::array<Z<k>, d>& eles): polys_(eles) {}

//         Ring() {
//             polys_.fill(Z<k>());
//         }

//         template <typename T>
//         explicit Ring<k, d>(std::initializer_list<T> eles) {
//             if (eles.size() != d) {
//                 throw std::out_of_range("Wrong size of d provided");
//             }
//             int i = 0;
//             for (const auto& ele : eles) {
//                 polys_[i] = Z<k>(ele);
//                 ++i;
//             }
//         }

//     private:
//         std::array<Z<k>, d> polys_;
//         static constexpr int d0_ = d;

// };


// /**
//  * Towering of Galois Ring
//  */
// template<typename R, int d> 
// class RingExt {
//     public:
//         explicit RingExt<R, d>(const std::array<R, d>& poly): polys_(poly) {;}

//         RingExt()  { polys_.fill(R());}

//         template <typename T>
//         explicit RingExt<R, d>(std::initializer_list<T> eles) {
//             if (eles.size() != d) {
//                 throw std::out_of_range("Unmatched extenssion degree provided");
//             }
//             std::copy(eles.begin(), eles.end(), polys_.begin());
//         }

//     private:
//         std::array<R, d> polys_;   // from x^0 to x^(d-1), modulus UP TO x^d
//         static constexpr int d0_ = d;
// };

// template <int k, int d0, int d1>  const RingExt<Ring<k, d1>, d0> moduli;
// template <int k> inline const RingExt<Ring<k, 2>, 3> moduli<k, 2, 3> = RingExt<Ring<k, 2>, 3>({Ring<k, 2>({0u, 1u}), Ring<k, 2>({0u, 1u}), Ring<k, 2>({1u, 0u})});
    

// int main() {
//     Z<2> gr1(1);
//     Ring<2, 3> gr2({1u, 1u, 1u});
//     moduli<2, 2, 3>;

//     //GR<GR<1, 2>, 3> gr_wrong;  

//     return 0;
// }


// ++++++++++++++++++++++++++++++++++++++++++ example 4++++++++++++++++++++++++++++++++++++++++++



template <int k>
class Z {
    public:
        explicit Z<k>(uint64_t& ele): val_(ele) {}

        uint64_t val_;
};

template<int k, int d>
class Ring {
   public:
    template <typename T>
    explicit Ring<k, d>(std::initializer_list<T> eles) {
        int i = 0;
        for (const auto& ele : eles) {  
            polys_[i] = Z<k>(ele);
            i++;
        }
    }

    std::array<Z<k>, d> polys_; 
    static constexpr int d_ = d;
    static constexpr std::tuple<> ds_ = {};

};


template <int k, int d1, typename R, int... ds>
int reduce();

template<typename R, int d> 
class RingExt {
   public:
    template <typename T>
    explicit RingExt<R, d>(std::initializer_list<T> eles) {
        std::copy(eles.begin(), eles.end(), polys_.begin());
    }

    void op() const {
        //std::apply([](auto &&... args) { reduce<d, R, args...>(); }, ds_);
        reduce<3, d, R, ds_>();
    }

    static constexpr auto ds_ = std::tuple_cat(R::ds_, std::make_tuple(std::integral_constant<int, d>{}));
    std::array<R, d> polys_; 
};



template <int k, int d0>  const std::array<Z<k>, d0 + 1> polynomial;
template <int k, int d0, int d1>  const std::array<Ring<k, d0>, d1 + 1> polynomial;
template <int k, int d0, int d1, int d2>  const std::array <RingExt <RingExt<k, d0>, d1>, d2 + 1> polynomial;
// more d0, d1, d2 here....

template <int k> inline const std::array<Z<k>, 3> polynomial<k, 2> = {Z<k>(1u), Z<k>(1u), Z<k>(1u)};
template <int k> inline const std::array<Ring<k, 2>, 3> polynomial<k, 2, 2> = {Ring<k, 2>({0u, 1u}), Ring<k, 2>({0u, 1u}), Ring<k, 2>({1u, 0u})};
// more d0, d1, d2 here....

template <int k, int d1, typename R, int... ds>
int reduce()
{
    constexpr auto red = polynomial<k, d1, ds...>;
    // calculations on R, does not matter
    return 1;      
}

int main() {

    RingExt<Ring<1, 2>, 3> r1; 
    RingExt<RingExt<Ring<1, 2>, 5>, 3> r2; 

    r2.op();

    return 0;
}
