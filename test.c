#include <emmintrin.h>
#include <smmintrin.h>
#include <wmmintrin.h>

void test signed(){
    __m128i a= _mm_set1_epi32(-1);
    __m128i b= _mm_set1_epi32(0);
    __m128i c= _mm_cmpgt_epi32(b,c);
    std::cout<<mm extract epi32(c,0)<< std::endl;
}

int main() {
    test signed();
    return 0;
}