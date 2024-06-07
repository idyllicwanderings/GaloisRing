#include <type_traits>
#include <concepts>
#include <array>
#include <cstdint>

template<int k, int d>
struct BR {
    using BaseType = void; //WHY?

    std::array<uint8_t, k> polys_; // 无符号整数数组成员
    static constexpr int d = d;
    using F = uint8_t;

    // 定义 operator-=
    BR<k, d>& operator-=(const  BR<k, d>& o) {
        for (int i = 0; i < k; ++i) {
            polys_[i] -= o.polys_[i];
        }
        return *this;
    }

    // 定义 operator-
    BR<k, d> operator-(const  BR<k, d>& o) const {
        BR<k, d> result = *this;
        result -= o;
        return result;
    }
};


template<typename R, int k>
struct GR;


template<typename T>
struct is_br_template : std::false_type {};
template<int k, int d>
struct is_br_template<BR<k, d>> : std::true_type {};


template<typename T>
struct is_gr_template : std::false_type {};
template<typename R, int k>
struct is_gr_template<GR<R, k>> : std::true_type {};


template<typename T>
concept IsBaseBR = is_br_template<T>::value && !is_gr_template<T>::value;

template<typename T>
concept ValidR = requires {
    requires IsBaseBR<T> || (is_gr_template<T>::value && requires {
        typename T::BaseType;
        requires IsBaseBR<typename T::BaseType>;
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
struct GR<R,k> {
    // 将 R 的最终 BR 类型通过别名暴露出来
    using BaseType = std::conditional_t<is_br_template<R>::value, R, typename R::BaseType>;
    using F = uint8_t;

};

static_assert(ring<GR<int, 5>>, "GR<int, 5> is ring");

int main() {

    GR<BR<1, 2>, 3> gr1; 
    GR<GR<BR<1, 2>, 5>, 3> gr2; 

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
