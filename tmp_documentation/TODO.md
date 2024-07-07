- [x] 写一下initilization list(BR, Z2K, GR)+ 测试

- [x] 写一个polynomial mult 接口

- [ ] 把googletest去掉，参考一下keccak，用makefile写test

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

    $(2^d - 1) * 2^{k - 1} - 1$

    64 bit

- [ ] 快速幂实现inverse for GR towering:

- [ ] GR1e GRT1e的test vector string

  - [ ] 固定GR test里sage script的 polynomial alphabet
  - [ ] 写GRT1e test(固定polynomial) 
  - [ ] 按照sage规则写一个写GR1e GRT1e的operator<<重载
  
- [ ] sage GR towering

  - [x] 写完sage GR towering生成
  
  - [x] 改成fixed modulus list（可选）
  
  - [x] 测试一层towering是否可以用robin那个结论
  
  - [x] 写生成.h的sequence存储
  
    



### 迷思

- [ ] exceptional sequence
  - [ ] 为什么0,1是一层towering的maximal sequence
  - [ ] 为什么在一层里系数为奇数就可逆
  - [ ] 多层towering的maximal sequence是啥
    - [ ] $GR(k,d)$ 最多有$2^d$个元素，那$(GR(k, d_1), d_2)$ 最多有$2^{d_1 \cdot d_2}$
      - [ ] 0000
  - [ ] 多层towering元素对应的逆为$(2^{\prod{d_i}} - 1) * 2^{k - 1} - 1$
- [ ] 学习一下hensel lift
- [ ] towering find polynomial:
  - [ ] 如果用primitive，[2,3]就会找不到一个primitive polynomial，然后失败。
  - [ ] 但是用irreducible就是正常的，为什么呢？GR made easy里定义说需要一个primitive polynomial。



