### 小项目清单

- [x] 写一下initilization list(BR, Z2K, GR)+ 测试

- [x] 写一个polynomial mult 接口

- [ ] 用makefile写test

- [ ] 把error handling统一一下

- [ ] demo测试一下keccak sha3

- [x] brlifttables写的不太对啊。fix

  - [x] ```c++
    // layer 1
    GR1e<Z2k<k>, d> //GR(2^k, d)
    GR1e<Z2k<k>, d + 1> //GR(2^k, d+1)
    
    // layer 2 <2,2,2>
    GRT1e< GR1e<z2k<k>, d0>, d1 + 1>  //moduli
    GRT1e< GR1e<z2k<k>, d0>, d1>  //element 
    //最外层d1 + 1项moduli
    // GR1e<z2k<k>, d0>
    ```

  - [x] br tower table需要单独处理第一层

- [x] ```c++
  requires requires {
   1 <= k && k <= 3;  //wrong
  }
  
  requires ( 1 <= k && k <= 3)  //correct
  ```

  concept requires 

  ```c++
  Z2k<>(): val_(0) {} // constructor correct
  Z2k<k>(): val_(0) {}  //wrong
  ```

- [ ] 重写test structure for Z2K:

  - [ ] workflow works like this: 

    unit-test.c for displaying information

    z2k.h includes NUM, OPS

    genTestVector.cc calls on sage and generate vector like 

    ```
    multiply
    a = 1, b = 2, a * b = 12
    ```
    
    z2k.inc includes 
    
    z2k.cc includes real test 

- [x] 快速幂实现inverse for GR1e:

  - [x] method:

    ```pseudocode
    a^b in GR(2^k, d)
    b is an integer, b can be stored in an array
    for (int i = n-1 ;i >=0 ; i--) {
    	if (b[i] == 1) res *= a;
    	a *= a;
    }
    ```

  - [x] check if the inverse is truly an inverse:

    ```pseudocode
    return a*b == 1
    ```

  - [ ] (2^d - 1) * (2^(k - 1)) - 1是一个大数，该怎么计算和存储

    $(2^d - 1) * 2^{k - 1} - 1$:

    64 bit：要回到之前的问题上，128bit怎么搞。

- [x] 快速幂实现inverse for GR towering:

- [x] GR1e GRT1e的test vector string

  - [x] 固定GR test里sage script的 polynomial alphabet
  - [ ] 写GRT1e test(固定polynomial) 
  - [ ] 按照sage规则写一个写GR1e GRT1e的operator<<重载

- [x] sage GR towering

  - [x] 写完sage GR towering生成
  - [x] 改成fixed modulus list（可选）
  - [x] 测试一层/多层towering是否可以用robin那个结论
  - [x] 写C++里面的全排列生成（exceptional sequence），也就是sequence generation on the flys
  
- [ ] GRT1e test

  - [x] sage test script
  - [ ] C++ output scirpt
  
- [x] inv的fast exponentiation改一下。不要存储

  - [x] GRT1E增加一个k的成员函数。

- [ ] gring.h编译通过

  - [x] 增加一个函数，把BRlifttables加进去
  - [ ] 处理一下reduce
  
- [ ] RO

  - [x] RO写一下。参见question 2
  - [ ] 想法子把这个库build进来。咋搞
  - [ ] test一下我的demo

- [ ] 




### 迷思

- [ ] exceptional sequence
  - [ ] 为什么0,1是一层towering的maximal sequence
  - [ ] 为什么在一层里系数为奇数就可逆
  - [x] 多层towering的maximal sequence是啥
    - [x] $GR(k,d)$ 最多有$2^d$个元素，那$(GR(k, d_1), d_2)$ 最多有$2^{d_1 \cdot d_2}$
      - [ ] 0000
  - [ ] 多层towering元素对应的逆为$(2^{\prod{d_i}} - 1) * 2^{k - 1} - 1$
- [ ] 学习一下hensel lift
  - [x] 学了，但是不太多。。。没懂

- [ ] towering find polynomial:
  - [ ] 如果用primitive，[2,3]就会找不到一个primitive polynomial，然后失败。
  - [ ] 但是用irreducible就是正常的，为什么呢？GR made easy里定义说需要一个primitive polynomial。



## summer schedule

- [ ] 7.20 先
  - [ ] GR代码跑起来 
  - [ ] test重建完 
  - [ ] （看是否可以写一个sac check + inner product check）
- [ ] 7.20-7.30 写slides + documentation → 7.30 slides初稿

工作时间：

- [ ] 7.7 - 7.13 工作 大周
- [ ] 7.15 - 7.19 工作 小周
- [ ] 7.22 - 7.28 工作 大周

其他事情：

- [ ] 7.12 follow up 一下sofie + sony 争取七月能够签合同。
- [ ] 如果七月不实习，那就主要做毕设。
- [ ] 八月实习一周去两天就行。考完全勤。
- [ ] 每天1h analog circuits（或者是一周6h analog circuits，有五个章节，主要是第二三章比较麻烦。）
- [ ] 7.11 把以前的纸质资料都整理出来。



### questions

Hi happy weekend/holidays!!!

I had a C++ metaprogramming question: how to pass a constant tuple into a variadic non-type (specifically int) template parameter pack?



Here is the function I tried to implement:

```c++
template <int k, int n, int d, typename R, int... ds>
std::array<R, d> reduce(const std::array<R, n>& x) {
    #include "grmodtables.h"
    constexpr auto red = (sizeof...(ds) == 0)? reduction_polynomial<d>() : grmodtables::moduli<k, ds..., d>;
    //...
    return res;
}
```

I wrote a demo to show the simplified architecture of the ```GR1e``` (GR with one extension) and ```GRT1e``` (GR with towering) classes. I also tried to use `std::apply`, but since there are no input arguments, this approach doesn’t match the non-type template parameters.

```c++

template<int k, int d>
class GR1E {
   public:
    static constexpr int d_ = d;
    static constexpr std::tuple<> ds_ = {};

};

template <int d1, typename R, int... ds>
int reduce() {
    return 1;      
}

template<typename R, int d> 
class GR {
   public:
    static constexpr auto ds_ = std::tuple_cat(R::ds_, std::make_tuple(std::integral_constant<int, d>{}));

    void op() const {
        std::apply([](auto &&... args) { reduce<d, R, args...>(); }, ds_); //failed
    }
};

template <typename... T> void my_func(const T&... tupleArgs) {
    std::cout << "my_func called with values: ";
    (std::cout << ... << tupleArgs);
    std::cout << std::endl;
}

template <int... T> void my_func1(int x1, int x2) {
    std::cout << "my_func1 called with values: ";
    std::cout << std::endl;
}


int main() {
    std::tuple<int, float> my_tuple = {1, 2.0f};
    std::apply([](auto &&... args) { my_func(args...); }, my_tuple);

    // static constexpr std::tuple<int, int> my_tuple1 = {1, 1};
    // std::apply([](auto &&... args) {  my_func1(args...); }, my_tuple1);

    GR<GR1E<1, 2>, 3> gr1; 
    GR<GR<GR1E<1, 2>, 5>, 3> gr2; 

    gr2.op();
    return 0;
}

```

There's also an alternative like below, but that'd require modifications of other files & functions and is less flexible :/

```c++
template <int d1, int len, std::array<int, len> T>
int reduce() {
    //...
    return 1;      
}
```



```c++

```



##### question 2

为啥可颂说SHAKE当做一个PRG？shake不是xof吗。。。

