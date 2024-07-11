#include <type_traits>
#include <concepts>
#include <array>
#include <cstdint>
#include <stdexcept>

template<int k, int d>
class GR1E {
   public:
    using BaseType = void; //WHY?

    std::array<uint8_t, k> polys_; // 无符号整数数组成员
    static constexpr int d_ = d;

    // 定义 operator-=
    GR1E<k, d>& operator-=(const  GR1E<k, d>& o) {
        for (int i = 0; i < k; ++i) {
            polys_[i] -= o.polys_[i];
        }
        return *this;
    }

    // 定义 operator-
    GR1E<k, d> operator-(const  GR1E<k, d>& o) const {
        GR1E<k, d> result = *this;
        result -= o;
        return result;
    }
};


template<typename R, int k>
class GR;


template<typename T>
struct is_br_template : std::false_type {};
template<int k, int d>
struct is_br_template<GR1E<k, d>> : std::true_type {};


template<typename T>
struct is_gr_template : std::false_type {};
template<typename R, int k>
struct is_gr_template<GR<R, k>> : std::true_type {};


template<typename T>
concept IsBaseGR1E = is_br_template<T>::value && !is_gr_template<T>::value;

template<typename T>
concept ValidR = requires {
    requires IsBaseGR1E<T> || (is_gr_template<T>::value && requires {
        typename T::BaseType;
        requires IsBaseGR1E<typename T::BaseType>;
    });
};

// more generic requires to include Z2K
// template <typename R, auto X>
// concept is_base_ring = requires(R r)
// {
//     std::array<X> val;
//     []<R ring>(const std::optional<R>&) { } (r.operator+());
// };



template<ValidR R, int k> 
class GR<R,k> {
   public:
    // 将 R 的最终 GR1E 类型通过别名暴露出来
    using BaseType = std::conditional_t<is_br_template<R>::value, R, typename R::BaseType>;
    //using F = uint8_t;

};


int main() {

    GR<GR1E<1, 2>, 3> gr1; 
    GR<GR<GR1E<1, 2>, 5>, 3> gr2; 

    //GR<GR<1, 2>, 3> gr_wrong;  

    return 0;
}





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
// class Z2k 
// {
//     public:
//         using F = typename datatype<type_idx<k>()>::type;
//         static_assert(2 <= k && k <= 32, "Unsupported Base Ring F");
//         static inline F MASK = make_mask<F, k>();

//         template <typename T>
//         explicit Z2k<k>(const T& ele): val_(F(ele) & MASK) {}

//         explicit Z2k<k>(F f, bool /*skip mask*/) : val_(std::move(f & MASK)) {}

//         Z2k() : val_(0) {}
//     private:
//         F val_;
// };



// template<int k, int d>
// requires ( 1 <= k && k <= 64 && 1 <= d && d <= 32)
// class GR1e 
// {
//     public: 
//         template <typename T>
//         explicit GR1e<k, d>(const std::array<T, d>& eles): polys_(eles) {}

//         explicit GR1e<k, d>(const std::array<Z2k<k>, d>& eles): polys_(eles) {}

//         GR1e() {
//             polys_.fill(Z2k<k>());
//         }

//         template <typename T>
//         explicit GR1e<k, d>(std::initializer_list<T> eles) {
//             if (eles.size() != d) {
//                 throw std::out_of_range("Wrong size of d provided");
//             }
//             int i = 0;
//             for (const auto& ele : eles) {
//                 polys_[i] = Z2k<k>(ele);
//                 ++i;
//             }
//         }

//     private:
//         std::array<Z2k<k>, d> polys_;
//         static constexpr int d0_ = d;

// };


// /**
//  * Towering of Galois Ring
//  */
// template<typename R, int d> 
// class GRT1e {
//     public:
//         explicit GRT1e<R, d>(const std::array<R, d>& poly): polys_(poly) {;}

//         GRT1e()  { polys_.fill(R());}

//         template <typename T>
//         explicit GRT1e<R, d>(std::initializer_list<T> eles) {
//             if (eles.size() != d) {
//                 throw std::out_of_range("Unmatched extenssion degree provided");
//             }
//             std::copy(eles.begin(), eles.end(), polys_.begin());
//         }

//     private:
//         std::array<R, d> polys_;   // from x^0 to x^(d-1), modulus UP TO x^d
//         static constexpr int d0_ = d;
// };

// template <int k, int d0, int d1> extern const GRT1e<GR1e<k, d1>, d0> moduli;
// template <int k> inline const GRT1e<GR1e<k, 2>, 3> moduli<k, 2, 3> = GRT1e<GR1e<k, 2>, 3>({GR1e<k, 2>({0u, 1u}), GR1e<k, 2>({0u, 1u}), GR1e<k, 2>({1u, 0u})});
    

// int main() {
//     Z2k<2> gr1(1);
//     GR1e<2, 3> gr2({1u, 1u, 1u});
//     //moduli<2> modu;

//     //GR<GR<1, 2>, 3> gr_wrong;  

//     return 0;
// }
