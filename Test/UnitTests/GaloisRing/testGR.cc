#include "testGR.h"


// const int N = 1000;

// template<typename F>
// void benchmark(const std::string& name, const std::string& file, F f) {
//     std::vector<std::string> in_a, in_b, in_c;
//     UT_testVectorParse(file, in_a, in_b, in_c);
//     if (in_a.empty()) return;

//     auto time = measure_us([&]() { f(in_a, in_b, in_c); }, N);
//     std::cout << name << ": " << time << " us\n";
// }

// template<typename F>
// long long measure_us(F f, int repeat = 1) {
//     auto start = std::chrono::high_resolution_clock::now();
//     for (int i = 0; i < repeat; ++i) f();
//     auto end = std::chrono::high_resolution_clock::now();
//     return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / repeat;
// }

// void testGR() {
//     benchmark("Addition", ADD_TV_FILE, [](auto& a, auto& b, auto&) {
//         volatile auto r = GR1e<k, d1>::from_list(a[0]) + GR1e<k, d1>::from_list(b[0]);
//     });

//     benchmark("Subtraction", SUB_TV_FILE, [](auto& a, auto& b, auto&) {
//         volatile auto r = GR1e<k, d1>::from_list(a[0]) - GR1e<k, d1>::from_list(b[0]);
//     });

//     benchmark("Multiplication", MUL_TV_FILE, [](auto& a, auto& b, auto&) {
//         volatile auto r = GR1e<k, d1>::from_list(a[0]) * GR1e<k, d1>::from_list(b[0]);
//     });

//     benchmark("Inverse", INV_TV_FILE, [](auto& a, auto&, auto&) {
//         volatile auto r = GR1e<k, d1>::from_list(a[0]).inv();
//     });

//     benchmark("Lift", LIFT_TV_FILE, [](auto& a, auto&, auto&) {
//         volatile auto r = liftGR<k, d1, d2>(GR1e<k, d1>::from_list(a[0]));
//     });
// }

// int main() {
//     testGR();
//     return 0;
// }



void testGR()
{
    
#ifdef UT_OUTPUT
void writeTestGR(void)
{
    FILE *f;
    unsigned int rho;
    //TODO

    f = fopen("TestGR.txt", "w");
    assert(f != NULL);
    // for(rho = 1; rho < 200/8; ++rho)
    //     writeTestSpongePRG(f, rho);
    fclose(f);
}
#endif

#ifdef GR_HAS_K_64BITS_HIGHER
    UT_startTest("GR addition ", "k > 64 bits");
    //TODO
    UT_endTest();
#else 

    std::vector<std::string> in_a;
    std::vector<std::string> in_b;
    std::vector<std::string> in_c;

    // UT_startTest("GR addition", "k < 64 bits");
    // UT_testVectorParse(ADD_TV_FILE, in_a, in_b, in_c);
    // std::cout << in_a.size() << std::endl;
    // for (int i = 0; i < in_a.size(); i++) {
    //     selfTestAddition(in_a[i], in_b[i], in_c[i]);
    // }
    // UT_displayInfo("GR  test: addition", " testcases passed");
    // in_a.clear();
    // in_b.clear();
    // in_c.clear();

    // ////////////////////////////////////////////////////////////////
    // UT_startTest("GR subtraction", "k < 64 bits");
    // UT_testVectorParse(SUB_TV_FILE, in_a, in_b, in_c);
    // for (int i = 0; i < in_a.size(); i++) {
    //     selfTestSubtraction(in_a[i], in_b[i], in_c[i]);
    // }
    // UT_displayInfo("GR  test: subtractions", in_a.size() + " testcases passed");
    // in_a.clear();
    // in_b.clear();
    // in_c.clear();

    // //////////////////////////////////////////////////////////////////
    // UT_startTest("GR multiplication", "k < 64 bits");
    // UT_testVectorParse(MUL_TV_FILE, in_a, in_b, in_c);
    // for (int i = 0; i < in_a.size(); i++) {
    //     selfTestMultiplication(in_a[i], in_b[i], in_c[i]);
    // }
    // UT_displayInfo("GR  test: mult", in_a.size() + " testcases passed");
    // in_a.clear();
    // in_b.clear();
    // in_c.clear();

    /////////////////////////////////////////////////////////////////////
    
    UT_testVectorParse(INV_TV_FILE, in_a, in_b, in_c);
    for (int i = 0; i < in_a.size(); i++) {
        selfTestInverse(in_a[i], in_b[i]);
    }
    in_a.clear();
    in_b.clear();
    in_c.clear();
    UT_displayInfo("GR  test: inverse", in_a.size() + " testcases passed");

    // /////////////////////////////////////////////////////////////////////
    // UT_startTest("GR lift", "k < 64 bits");
    // UT_testVectorParse(LIFT_TV_FILE, in_a, in_b, in_c);
    // for (int i = 0; i < in_a.size(); i++) {
    //     selfTestLift(in_a[i], in_b[i]);
    // }

    // UT_displayInfo("GR  test: lift", in_a.size() + " testcases passed");
    // in_a.clear();
    // in_b.clear();
    // in_c.clear();

    // /////////////////////////////////////////////////////////////////////


    UT_endTest();
#endif
}

void selfTestAddition(std::string in_a, std::string in_b, std::string expected_c) {
    GR1e<k, d1> a = GR1e<k, d1>::from_list(in_a);
    GR1e<k, d1> b = GR1e<k, d1>::from_list(in_b);
    GR1e<k, d1> c = GR1e<k, d1>::from_list(expected_c);
    auto res = a + b;
    UT_displayInfo("c ", c.force_str().c_str());
    UT_displayInfo("a + b ", res.force_str().c_str());
    assert(c.force_str() == res.force_str());
    UT_displayInfo("------------------------------------- PASSED ", "-------------------------------------");
}

void selfTestSubtraction(std::string in_a, std::string in_b, std::string expected_c) {
    GR1e<k, d1> a = GR1e<k, d1>::from_list(in_a);
    GR1e<k, d1> b = GR1e<k, d1>::from_list(in_b);
    GR1e<k, d1> c = GR1e<k, d1>::from_list(expected_c);
    auto res = a - b;
    UT_displayInfo("c ", c.force_str().c_str());
    UT_displayInfo("a - b ", res.force_str().c_str());
    assert (c.force_str() == res.force_str());
    UT_displayInfo("------------------------------------- PASSED ", "-------------------------------------");

}

void selfTestMultiplication(std::string in_a, std::string in_b, std::string expected_c) {
    GR1e<k, d1> a = GR1e<k, d1>::from_list(in_a);
    GR1e<k, d1> b = GR1e<k, d1>::from_list(in_b);
    GR1e<k, d1> c = GR1e<k, d1>::from_list(expected_c);
    auto res = a * b;

    UT_displayInfo("a ", a.force_str().c_str());
    UT_displayInfo("b ", b.force_str().c_str());
    UT_displayInfo("c ", c.force_str().c_str());
    UT_displayInfo("a * b ", res.force_str().c_str());

    assert(c.force_str() == (a * b).force_str());
    UT_displayInfo("------------------------------------- PASSED ", "-------------------------------------");
}

void selfTestInverse(std::string in_a, std::string in_b) {
    GR1e<k, d1> a = GR1e<k, d1>::from_list(in_a);
    GR1e<k, d1> b = GR1e<k, d1>::from_list(in_b);

    UT_displayInfo("a ", a.force_str().c_str());
    UT_displayInfo("expected a ", b.force_str().c_str());
    auto res = a.inv();
    UT_displayInfo("a.inv() ", res.force_str().c_str());

    assert(b.force_str() == res.force_str());
    UT_displayInfo("------------------------------------- PASSED ", "-------------------------------------");
}


void selfTestLift(std::string in_a, std::string in_b) {
    GR1e<k, d1> a = GR1e<k, d1>::from_list(in_a);
    GR1e<k, d2> b = GR1e<k, d2>::from_list(in_b);
    auto res = liftGR<k, d1, d2>(a);
    
    UT_displayInfo("a ", a.force_str().c_str());
    UT_displayInfo("b ", b.force_str().c_str());
    UT_displayInfo("lift(a) ", res.force_str().c_str());

    assert(b.force_str() == res.force_str());
    UT_displayInfo("------------------------------------- PASSED ", "-------------------------------------");
}



int main()
{
    testGR();
    return 0;
}



// namespace lift_dispatch {
//     constexpr int k_values[] = {32, 48, 64, 72, 96, 128};
//     constexpr int max_d = 16;
    
//     template<int k, int d>
//     void test_add(string_view in_a, string_view in_b, string_view expected_c) {
//         GR1e<k, d1> a = GR1e<k, d1>::from_list(in_a);
//         GR1e<k, d1> b = GR1e<k, d1>::from_list(in_b);
//         GR1e<k, d1> c = GR1e<k, d1>::from_list(expected_c);
//         auto res = a + b;
//         UT_displayInfo("add(a, b)", res.force_str().c_str());
//     }
    
//     template<int k, int d>
//     void test_mul(string_view in_a, string_view in_b, string_view expected_c) {
//         GR1e<k, d1> a = GR1e<k, d1>::from_list(in_a);
//         GR1e<k, d1> b = GR1e<k, d1>::from_list(in_b);
//         auto res = a * b;
//         UT_displayInfo("mul(a, b)", res.force_str().c_str());
//     }
    
//     template<int k, int d1, int d2>
//     void test_lift(string_view in_a, string_view in_b) {
//         auto a = GR1e<k, d1>::from_list(in_a);
//         auto b = GR1e<k, d2>::from_list(in_b);
//         auto res = liftGR<k, d1, d2>(a);
//         UT_displayInfo("a", a.force_str().c_str());
//         UT_displayInfo("b", b.force_str().c_str());
//         UT_displayInfo("lift(a)", res.force_str().c_str());
//         assert(b.force_str() == res.force_str());
//     }
    
//     template <int k, size_t... D1s>
//     void dispatchD1_lift(int d1, int d2, std::index_sequence<D1s...>) {
//         using F = void(*)(int);
//         constexpr F funcs[] = {[](int d2_val) { dispatchD2_lift<k, D1s+1>(d2_val); }...};
//         if (d1 < 1 || d1 > max_d) return;
//         funcs[d1 - 1](d2);
//     }
    
//     template <int k, int d1, size_t... D2s>
//     void dispatchD2_lift(int d2, std::index_sequence<D2s...>) {
//         using F = void(*)();
//         constexpr F funcs[] = {[]() { test_lift<k, d1, D2s+1>(); }...};
//         if (d2 < 1 || d2 > max_d) return;
//         funcs[d2 - 1]();
//     }
    
//     template <int k, int d1>
//     void dispatchD2_lift(int d2) {
//         dispatchD2_lift<k, d1>(d2, std::make_index_sequence<max_d>());
//     }
    
//     template <size_t... Ks>
//     void dispatchK_lift(int k, int d1, int d2, std::index_sequence<Ks...>) {
//         using F = void(*)(int, int);
//         constexpr int kv[] = {32, 64, 96, 128};
//         constexpr F funcs[] = {
//             [](int d1_val, int d2_val) { dispatchD1_lift<kv[Ks]>(d1_val, d2_val, std::make_index_sequence<max_d>()); }...
//         };
//         for (size_t i = 0; i < sizeof...(Ks); ++i) {
//             if (k == kv[i]) { funcs[i](d1, d2); return; }
//         }
//     }
    
//     void dispatchLiftTest(int k, int d1, int d2) {
//         dispatchK_lift(k, d1, d2, std::make_index_sequence<4>());
//     }
    
//     template <int k, size_t... Ds>
//     void dispatchD_basic(int d, std::index_sequence<Ds...>, const std::string& op) {
//         using F = void(*)();
//         if (op == "add") {
//             constexpr F funcs[] = { []() { test_add<k, Ds+1>(); }... };
//             funcs[d - 1]();
//         } else if (op == "mul") {
//             constexpr F funcs[] = { []() { test_mul<k, Ds+1>(); }... };
//             funcs[d - 1]();
//         }
//     }
    
//     void dispatchBasicOp(int k, int d, const std::string& op) {
//         for (int k_val : k_values) {
//             if (k == k_val) {
//                 dispatchD_basic<k_val>(d, std::make_index_sequence<max_d>(), op);
//                 return;
//             }
//         }
//     }
    
//     } // namespace lift_dispatch
    
// int main() {
//     std::srand(std::time(0));
//     using namespace lift_dispatch;
    
//     for (int k : k_values) {
//         for (int d = 1; d <= max_d; ++d) {
//             dispatchBasicOp(k, d, "add");
//             dispatchBasicOp(k, d, "mul");
//             for (int d2 = d; d2 <= max_d; ++d2) {
//                 dispatchLiftTest(k, d, d2);
//             }
//         }
//     }
//     return 0;
// }