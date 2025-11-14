/*
 *  Copyright 2016 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include <octk_ref_counted_object.hpp>
#include <octk_string_view.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <utility>
#include <memory>
#include <string>

using namespace octk;

namespace
{

class A
{
public:
    explicit A() {}

    A(const A &) = delete;
    A &operator=(const A &) = delete;
};

class RefClass : public RefCountInterface
{
public:
    RefClass() {}

protected:
    ~RefClass() override {}
};

class RefClassWithRvalue : public RefCountInterface
{
public:
    explicit RefClassWithRvalue(std::unique_ptr<A> a) : a_(std::move(a)) {}

protected:
    ~RefClassWithRvalue() override {}

public:
    std::unique_ptr<A> a_;
};

class RefClassWithMixedValues : public RefCountInterface
{
public:
    RefClassWithMixedValues(std::unique_ptr<A> a, int b, StringView c)
        : a_(std::move(a)), b_(b), c_(c) {}

protected:
    ~RefClassWithMixedValues() override {}

public:
    std::unique_ptr<A> a_;
    int b_;
    std::string c_;
};

class Foo
{
public:
    Foo() {}
    Foo(int i, int j) : foo_(i + j) {}
    int foo_ = 0;
};

class FooItf : public RefCountInterface
{
public:
    FooItf() {}
    FooItf(int i, int j) : foo_(i + j) {}
    int foo_ = 0;
};
}  // namespace

TEST(RefCountedObject, HasOneRef)
{
    SharedRefPtr<RefCountedObject<RefClass>> aref(new RefCountedObject<RefClass>());
    EXPECT_TRUE(aref->HasOneRef());
    aref->addRef();
    EXPECT_FALSE(aref->HasOneRef());
    EXPECT_EQ(aref->Release(), RefCountReleaseStatus::kOtherRefsRemained);
    EXPECT_TRUE(aref->HasOneRef());
}

TEST(RefCountedObject, SupportRValuesInCtor)
{
    std::unique_ptr<A> a(new A());
    SharedRefPtr<RefClassWithRvalue> ref(new RefCountedObject<RefClassWithRvalue>(std::move(a)));
    EXPECT_TRUE(ref->a_.get() != nullptr);
    EXPECT_TRUE(a.get() == nullptr);
}

TEST(RefCountedObject, SupportMixedTypesInCtor)
{
    std::unique_ptr<A> a(new A());
    int b = 9;
    std::string c = "hello";
    SharedRefPtr<RefClassWithMixedValues> ref(new RefCountedObject<RefClassWithMixedValues>(std::move(a), b, c));
    EXPECT_TRUE(ref->a_.get() != nullptr);
    EXPECT_TRUE(a.get() == nullptr);
    EXPECT_EQ(b, ref->b_);
    EXPECT_EQ(c, ref->c_);
}

TEST(FinalRefCountedObject, CanWrapIntoScopedRefptr)
{
    //TODO::fix
//    using WrappedTyped = FinalRefCountedObject<A>;
//    static_assert(!std::is_polymorphic<WrappedTyped>::value, "");
//    SharedRefPtr<WrappedTyped> ref(new WrappedTyped());
//    EXPECT_TRUE(ref.get());
//    EXPECT_TRUE(ref->HasOneRef());
//    // Test reference counter is updated on some simple operations.
//    SharedRefPtr<WrappedTyped> ref2 = ref;
//    EXPECT_FALSE(ref->HasOneRef());
//    EXPECT_FALSE(ref2->HasOneRef());
//
//    ref = nullptr;
//    EXPECT_TRUE(ref2->HasOneRef());
}

TEST(FinalRefCountedObject, CanCreateFromMovedType)
{
    class MoveOnly
    {
    public:
        MoveOnly(int a) : a_(a) {}
        MoveOnly(MoveOnly &&) = default;

        int a() { return a_; }

    private:
        int a_;
    };
    MoveOnly foo(5);
    auto ref = utils::makeRefCounted<MoveOnly>(std::move(foo));
    EXPECT_EQ(ref->a(), 5);
}

// This test is mostly a compile-time test for SharedRefPtr compatibility.
TEST(RefCounted, SmartPointers)
{
    // Sanity compile-time tests. FooItf is virtual, Foo is not, FooItf inherits
    // from RefCountInterface, Foo does not.
    static_assert(std::is_base_of<RefCountInterface, FooItf>::value, "");
    static_assert(!std::is_base_of<RefCountInterface, Foo>::value, "");
    static_assert(std::is_polymorphic<FooItf>::value, "");
    static_assert(!std::is_polymorphic<Foo>::value, "");

    {
        // Test with FooItf, a class that inherits from RefCountInterface.
        // Check that we get a valid FooItf reference counted object.
        auto p = utils::makeRefCounted<FooItf>(2, 3);
        EXPECT_NE(p.get(), nullptr);
        EXPECT_EQ(p->foo_, 5);  // the FooItf ctor just stores 2+3 in foo_.

        // Declaring what should result in the same type as `p` is of.
        SharedRefPtr<FooItf> p2 = p;
    }

    {
        // Same for `Foo`
        auto p = utils::makeRefCounted<Foo>(2, 3);
        EXPECT_NE(p.get(), nullptr);
        EXPECT_EQ(p->foo_, 5);
        SharedRefPtr<FinalRefCountedObject<Foo>> p2 = p;
    }
}
