#include <emmintrin.h>
#include <smmintrin.h>
#include <wmmintrin.h>

#include<gtest/gtest.h>


void test_signed(){
    __m128i a= _mm_set1_epi32(-1);
    __m128i b= _mm_set1_epi32(0);
    __m128i c= _mm_cmpgt_epi32(b,c);
    std::cout<<mm extract epi32(c,0)<< std::endl;
};

TEST(testCase, test0){
    EXPECT_EQ(test_signed(), ); 
}




int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv); 
  return RUN_ALL_TESTS(); 
}
