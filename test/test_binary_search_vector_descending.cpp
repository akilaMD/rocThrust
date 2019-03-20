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
#include <thrust/binary_search.h>
#include <thrust/functional.h>
#include <thrust/sequence.h>
#include <thrust/sort.h>

#include "test_header.hpp"

#if THRUST_DEVICE_COMPILER == THRUST_DEVICE_COMPILER_HCC

TESTS_DEFINE(BinarySearchVectorDescendingTests, FullTestsParams);
TESTS_DEFINE(BinarySearchVectorDescendingIntegerTests, SignedIntegerTestsParams);

// convert xxx_vector<T1> to xxx_vector<T2>
template <class ExampleVector, typename NewType>
struct vector_like
{
    typedef typename ExampleVector::allocator_type alloc;
    typedef typename alloc::template rebind<NewType>::other new_alloc;
    typedef thrust::detail::vector_base<NewType, new_alloc> type;
};

TYPED_TEST(BinarySearchVectorDescendingTests, TestVectorLowerBoundDescendingSimple)
{
    using Vector = typename TestFixture::input_type;
    using T = typename Vector::value_type;
    Vector vec(5);

    vec[0] = 8;
    vec[1] = 7;
    vec[2] = 5;
    vec[3] = 2;
    vec[4] = 0;

    Vector input(10);
    thrust::sequence(input.begin(), input.end());

    typedef typename vector_like<Vector, int>::type IntVector;

    // test with integral output type
    IntVector integral_output(10);
    typename IntVector::iterator output_end = thrust::lower_bound(vec.begin(), vec.end(), input.begin(), input.end(), integral_output.begin(), thrust::greater<T>());

    ASSERT_EQ_QUIET(integral_output.end(), output_end);

    ASSERT_EQ(4, integral_output[0]);
    ASSERT_EQ(4, integral_output[1]);
    ASSERT_EQ(3, integral_output[2]);
    ASSERT_EQ(3, integral_output[3]);
    ASSERT_EQ(3, integral_output[4]);
    ASSERT_EQ(2, integral_output[5]);
    ASSERT_EQ(2, integral_output[6]);
    ASSERT_EQ(1, integral_output[7]);
    ASSERT_EQ(0, integral_output[8]);
    ASSERT_EQ(0, integral_output[9]);
}

TYPED_TEST(BinarySearchVectorDescendingTests, TestVectorUpperBoundDescendingSimple)
{
    using Vector = typename TestFixture::input_type;
    Vector vec(5);

    vec[0] = 8;
    vec[1] = 7;
    vec[2] = 5;
    vec[3] = 2;
    vec[4] = 0;

    Vector input(10);
    thrust::sequence(input.begin(), input.end());

    using IntVector = typename vector_like<Vector, int>::type;

    // test with integral output type
    IntVector integral_output(10);
    typename IntVector::iterator output_end = thrust::upper_bound(vec.begin(), vec.end(), input.begin(), input.end(), integral_output.begin(), thrust::greater<int>());

    ASSERT_EQ_QUIET(output_end, integral_output.end());

    ASSERT_EQ(5, integral_output[0]);
    ASSERT_EQ(4, integral_output[1]);
    ASSERT_EQ(4, integral_output[2]);
    ASSERT_EQ(3, integral_output[3]);
    ASSERT_EQ(3, integral_output[4]);
    ASSERT_EQ(3, integral_output[5]);
    ASSERT_EQ(2, integral_output[6]);
    ASSERT_EQ(2, integral_output[7]);
    ASSERT_EQ(1, integral_output[8]);
    ASSERT_EQ(0, integral_output[9]);
}

TYPED_TEST(BinarySearchVectorDescendingTests, TestVectorBinarySearchDescendingSimple)
{
    using Vector = typename TestFixture::input_type;
    Vector vec(5);

    vec[0] = 8;
    vec[1] = 7;
    vec[2] = 5;
    vec[3] = 2;
    vec[4] = 0;

    Vector input(10);
    thrust::sequence(input.begin(), input.end());

    using BoolVector = typename vector_like<Vector, bool>::type;
    using IntVector = typename vector_like<Vector,  int>::type;

    // test with boolean output type
    BoolVector bool_output(10);
    typename BoolVector::iterator bool_output_end = thrust::binary_search(vec.begin(), vec.end(), input.begin(), input.end(), bool_output.begin(), thrust::greater<int>());

    ASSERT_EQ_QUIET(bool_output_end, bool_output.end());

    ASSERT_EQ(true,  bool_output[0]);
    ASSERT_EQ(false, bool_output[1]);
    ASSERT_EQ(true,  bool_output[2]);
    ASSERT_EQ(false, bool_output[3]);
    ASSERT_EQ(false, bool_output[4]);
    ASSERT_EQ(true,  bool_output[5]);
    ASSERT_EQ(false, bool_output[6]);
    ASSERT_EQ(true,  bool_output[7]);
    ASSERT_EQ(true,  bool_output[8]);
    ASSERT_EQ(false, bool_output[9]);

    // test with integral output type
    IntVector integral_output(10, 2);
    typename IntVector::iterator int_output_end = thrust::binary_search(vec.begin(), vec.end(), input.begin(), input.end(), integral_output.begin(), thrust::greater<int>());

    ASSERT_EQ_QUIET(int_output_end, integral_output.end());

    ASSERT_EQ(1, integral_output[0]);
    ASSERT_EQ(0, integral_output[1]);
    ASSERT_EQ(1, integral_output[2]);
    ASSERT_EQ(0, integral_output[3]);
    ASSERT_EQ(0, integral_output[4]);
    ASSERT_EQ(1, integral_output[5]);
    ASSERT_EQ(0, integral_output[6]);
    ASSERT_EQ(1, integral_output[7]);
    ASSERT_EQ(1, integral_output[8]);
    ASSERT_EQ(0, integral_output[9]);
}

TYPED_TEST(BinarySearchVectorDescendingIntegerTests, TestVectorLowerBoundDescending)
{
    using T = typename TestFixture::input_type;
    for (auto size : get_sizes())
    {
        SCOPED_TRACE(testing::Message() << "with size = " << size);

        thrust::host_vector<T> h_vec = get_random_data<T>(size,
                                                          std::numeric_limits<T>::min(),
                                                          std::numeric_limits<T>::max());
        thrust::sort(h_vec.begin(), h_vec.end(), thrust::greater<T>());
        thrust::device_vector<T> d_vec = h_vec;

        thrust::host_vector<T> h_input = get_random_data<T>(2*size,
                                                            std::numeric_limits<T>::min(),
                                                            std::numeric_limits<T>::max());
        thrust::device_vector<T> d_input = h_input;

        thrust::host_vector<int>   h_output(2*size);
        thrust::device_vector<int> d_output(2*size);

        thrust::lower_bound(h_vec.begin(), h_vec.end(), h_input.begin(), h_input.end(), h_output.begin(), thrust::greater<T>());
        thrust::lower_bound(d_vec.begin(), d_vec.end(), d_input.begin(), d_input.end(), d_output.begin(), thrust::greater<T>());

        ASSERT_EQ(h_output, d_output);
    }
}

TYPED_TEST(BinarySearchVectorDescendingIntegerTests, TestVectorUpperBoundDescending)
{
    using T = typename TestFixture::input_type;
    for (auto size : get_sizes())
    {
        SCOPED_TRACE(testing::Message() << "with size = " << size);

        thrust::host_vector<T> h_vec = get_random_data<T>(size,
                                                          std::numeric_limits<T>::min(),
                                                          std::numeric_limits<T>::max());
        thrust::sort(h_vec.begin(), h_vec.end(), thrust::greater<T>());
        thrust::device_vector<T> d_vec = h_vec;

        thrust::host_vector<T> h_input = get_random_data<T>(2*size,
                                                            std::numeric_limits<T>::min(),
                                                            std::numeric_limits<T>::max());
        thrust::device_vector<T> d_input = h_input;

        thrust::host_vector<int>   h_output(2*size);
        thrust::device_vector<int> d_output(2*size);

        thrust::upper_bound(h_vec.begin(), h_vec.end(), h_input.begin(), h_input.end(), h_output.begin(), thrust::greater<T>());
        thrust::upper_bound(d_vec.begin(), d_vec.end(), d_input.begin(), d_input.end(), d_output.begin(), thrust::greater<T>());

        ASSERT_EQ(h_output, d_output);
    }
}

TYPED_TEST(BinarySearchVectorDescendingIntegerTests, TestVectorBinarySearchDescending)
{
    using T = typename TestFixture::input_type;
    for (auto size : get_sizes())
    {
        SCOPED_TRACE(testing::Message() << "with size = " << size);

        thrust::host_vector<T> h_vec = get_random_data<T>(size,
                                                          std::numeric_limits<T>::min(),
                                                          std::numeric_limits<T>::max());
        thrust::sort(h_vec.begin(), h_vec.end(), thrust::greater<T>());
        thrust::device_vector<T> d_vec = h_vec;

        thrust::host_vector<T> h_input = get_random_data<T>(2*size,
                                                            std::numeric_limits<T>::min(),
                                                            std::numeric_limits<T>::max());
        thrust::device_vector<T> d_input = h_input;

        thrust::host_vector<int>   h_output(2*size);
        thrust::device_vector<int> d_output(2*size);

        thrust::binary_search(h_vec.begin(), h_vec.end(), h_input.begin(), h_input.end(), h_output.begin(), thrust::greater<T>());
        thrust::binary_search(d_vec.begin(), d_vec.end(), d_input.begin(), d_input.end(), d_output.begin(), thrust::greater<T>());

        ASSERT_EQ(h_output, d_output);
    }
}

#endif // THRUST_DEVICE_COMPILER == THRUST_DEVICE_COMPILER_HCC
