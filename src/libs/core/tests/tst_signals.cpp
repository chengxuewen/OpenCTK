/***********************************************************************************************************************
**
** Library: OpenCTK
**
** Copyright (C) 2025~Present ChengXueWen.
** Copyright 2016 The WebRTC Project Authors.
**
** License: MIT License
**
** Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
** documentation files (the "Software"), to deal in the Software without restriction, including without limitation
** the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
** and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
**
** The above copyright notice and this permission notice shall be included in all copies or substantial portions
** of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
** TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
** THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
** CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
** IN THE SOFTWARE.
**
***********************************************************************************************************************/

#include <octk_signals.hpp>

#include <list>
#include <atomic>
#include <thread>
#include <memory>
#include <random>
#include <utility>

#include <gtest/gtest.h>

OCTK_BEGIN_NAMESPACE

namespace
{
static std::atomic<std::int64_t> sum{0};

static void f1(int i) { sum += i; }
static void f2(int i) noexcept { sum += 2 * i; }

static void f(int i) { sum += i; }
static void ff(int i) { sum += i; }
static void fff(int i) { sum += i; }
static void ffff(int i) { sum += i; }

void fc(signals::Connection &c, int i)
{
    sum += i;
    c.disconnect();
}

struct s
{
    static void s1(int i) { sum += i; }
    static void s2(int i) noexcept { sum += 2 * i; }

    static void sf(signals::Connection &c, int i)
    {
        sum += i;
        c.disconnect();
    }
    void f(signals::Connection &c, int i)
    {
        sum += i;
        c.disconnect();
    }

    void f1(int i) { sum += i; }
    void f2(int i) const { sum += i; }
    void f3(int i) volatile { sum += i; }
    void f4(int i) const volatile { sum += i; }
    void f5(int i) noexcept { sum += i; }
    void f6(int i) const noexcept { sum += i; }
    void f7(int i) volatile noexcept { sum += i; }
    void f8(int i) const volatile noexcept { sum += i; }
};

struct oo
{
    void operator()(int i) { sum += i; }
    void operator()(double i) { sum += static_cast<int>(std::round(4 * i)); }
};
struct o
{
    void operator()(signals::Connection &c, int i)
    {
        sum += i;
        c.disconnect();
    }
};
struct o1
{
    void operator()(int i) { sum += i; }
};
struct o2
{
    void operator()(int i) const { sum += i; }
};
struct o3
{
    void operator()(int i) volatile { sum += i; }
};
struct o4
{
    void operator()(int i) const volatile { sum += i; }
};
struct o5
{
    void operator()(int i) noexcept { sum += i; }
};
struct o6
{
    void operator()(int i) const noexcept { sum += i; }
};
struct o7
{
    void operator()(int i) volatile noexcept { sum += i; }
};
struct o8
{
    void operator()(int i) const volatile noexcept { sum += i; }
};

struct dummy
{
};

static_assert(signals::trait::is_callable_v<signals::trait::TypeList<int>, decltype(&s::f1), std::shared_ptr<s>>, "");


using res_container = std::vector<signals::GroupId>;

static constexpr size_t num_groups = 100;
static constexpr size_t num_slots = 1000;

static auto pusher(int pos)
{
    return [pos = std::move(pos)](res_container &c) { c.push_back(pos); };
}

static auto adder(int v)
{
    return [v = std::move(v)](int &s) { s += v; };
}

template <typename T> struct object
{
    object();
    object(T i)
        : v{i}
    {
    }

    const T &val() const { return v; }
    T &val() { return v; }
    void set_val(const T &i)
    {
        if (i != v)
        {
            v = i;
            s(i);
        }
    }


    void inc_val(const T &i)
    {
        if (i != v)
        {
            v++;
            s(v);
        }
    }

    void dec_val(const T &i)
    {
        if (i != v)
        {
            v--;
            s(v);
        }
    }


    signals::Signal<T> &sig() { return s; }

private:
    T v;
    signals::Signal<T> s;
};


static void emit_many(signals::Signal<int> &sig)
{
    for (int i = 0; i < 10000; ++i)
        sig(1);
}

static void connect_emit(signals::Signal<int> &sig)
{
    for (int i = 0; i < 100; ++i)
    {
        auto s = sig.connect_scoped(f);
        for (int j = 0; j < 100; ++j)
            sig(1);
    }
}

static void connect_cross(signals::Signal<int> &s1, signals::Signal<int> &s2, std::atomic<int> &go)
{
    auto cross = s1.connect(
        [&](int i)
        {
            if (i & 1)
                f(i);
            else
                s2(i + 1);
        });

    go++;
    while (go != 3)
        std::this_thread::yield();

    for (int i = 0; i < 1000000; ++i)
        s1(i);
}
} // namespace

TEST(SlotsGroupsTest, TestRandomGroups)
{
    res_container results;
    signals::Signal<res_container &> sig;

    std::mt19937_64 gen{std::random_device()()};

    // create N groups with random ids
    std::uniform_int_distribution<int> dist(std::numeric_limits<int>::lowest());
    std::array<signals::GroupId, num_groups> gids;
    std::generate_n(gids.begin(), num_groups, [&] { return dist(gen); });

    // create
    std::uniform_int_distribution<size_t> slots_dist{0, num_groups - 1};

    for (size_t i = 0; i < num_slots; ++i)
    {
        auto gid = gids[slots_dist(gen)];
        sig.connect(pusher(gid), gid);
    }

    // signal
    sig(results);

    // check that the resulting container is sorted
    ASSERT_TRUE(std::is_sorted(results.begin(), results.end()));
}

TEST(SlotsGroupsTest, TestDisconnectGroup)
{
    int sum = 0;
    signals::Signal<int &> sig;
    sig.connect(adder(3), 3);
    sig.connect(adder(1), 1);
    sig.connect(adder(2), 2);

    sig(sum);
    ASSERT_TRUE(sum == 6);

    sig.disconnect(2);
    sig(sum);
    ASSERT_TRUE(sum == 10);
}

TEST(SignalTest, TestFreeConnection)
{
    sum = 0;
    signals::Signal<int> sig;

    auto c1 = sig.connect(f1);
    sig(1);
    ASSERT_TRUE(sum == 1);

    sig.connect(f2);
    signals::connect(sig, f1);
    sig(1);
    ASSERT_TRUE(sum == 5);
}

TEST(SignalTest, TestStaticConnection)
{
    sum = 0;
    signals::Signal<int> sig;

    sig.connect(&s::s1);
    sig(1);
    ASSERT_TRUE(sum == 1);

    sig.connect(&s::s2);
    signals::connect(sig, &s::s1);
    sig(1);
    ASSERT_TRUE(sum == 5);
}

TEST(SignalTest, TestPmfConnection)
{
    sum = 0;
    signals::Signal<int> sig;
    s p;

    sig.connect(&s::f1, &p);
    sig.connect(&s::f2, &p);
    sig.connect(&s::f3, &p);
    sig.connect(&s::f4, &p);
    sig.connect(&s::f5, &p);
    sig.connect(&s::f6, &p);
    sig.connect(&s::f7, &p);
    sig.connect(&s::f8, &p);
    signals::connect(sig, &s::f1, &p);

    sig(1);
    ASSERT_TRUE(sum == 9);
}

TEST(SignalTest, TestConstPmfConnection)
{
    sum = 0;
    signals::Signal<int> sig;
    const s p;

    sig.connect(&s::f2, &p);
    sig.connect(&s::f4, &p);
    sig.connect(&s::f6, &p);
    sig.connect(&s::f8, &p);
    signals::connect(sig, &s::f2, &p);

    sig(1);
    ASSERT_TRUE(sum == 5);
}

TEST(SignalTest, TestFunctionObjectConnection)
{
    sum = 0;
    signals::Signal<int> sig;

    sig.connect(o1{});
    sig.connect(o2{});
    sig.connect(o3{});
    sig.connect(o4{});
    sig.connect(o5{});
    sig.connect(o6{});
    sig.connect(o7{});
    sig.connect(o8{});
    signals::connect(sig, o1{});

    sig(1);
    ASSERT_TRUE(sum == 9);
}

TEST(SignalTest, TestOverloadedFunctionObjectConnection)
{
    sum = 0;
    signals::Signal<int> sig;
    signals::Signal<double> sig1;

    sig.connect(oo{});
    signals::connect(sig, oo{});
    sig(1);
    ASSERT_TRUE(sum == 2);

    sig1.connect(oo{});
    signals::connect(sig1, oo{});
    sig1(1);
    ASSERT_TRUE(sum == 10);
}

TEST(SignalTest, TestLambdaConnection)
{
    sum = 0;
    signals::Signal<int> sig;

    sig.connect([&](int i) { sum += i; });
    signals::connect(sig, [&](int i) { sum += i; });
    sig(1);
    ASSERT_TRUE(sum == 2);

    sig.connect([&](int i) mutable { sum += 2 * i; });
    signals::connect(sig, [&](int i) mutable { sum += 2 * i; });
    sig(1);
    ASSERT_TRUE(sum == 8);
}

TEST(SignalTest, TestGenericLambdaConnection)
{
    std::stringstream s;

    auto f = [&](auto a, auto... args)
    {
        using result_t = int[];
        s << a;
        result_t r{
            1,
            ((void)(s << args), 1)...,
        };
        (void)r;
    };

    signals::Signal<int> sig1;
    signals::Signal<std::string> sig2;
    signals::Signal<double> sig3;

    sig1.connect(f);
    sig2.connect(f);
    sig3.connect(f);
    signals::connect(sig1, f);
    signals::connect(sig2, f);
    signals::connect(sig3, f);
    sig1(1);
    sig2("foo");
    sig3(4.1);

    ASSERT_TRUE(s.str() == "11foofoo4.14.1");
}

TEST(SignalTest, TestLvalueEmission)
{
    sum = 0;
    signals::Signal<int> sig;

    auto c1 = sig.connect(f1);
    int v = 1;
    sig(v);
    ASSERT_TRUE(sum == 1);

    sig.connect(f2);
    sig(v);
    ASSERT_TRUE(sum == 4);
}

TEST(SignalTest, TestMutation)
{
    int res = 0;
    signals::Signal<int &> sig;

    sig.connect([](int &r) { r += 1; });
    sig(res);
    ASSERT_TRUE(res == 1);

    sig.connect([](int &r) mutable { r += 2; });
    sig(res);
    ASSERT_TRUE(res == 4);
}

TEST(SignalTest, TestCompatibleArgs)
{
    long ll = 0;
    std::string ss;
    short ii = 0;

    auto f = [&](long l, const std::string &s, short i)
    {
        ll = l;
        ss = s;
        ii = i;
    };

    signals::Signal<int, std::string, bool> sig;
    sig.connect(f);
    sig('0', "foo", true);

    ASSERT_TRUE(ll == 48);
    ASSERT_TRUE(ss == "foo");
    ASSERT_TRUE(ii == 1);
}

TEST(SignalTest, TestCompatibleArgsChaining)
{
    long ll = 0;
    std::string ss;
    short ii = 0;

    auto f = [&](long l, const std::string &s, short i)
    {
        ll = l;
        ss = s;
        ii = i;
    };

    signals::Signal<long, std::string, short> sig1;
    sig1.connect(f);

    signals::Signal<int, std::string, bool> sig2;
    // signals::connect(sig2, sig1);
    sig2('0', "foo", true);

    ASSERT_TRUE(ll == 48);
    ASSERT_TRUE(ss == "foo");
    ASSERT_TRUE(ii == 1);
}

TEST(SignalTest, TestDisconnection)
{
    // test removing only connected
    {
        sum = 0;
        signals::Signal<int> sig;

        auto sc = sig.connect(f1);
        sig(1);
        ASSERT_TRUE(sum == 1);

        sc.disconnect();
        sig(1);
        ASSERT_TRUE(sum == 1);
        ASSERT_TRUE(!sc.valid());
    }

    // test removing first connected
    {
        sum = 0;
        signals::Signal<int> sig;

        auto sc = sig.connect(f1);
        sig(1);
        ASSERT_TRUE(sum == 1);

        sig.connect(f2);
        sig(1);
        ASSERT_TRUE(sum == 4);

        sc.disconnect();
        sig(1);
        ASSERT_TRUE(sum == 6);
        ASSERT_TRUE(!sc.valid());
    }

    // test removing last connected
    {
        sum = 0;
        signals::Signal<int> sig;

        sig.connect(f1);
        sig(1);
        ASSERT_TRUE(sum == 1);

        auto sc = sig.connect(f2);
        sig(1);
        ASSERT_TRUE(sum == 4);

        sc.disconnect();
        sig(1);
        ASSERT_TRUE(sum == 5);
        ASSERT_TRUE(!sc.valid());
    }
}

TEST(SignalTest, TestDisconnectionByCallable)
{
    // disconnect a function pointer
    {
        sum = 0;
        signals::Signal<int> sig;

        sig.connect(f1);
        sig.connect(f2);
        sig.connect(f2);
        sig(1);
        ASSERT_TRUE(sum == 5);
        auto c = sig.disconnect(&f2);
        ASSERT_TRUE(c == 2);
        sig(1);
        ASSERT_TRUE(sum == 6);
    }

    // disconnect a function
    {
        sum = 0;
        signals::Signal<int> sig;

        sig.connect(f1);
        sig.connect(f2);
        sig(1);
        ASSERT_TRUE(sum == 3);
        sig.disconnect(f1);
        sig(1);
        ASSERT_TRUE(sum == 5);
    }

#if OCTK_RTTI_ENABLED
    // disconnect by pmf
    {
        sum = 0;
        signals::Signal<int> sig;
        s p;

        sig.connect(&s::f1, &p);
        sig.connect(&s::f2, &p);
        sig(1);
        ASSERT_TRUE(sum == 2);
        sig.disconnect(&s::f1);
        sig(1);
        ASSERT_TRUE(sum == 3);
    }

    // disconnect by function object
    {
        sum = 0;
        signals::Signal<int> sig;

        sig.connect(o1{});
        sig.connect(o2{});
        sig(1);
        ASSERT_TRUE(sum == 2);
        // sig.disconnect(o1{});
        sig(1);
        ASSERT_TRUE(sum == 3);
    }

    // disconnect by lambda
    {
        sum = 0;
        signals::Signal<int> sig;
        auto l1 = [&](int i) { sum += i; };
        auto l2 = [&](int i) { sum += 2 * i; };
        sig.connect(l1);
        sig.connect(l2);
        sig(1);
        ASSERT_TRUE(sum == 3);
        // sig.disconnect(l1);
        sig(1);
        ASSERT_TRUE(sum == 5);
    }
#endif
}

TEST(SignalTest, TestDisconnectionByObject)
{
    // disconnect by pointer
    {
        sum = 0;
        signals::Signal<int> sig;
        s p1, p2;

        sig.connect(&s::f1, &p1);
        sig.connect(&s::f2, &p2);
        sig(1);
        ASSERT_TRUE(sum == 2);
        sig.disconnect(&p1);
        sig(1);
        ASSERT_TRUE(sum == 3);
    }

    // disconnect by shared pointer
    {
        sum = 0;
        signals::Signal<int> sig;
        auto p1 = std::make_shared<s>();
        s p2;

        sig.connect(&s::f1, p1);
        sig.connect(&s::f2, &p2);
        sig(1);
        ASSERT_TRUE(sum == 2);
        sig.disconnect(p1);
        sig(1);
        ASSERT_TRUE(sum == 3);
    }
}

TEST(SignalTest, TestDisconnectionByObjectAndPmf)
{
    // disconnect by pointer
    {
        sum = 0;
        signals::Signal<int> sig;
        s p1, p2;

        sig.connect(&s::f1, &p1);
        sig.connect(&s::f1, &p2);
        sig.connect(&s::f2, &p1);
        sig.connect(&s::f2, &p2);
        sig(1);
        ASSERT_TRUE(sum == 4);
        sig.disconnect(&s::f1, &p2);
        sig(1);
        ASSERT_TRUE(sum == 7);
    }

    // disconnect by shared pointer
    {
        sum = 0;
        signals::Signal<int> sig;
        auto p1 = std::make_shared<s>();
        auto p2 = std::make_shared<s>();

        sig.connect(&s::f1, p1);
        sig.connect(&s::f1, p2);
        sig.connect(&s::f2, p1);
        sig.connect(&s::f2, p2);
        sig(1);
        ASSERT_TRUE(sum == 4);
        sig.disconnect(&s::f1, p2);
        sig(1);
        ASSERT_TRUE(sum == 7);
    }

    // disconnect by tracker
    {
        sum = 0;
        signals::Signal<int> sig;

        auto t = std::make_shared<bool>();
        sig.connect(f1);
        sig.connect(f2);
        sig.connect(f1, t);
        sig.connect(f2, t);
        sig(1);
        ASSERT_TRUE(sum == 6);
        sig.disconnect(f2, t);
        sig(1);
        ASSERT_TRUE(sum == 10);
    }
}

TEST(SignalTest, TestScopedConnection)
{
    sum = 0;
    signals::Signal<int> sig;

    {
        auto sc1 = sig.connect_scoped(f1);
        sig(1);
        ASSERT_TRUE(sum == 1);

        auto sc2 = sig.connect_scoped(f2);
        sig(1);
        ASSERT_TRUE(sum == 4);
    }

    sig(1);
    ASSERT_TRUE(sum == 4);

    sum = 0;

    {
        signals::ScopedConnection sc1 = sig.connect(f1);
        sig(1);
        ASSERT_TRUE(sum == 1);

        auto sc2 = sig.connect_scoped(f2);
        sig(1);
        ASSERT_TRUE(sum == 4);
    }

    sig(1);
    ASSERT_TRUE(sum == 4);
}

TEST(SignalTest, test_connection_blocking)
{
    sum = 0;
    signals::Signal<int> sig;

    auto c1 = sig.connect(f1);
    sig.connect(f2);
    sig(1);
    ASSERT_TRUE(sum == 3);

    c1.block();
    sig(1);
    ASSERT_TRUE(sum == 5);

    c1.unblock();
    sig(1);
    ASSERT_TRUE(sum == 8);
}

TEST(SignalTest, TestConnectionBlocker)
{
    sum = 0;
    signals::Signal<int> sig;

    auto c1 = sig.connect(f1);
    sig.connect(f2);
    sig(1);
    ASSERT_TRUE(sum == 3);

    {
        auto cb = c1.blocker();
        sig(1);
        ASSERT_TRUE(sum == 5);
    }

    sig(1);
    ASSERT_TRUE(sum == 8);
}

TEST(SignalTest, TestSignalBlocking)
{
    sum = 0;
    signals::Signal<int> sig;

    sig.connect(f1);
    sig.connect(f2);
    sig(1);
    ASSERT_TRUE(sum == 3);

    sig.block();
    sig(1);
    ASSERT_TRUE(sum == 3);

    sig.unblock();
    sig(1);
    ASSERT_TRUE(sum == 6);
}

TEST(SignalTest, TestAllDisconnection)
{
    sum = 0;
    signals::Signal<int> sig;

    sig.connect(f1);
    sig.connect(f2);
    sig(1);
    ASSERT_TRUE(sum == 3);

    sig.disconnect_all();
    sig(1);
    ASSERT_TRUE(sum == 3);
}

TEST(SignalTest, TestConnectionCopyingMoving)
{
    sum = 0;
    signals::Signal<int> sig;

    auto sc1 = sig.connect(f1);
    auto sc2 = sig.connect(f2);

    auto sc3 = sc1;
    auto sc4{sc2};

    auto sc5 = std::move(sc3);
    auto sc6{std::move(sc4)};

    sig(1);
    ASSERT_TRUE(sum == 3);

    sc5.block();
    sig(1);
    ASSERT_TRUE(sum == 5);

    sc1.unblock();
    sig(1);
    ASSERT_TRUE(sum == 8);

    sc6.disconnect();
    sig(1);
    ASSERT_TRUE(sum == 9);
}

TEST(SignalTest, TestScopedConnectionMoving)
{
    sum = 0;
    signals::Signal<int> sig;

    {
        auto sc1 = sig.connect_scoped(f1);
        sig(1);
        ASSERT_TRUE(sum == 1);

        auto sc2 = sig.connect_scoped(f2);
        sig(1);
        ASSERT_TRUE(sum == 4);

        auto sc3 = std::move(sc1);
        sig(1);
        ASSERT_TRUE(sum == 7);

        auto sc4{std::move(sc2)};
        sig(1);
        ASSERT_TRUE(sum == 10);
    }

    sig(1);
    ASSERT_TRUE(sum == 10);
}

TEST(SignalTest, TestSignalMoving)
{
    sum = 0;
    signals::Signal<int> sig;

    sig.connect(f1);
    sig.connect(f2);

    sig(1);
    ASSERT_TRUE(sum == 3);

    auto sig2 = std::move(sig);
    sig2(1);
    ASSERT_TRUE(sum == 6);

    auto sig3 = std::move(sig2);
    sig3(1);
    ASSERT_TRUE(sum == 9);
}

TEST(SignalTest, TestLoop)
{
    object<int> i1(0);
    object<int> i2(3);

    i1.sig().connect(&object<int>::set_val, &i2);
    i2.sig().connect(&object<int>::set_val, &i1);

    i1.set_val(1);

    ASSERT_TRUE(i1.val() == 1);
    ASSERT_TRUE(i2.val() == 1);
}

TEST(SignalTrackingTest, TestTrackOther)
{
    sum = 0;
    signals::Signal<int> sig;

    auto d1 = std::make_shared<dummy>();
    auto conn1 = sig.connect(f1, d1);

    auto d2 = std::make_shared<dummy>();
    std::weak_ptr<dummy> w2 = d2;
    auto conn2 = sig.connect(o1(), w2);

    sig(1);
    EXPECT_EQ(sum, 2);

    d1.reset();
    sig(1);
    EXPECT_EQ(sum, 3);
    ASSERT_TRUE(!conn1.valid());

    d2.reset();
    sig(1);
    EXPECT_EQ(sum, 3);
    ASSERT_TRUE(!conn2.valid());
}

TEST(SignalTrackingTest, TestTrackOverloadedFunctionObject)
{
    sum = 0;
    signals::Signal<int> sig;
    signals::Signal<double> sig1;

    auto d1 = std::make_shared<dummy>();
    auto conn1 = sig.connect(oo{}, d1);
    sig(1);
    ASSERT_TRUE(sum == 1);

    d1.reset();
    sig(1);
    ASSERT_TRUE(sum == 1);
    ASSERT_TRUE(!conn1.valid());

    auto d2 = std::make_shared<dummy>();
    std::weak_ptr<dummy> w2 = d2;
    auto conn2 = sig1.connect(oo{}, w2);
    sig1(1);
    ASSERT_TRUE(sum == 5);

    d2.reset();
    sig1(1);
    ASSERT_TRUE(sum == 5);
    ASSERT_TRUE(!conn2.valid());
}

TEST(SignalTrackingTest, TestTrackGenericLambda)
{
    std::stringstream s;

    auto f = [&](auto a, auto... args)
    {
        using result_t = int[];
        s << a;
        result_t r{
            1,
            ((void)(s << args), 1)...,
        };
        (void)r;
    };

    signals::Signal<int> sig1;
    signals::Signal<std::string> sig2;
    signals::Signal<double> sig3;

    auto d1 = std::make_shared<dummy>();
    sig1.connect(f, d1);
    sig2.connect(f, d1);
    sig3.connect(f, d1);

    sig1(1);
    sig2("foo");
    sig3(4.1);
    ASSERT_TRUE(s.str() == "1foo4.1");

    d1.reset();
    sig1(2);
    sig2("bar");
    sig3(3.0);
    ASSERT_TRUE(s.str() == "1foo4.1");
}

TEST(SignalThreadedTest, TestThreadedMix)
{
    sum = 0;

    signals::Signal<int> sig;

    std::array<std::thread, 10> threads;
    for (auto &t : threads)
        t = std::thread(connect_emit, std::ref(sig));

    for (auto &t : threads)
        t.join();
}

TEST(SignalThreadedTest, TestThreadedEmission)
{
    sum = 0;

    signals::Signal<int> sig;
    sig.connect(f);

    std::array<std::thread, 10> threads;
    for (auto &t : threads)
        t = std::thread(emit_many, std::ref(sig));

    for (auto &t : threads)
        t.join();

    EXPECT_TRUE(sum == 100000l);
}

TEST(SignalThreadedTest, TestThreadedCrossed)
{
    sum = 0;

    signals::Signal<int> sig1;
    signals::Signal<int> sig2;

    std::atomic<int> go{0};

    std::thread t1(connect_cross, std::ref(sig1), std::ref(sig2), std::ref(go));
    std::thread t2(connect_cross, std::ref(sig2), std::ref(sig1), std::ref(go));

    while (go != 2)
        std::this_thread::yield();
    go++;

    t1.join();
    t2.join();

    EXPECT_TRUE(sum == std::int64_t(1000000000000ll));
}

TEST(SignalThreadedTest, TestThreadedMisc)
{
    sum = 0;
    signals::Signal<int> sig;
    std::atomic<bool> run{true};

    auto emitter = [&]
    {
        while (run)
        {
            sig(1);
        }
    };

    auto conn = [&]
    {
        while (run)
        {
            for (int i = 0; i < 10; ++i)
            {
                sig.connect(f);
                sig.connect(ff);
                sig.connect(fff);
            }
        }
    };

    auto disconn = [&]
    {
        unsigned int i = 0;
        while (run)
        {
            if (i == 0)
                sig.disconnect(f);
            else if (i == 1)
                sig.disconnect(ff);
            else
                sig.disconnect(fff);
            i++;
            i = i % 3;
        }
    };

    std::array<std::thread, 20> emitters;
    std::array<std::thread, 20> conns;
    std::array<std::thread, 20> disconns;

    for (auto &t : conns)
        t = std::thread(conn);
    for (auto &t : emitters)
        t = std::thread(emitter);
    for (auto &t : disconns)
        t = std::thread(disconn);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    run = false;

    for (auto &t : emitters)
        t.join();
    for (auto &t : disconns)
        t.join();
    for (auto &t : conns)
        t.join();
}

namespace
{
void fun() { }

struct b1
{
    virtual ~b1() = default;
    static void sm() { sum++; }
    void m() { sum++; }
    virtual void vm() { sum++; }
};

struct b2
{
    virtual ~b2() = default;
    static void sm() { sum++; }
    void m() { sum++; }
    virtual void vm() { sum++; }
};

struct c
{
    virtual ~c() = default;
    virtual void w() { }
};

struct d : b1
{
    static void sm() { sum++; }
    void m() { sum++; }
    void vm() override { sum++; }
};

struct e : b1, c
{
    static void sm() { sum++; }
    void m() const { sum++; }
    void vm() override { sum++; }
};

struct StructF : virtual b1
{
    static void sm() { sum++; }
    void m() const { sum++; }
    void vm() override { sum++; }
};

template <typename T> union sizer
{
    T t;
    char data[sizeof(T)];
};

template <typename T> std::string ptr_string(const T &t)
{
    sizer<T> ss;
    std::uninitialized_fill(std::begin(ss.data), std::end(ss.data), '\0');
    ss.t = t;
    std::string addr;
    for (char i : ss.data)
    {
        char b[] = "00";
        std::snprintf(b, 3, "%02X", static_cast<unsigned char>(i));
        addr += b;
    }

    // shorten string
    while (addr.size() >= 2)
    {
        auto si = addr.size();
        if (addr[si - 1] == '0' && addr[si - 2] == '0')
        {
            addr.pop_back();
            addr.pop_back();
        }
        else
        {
            break;
        }
    }
    return addr;
}

template <typename T> std::string print(std::string name, const T &t)
{
    auto addr = ptr_string(t);
    std::cout << name << "\t" << sizeof(t) << "\t0x" << addr << std::endl;
    return addr;
}
} // namespace
TEST(SignalPMFTest, testAddress)
{
    sum = 0;
    std::vector<std::string> addrs;

    addrs.push_back(print("fun", &fun));
    addrs.push_back(print("&b1::sm", &b1::sm));
    addrs.push_back(print("&b1::m", &b1::m));
    addrs.push_back(print("&b1::vm", &b1::vm));
    addrs.push_back(print("&b2::sm", &b2::sm));
    addrs.push_back(print("&b2::m", &b2::m));
    addrs.push_back(print("&b2::vm", &b2::vm));
    addrs.push_back(print("&d::sm", &d::sm));
    addrs.push_back(print("&d::m", &d::m));
    addrs.push_back(print("&d::vm", &d::vm));
    addrs.push_back(print("&e::sm", &e::sm));
    addrs.push_back(print("&e::m", &e::m));
    addrs.push_back(print("&e::vm", &e::vm));

    std::sort(addrs.begin(), addrs.end());
    auto last = std::unique(addrs.begin(), addrs.end());
    std::cout << "Address duplicates: " << std::distance(last, addrs.end()) << std::endl;

    signals::Signal<> sig;

    auto sb1 = std::make_shared<b1>();
    auto sb2 = std::make_shared<b2>();
    auto sd = std::make_shared<d>();
    auto se = std::make_shared<e>();
    auto sf = std::make_shared<StructF>();

    sig.connect(&b1::sm);
    sig.connect(&b1::m, sb1);
    sig.connect(&b1::vm, sb1);
    sig.connect(&b2::sm);
    sig.connect(&b2::m, sb2);
    sig.connect(&b2::vm, sb2);
    sig.connect(&d::sm);
    sig.connect(&d::m, sd);
    sig.connect(&d::vm, sd);
    sig.connect(&e::sm);
    sig.connect(&e::m, se);
    sig.connect(&e::vm, se);
    sig.connect(&StructF::sm);
    sig.connect(&StructF::m, sf);
    sig.connect(&StructF::vm, sf);

    sig();
    EXPECT_TRUE(sum == 15);

#if OCTK_RTTI_ENABLED
    size_t n = 0;
    // n = sig.disconnect(&b1::sm);
    EXPECT_TRUE(n == 1);
    n = sig.disconnect(&b1::m);
    EXPECT_TRUE(n == 1);
    n = sig.disconnect(&b1::vm);
    EXPECT_TRUE(n == 1);
    // n = sig.disconnect(&b2::sm);
    EXPECT_TRUE(n == 1);
    n = sig.disconnect(&b2::m);
    EXPECT_TRUE(n == 1);
    n = sig.disconnect(&b2::vm);
    EXPECT_TRUE(n == 1);
    // n = sig.disconnect(&d::sm);
    EXPECT_TRUE(n == 1);
    n = sig.disconnect(&d::m);
    EXPECT_TRUE(n == 1);
    n = sig.disconnect(&d::vm);
    EXPECT_TRUE(n == 1);
    // n = sig.disconnect(&e::sm);
    EXPECT_TRUE(n == 1);
    n = sig.disconnect(&e::m);
    EXPECT_TRUE(n == 1);
    n = sig.disconnect(&e::vm);
    EXPECT_TRUE(n == 1);
    // n = sig.disconnect(&StructF::sm);
    EXPECT_TRUE(n == 1);
    n = sig.disconnect(&StructF::m);
    EXPECT_TRUE(n == 1);
    n = sig.disconnect(&StructF::vm);
    EXPECT_TRUE(n == 1);
#endif
}

TEST(SignalPerformanceTest, TestSignalPerformance)
{
    using Clock = std::chrono::high_resolution_clock;
    using TimePoint = std::chrono::time_point<Clock>;

    constexpr int count = 1000;
    double ref_ns = 0.;
    signals::Signal<> sig;
    {
        std::vector<signals::ScopedConnection> connections;
        connections.reserve(count);
        for (int i = 0; i < count; i++)
        {
            connections.emplace_back(sig.connect([] { }));
        }

        // Measure first signal time as reference
        const TimePoint begin = Clock::now();
        sig();
        ref_ns = double(std::chrono::duration_cast<std::chrono::nanoseconds>(Clock::now() - begin).count());
    }

    // Measure signal after all slot were disconnected
    const TimePoint begin = Clock::now();
    sig();
    const double after_disconnection_ns = double(
        std::chrono::duration_cast<std::chrono::nanoseconds>(Clock::now() - begin).count());

    // Ensure that the signal cost is not > at 10%
    const auto max_delta = 0.1;
    const auto delta = (after_disconnection_ns - ref_ns) / ref_ns;

    std::cout << "ref: " << ref_ns / 1000 << " us" << std::endl;
    std::cout << "after: " << after_disconnection_ns / 1000 << " us" << std::endl;
    std::cout << "delta: " << delta << " SU" << std::endl;

    EXPECT_TRUE(delta < max_delta);
}

TEST(SignalExtendedTest, test_free_connection)
{
    sum = 0;
    signals::Signal<int> sig;
    sig.connect_extended(fc);

    sig(1);
    EXPECT_TRUE(sum == 1);
    sig(1);
    EXPECT_TRUE(sum == 1);
}

TEST(SignalExtendedTest, test_static_connection)
{
    sum = 0;
    signals::Signal<int> sig;
    sig.connect_extended(&s::sf);

    sig(1);
    EXPECT_TRUE(sum == 1);
    sig(1);
    EXPECT_TRUE(sum == 1);
}

TEST(SignalExtendedTest, test_pmf_connection)
{
    sum = 0;
    signals::Signal<int> sig;
    s p;
    sig.connect_extended(&s::f, &p);

    sig(1);
    EXPECT_TRUE(sum == 1);
    sig(1);
    EXPECT_TRUE(sum == 1);
}

TEST(SignalExtendedTest, test_tracked_pmf_connection)
{
    sum = 0;
    signals::Signal<int> sig;
    auto p = std::make_shared<s>();
    sig.connect_extended(&s::f, p);

    sig(1);
    EXPECT_TRUE(sum == 1);
    sig(1);
    EXPECT_TRUE(sum == 1);

    sig.connect_extended(&s::f, p);
    p.reset();
    sig(1);
    EXPECT_TRUE(sum == 1);
}

TEST(SignalExtendedTest, test_function_object_connection)
{
    sum = 0;
    signals::Signal<int> sig;
    sig.connect_extended(o{});

    sig(1);
    EXPECT_TRUE(sum == 1);
    sig(1);
    EXPECT_TRUE(sum == 1);
}

TEST(SignalExtendedTest, test_tracked_connection)
{
    sum = 0;
    struct dummy
    {
    };
    auto d = std::make_shared<dummy>();

    signals::Signal<int> sig;
    sig.connect_extended(o{}, d);

    sig(1);
    EXPECT_TRUE(sum == 1);
    sig(1);
    EXPECT_TRUE(sum == 1);

    sig.connect_extended(o{}, d);
    d.reset();
    sig(1);
    EXPECT_TRUE(sum == 1);
}

TEST(SignalExtendedTest, test_lambda_connection)
{
    sum = 0;
    signals::Signal<int> sig;

    sig.connect_extended(
        [&](signals::Connection &c, int i)
        {
            sum += i;
            c.disconnect();
        });
    sig(1);
    EXPECT_TRUE(sum == 1);

    sig.connect_extended(
        [&](signals::Connection &c, int i) mutable
        {
            sum += 2 * i;
            c.disconnect();
        });
    sig(1);
    EXPECT_TRUE(sum == 3);
    sig(1);
    EXPECT_TRUE(sum == 3);
}

TEST(SignalRecursiveTest, test_recursive)
{
    object<int> i1(-1);
    object<int> i2(10);

    i1.sig().connect(&object<int>::dec_val, &i2);
    i2.sig().connect(&object<int>::inc_val, &i1);

    i1.inc_val(0);

    EXPECT_TRUE(i1.val() == i2.val());
}

TEST(SignalRecursiveTest, test_self_recursive)
{
    int i = 0;

    signals::Signal<int> s;
    s.connect(
        [&](int v)
        {
            if (i < 10)
            {
                i++;
                s(v + 1);
            }
        });

    s(0);

    EXPECT_TRUE(i == 10);
}

namespace
{

struct so : signals::observer
{
    ~so() override { this->disconnect_all(); }

    void f1(int &i) { ++i; }
};

struct so_st : signals::observer_st
{
    void f1(int &i) { ++i; }
};

struct s_plain
{
    void f1(int &i) { ++i; }
};

template <typename T, template <typename...> class SIG_T> void test_observer()
{
    SIG_T<int &> sig;

    // Automatic disconnect via observer inheritance
    {
        T p1;
        sig.connect(&T::f1, &p1);
        EXPECT_TRUE(sig.slot_count() == 1);

        {
            T p2;
            sig.connect(&T::f1, &p2);
            EXPECT_TRUE(sig.slot_count() == 2);
        }

        EXPECT_TRUE(sig.slot_count() == 1);
    }

    EXPECT_TRUE(sig.slot_count() == 0);

    // No automatic disconnect
    {
        s_plain p;
        sig.connect(&s_plain::f1, &p);
        EXPECT_TRUE(sig.slot_count() == 1);
    }

    EXPECT_TRUE(sig.slot_count() == 1);
}

template <typename T, template <typename...> class SIG_T> void test_observer_signals()
{
    int sum = 0;
    SIG_T<int &> sig;

    {
        T p1;
        sig.connect(&T::f1, &p1);
        sig(sum);
        EXPECT_TRUE(sum == 1);
        {
            T p2;
            sig.connect(&T::f1, &p2);
            sig(sum);
            EXPECT_TRUE(sum == 3);
        }
        sig(sum);
        EXPECT_TRUE(sum == 4);
    }

    sig(sum);
    EXPECT_TRUE(sum == 4);
}

template <typename T, template <typename...> class SIG_T> void test_observer_signals_heap()
{
    int sum = 0;
    SIG_T<int &> sig;

    {
        auto *p1 = new T;
        sig.connect(&T::f1, p1);
        sig(sum);
        EXPECT_TRUE(sum == 1);
        {
            auto *p2 = new T;
            sig.connect(&T::f1, p2);
            sig(sum);
            EXPECT_TRUE(sum == 3);
            delete p2;
        }
        sig(sum);
        EXPECT_TRUE(sum == 4);
        delete p1;
    }

    sig(sum);
    EXPECT_TRUE(sum == 4);
}


template <typename T, template <typename...> class SIG_T> void test_observer_signals_shared()
{
    int sum = 0;
    SIG_T<int &> sig;

    {
        auto p1 = std::make_shared<T>();
        sig.connect(&T::f1, p1);
        sig(sum);
        EXPECT_TRUE(sum == 1);
        {
            auto p2 = std::make_shared<T>();
            sig.connect(&T::f1, p2);
            sig(sum);
            EXPECT_TRUE(sum == 3);
        }
        sig(sum);
        EXPECT_TRUE(sum == 4);
    }

    sig(sum);
    EXPECT_TRUE(sum == 4);
}

template <typename T, template <typename...> class SIG_T> void test_observer_signals_list()
{
    int sum = 0;
    SIG_T<int &> sig;

    {
        std::list<T> l;
        for (auto i = 0; i < 10; ++i)
        {
            l.emplace_back();
            sig.connect(&T::f1, &l.back());
        }

        EXPECT_TRUE(sig.slot_count() == 10);
        sig(sum);
        EXPECT_TRUE(sum == 10);
    }

    EXPECT_TRUE(sig.slot_count() == 0);
    sig(sum);
    EXPECT_TRUE(sum == 10);
}

template <typename T, template <typename...> class SIG_T> void test_observer_signals_vector()
{
    int sum = 0;
    SIG_T<int &> sig;

    {
        std::vector<std::unique_ptr<T>> v;
        for (auto i = 0; i < 10; ++i)
        {
            v.emplace_back(new T);
            sig.connect(&T::f1, v.back().get());
        }
        EXPECT_TRUE(sig.slot_count() == 10);
        sig(sum);
        EXPECT_TRUE(sum == 10);
    }

    EXPECT_TRUE(sig.slot_count() == 0);
    sig(sum);
    EXPECT_TRUE(sum == 10);
}
} // namespace
TEST(SignalObserverTest, test_observer)
{
    test_observer<so, signals::Signal>();
    test_observer<so_st, signals::SignalUnsafe>();
    test_observer_signals<so, signals::Signal>();
    test_observer_signals<so_st, signals::SignalUnsafe>();
    test_observer_signals_heap<so, signals::Signal>();
    test_observer_signals_heap<so_st, signals::SignalUnsafe>();
    test_observer_signals_shared<so, signals::Signal>();
    test_observer_signals_shared<so_st, signals::SignalUnsafe>();
    test_observer_signals_list<so, signals::Signal>();
    test_observer_signals_list<so_st, signals::SignalUnsafe>();
    test_observer_signals_vector<so, signals::Signal>();
    test_observer_signals_vector<so_st, signals::SignalUnsafe>();
}

namespace
{

static constexpr signals::GroupId grps = 30;
static constexpr int64_t slts = 3;
static constexpr int64_t emissions = 10000;
static constexpr int64_t runs = 1000;

static void fungs(int64_t &i) { i++; }
static void test_groups(int64_t &i)
{
    signals::Signal<int64_t &> sig;

    for (int64_t s = 0; s < slts; ++s)
    {
        for (signals::GroupId g = 0; g < grps; ++g)
        {
            sig.connect(fungs, grps - g);
        }
    }

    for (int64_t e = 0; e < emissions; ++e)
    {
        sig(i);
    }
}
} // namespace
TEST(SlotsBenchTest, test_groups)
{
    int64_t i = 0;

    for (int64_t r = 0; r < runs; ++r)
    {
        test_groups(i);
    }

    EXPECT_TRUE(i == grps * slts * emissions * runs);
}

namespace
{

void f1(int, char, float) { }
void f2(int, char, float) noexcept { }

void f(int) { }
void f(int, char, float) { }

struct oo
{
    void operator()(int) { }
    void operator()(int, char, float) { }
};

struct s
{
    static void s1(int, char, float) { }
    static void s2(int, char, float) noexcept { }

    void f1(int, char, float) { }
    void f2(int, char, float) const { }
    void f3(int, char, float) volatile { }
    void f4(int, char, float) const volatile { }
    void f5(int, char, float) noexcept { }
    void f6(int, char, float) const noexcept { }
    void f7(int, char, float) volatile noexcept { }
    void f8(int, char, float) const volatile noexcept { }
};

struct o1
{
    void operator()(int, char, float) { }
};
struct o2
{
    void operator()(int, char, float) const { }
};
struct o3
{
    void operator()(int, char, float) volatile { }
};
struct o4
{
    void operator()(int, char, float) const volatile { }
};
struct o5
{
    void operator()(int, char, float) noexcept { }
};
struct o6
{
    void operator()(int, char, float) const noexcept { }
};
struct o7
{
    void operator()(int, char, float) volatile noexcept { }
};
struct o8
{
    void operator()(int, char, float) const volatile noexcept { }
};
using tl = TypeList<int, char, float>;
} // namespace
TEST(IsCallableTest, Function)
{
    static_assert(signals::detail::is_callable_v<decltype(f1), tl>, "");
    static_assert(traits::is_callable_v<decltype(f2), tl>, "");
    static_assert(traits::is_callable_v<decltype(&s::s1), tl>, "");
    static_assert(traits::is_callable_v<decltype(&s::s2), tl>, "");
    static_assert(traits::is_callable_v<oo, tl>, "");
    static_assert(traits::is_callable_v<decltype(&s::f1), s *, tl>, "");
    static_assert(traits::is_callable_v<decltype(&s::f2), s *, tl>, "");
    static_assert(traits::is_callable_v<decltype(&s::f3), s *, tl>, "");
    static_assert(traits::is_callable_v<decltype(&s::f4), s *, tl>, "");
    static_assert(traits::is_callable_v<decltype(&s::f5), s *, tl>, "");
    static_assert(traits::is_callable_v<decltype(&s::f6), s *, tl>, "");
    static_assert(traits::is_callable_v<decltype(&s::f7), s *, tl>, "");
    static_assert(traits::is_callable_v<decltype(&s::f8), s *, tl>, "");
    static_assert(traits::is_callable_v<o1, tl>, "");
    static_assert(traits::is_callable_v<o2, tl>, "");
    static_assert(traits::is_callable_v<o3, tl>, "");
    static_assert(traits::is_callable_v<o4, tl>, "");
    static_assert(traits::is_callable_v<o5, tl>, "");
    static_assert(traits::is_callable_v<o6, tl>, "");
    static_assert(traits::is_callable_v<o7, tl>, "");
    static_assert(traits::is_callable_v<o8, tl>, "");
}

OCTK_END_NAMESPACE
