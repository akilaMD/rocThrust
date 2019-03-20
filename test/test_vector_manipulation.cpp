// MIT License
//
// Copyright (c) 2019 Advanced Micro Devices, Inc. All rights reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

// Thrust
#include <thrust/device_malloc_allocator.h>
#include <thrust/device_vector.h>
#include <thrust/host_vector.h>

#include "test_header.hpp"

#if THRUST_DEVICE_COMPILER == THRUST_DEVICE_COMPILER_HCC

TESTS_DEFINE(VectorManipulationTests, FullTestsParams);

TYPED_TEST(VectorManipulationTests, TestVectorManipulation)
{
  using Vector = typename TestFixture::input_type;
  using T = typename Vector::value_type;
  using Iterator = typename Vector::iterator;

  const std::vector<size_t> sizes = get_sizes();
  for(auto size : sizes)
  {
    thrust::host_vector<T> src = get_random_data<T>(size,
                                                    std::numeric_limits<T>::min(),
                                                    std::numeric_limits<T>::max());

    ASSERT_EQ(src.size(), size);

    // basic initialization
    Vector test0(size);
    Vector test1(size, T(3));
    ASSERT_EQ(test0.size(), size);
    ASSERT_EQ(test1.size(), size);
    ASSERT_EQ((test1 == std::vector<T>(size, T(3))), true);

    // initializing from other vector
    std::vector<T> stl_vector(src.begin(), src.end());
    Vector cpy0 = src;
    Vector cpy1(stl_vector);
    Vector cpy2(stl_vector.begin(), stl_vector.end());

    ASSERT_EQ(cpy0, src);
    ASSERT_EQ(cpy1, src);
    ASSERT_EQ(cpy2, src);

    // resizing
    Vector vec1(src);
    vec1.resize(size + 3);
    ASSERT_EQ(vec1.size(), size + 3);
    vec1.resize(size);
    ASSERT_EQ(vec1.size(), size);
    ASSERT_EQ(vec1, src);

    vec1.resize(size + 20, T(11));
    Vector tail(vec1.begin() + size, vec1.end());
    ASSERT_EQ( (tail == std::vector<T>(20, T(11))), true);

    // shrinking a vector should not invalidate iterators
    Iterator first = vec1.begin();
    vec1.resize(10);
    ASSERT_EQ(first, vec1.begin());

    vec1.resize(0);
    ASSERT_EQ(vec1.size(), 0);
    ASSERT_EQ(vec1.empty(), true);
    vec1.resize(10);
    ASSERT_EQ(vec1.size(), 10);
    vec1.clear();
    ASSERT_EQ(vec1.size(), 0);
    vec1.resize(5);
    ASSERT_EQ(vec1.size(), 5);

    // push_back
    Vector vec2;
    for(size_t i = 0; i < 10; ++i)
    {
        ASSERT_EQ(vec2.size(), i);
        vec2.push_back( (T) i );
        ASSERT_EQ(vec2.size(), i + 1);
        for(size_t j = 0; j <= i; j++)
            ASSERT_EQ(vec2[j],     j);
        ASSERT_EQ(vec2.back(), i);
    }

    // pop_back
    for(size_t i = 10; i > 0; --i)
    {
        ASSERT_EQ(vec2.size(), i);
        ASSERT_EQ(vec2.back(), i-1);
        vec2.pop_back();
        ASSERT_EQ(vec2.size(), i-1);
        for(size_t j = 0; j < i; j++)
            ASSERT_EQ(vec2[j], j);
    }
  }
}

#endif // THRUST_DEVICE_COMPILER == THRUST_DEVICE_COMPILER_HCC
