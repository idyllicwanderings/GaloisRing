
int main() {


    randomness::RO ro;
    ro.gen_random_bytes();

    // Usage of Galois Ring of one extension
    GR1e<64, 8> a = GR1e<64, 8>::random_element(ro);
    GR1e<64, 8> b = GR1e<64, 8>::random_element(ro);
    GR1e<64, 8> c;

    auto start = std::chrono::high_resolution_clock::now();
    c = a * b;                                          // Multiplication
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Multiplication: " << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() << "ns" << std::endl;

    start = std::chrono::high_resolution_clock::now();
    


    return 1;
}
