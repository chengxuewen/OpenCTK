/*
 *  Copyright 2016 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include <octk_shared_buffer.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cstdint>

using namespace octk;

namespace
{

// clang-format off
const uint8_t kTestData[] = {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7,
                             0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf};
// clang-format on

}  // namespace

void EnsureBuffersShareData(const SharedBuffer &buf1,
                            const SharedBuffer &buf2)
{
    // Data is shared between buffers.
    EXPECT_EQ(buf1.size(), buf2.size());
    EXPECT_EQ(buf1.capacity(), buf2.capacity());
    const uint8_t *data1 = buf1.data();
    const uint8_t *data2 = buf2.data();
    EXPECT_EQ(data1, data2);
    EXPECT_EQ(buf1, buf2);
}

void EnsureBuffersDontShareData(const SharedBuffer &buf1,
                                const SharedBuffer &buf2)
{
    // Data is not shared between buffers.
    const uint8_t *data1 = buf1.cdata();
    const uint8_t *data2 = buf2.cdata();
    EXPECT_NE(data1, data2);
}

TEST(SharedBufferTest, TestCreateEmptyData)
{
    SharedBuffer buf(static_cast<const uint8_t *>(nullptr), 0);
    EXPECT_TRUE(buf.empty());
    EXPECT_EQ(buf.size(), 0u);
    EXPECT_EQ(buf.capacity(), 0u);
    EXPECT_EQ(buf.data(), nullptr);
}

TEST(SharedBufferTest, CreateEmptyDataWithCapacity)
{
    SharedBuffer buf(0, 16);
    EXPECT_TRUE(buf.empty());
    EXPECT_EQ(buf.size(), 0u);
    EXPECT_EQ(buf.capacity(), 16u);
    EXPECT_NE(buf.MutableData(), nullptr);
}

TEST(SharedBufferTest, TestMoveConstruct)
{
    EXPECT_TRUE(std::is_nothrow_move_constructible<SharedBuffer>::value);

    SharedBuffer buf1(kTestData, 3, 10);
    size_t buf1_size = buf1.size();
    size_t buf1_capacity = buf1.capacity();
    const uint8_t *buf1_data = buf1.cdata();

    SharedBuffer buf2(std::move(buf1));
    EXPECT_TRUE(buf1.empty());
    EXPECT_EQ(buf1.size(), 0u);
    EXPECT_EQ(buf1.capacity(), 0u);
    EXPECT_EQ(buf1.data(), nullptr);
    EXPECT_FALSE(buf2.empty());
    EXPECT_EQ(buf2.size(), buf1_size);
    EXPECT_EQ(buf2.capacity(), buf1_capacity);
    EXPECT_EQ(buf2.data(), buf1_data);
}

TEST(SharedBufferTest, TestMoveAssign)
{
    SharedBuffer buf1(kTestData, 3, 10);
    size_t buf1_size = buf1.size();
    size_t buf1_capacity = buf1.capacity();
    const uint8_t *buf1_data = buf1.cdata();

    SharedBuffer buf2;
    buf2 = std::move(buf1);
    EXPECT_EQ(buf1.size(), 0u);
    EXPECT_EQ(buf1.capacity(), 0u);
    EXPECT_EQ(buf1.data(), nullptr);
    EXPECT_EQ(buf2.size(), buf1_size);
    EXPECT_EQ(buf2.capacity(), buf1_capacity);
    EXPECT_EQ(buf2.data(), buf1_data);
}

TEST(SharedBufferTest, TestSwap)
{
    SharedBuffer buf1(kTestData, 3, 10);
    size_t buf1_size = buf1.size();
    size_t buf1_capacity = buf1.capacity();
    const uint8_t *buf1_data = buf1.cdata();

    SharedBuffer buf2(kTestData, 6, 20);
    size_t buf2_size = buf2.size();
    size_t buf2_capacity = buf2.capacity();
    const uint8_t *buf2_data = buf2.cdata();

    std::swap(buf1, buf2);
    EXPECT_EQ(buf1.size(), buf2_size);
    EXPECT_EQ(buf1.capacity(), buf2_capacity);
    EXPECT_EQ(buf1.data(), buf2_data);
    EXPECT_EQ(buf2.size(), buf1_size);
    EXPECT_EQ(buf2.capacity(), buf1_capacity);
    EXPECT_EQ(buf2.data(), buf1_data);
}

TEST(SharedBufferTest, TestAppendData)
{
    SharedBuffer buf1(kTestData, 3, 10);
    SharedBuffer buf2(buf1);

    EnsureBuffersShareData(buf1, buf2);

    // AppendData copies the underlying buffer.
    buf2.AppendData("foo");
    EXPECT_EQ(buf2.size(), buf1.size() + 4);  // "foo" + trailing 0x00
    EXPECT_EQ(buf2.capacity(), buf1.capacity());
    EXPECT_NE(buf2.data(), buf1.data());

    EXPECT_EQ(buf1, SharedBuffer(kTestData, 3));
    const int8_t exp[] = {0x0, 0x1, 0x2, 'f', 'o', 'o', 0x0};
    EXPECT_EQ(buf2, SharedBuffer(exp));
}

TEST(SharedBufferTest, SetEmptyData)
{
    SharedBuffer buf(10);

    buf.SetData<uint8_t>(nullptr, 0);

    EXPECT_EQ(0u, buf.size());
    EXPECT_TRUE(buf.empty());
}

TEST(SharedBufferTest, SetDataNoMoreThanCapacityDoesntCauseReallocation)
{
    SharedBuffer buf1(3, 10);
    const uint8_t *const original_allocation = buf1.cdata();

    buf1.SetData(kTestData, 10);

    EXPECT_EQ(original_allocation, buf1.cdata());
    EXPECT_EQ(buf1, SharedBuffer(kTestData, 10));
}

TEST(SharedBufferTest, SetDataMakeReferenceCopy)
{
    SharedBuffer buf1(kTestData, 3, 10);
    SharedBuffer buf2;

    buf2.SetData(buf1);

    EnsureBuffersShareData(buf1, buf2);
}

TEST(SharedBufferTest, SetDataOnSharedKeepsOriginal)
{
    const uint8_t data[] = "foo";
    SharedBuffer buf1(kTestData, 3, 10);
    const uint8_t *const original_allocation = buf1.cdata();
    SharedBuffer buf2(buf1);

    buf2.SetData(data);

    EnsureBuffersDontShareData(buf1, buf2);
    EXPECT_EQ(original_allocation, buf1.cdata());
    EXPECT_EQ(buf1, SharedBuffer(kTestData, 3));
    EXPECT_EQ(buf2, SharedBuffer(data));
}

TEST(SharedBufferTest, SetDataOnSharedKeepsCapacity)
{
    SharedBuffer buf1(kTestData, 3, 10);
    SharedBuffer buf2(buf1);
    EnsureBuffersShareData(buf1, buf2);

    buf2.SetData(kTestData, 2);

    EnsureBuffersDontShareData(buf1, buf2);
    EXPECT_EQ(2u, buf2.size());
    EXPECT_EQ(10u, buf2.capacity());
}

TEST(SharedBufferTest, TestEnsureCapacity)
{
    SharedBuffer buf1(kTestData, 3, 10);
    SharedBuffer buf2(buf1);

    // Smaller than existing capacity -> no change and still same contents.
    buf2.EnsureCapacity(8);
    EnsureBuffersShareData(buf1, buf2);
    EXPECT_EQ(buf1.size(), 3u);
    EXPECT_EQ(buf1.capacity(), 10u);
    EXPECT_EQ(buf2.size(), 3u);
    EXPECT_EQ(buf2.capacity(), 10u);

    // Lager than existing capacity -> data is cloned.
    buf2.EnsureCapacity(16);
    EnsureBuffersDontShareData(buf1, buf2);
    EXPECT_EQ(buf1.size(), 3u);
    EXPECT_EQ(buf1.capacity(), 10u);
    EXPECT_EQ(buf2.size(), 3u);
    EXPECT_EQ(buf2.capacity(), 16u);
    // The size and contents are still the same.
    EXPECT_EQ(buf1, buf2);
}

TEST(SharedBufferTest, SetSizeDoesntChangeOriginal)
{
    SharedBuffer buf1(kTestData, 3, 10);
    const uint8_t *const original_allocation = buf1.cdata();
    SharedBuffer buf2(buf1);

    buf2.SetSize(16);

    EnsureBuffersDontShareData(buf1, buf2);
    EXPECT_EQ(original_allocation, buf1.cdata());
    EXPECT_EQ(3u, buf1.size());
    EXPECT_EQ(10u, buf1.capacity());
}

TEST(SharedBufferTest, SetSizeCloneContent)
{
    SharedBuffer buf1(kTestData, 3, 10);
    SharedBuffer buf2(buf1);

    buf2.SetSize(16);

    EXPECT_EQ(buf2.size(), 16u);
    EXPECT_EQ(0, memcmp(buf2.data(), kTestData, 3));
}

TEST(SharedBufferTest, SetSizeMayIncreaseCapacity)
{
    SharedBuffer buf(kTestData, 3, 10);

    buf.SetSize(16);

    EXPECT_EQ(16u, buf.size());
    EXPECT_EQ(16u, buf.capacity());
}

TEST(SharedBufferTest, SetSizeDoesntDecreaseCapacity)
{
    SharedBuffer buf1(kTestData, 5, 10);
    SharedBuffer buf2(buf1);

    buf2.SetSize(2);

    EXPECT_EQ(2u, buf2.size());
    EXPECT_EQ(10u, buf2.capacity());
}

TEST(SharedBufferTest, ClearDoesntChangeOriginal)
{
    SharedBuffer buf1(kTestData, 3, 10);
    const uint8_t *const original_allocation = buf1.cdata();
    SharedBuffer buf2(buf1);

    buf2.Clear();

    EnsureBuffersDontShareData(buf1, buf2);
    EXPECT_EQ(3u, buf1.size());
    EXPECT_EQ(10u, buf1.capacity());
    EXPECT_EQ(original_allocation, buf1.cdata());
    EXPECT_EQ(0u, buf2.size());
}

TEST(SharedBufferTest, ClearDoesntChangeCapacity)
{
    SharedBuffer buf1(kTestData, 3, 10);
    SharedBuffer buf2(buf1);

    buf2.Clear();

    EXPECT_EQ(0u, buf2.size());
    EXPECT_EQ(10u, buf2.capacity());
}

TEST(SharedBufferTest, DataAccessorDoesntCloneData)
{
    SharedBuffer buf1(kTestData, 3, 10);
    SharedBuffer buf2(buf1);

    EXPECT_EQ(buf1.data(), buf2.data());
}

TEST(SharedBufferTest, MutableDataClonesDataWhenShared)
{
    SharedBuffer buf1(kTestData, 3, 10);
    SharedBuffer buf2(buf1);
    const uint8_t *cdata = buf1.data();

    uint8_t *data1 = buf1.MutableData();
    uint8_t *data2 = buf2.MutableData();
    // buf1 was cloned above.
    EXPECT_NE(data1, cdata);
    // Therefore buf2 was no longer sharing data and was not cloned.
    EXPECT_EQ(data2, cdata);
}

TEST(SharedBufferTest, SeveralReads)
{
    SharedBuffer buf1(kTestData, 3, 10);
    SharedBuffer buf2(buf1);

    EnsureBuffersShareData(buf1, buf2);
    for (size_t i = 0; i != 3u; ++i)
    {
        EXPECT_EQ(buf1[i], kTestData[i]);
    }
    EnsureBuffersShareData(buf1, buf2);
}

TEST(SharedBufferTest, SeveralWrites)
{
    SharedBuffer buf1(kTestData, 3, 10);
    SharedBuffer buf2(buf1);

    EnsureBuffersShareData(buf1, buf2);
    for (size_t i = 0; i != 3u; ++i)
    {
        buf1.MutableData()[i] = kTestData[i] + 1;
    }
    EXPECT_EQ(buf1.size(), 3u);
    EXPECT_EQ(buf1.capacity(), 10u);
    EXPECT_EQ(buf2.size(), 3u);
    EXPECT_EQ(buf2.capacity(), 10u);
    EXPECT_EQ(0, memcmp(buf2.cdata(), kTestData, 3));
}

TEST(SharedBufferTest, CreateSlice)
{
    SharedBuffer buf(kTestData, 10, 10);
    SharedBuffer slice = buf.Slice(3, 4);
    EXPECT_EQ(slice.size(), 4u);
    EXPECT_EQ(0, memcmp(buf.cdata() + 3, slice.cdata(), 4));
}

TEST(SharedBufferTest, NoCopyDataOnSlice)
{
    SharedBuffer buf(kTestData, 10, 10);
    SharedBuffer slice = buf.Slice(3, 4);
    EXPECT_EQ(buf.cdata() + 3, slice.cdata());
}

TEST(SharedBufferTest, WritingCopiesData)
{
    SharedBuffer buf(kTestData, 10, 10);
    SharedBuffer slice = buf.Slice(3, 4);
    slice.MutableData()[0] = 0xaa;
    EXPECT_NE(buf.cdata() + 3, slice.cdata());
    EXPECT_EQ(0, memcmp(buf.cdata(), kTestData, 10));
}

TEST(SharedBufferTest, WritingToBufferDoesntAffectsSlice)
{
    SharedBuffer buf(kTestData, 10, 10);
    SharedBuffer slice = buf.Slice(3, 4);
    buf.MutableData()[0] = 0xaa;
    EXPECT_NE(buf.cdata() + 3, slice.cdata());
    EXPECT_EQ(0, memcmp(slice.cdata(), kTestData + 3, 4));
}

TEST(SharedBufferTest, SliceOfASlice)
{
    SharedBuffer buf(kTestData, 10, 10);
    SharedBuffer slice = buf.Slice(3, 7);
    SharedBuffer slice2 = slice.Slice(2, 3);
    EXPECT_EQ(slice2.size(), 3u);
    EXPECT_EQ(slice.cdata() + 2, slice2.cdata());
    EXPECT_EQ(buf.cdata() + 5, slice2.cdata());
}

TEST(SharedBufferTest, SlicesAreIndependent)
{
    SharedBuffer buf(kTestData, 10, 10);
    SharedBuffer slice = buf.Slice(3, 7);
    SharedBuffer slice2 = buf.Slice(3, 7);
    slice2.MutableData()[0] = 0xaa;
    EXPECT_EQ(buf.cdata() + 3, slice.cdata());
}

TEST(SharedBufferTest, AcceptsVectorLikeTypes)
{
    std::vector<uint8_t> a = {1, 2};
    std::vector<int8_t> b = {3, 4};
    ArrayView<uint8_t> c(a);
    ArrayView<const int8_t> d(b);

    SharedBuffer a_buf(a);
    SharedBuffer b_buf(b);
    SharedBuffer c_buf(c);
    SharedBuffer d_buf(d);

    SharedBuffer all;
    all.AppendData(a);
    all.AppendData(b);
    all.AppendData(c);
    all.AppendData(d);

    EXPECT_EQ(all.size(), 8U);
}
