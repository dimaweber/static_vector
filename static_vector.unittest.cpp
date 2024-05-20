#include "static_vector.h"

#include <gtest/gtest.h>

#include <string>

using namespace std::literals;

TEST(StaticVector, DefaultConstructor)
{
    skl::static_vector<std::string, 4> st_vec;
    std::vector<std::string>           std_vec;

    EXPECT_EQ(st_vec.size( ), 0);
    EXPECT_EQ(std_vec.size( ), st_vec.size( ));

    EXPECT_EQ(st_vec.max_size( ), 4);

    EXPECT_EQ(st_vec.capacity( ), 4);

    EXPECT_EQ(st_vec.begin( ), st_vec.end( ));
    EXPECT_THROW(st_vec.at(0), std::out_of_range);
}

TEST(StaticVector, CountDefConstructor)
{
    skl::static_vector<std::string, 5> st_vec(3);

    EXPECT_EQ(st_vec.size( ), 3);

    EXPECT_EQ(st_vec.max_size( ), 5);

    EXPECT_NE(st_vec.begin( ), st_vec.end( ));
    EXPECT_EQ(st_vec.end( ), st_vec.begin( ) + 3);

    EXPECT_NO_THROW(EXPECT_EQ(st_vec.at(0), ""));

    EXPECT_NO_THROW(EXPECT_EQ(st_vec.at(2), ""));
    EXPECT_THROW(st_vec.at(3), std::out_of_range);
}

TEST(StaticVector, CountValueConstructor)
{
    skl::static_vector<std::string, 5> st_vec(3, "137");
    std::vector<std::string>           std_vec(3, "137");

    EXPECT_EQ(st_vec.size( ), 3);
    EXPECT_EQ(std_vec.size( ), st_vec.size( ));

    EXPECT_EQ(st_vec.max_size( ), 5);

    EXPECT_NE(st_vec.begin( ), st_vec.end( ));
    EXPECT_EQ(st_vec.end( ), st_vec.begin( ) + 3);

    EXPECT_NO_THROW(EXPECT_EQ(st_vec.at(0), "137"));
    EXPECT_EQ(std_vec.at(0), st_vec.at(0));

    EXPECT_NO_THROW(EXPECT_EQ(st_vec.at(2), "137"));
    EXPECT_THROW(st_vec.at(3), std::out_of_range);
}

TEST(StaticVector, InitializerListConstructor)
{
    skl::static_vector<std::string, 5> st_vec({"2", "5", "7", "9"});
    EXPECT_EQ(st_vec.size( ), 4);
    EXPECT_EQ(st_vec.max_size( ), 5);
    EXPECT_NE(st_vec.begin( ), st_vec.end( ));
    EXPECT_EQ(st_vec.end( ), st_vec.begin( ) + 4);
    EXPECT_NO_THROW(EXPECT_EQ(st_vec.at(0), "2"));
    EXPECT_NO_THROW(EXPECT_EQ(st_vec.at(2), "7"));
    EXPECT_THROW(st_vec.at(4), std::out_of_range);
}

TEST(StaticVector, CopyConstructor)
{
    skl::static_vector<std::string, 7> src {"uno", "dos", "tres", "cuatro", "cinco", "seis"};
    skl::static_vector<std::string, 7> dst {src};

    EXPECT_EQ(src.size( ), dst.size( ));
    EXPECT_EQ(src, dst);

    EXPECT_NE(src.cbegin( ), dst.cbegin( ));

    skl::static_vector<std::shared_ptr<int>, 7> shared_src {std::make_shared<int>(44), std::make_shared<int>(22)};
    std::shared_ptr<int>                        e_44 = shared_src[0];
    std::shared_ptr<int>                        e_22 = shared_src[1];

    EXPECT_EQ(e_22.use_count( ), 2);
    EXPECT_EQ(e_44.use_count( ), 2);

    skl::static_vector<std::shared_ptr<int>, 7> shared_dst {shared_src};

    EXPECT_EQ(e_44.use_count( ), 3);
    EXPECT_EQ(e_22.use_count( ), 3);
}

TEST(StaticVector, MoveConstructor)
{
    const std::initializer_list<std::string> ilist {"uno", "dos", "tres", "cuatro", "cinco", "seis"};
    skl::static_vector<std::string, 7>       src {ilist};
    skl::static_vector<std::string, 7>       dst {std::move(src)};

    EXPECT_EQ(dst.size( ), 6);
#if __cplusplus >= 202002L
    EXPECT_TRUE(std::ranges::equal(dst, ilist));
#else
    EXPECT_TRUE(std::equal(dst.begin( ), dst.end( ), ilist.begin( ), ilist.end( )));
#endif

    skl::static_vector<std::shared_ptr<int>, 7> shared_src {std::make_shared<int>(44), std::make_shared<int>(22)};
    std::shared_ptr<int>                        e_44 = shared_src[0];
    std::shared_ptr<int>                        e_22 = shared_src[1];

    EXPECT_EQ(e_22.use_count( ), 2);
    EXPECT_EQ(e_44.use_count( ), 2);

    skl::static_vector<std::shared_ptr<int>, 7> shared_dst {std::move(shared_src)};

    EXPECT_EQ(e_44.use_count( ), 2);
    EXPECT_EQ(e_22.use_count( ), 2);
}

TEST(StaticVector, At)
{
    skl::static_vector<std::string, 5> st_vec({"3", "9", "27", "81"});
    EXPECT_NO_THROW(EXPECT_EQ(st_vec.at(0), "3"));
    EXPECT_NO_THROW(EXPECT_EQ(st_vec.at(1), "9"));
    EXPECT_NO_THROW(EXPECT_EQ(st_vec.at(2), "27"));
    EXPECT_NO_THROW(EXPECT_EQ(st_vec.at(3), "81"));
    EXPECT_THROW(st_vec.at(4), std::out_of_range);
    EXPECT_THROW(st_vec.at(5), std::out_of_range);
    EXPECT_THROW(st_vec.at(-1), std::out_of_range);

    EXPECT_NO_THROW(st_vec.at(0) += "44");
    EXPECT_EQ(st_vec.at(0), "344");
}

TEST(StaticVector, OperatorAssignCopy)
{
    skl::static_vector<std::string, 7> dst {"uno", "dos", "tres", "cuatro", "cinco", "seis"};
    skl::static_vector<std::string, 7> src {"one", "two", "three"};

    dst = src;

    EXPECT_EQ(dst.size( ), src.size( ));
#if __cplusplus >= 202002L
    EXPECT_TRUE(std::ranges::equal(src, dst));
#else
    EXPECT_TRUE(std::equal(src.begin( ), src.end( ), dst.begin( ), dst.end( )));
#endif

    EXPECT_NE(src.cbegin( ), dst.cbegin( ));

    skl::static_vector<std::shared_ptr<int>, 7> shared_dst {std::make_shared<int>(44)};
    skl::static_vector<std::shared_ptr<int>, 7> shared_src {std::make_shared<int>(22)};
    std::shared_ptr<int>                        e_44 = shared_dst[0];
    std::shared_ptr<int>                        e_22 = shared_src[0];

    EXPECT_EQ(e_22.use_count( ), 2);
    EXPECT_EQ(e_44.use_count( ), 2);

    shared_dst = shared_src;

    EXPECT_EQ(e_44.use_count( ), 1);
    EXPECT_EQ(e_22.use_count( ), 3);
}

TEST(StaticVector, OperatorAssignMove)
{
    const auto                         ilist = std::initializer_list<std::string> {"one", "two", "three"};
    skl::static_vector<std::string, 7> dst {"uno", "dos", "tres", "cuatro", "cinco", "seis"};
    skl::static_vector<std::string, 7> src {ilist};

    dst = std::move(src);

    EXPECT_EQ(dst.size( ), 3);
#if __cplusplus >= 202002L
    EXPECT_TRUE(std::ranges::equal(dst, ilist));
#else
    EXPECT_TRUE(std::equal(dst.begin( ), dst.end( ), ilist.begin( ), ilist.end( )));
#endif

    skl::static_vector<std::shared_ptr<int>, 7> shared_dst {std::make_shared<int>(44)};
    skl::static_vector<std::shared_ptr<int>, 7> shared_src {std::make_shared<int>(22)};
    std::shared_ptr<int>                        e_44 = shared_dst[0];
    std::shared_ptr<int>                        e_22 = shared_src[0];

    EXPECT_EQ(e_22.use_count( ), 2);
    EXPECT_EQ(e_44.use_count( ), 2);

    shared_dst = std::move(shared_src);

    EXPECT_EQ(e_44.use_count( ), 1);
    EXPECT_EQ(e_22.use_count( ), 2);
}

TEST(StaticVector, OperatorAssignIList)
{
    const std::initializer_list<std::string> ilist {"one", "two", "three"};
    skl::static_vector<std::string, 7>       dst {"uno", "dos", "tres", "cuatro", "cinco", "seis"};

    dst = ilist;

    EXPECT_EQ(dst.size( ), ilist.size( ));
#if __cplusplus >= 202002L
    EXPECT_TRUE(std::ranges::equal(dst, ilist));
#else
    EXPECT_TRUE(std::equal(dst.begin( ), dst.end( ), ilist.begin( ), ilist.end( )));
#endif
}

TEST(StaticVector, SelfAssign)
{
    const std::initializer_list<std::string> ilist {"2", "3", "5", "7", "11"};
    skl::static_vector<std::string, 5>       st_vec {ilist};

#if defined(__GNUC__)
    #if __GNUC_PREREQ(13, 0)
    GTEST_SKIP_("gcc 13 don't allow self assign so no check needed");
    #else
    st_vec = st_vec;
    EXPECT_EQ(st_vec.size( ), 5);
        #if __cplusplus >= 202002L
    EXPECT_TRUE(std::ranges::equal(st_vec, ilist));
        #else
    EXPECT_TRUE(std::equal(st_vec.begin( ), st_vec.end( ), ilist.begin( ), ilist.end( )));
        #endif

    st_vec = std::move(st_vec);
    EXPECT_EQ(st_vec.size( ), 5);
        #if __cplusplus >= 202002L
    EXPECT_TRUE(std::ranges::equal(st_vec, ilist));
        #else
    EXPECT_TRUE(std::equal(st_vec.begin( ), st_vec.end( ), ilist.begin( ), ilist.end( )));
        #endif
    #endif
#endif
}

TEST(StaticVector, AssignCountValue)
{
    const auto                         ilist = std::initializer_list<std::string> {"bee", "bee", "bee", "bee"};
    skl::static_vector<std::string, 5> st_vec {"2", "3", "5", "7", "11"};

    st_vec.assign(4, "bee");

    EXPECT_EQ(st_vec.size( ), 4);
#if __cplusplus >= 202002L
    EXPECT_TRUE(std::ranges::equal(st_vec, ilist));
#else
    EXPECT_TRUE(std::equal(st_vec.begin( ), st_vec.end( ), ilist.begin( ), ilist.end( )));
#endif
}

TEST(StaticVector, AssignIterator)
{
    skl::static_vector<std::string, 7> dst {"one", "two", "three"};
    skl::static_vector<std::string, 7> src {"uno", "dos", "tres", "cuatro", "cinco", "seis"};

    dst.assign(src.cbegin( ) + 2, src.cend( ) - 2);

    EXPECT_EQ(dst.size( ), 2);
    EXPECT_EQ(dst[0], src[2]);
    EXPECT_EQ(dst[1], src[3]);
    EXPECT_NE(dst.cbegin( ), src.cbegin( ));
}

TEST(StaticVector, AssignIList)
{
    const std::initializer_list<std::string> ilist {"one", "two", "three"};
    skl::static_vector<std::string, 7>       dst {"uno", "dos", "tres", "cuatro", "cinco", "seis"};

    dst.assign(ilist);

    EXPECT_EQ(dst.size( ), ilist.size( ));
#if __cplusplus >= 202002L
    EXPECT_TRUE(std::ranges::equal(dst, ilist));
#else
    EXPECT_TRUE(std::equal(dst.begin( ), dst.end( ), ilist.begin( ), ilist.end( )));
#endif
}

TEST(StaticVector, OperatorSqBraces)
{
    [[maybe_unused]] std::array<int, 16> prefix_seg_fault_guard;
    skl::static_vector<std::string, 5>   st_vec({"1", "2", "3", "5"});
    [[maybe_unused]] std::array<int, 16> suffix_seg_fault_guard;

    EXPECT_NO_THROW(EXPECT_EQ(st_vec[0], "1"));
    EXPECT_NO_THROW(EXPECT_EQ(st_vec[1], "2"));
    EXPECT_NO_THROW(EXPECT_EQ(st_vec[2], "3"));
    EXPECT_NO_THROW(EXPECT_EQ(st_vec[3], "5"));
#if SKL_STATIC_VECTOR_DO_RANGE_CHECKS
    EXPECT_DEATH((void)st_vec[4], "");
    EXPECT_DEATH((void)st_vec[5], "");
    EXPECT_DEATH((void)st_vec[-1], "");
#else
    EXPECT_NO_THROW((void)st_vec[4]);
    EXPECT_NO_THROW((void)st_vec[5]);
    EXPECT_NO_THROW((void)st_vec[-1]);
#endif

    st_vec[0] = "-20";
    EXPECT_EQ(st_vec[0], "-20");
}

TEST(StaticVector, AtAndSqBracesAccessSameElements)
{
    skl::static_vector<int, 5> st_vec({2, 4, 8, 16, 32});
    for ( decltype(st_vec)::size_type idx = 0; idx < st_vec.size( ); idx++ )
        EXPECT_EQ(st_vec.at(idx), st_vec[idx]);

    for ( decltype(st_vec)::size_type idx = 0; idx < st_vec.size( ); idx++ )
        st_vec.at(idx) /= 2;
    for ( decltype(st_vec)::size_type idx = 0; idx < st_vec.size( ); idx++ )
        EXPECT_EQ(st_vec[idx], 1 << idx);

    for ( decltype(st_vec)::size_type idx = 0; idx < st_vec.size( ); idx++ )
        st_vec[idx] *= 2;
    for ( decltype(st_vec)::size_type idx = 0; idx < st_vec.size( ); idx++ )
        EXPECT_EQ(st_vec.at(idx), 1 << (idx + 1));
}

TEST(StaticVector, Front)
{
    skl::static_vector<std::string, 5> st_vec {"2", "3", "5", "7", "11"};
    EXPECT_EQ(st_vec.front( ), st_vec.at(0));

    st_vec.front( ) = "33";
    EXPECT_EQ(st_vec[0], "33");

    EXPECT_EQ(st_vec.front( ), *st_vec.begin( ));
}

TEST(StaticVector, Back)
{
    skl::static_vector<std::string, 5> st_vec {"1", "4", "1", "5"};
    EXPECT_EQ(st_vec.back( ), st_vec.at(3));

    st_vec.back( ) = "33";
    EXPECT_EQ(st_vec[3], "33");

    EXPECT_EQ(st_vec.back( ), *(st_vec.end( ) - 1));
}

TEST(StaticVector, Data)
{
    skl::static_vector<std::string, 5> st_vec {"1", "4", "1", "5"};
    EXPECT_EQ(st_vec.data( ), &st_vec[0]);

    *st_vec.data( ) = "33";
    EXPECT_EQ(st_vec.front( ), "33");
}

TEST(StaticVector, Iterators)
{
    skl::static_vector<std::string, 5> st_vec {"4", "16", "64", "256", "1024"};

    auto b_iter = st_vec.begin( );
    EXPECT_EQ(*b_iter, st_vec.at(0));
    *b_iter = "5";
    EXPECT_EQ(st_vec.at(0), "5");

    b_iter++;
    EXPECT_EQ(*b_iter, st_vec.at(1));
    *b_iter = "25";
    EXPECT_EQ(st_vec.at(1), "25");

    auto e_iter = st_vec.end( );
    EXPECT_EQ(*(e_iter - 1), "1024");

    auto rb_iter = st_vec.rbegin( );
    EXPECT_EQ(*rb_iter, st_vec.at(4));

    auto re_iter = st_vec.rend( );
    EXPECT_EQ(*(re_iter - 1), st_vec.at(0));
}

TEST(StaticVector, ReverseIterators)
{
    std::initializer_list<std::string> ilist {"uno", "dos", "tres", "cuatro", "cinco", "seis"};
    skl::static_vector<std::string, 7> st_vec {ilist};
    std::vector<std::string>           std_vec {ilist};

    EXPECT_TRUE(std::equal(st_vec.crbegin( ), st_vec.crend( ), std_vec.crbegin( ), std_vec.crend( )));

    EXPECT_EQ(*st_vec.rbegin( ), "seis");
    EXPECT_EQ(*(st_vec.rend( ) - 1), "uno");
}

TEST(StaticVector, StdAlgoCompatibleGenerateFill)
{
    skl::static_vector<int, 5> st_vec;
    EXPECT_TRUE(st_vec.empty( ));

    std::generate_n(std::back_inserter(st_vec), 4, [] { return 34; });
    EXPECT_FALSE(st_vec.empty( ));
    EXPECT_EQ(st_vec.size( ), 4);
    EXPECT_NO_THROW(EXPECT_EQ(st_vec.at(0), 34));
    EXPECT_NO_THROW(EXPECT_EQ(st_vec.at(3), 34));
    EXPECT_THROW(st_vec.at(4), std::out_of_range);

    std::fill(st_vec.begin( ), st_vec.end( ), 17);
    EXPECT_FALSE(st_vec.empty( ));
    EXPECT_EQ(st_vec.size( ), 4);
    EXPECT_NO_THROW(EXPECT_EQ(st_vec.at(0), 17));
    EXPECT_NO_THROW(EXPECT_EQ(st_vec.at(3), 17));
    EXPECT_THROW(st_vec.at(4), std::out_of_range);

    for ( int a: st_vec )
        EXPECT_EQ(a, 17);
}

TEST(StaticVector, StdAlgoCompatibleSortUniqueIsSorted)
{
    skl::static_vector<int, 20> st_vec {1, 2, 1, 1, 3, 3, 3, 4, 5, 4};
    EXPECT_EQ(st_vec.size( ), 10);

    EXPECT_FALSE(std::is_sorted(st_vec.cbegin( ), st_vec.cend( )));

    auto last = std::unique(st_vec.begin( ), st_vec.end( ));
    st_vec.erase(last, st_vec.end( ));
    EXPECT_EQ(st_vec.size( ), 7);
    EXPECT_FALSE(std::is_sorted(st_vec.cbegin( ), st_vec.cend( )));

    std::sort(st_vec.begin( ), st_vec.end( ));
    EXPECT_EQ(st_vec.size( ), 7);
    EXPECT_TRUE(std::is_sorted(st_vec.begin( ), st_vec.end( )));

    last = std::unique(st_vec.begin( ), st_vec.end( ));
    st_vec.erase(last, st_vec.end( ));
    EXPECT_EQ(st_vec.size( ), 5);
    EXPECT_TRUE(std::is_sorted(st_vec.begin( ), st_vec.end( )));
}

struct A {
    int val {0};

    A( ) { d_constructorCallCount++; }

    A(int a) : val {a} { ++i_constructorCallCount; }

    A(const A& a) : val {a.val} { c_constructorCallCount++; }

    A(A&& a)
    {
        std::swap(val, a.val);
        a.val = 0xfafbfcfd;
        m_constructorCallCount++;
    }

    ~A( ) { ++destructorCallCount; }

    A& operator= (const A& a)
    {
        val = a.val;
        c_assignCallCount++;
        return *this;
    }
#if __cplusplus >= 202002L
    std::strong_ordering operator<=> (const A& a) const { return val <=> a.val; }

    std::strong_ordering operator<=> (int a) const { return val <=> a; }
#endif

    A& operator= (A&& a) noexcept
    {
        std::swap(val, a.val);
        a.val = 0xcacbcccd;
        m_assignCallCount++;
        return *this;
    }

    static void reset_cpp_statistics ( )
    {
        destructorCallCount    = 0;
        d_constructorCallCount = 0;
        i_constructorCallCount = 0;
        c_assignCallCount      = 0;
        c_constructorCallCount = 0;
        m_assignCallCount      = 0;
        m_constructorCallCount = 0;
    }

    inline static int destructorCallCount {0};
    inline static int d_constructorCallCount {0};
    inline static int i_constructorCallCount {0};
    inline static int c_constructorCallCount {0};
    inline static int m_constructorCallCount {0};
    inline static int c_assignCallCount {0};
    inline static int m_assignCallCount {0};
};

TEST(StaticVector, Clear)
{
    skl::static_vector<A, 5> st_vec;
    std::generate_n(std::back_inserter(st_vec), st_vec.max_size( ), [] { return A {44}; });

    A::reset_cpp_statistics( );
    EXPECT_EQ(st_vec.size( ), 5);
    st_vec.clear( );
    EXPECT_EQ(st_vec.size( ), 0);

    EXPECT_EQ(A::m_assignCallCount, 0);
    EXPECT_EQ(A::c_assignCallCount, 0);
    EXPECT_EQ(A::m_constructorCallCount, 0);
    EXPECT_EQ(A::c_constructorCallCount, 0);
    EXPECT_EQ(A::destructorCallCount, 5);
}

TEST(StaticVector, ClearScalar)
{
    skl::static_vector<int, 5> st_vec {2, 34, 5, 3, 33};
    EXPECT_EQ(st_vec.size( ), 5);
    const auto si = st_vec.cbegin( );
    st_vec.clear( );

    EXPECT_EQ(st_vec.size( ), 0);
    EXPECT_EQ(st_vec.cbegin( ), si);
    EXPECT_EQ(st_vec.cend( ), st_vec.cbegin( ));
}

TEST(StaticVector, ClearObject)
{
    skl::static_vector<std::string, 5> st_vec {"2", "34", "5", "3", "33"};
    EXPECT_EQ(st_vec.size( ), 5);
    const auto si = st_vec.cbegin( );
    st_vec.clear( );

    EXPECT_EQ(st_vec.size( ), 0);
    EXPECT_EQ(st_vec.cbegin( ), si);
    EXPECT_EQ(st_vec.cend( ), st_vec.cbegin( ));
}

TEST(StaticVector, EraseMoveCorrectness)
{
    skl::static_vector<A, 8> st_vec {3, 45, 55, 2, 3, 37, 28, 12};
    EXPECT_EQ(st_vec.size( ), 8);

    A::reset_cpp_statistics( );

    st_vec.pop_back( );
    // {3, 45, 55, 2, 3, 37, 28}
    EXPECT_EQ(st_vec.size( ), 7);
    EXPECT_EQ(st_vec.back( ).val, 28);

    auto iter = st_vec.erase(st_vec.begin( ));
    // {45, 55, 2, 3, 37, 28}
    EXPECT_EQ(iter->val, 45);
    EXPECT_EQ(iter, st_vec.begin( ));
    EXPECT_EQ(st_vec.size( ), 6);
    EXPECT_EQ(st_vec.front( ).val, 45);
    EXPECT_EQ(st_vec.back( ).val, 28);

    iter = st_vec.erase(st_vec.end( ) - 1);
    // {45, 55, 2, 3, 37}
    EXPECT_EQ(iter, st_vec.end( ));
    EXPECT_EQ(st_vec.size( ), 5);
    EXPECT_EQ(st_vec.front( ).val, 45);
    EXPECT_EQ(st_vec.back( ).val, 37);

    iter = st_vec.erase(st_vec.begin( ) + 1, st_vec.end( ) - 1);
    // {45, 37}
    EXPECT_EQ(iter, st_vec.end( ) - 1);
    EXPECT_EQ(st_vec.size( ), 2);
    EXPECT_EQ(st_vec.front( ).val, 45);
    EXPECT_EQ(st_vec.back( ).val, 37);

    EXPECT_EQ(A::d_constructorCallCount, 0);
    EXPECT_EQ(A::m_assignCallCount, 0);
    EXPECT_EQ(A::c_assignCallCount, 0);
    EXPECT_EQ(A::m_constructorCallCount, 7);
    EXPECT_EQ(A::c_constructorCallCount, 0);
    EXPECT_EQ(A::destructorCallCount, 6);
}

TEST(StaticVector, InsertMoveCorrectness)
{
    skl::static_vector<A, 8> st_vec {3};
    EXPECT_EQ(st_vec.size( ), 1);

    A::reset_cpp_statistics( );

    A a1 {45};
    st_vec.push_back(a1);
    // {3, 45}
    EXPECT_EQ(st_vec.size( ), 2);
    EXPECT_EQ(st_vec.back( ).val, 45);
    EXPECT_EQ(A::d_constructorCallCount, 0);
    EXPECT_EQ(A::m_assignCallCount, 0);
    EXPECT_EQ(A::c_assignCallCount, 0);
    EXPECT_EQ(A::m_constructorCallCount, 0);
    EXPECT_EQ(A::c_constructorCallCount, 1);
    EXPECT_EQ(A::destructorCallCount, 0);

    st_vec.push_back(A {28});
    // {3, 45, 28}
    EXPECT_EQ(st_vec.size( ), 3);
    EXPECT_EQ(st_vec.front( ).val, 3);
    EXPECT_EQ(st_vec.back( ).val, 28);
    EXPECT_EQ(A::d_constructorCallCount, 0);
    EXPECT_EQ(A::m_assignCallCount, 0);
    EXPECT_EQ(A::c_assignCallCount, 0);
    EXPECT_EQ(A::m_constructorCallCount, 1);
    EXPECT_EQ(A::c_constructorCallCount, 1);
    EXPECT_EQ(A::destructorCallCount, 1);

    auto iter = st_vec.insert(st_vec.end( ) - 1, 55);
    // {3, 45, 55, 28}
    EXPECT_EQ(iter, st_vec.end( ) - 2);
    EXPECT_EQ(st_vec.size( ), 4);
    EXPECT_EQ(A::d_constructorCallCount, 0);
    EXPECT_EQ(A::m_assignCallCount, 0);
    EXPECT_EQ(A::c_assignCallCount, 0);
    EXPECT_EQ(A::m_constructorCallCount, 3);
    EXPECT_EQ(A::c_constructorCallCount, 1);
    EXPECT_EQ(A::destructorCallCount, 2);

    iter = st_vec.insert(st_vec.begin( ) + 1, {26, 18, 77, 13});
    // {3,26,18,77,13,45,55,28}
    EXPECT_EQ(iter, st_vec.begin( ) + 1);
    EXPECT_EQ(st_vec.size( ), 8);
    EXPECT_EQ(A::d_constructorCallCount, 0);
    EXPECT_EQ(A::m_assignCallCount, 0);
    EXPECT_EQ(A::c_assignCallCount, 0);
    EXPECT_EQ(A::m_constructorCallCount, 6);
    EXPECT_EQ(A::c_constructorCallCount, 5);
    EXPECT_EQ(A::destructorCallCount, 6);
}

TEST(StaticVector, InsertObjectSingleInFront_rvalue)
{
    skl::static_vector<std::string, 8> st_vec {"3", "45", "55", "2"};
    EXPECT_EQ(st_vec.size( ), 4);
    const auto sb = st_vec.cbegin( );
    const auto se = st_vec.cend( );

    auto iter = st_vec.insert(st_vec.begin( ), "29");
    EXPECT_EQ(st_vec.cbegin( ), sb);
    EXPECT_EQ(st_vec.cend( ), se + 1);
    EXPECT_EQ(st_vec.size( ), 5);
    EXPECT_EQ(iter, st_vec.cbegin( ));
    EXPECT_EQ(st_vec.front( ), "29");

    const auto list = std::initializer_list<std::string> {"29", "3", "45", "55", "2"};
#if __cplusplus >= 202002L
    EXPECT_TRUE(std::ranges::equal(st_vec, list));
#else
    EXPECT_TRUE(std::equal(st_vec.begin( ), st_vec.end( ), list.begin( ), list.end( )));
#endif
}

TEST(StaticVector, InsertObjectSingleInFront_ref)
{
    skl::static_vector<std::string, 8> st_vec {"3", "45", "55", "2"};
    EXPECT_EQ(st_vec.size( ), 4);
    const auto sb = st_vec.cbegin( );
    const auto se = st_vec.cend( );

    const std::string value {"29"};
    auto              iter = st_vec.insert(st_vec.begin( ), value);
    EXPECT_EQ(st_vec.cbegin( ), sb);
    EXPECT_EQ(st_vec.cend( ), se + 1);
    EXPECT_EQ(st_vec.size( ), 5);
    EXPECT_EQ(iter, st_vec.cbegin( ));
    EXPECT_EQ(st_vec.front( ), value);

    auto list = std::initializer_list<std::string> {value, "3", "45", "55", "2"};
#if __cplusplus >= 202002L
    EXPECT_TRUE(std::ranges::equal(st_vec, list));
#else
    EXPECT_TRUE(std::equal(st_vec.begin( ), st_vec.end( ), list.begin( ), list.end( )));
#endif
}

TEST(StaticVector, InsertScalarSingleInFront_rvalue)
{
    skl::static_vector<int, 8> st_vec {3, 45, 55, 2};
    EXPECT_EQ(st_vec.size( ), 4);
    const auto sb = st_vec.cbegin( );
    const auto se = st_vec.cend( );

    auto iter = st_vec.insert(st_vec.begin( ), 29);
    EXPECT_EQ(st_vec.cbegin( ), sb);
    EXPECT_EQ(st_vec.cend( ), se + 1);
    EXPECT_EQ(st_vec.size( ), 5);
    EXPECT_EQ(iter, st_vec.cbegin( ));
    EXPECT_EQ(st_vec.front( ), 29);

    auto list = std::initializer_list<int> {29, 3, 45, 55, 2};
#if __cplusplus >= 202002L
    EXPECT_TRUE(std::ranges::equal(st_vec, list));
#else
    EXPECT_TRUE(std::equal(st_vec.begin( ), st_vec.end( ), list.begin( ), list.end( )));
#endif
}

TEST(StaticVector, InsertScalarSingleInFront_ref)
{
    skl::static_vector<int, 8> st_vec {3, 45, 55, 2};
    EXPECT_EQ(st_vec.size( ), 4);
    const auto sb = st_vec.cbegin( );
    const auto se = st_vec.cend( );

    const int value {29};
    auto      iter = st_vec.insert(st_vec.begin( ), value);
    EXPECT_EQ(st_vec.cbegin( ), sb);
    EXPECT_EQ(st_vec.cend( ), se + 1);
    EXPECT_EQ(st_vec.size( ), 5);
    EXPECT_EQ(iter, st_vec.cbegin( ));
    EXPECT_EQ(st_vec.front( ), value);

    auto list = std::initializer_list<int> {value, 3, 45, 55, 2};
#if __cplusplus >= 202002L
    EXPECT_TRUE(std::ranges::equal(st_vec, list));
#else
    EXPECT_TRUE(std::equal(st_vec.begin( ), st_vec.end( ), list.begin( ), list.end( )));
#endif
}

TEST(StaticVector, InsertObjectSingleInEnd)
{
    skl::static_vector<std::string, 8> st_vec {"29", "3", "45", "55", "2"};
    EXPECT_EQ(st_vec.size( ), 5);
    const auto sb = st_vec.cbegin( );
    const auto se = st_vec.cend( );

    auto iter = st_vec.insert(st_vec.end( ), "51");
    EXPECT_EQ(st_vec.cbegin( ), sb);
    EXPECT_EQ(st_vec.cend( ), se + 1);
    EXPECT_EQ(st_vec.size( ), 6);
    EXPECT_EQ(st_vec.back( ), "51");
    EXPECT_EQ(iter, st_vec.end( ) - 1);

    auto list = std::initializer_list<std::string> {"29", "3", "45", "55", "2", "51"};
#if __cplusplus >= 202002L
    EXPECT_TRUE(std::ranges::equal(st_vec, list));
#else
    EXPECT_TRUE(std::equal(st_vec.begin( ), st_vec.end( ), list.begin( ), list.end( )));
#endif
}

TEST(StaticVector, InsertScalarSingleInEnd)
{
    skl::static_vector<int, 8> st_vec {29, 3, 45, 55, 2};
    EXPECT_EQ(st_vec.size( ), 5);
    const auto sb = st_vec.cbegin( );
    const auto se = st_vec.cend( );

    auto iter = st_vec.insert(st_vec.end( ), 51);
    EXPECT_EQ(st_vec.cbegin( ), sb);
    EXPECT_EQ(st_vec.cend( ), se + 1);
    EXPECT_EQ(st_vec.size( ), 6);
    EXPECT_EQ(st_vec.back( ), 51);
    EXPECT_EQ(iter, st_vec.end( ) - 1);

    auto list = std::initializer_list<int> {29, 3, 45, 55, 2, 51};
#if __cplusplus >= 202002L
    EXPECT_TRUE(std::ranges::equal(st_vec, list));
#else
    EXPECT_TRUE(std::equal(st_vec.begin( ), st_vec.end( ), list.begin( ), list.end( )));
#endif
}

TEST(StaticVector, InsertObjectSingleAtMiddle)
{
    A::destructorCallCount = 0;
    skl::static_vector<std::string, 8> st_vec {"29", "3", "45", "55", "2", "51"};
    EXPECT_EQ(st_vec.size( ), 6);
    const auto sb = st_vec.cbegin( );
    const auto se = st_vec.cend( );

    auto iter = st_vec.insert(st_vec.begin( ) + 2, "67");
    EXPECT_EQ(st_vec.cbegin( ), sb);
    EXPECT_EQ(st_vec.cend( ), se + 1);
    EXPECT_EQ(st_vec.size( ), 7);
    EXPECT_EQ(st_vec.at(2), "67");
    EXPECT_EQ(*iter, "67");

    auto list = std::initializer_list<std::string> {"29", "3", "67", "45", "55", "2", "51"};
#if __cplusplus >= 202002L
    EXPECT_TRUE(std::ranges::equal(st_vec, list));
#else
    EXPECT_TRUE(std::equal(st_vec.begin( ), st_vec.end( ), list.begin( ), list.end( )));
#endif
}

TEST(StaticVector, InsertScalarSingleAtMiddle)
{
    A::destructorCallCount = 0;
    skl::static_vector<int, 8> st_vec {29, 3, 45, 55, 2, 51};
    EXPECT_EQ(st_vec.size( ), 6);
    const auto sb = st_vec.cbegin( );
    const auto se = st_vec.cend( );

    auto iter = st_vec.insert(st_vec.begin( ) + 2, 67);
    EXPECT_EQ(st_vec.cbegin( ), sb);
    EXPECT_EQ(st_vec.cend( ), se + 1);
    EXPECT_EQ(st_vec.size( ), 7);
    EXPECT_EQ(st_vec.at(2), 67);
    EXPECT_EQ(*iter, 67);

    auto list = std::initializer_list<int> {29, 3, 67, 45, 55, 2, 51};
#if __cplusplus >= 202002L
    EXPECT_TRUE(std::ranges::equal(st_vec, list));
#else
    EXPECT_TRUE(std::equal(st_vec.begin( ), st_vec.end( ), list.begin( ), list.end( )));
#endif
}

TEST(StaticVector, InsertObjectCountInFront)
{
    skl::static_vector<std::string, 8> st_vec {"3", "45", "55", "2"};
    EXPECT_EQ(st_vec.size( ), 4);

    auto iter = st_vec.insert(st_vec.begin( ), 0, "27");
    EXPECT_EQ(st_vec.size( ), 4);
    EXPECT_EQ(iter, st_vec.begin( ));
    EXPECT_EQ(*iter, "3");
    const std::initializer_list<std::string>& list = std::initializer_list<std::string> {"3", "45", "55", "2"};
#if __cplusplus >= 202002L
    EXPECT_TRUE(std::ranges::equal(st_vec, list));
#else
    EXPECT_TRUE(std::equal(st_vec.begin( ), st_vec.end( ), list.begin( ), list.end( )));
#endif

    iter = st_vec.insert(st_vec.begin( ), 1, "29");
    EXPECT_EQ(st_vec.size( ), 5);
    EXPECT_EQ(iter, st_vec.begin( ));
    EXPECT_EQ(*iter, "29");
    EXPECT_EQ(st_vec[1], "3");
    auto list1 = std::initializer_list<std::string> {"29", "3", "45", "55", "2"};
#if __cplusplus >= 202002L
    EXPECT_TRUE(std::ranges::equal(st_vec, list1));
#else
    EXPECT_TRUE(std::equal(st_vec.begin( ), st_vec.end( ), list1.begin( ), list1.end( )));
#endif

    iter = st_vec.insert(st_vec.begin( ), 2, "31");
    EXPECT_EQ(st_vec.size( ), 7);
    EXPECT_EQ(iter, st_vec.begin( ));
    EXPECT_EQ(*iter, "31");
    EXPECT_EQ(st_vec[1], "31");
    EXPECT_EQ(st_vec[2], "29");
    auto list2 = std::initializer_list<std::string> {"31", "31", "29", "3", "45", "55", "2"};
#if __cplusplus >= 202002L
    EXPECT_TRUE(std::ranges::equal(st_vec, list2));
#else
    EXPECT_TRUE(std::equal(st_vec.begin( ), st_vec.end( ), list2.begin( ), list2.end( )));
#endif
}

TEST(StaticVector, InsertScalarCountInFront)
{
    skl::static_vector<int, 8> st_vec {3, 45, 55, 2};
    EXPECT_EQ(st_vec.size( ), 4);

    auto iter = st_vec.insert(st_vec.begin( ), 0L, 27);
    EXPECT_EQ(st_vec.size( ), 4);
    EXPECT_EQ(iter, st_vec.begin( ));
    EXPECT_EQ(*iter, 3);
    auto list = std::initializer_list<int> {3, 45, 55, 2};
#if __cplusplus >= 202002L
    EXPECT_TRUE(std::ranges::equal(st_vec, list));
#else
    EXPECT_TRUE(std::equal(st_vec.begin( ), st_vec.end( ), list.begin( ), list.end( )));
#endif

    iter = st_vec.insert(st_vec.begin( ), 1L, 29);
    EXPECT_EQ(st_vec.size( ), 5);
    EXPECT_EQ(iter, st_vec.begin( ));
    EXPECT_EQ(*iter, 29);
    EXPECT_EQ(st_vec[1], 3);
    auto list1 = std::initializer_list<int> {29, 3, 45, 55, 2};
#if __cplusplus >= 202002L
    EXPECT_TRUE(std::ranges::equal(st_vec, list1));
#else
    EXPECT_TRUE(std::equal(st_vec.begin( ), st_vec.end( ), list1.begin( ), list1.end( )));
#endif

    iter = st_vec.insert(st_vec.begin( ), 2L, 31);
    EXPECT_EQ(st_vec.size( ), 7);
    EXPECT_EQ(iter, st_vec.begin( ));
    EXPECT_EQ(*iter, 31);
    EXPECT_EQ(st_vec[1], 31);
    EXPECT_EQ(st_vec[2], 29);
    auto list2 = std::initializer_list<int> {31, 31, 29, 3, 45, 55, 2};
#if __cplusplus >= 202002L
    EXPECT_TRUE(std::ranges::equal(st_vec, list2));
#else
    EXPECT_TRUE(std::equal(st_vec.begin( ), st_vec.end( ), list2.begin( ), list2.end( )));
#endif
}

TEST(StaticVector, InsertObjectCountInMiddle)
{
    skl::static_vector<std::string, 8> st_vec {"3", "45", "55", "2"};
    EXPECT_EQ(st_vec.size( ), 4);

    auto iter = st_vec.insert(st_vec.begin( ) + 2, 0, "27");
    // {3, 45, 55, 2}
    EXPECT_EQ(st_vec.size( ), 4);
    EXPECT_EQ(iter, st_vec.begin( ) + 2);
    EXPECT_EQ(*iter, "55");

    iter = st_vec.insert(st_vec.begin( ) + 2, 1, "29");
    // {3, 45, 29, 55, 2}
    EXPECT_EQ(st_vec.size( ), 5);
    EXPECT_EQ(iter, st_vec.begin( ) + 2);
    EXPECT_EQ(*iter, "29");
    EXPECT_EQ(st_vec[3], "55");

    iter = st_vec.insert(st_vec.begin( ) + 4, 2, "31");
    // {3, 45, 29, 55, 31, 31, 2}
    EXPECT_EQ(st_vec.size( ), 7);
    EXPECT_EQ(iter, st_vec.begin( ) + 4);
    EXPECT_EQ(*iter, "31");
    EXPECT_EQ(st_vec[5], "31");
    EXPECT_EQ(st_vec[6], "2");
}

TEST(StaticVector, InsertScalarCountInMiddle)
{
    skl::static_vector<int, 8> st_vec {3, 45, 55, 2};
    EXPECT_EQ(st_vec.size( ), 4);

    auto iter = st_vec.insert(st_vec.begin( ) + 2, 0, 27l);
    // {3, 45, 55, 2}
    EXPECT_EQ(st_vec.size( ), 4);
    EXPECT_EQ(iter, st_vec.begin( ) + 2);
    EXPECT_EQ(*iter, 55);

    iter = st_vec.insert(st_vec.begin( ) + 2, 1l, 29);
    // {3, 45, 29, 55, 2}
    EXPECT_EQ(st_vec.size( ), 5);
    EXPECT_EQ(iter, st_vec.begin( ) + 2);
    EXPECT_EQ(*iter, 29);
    EXPECT_EQ(st_vec[3], 55);

    iter = st_vec.insert(st_vec.begin( ) + 4, 2, 31l);
    // {3, 45, 29, 55, 31, 31, 2}
    EXPECT_EQ(st_vec.size( ), 7);
    EXPECT_EQ(iter, st_vec.begin( ) + 4);
    EXPECT_EQ(*iter, 31);
    EXPECT_EQ(st_vec[5], 31);
    EXPECT_EQ(st_vec[6], 2);
}

TEST(StaticVector, InsertCountAtEnd)
{
    skl::static_vector<std::string, 8> st_vec {"3", "45", "55", "2"};
    EXPECT_EQ(st_vec.size( ), 4);

    auto iter = st_vec.insert(st_vec.end( ), 0, "27");
    // {3, 45, 55, 2}
    EXPECT_EQ(st_vec.size( ), 4);
    EXPECT_EQ(iter, st_vec.end( ));
    EXPECT_EQ(st_vec.back( ), "2");

    iter = st_vec.insert(st_vec.end( ), 1, "29");
    // {3, 45, 55, 2, 29}
    EXPECT_EQ(st_vec.size( ), 5);
    EXPECT_EQ(iter, st_vec.end( ) - 1);
    EXPECT_EQ(*iter, "29");
    EXPECT_EQ(*(st_vec.end( ) - 2), "2");

    iter = st_vec.insert(st_vec.end( ), 2, "31");
    // {3, 45, 55, 2, 29, 31, 31}
    EXPECT_EQ(st_vec.size( ), 7);
    EXPECT_EQ(iter, st_vec.end( ) - 2);
    EXPECT_EQ(*iter, "31");
    EXPECT_EQ(*(iter + 1), "31");
    EXPECT_EQ(*(iter - 1), "29");
}

TEST(StaticVector, InsertIteratorInFront)
{
    skl::static_vector<std::string, 4> src {"05", "07", "11", "13"};
    skl::static_vector<std::string, 8> dst {"17", "19", "23", "29"};

    auto iter = dst.insert(dst.begin( ), src.cbegin( ), src.cend( ));
    EXPECT_EQ(iter, dst.begin( ));
    EXPECT_EQ(dst.size( ), 8);
    EXPECT_EQ(dst[0], "05");
    EXPECT_EQ(dst[3], "13");
    EXPECT_EQ(dst[4], "17");
    EXPECT_EQ(dst[7], "29");
    EXPECT_TRUE(std::is_sorted(dst.cbegin( ), dst.cend( )));
}

TEST(StaticVector, InsertIteratorInMiddle)
{
    skl::static_vector<std::string, 4> src {"11", "13", "17", "19"};
    skl::static_vector<std::string, 8> dst {"05", "07", "23", "29"};

    auto iter = dst.insert(dst.begin( ) + 2, src.cbegin( ), src.cend( ));
    EXPECT_EQ(iter, dst.begin( ) + 2);
    EXPECT_EQ(dst.size( ), 8);
    EXPECT_EQ(dst[0], "05");
    EXPECT_EQ(dst[3], "13");
    EXPECT_EQ(dst[4], "17");
    EXPECT_EQ(dst[7], "29");
    EXPECT_TRUE(std::is_sorted(dst.cbegin( ), dst.cend( )));
}

TEST(StaticVector, InsertIteratorAtEnd)
{
    skl::static_vector<std::string, 8> dst {"05", "07", "11", "13"};
    skl::static_vector<std::string, 4> src {"17", "19", "23", "29"};

    auto iter = dst.insert(dst.end( ), src.cbegin( ), src.cend( ));
    EXPECT_EQ(iter, dst.begin( ) + 4);
    EXPECT_EQ(dst.size( ), 8);
    EXPECT_EQ(dst[0], "05");
    EXPECT_EQ(dst[3], "13");
    EXPECT_EQ(dst[4], "17");
    EXPECT_EQ(dst[7], "29");
    EXPECT_TRUE(std::is_sorted(dst.cbegin( ), dst.cend( )));
}

TEST(StaticVector, InsertInitializerList)
{
    skl::static_vector<std::string, 8> dst {"002", "004", "128", "256"};
    dst.insert(dst.begin( ) + 2, {"008", "016", "032", "064"});

    EXPECT_TRUE(std::is_sorted(dst.cbegin( ), dst.cend( )));
    EXPECT_EQ(dst.size( ), 8);
}

TEST(StaticVector, SingleEraseEmptyFront)
{
    skl::static_vector<std::string, 8> st_vec;
    EXPECT_NO_THROW(st_vec.erase(st_vec.cbegin( )));
    EXPECT_EQ(st_vec.size( ), 0);
}

TEST(StaticVector, SigleEraseSingleFront)
{
    skl::static_vector<std::string, 8> st_vec {"3.14"};
    auto                               iter = st_vec.erase(st_vec.cbegin( ));
    EXPECT_EQ(iter, st_vec.end( ));
    EXPECT_EQ(st_vec.size( ), 0);
}

TEST(StaticVector, SingleEraseMulti2Front)
{
    skl::static_vector<std::string, 8> st_vec {"3.14", "2.71"};
    auto                               iter = st_vec.erase(st_vec.cbegin( ));
    EXPECT_EQ(iter, st_vec.begin( ));
    EXPECT_EQ(*iter, "2.71");
    EXPECT_EQ(st_vec.size( ), 1);
    EXPECT_EQ(st_vec[0], "2.71");
}

TEST(StaticVector, SingleEraseMulti3Front)
{
    skl::static_vector<std::string, 8> st_vec {"3.14", "2.71", "1.41"};
    auto                               iter = st_vec.erase(st_vec.cbegin( ));
    EXPECT_EQ(iter, st_vec.begin( ));
    EXPECT_EQ(*iter, "2.71");
    EXPECT_EQ(st_vec.size( ), 2);
    EXPECT_EQ(st_vec[0], "2.71");
    EXPECT_EQ(st_vec[1], "1.41");
}

TEST(StaticVector, SingleEraseMulti3Last)
{
    skl::static_vector<std::string, 8> st_vec {"3.14", "2.71", "1.41"};
    auto                               iter = st_vec.erase(st_vec.cend( ) - 1);
    EXPECT_EQ(iter, st_vec.cend( ));
    EXPECT_EQ(st_vec.size( ), 2);
    EXPECT_EQ(st_vec[0], "3.14");
    EXPECT_EQ(st_vec[1], "2.71");
}

TEST(StaticVector, SingleEraseMulti3Middle)
{
    skl::static_vector<std::string, 8> st_vec {"3.14", "2.71", "1.41"};
    auto                               iter = st_vec.erase(st_vec.cbegin( ) + 1);
    EXPECT_EQ(iter, st_vec.cbegin( ) + 1);
    EXPECT_EQ(*iter, "1.41");
    EXPECT_EQ(st_vec.size( ), 2);
    EXPECT_EQ(st_vec[0], "3.14");
    EXPECT_EQ(st_vec[1], "1.41");
}

TEST(StaticVector, MultiEraseEmpty)
{
    skl::static_vector<std::string, 8> st_vec { };
    auto                               iter = st_vec.erase(st_vec.begin( ), st_vec.end( ));
    EXPECT_EQ(iter, st_vec.cend( ));
    EXPECT_EQ(st_vec.size( ), 0);
}

TEST(StaticVector, MultiEraseFront)
{
    skl::static_vector<std::string, 8> st_vec {"3", "5", "7", "11", "13", "17", "19"};
    auto                               iter = st_vec.erase(st_vec.cbegin( ), st_vec.cbegin( ) + 3);
    EXPECT_EQ(iter, st_vec.cbegin( ));
    EXPECT_EQ(*iter, "11");
    EXPECT_EQ(st_vec.size( ), 4);
    EXPECT_EQ(st_vec.front( ), "11");
    EXPECT_EQ(st_vec[1], "13");
    EXPECT_EQ(st_vec.back( ), "19");
}

TEST(StaticVector, MultiEraseMiddle)
{
    skl::static_vector<std::string, 8> st_vec {"3", "5", "7", "11", "13", "17", "19"};
    auto                               iter = st_vec.erase(st_vec.cbegin( ) + 2, st_vec.cbegin( ) + 5);
    EXPECT_EQ(iter, st_vec.cbegin( ) + 2);
    EXPECT_EQ(*iter, "17");
    EXPECT_EQ(st_vec.size( ), 4);
    EXPECT_EQ(st_vec.front( ), "3");
    EXPECT_EQ(st_vec[1], "5");
    EXPECT_EQ(st_vec.back( ), "19");
}

TEST(StaticVector, MultiEraseEnd)
{
    skl::static_vector<std::string, 8> st_vec {"3", "5", "7", "11", "13", "17", "19"};
    auto                               iter = st_vec.erase(st_vec.cbegin( ) + 5, st_vec.cend( ));
    EXPECT_EQ(iter, st_vec.cend( ));
    EXPECT_EQ(st_vec.size( ), 5);
    EXPECT_EQ(st_vec.front( ), "3");
    EXPECT_EQ(st_vec[1], "5");
    EXPECT_EQ(st_vec.back( ), "13");
}

TEST(StaticVector, EmplaceEmpty)
{
    skl::static_vector<std::string, 9> st_vec;
    std::vector<std::string>           std_vec;

    auto iter   = st_vec.emplace(st_vec.begin( ), 3, 'a');
    auto s_iter = std_vec.emplace(std_vec.begin( ), 3, 'a');

    EXPECT_EQ(st_vec.size( ), 1);
    EXPECT_EQ(iter, st_vec.begin( ));
    EXPECT_EQ(*iter, *s_iter);
    EXPECT_EQ(*iter, "aaa");

#if __cplusplus >= 202002L
    EXPECT_TRUE(std::ranges::equal(st_vec, std_vec));
#else
    EXPECT_TRUE(std::equal(st_vec.begin( ), st_vec.end( ), std_vec.begin( ), std_vec.end( )));
#endif
}

TEST(StaticVector, EmplaceFront)
{
    skl::static_vector<std::string, 8> st_vec {"lorem", "ipsum", "dolor", "set", "amet"};
    std::vector<std::string>           std_vec {"lorem", "ipsum", "dolor", "set", "amet"};

    auto iter   = st_vec.emplace(st_vec.cbegin( ), 3, '_');
    auto s_iter = std_vec.emplace(std_vec.cbegin( ), 3, '_');

    EXPECT_EQ(st_vec.size( ), 6);
    EXPECT_EQ(iter, st_vec.begin( ));
    EXPECT_EQ(*iter, *s_iter);
    EXPECT_EQ(*iter, "___");
    EXPECT_EQ(st_vec[1], "lorem");
    EXPECT_EQ(st_vec[5], "amet");

#if __cplusplus >= 202002L
    EXPECT_TRUE(std::ranges::equal(st_vec, std_vec));
#else
    EXPECT_TRUE(std::equal(st_vec.begin( ), st_vec.end( ), std_vec.begin( ), std_vec.end( )));
#endif
}

TEST(StaticVector, EmplaceMiddle)
{
    skl::static_vector<std::string, 8> st_vec {"lorem", "ipsum", "dolor", "set", "amet"};
    std::vector<std::string>           std_vec {"lorem", "ipsum", "dolor", "set", "amet"};

    auto iter   = st_vec.emplace(st_vec.cbegin( ) + 3, 3, '_');
    auto s_iter = std_vec.emplace(std_vec.cbegin( ) + 3, 3, '_');

    EXPECT_EQ(st_vec.size( ), 6);
    EXPECT_EQ(iter, st_vec.begin( ) + 3);
    EXPECT_EQ(*iter, *s_iter);
    EXPECT_EQ(*iter, "___");
    EXPECT_EQ(st_vec[1], "ipsum");
    EXPECT_EQ(st_vec[3], "___");
    EXPECT_EQ(st_vec[5], "amet");

#if __cplusplus >= 202002L
    EXPECT_TRUE(std::ranges::equal(st_vec, std_vec));
#else
    EXPECT_TRUE(std::equal(st_vec.begin( ), st_vec.end( ), std_vec.begin( ), std_vec.end( )));
#endif
}

TEST(StaticVector, EmplaceEnd)
{
    skl::static_vector<std::string, 8> st_vec {"lorem", "ipsum", "dolor", "set", "amet"};
    std::vector<std::string>           std_vec {"lorem", "ipsum", "dolor", "set", "amet"};

    auto iter   = st_vec.emplace(st_vec.cend( ), 3, '+');
    auto s_iter = std_vec.emplace(std_vec.cend( ), 3, '+');

    EXPECT_EQ(st_vec.size( ), 6);
    EXPECT_EQ(iter, st_vec.end( ) - 1);
    EXPECT_EQ(*iter, *s_iter);
    EXPECT_EQ(*iter, "+++");
    EXPECT_EQ(st_vec[1], "ipsum");
    EXPECT_EQ(st_vec[3], "set");
    EXPECT_EQ(st_vec[5], "+++");

#if __cplusplus >= 202002L
    EXPECT_TRUE(std::ranges::equal(st_vec, std_vec));
#else
    EXPECT_TRUE(std::equal(st_vec.begin( ), st_vec.end( ), std_vec.begin( ), std_vec.end( )));
#endif
}

TEST(StaticVector, SwapObjects)
{
    std::initializer_list<std::string> i1 {"lorem", "ipsum", "dolor", "set", "amet"};
    std::initializer_list<std::string> i2 {"3", "5", "7", "11", "13", "17", "19"};
    skl::static_vector<std::string, 7> a1 {i1};
    skl::static_vector<std::string, 7> a2 {i2};

    EXPECT_EQ(a1.size( ), i1.size( ));
    EXPECT_EQ(a2.size( ), i2.size( ));
#if __cplusplus >= 202002L
    EXPECT_TRUE(std::ranges::equal(a1, i1));
#else
    EXPECT_TRUE(std::equal(a1.begin( ), a1.end( ), i1.begin( ), i1.end( )));
#endif
#if __cplusplus >= 202002L
    EXPECT_TRUE(std::ranges::equal(a2, i2));
#else
    EXPECT_TRUE(std::equal(a2.begin( ), a2.end( ), i2.begin( ), i2.end( )));
#endif

    std::swap(a1, a2);
    EXPECT_EQ(a1.size( ), i2.size( ));
    EXPECT_EQ(a2.size( ), i1.size( ));
#if __cplusplus >= 202002L
    EXPECT_TRUE(std::ranges::equal(a1, i2));
#else
    EXPECT_TRUE(std::equal(a1.begin( ), a1.end( ), i2.begin( ), i2.end( )));
#endif
#if __cplusplus >= 202002L
    EXPECT_TRUE(std::ranges::equal(a2, i1));
#else
    EXPECT_TRUE(std::equal(a2.begin( ), a2.end( ), i1.begin( ), i1.end( )));
#endif

    a1.swap(a2);
    EXPECT_EQ(a1.size( ), i1.size( ));
    EXPECT_EQ(a2.size( ), i2.size( ));
#if __cplusplus >= 202002L
    EXPECT_TRUE(std::ranges::equal(a1, i1));
#else
    EXPECT_TRUE(std::equal(a1.begin( ), a1.end( ), i1.begin( ), i1.end( )));
#endif
#if __cplusplus >= 202002L
    EXPECT_TRUE(std::ranges::equal(a2, i2));
#else
    EXPECT_TRUE(std::equal(a2.begin( ), a2.end( ), i2.begin( ), i2.end( )));
#endif

    std::swap(a1, a2);
    EXPECT_EQ(a1.size( ), i2.size( ));
    EXPECT_EQ(a2.size( ), i1.size( ));
#if __cplusplus >= 202002L
    EXPECT_TRUE(std::ranges::equal(a1, i2));
#else
    EXPECT_TRUE(std::equal(a1.begin( ), a1.end( ), i2.begin( ), i2.end( )));
#endif
#if __cplusplus >= 202002L
    EXPECT_TRUE(std::ranges::equal(a2, i1));
#else
    EXPECT_TRUE(std::equal(a2.begin( ), a2.end( ), i1.begin( ), i1.end( )));
#endif

    a2.swap(a1);
    EXPECT_EQ(a1.size( ), i1.size( ));
    EXPECT_EQ(a2.size( ), i2.size( ));
#if __cplusplus >= 202002L
    EXPECT_TRUE(std::ranges::equal(a1, i1));
#else
    EXPECT_TRUE(std::equal(a1.begin( ), a1.end( ), i1.begin( ), i1.end( )));
#endif
#if __cplusplus >= 202002L
    EXPECT_TRUE(std::ranges::equal(a2, i2));
#else
    EXPECT_TRUE(std::equal(a2.begin( ), a2.end( ), i2.begin( ), i2.end( )));
#endif
}

TEST(StaticVector, SwapTrivial)
{
    std::initializer_list<int> i1 {1, 1, 2, 3, 5, 8};
    std::initializer_list<int> i2 {3, 9, 27};
    skl::static_vector<int, 7> a1 {i1};
    skl::static_vector<int, 7> a2 {i2};

    EXPECT_EQ(a1.size( ), i1.size( ));
    EXPECT_EQ(a2.size( ), i2.size( ));
#if __cplusplus >= 202002L
    EXPECT_TRUE(std::ranges::equal(a1, i1));
#else
    EXPECT_TRUE(std::equal(a1.begin( ), a1.end( ), i1.begin( ), i1.end( )));
#endif
#if __cplusplus >= 202002L
    EXPECT_TRUE(std::ranges::equal(a2, i2));
#else
    EXPECT_TRUE(std::equal(a2.begin( ), a2.end( ), i2.begin( ), i2.end( )));
#endif

    std::swap(a1, a2);
    EXPECT_EQ(a1.size( ), i2.size( ));
    EXPECT_EQ(a2.size( ), i1.size( ));
#if __cplusplus >= 202002L
    EXPECT_TRUE(std::ranges::equal(a1, i2));
#else
    EXPECT_TRUE(std::equal(a1.begin( ), a1.end( ), i2.begin( ), i2.end( )));
#endif
#if __cplusplus >= 202002L
    EXPECT_TRUE(std::ranges::equal(a2, i1));
#else
    EXPECT_TRUE(std::equal(a2.begin( ), a2.end( ), i1.begin( ), i1.end( )));
#endif

    a1.swap(a2);
    EXPECT_EQ(a1.size( ), i1.size( ));
    EXPECT_EQ(a2.size( ), i2.size( ));
#if __cplusplus >= 202002L
    EXPECT_TRUE(std::ranges::equal(a1, i1));
#else
    EXPECT_TRUE(std::equal(a1.begin( ), a1.end( ), i1.begin( ), i1.end( )));
#endif
#if __cplusplus >= 202002L
    EXPECT_TRUE(std::ranges::equal(a2, i2));
#else
    EXPECT_TRUE(std::equal(a2.begin( ), a2.end( ), i2.begin( ), i2.end( )));
#endif

    std::swap(a1, a2);
    EXPECT_EQ(a1.size( ), i2.size( ));
    EXPECT_EQ(a2.size( ), i1.size( ));
#if __cplusplus >= 202002L
    EXPECT_TRUE(std::ranges::equal(a1, i2));
#else
    EXPECT_TRUE(std::equal(a1.begin( ), a1.end( ), i2.begin( ), i2.end( )));
#endif
#if __cplusplus >= 202002L
    EXPECT_TRUE(std::ranges::equal(a2, i1));
#else
    EXPECT_TRUE(std::equal(a2.begin( ), a2.end( ), i1.begin( ), i1.end( )));
#endif

    a2.swap(a1);
    EXPECT_EQ(a1.size( ), i1.size( ));
    EXPECT_EQ(a2.size( ), i2.size( ));
#if __cplusplus >= 202002L
    EXPECT_TRUE(std::ranges::equal(a1, i1));
#else
    EXPECT_TRUE(std::equal(a1.begin( ), a1.end( ), i1.begin( ), i1.end( )));
#endif
#if __cplusplus >= 202002L
    EXPECT_TRUE(std::ranges::equal(a2, i2));
#else
    EXPECT_TRUE(std::equal(a2.begin( ), a2.end( ), i2.begin( ), i2.end( )));
#endif
}

TEST(StaticVector, ResizeLess)
{
    skl::static_vector<std::string, 9> st_vec {"lorem", "ipsum", "sit", "dolor", "amet", "consectetur", "adipiscing"};
    std::vector<std::string>           std_vec {"lorem", "ipsum", "sit", "dolor", "amet", "consectetur", "adipiscing"};

    EXPECT_EQ(st_vec.size( ), 7);
    EXPECT_NO_THROW(st_vec.resize(5));
    std_vec.resize(5);

    EXPECT_EQ(st_vec.size( ), 5);
    EXPECT_EQ(st_vec.size( ), std_vec.size( ));
#if __cplusplus >= 202002L
    EXPECT_TRUE(std::ranges::equal(std_vec, st_vec));
#else
    EXPECT_TRUE(std::equal(std_vec.begin( ), std_vec.end( ), st_vec.begin( ), st_vec.end( )));
#endif
}

TEST(StaticVector, ResizeMore)
{
    skl::static_vector<std::string, 9> st_vec {"lorem", "ipsum", "sit", "dolor", "amet", "consectetur", "adipiscing"};
    std::vector<std::string>           std_vec {"lorem", "ipsum", "sit", "dolor", "amet", "consectetur", "adipiscing"};

    EXPECT_EQ(st_vec.size( ), 7);
    EXPECT_NO_THROW(st_vec.resize(9));
    std_vec.resize(9);

    EXPECT_EQ(st_vec.size( ), 9);
    EXPECT_EQ(st_vec.size( ), std_vec.size( ));
#if __cplusplus >= 202002L
    EXPECT_TRUE(std::ranges::equal(std_vec, st_vec));
#else
    EXPECT_TRUE(std::equal(std_vec.begin( ), std_vec.end( ), st_vec.begin( ), st_vec.end( )));
#endif

    EXPECT_EQ(st_vec[7], "");
    EXPECT_EQ(st_vec[8], "");
}

TEST(StaticVector, ResizeMoreValue)
{
    skl::static_vector<std::string, 9> st_vec {"lorem", "ipsum", "sit", "dolor", "amet", "consectetur", "adipiscing"};
    std::vector<std::string>           std_vec {"lorem", "ipsum", "sit", "dolor", "amet", "consectetur", "adipiscing"};

    EXPECT_EQ(st_vec.size( ), 7);
    EXPECT_NO_THROW(st_vec.resize(9, "latin"));
    std_vec.resize(9, "latin");

    EXPECT_EQ(st_vec.size( ), 9);
    EXPECT_EQ(st_vec.size( ), std_vec.size( ));
#if __cplusplus >= 202002L
    EXPECT_TRUE(std::ranges::equal(std_vec, st_vec));
#else
    EXPECT_TRUE(std::equal(std_vec.begin( ), std_vec.end( ), st_vec.begin( ), st_vec.end( )));
#endif

    EXPECT_EQ(st_vec[7], "latin");
    EXPECT_EQ(st_vec[8], "latin");
}

TEST(StaticVector, ResizeOverflow)
{
    skl::static_vector<std::string, 9> st_vec {"lorem", "ipsum", "sit", "dolor", "amet", "consectetur", "adipiscing"};

    EXPECT_EQ(st_vec.size( ), 7);
    EXPECT_THROW(st_vec.resize(10), std::length_error);

    EXPECT_EQ(st_vec.size( ), 7);
}

TEST(StaticVector, CompareEquality)
{
    skl::static_vector<int, 10> a {1, 2, 3, 5, 7};
    skl::static_vector<int, 10> b {1, 2, 3, 4, 5};
    skl::static_vector<int, 10> c {1, 2, 3, 4, 5};
    skl::static_vector<int, 10> d {1, 2, 3, 4, 5, 8};

    EXPECT_EQ(a, a);
    EXPECT_EQ(b, c);
    EXPECT_EQ(c, b);
    EXPECT_NE(a, b);
    EXPECT_NE(b, a);

    EXPECT_NE(a, d);
    EXPECT_NE(a, b);
}

TEST(StaticVector, CompareRelation)
{
    skl::static_vector<int, 10> a {1, 2, 3, 5, 7};
    skl::static_vector<int, 10> b {1, 2, 3, 4, 5};
    skl::static_vector<int, 10> c {1, 2, 3, 4, 5};

    EXPECT_TRUE(a > b);
    EXPECT_FALSE(a < b);

    EXPECT_TRUE(b < a);
    EXPECT_FALSE(b > a);

    EXPECT_TRUE(a >= b);
    EXPECT_TRUE(b <= a);
    EXPECT_FALSE(a <= b);
    EXPECT_FALSE(b >= a);

    EXPECT_TRUE(b <= c);
    EXPECT_TRUE(b >= c);
    EXPECT_TRUE(c <= b);
    EXPECT_TRUE(c >= b);

    EXPECT_TRUE(b == c);
    EXPECT_TRUE(c == b);
    EXPECT_FALSE(b != c);
    EXPECT_FALSE(c != b);

    EXPECT_TRUE(b != a);
    EXPECT_TRUE(a != b);
    EXPECT_FALSE(a == b);
    EXPECT_FALSE(b == a);
}

#if __cplusplus >= 202002L
TEST(StaticVector, RangesCompatible)
{
    skl::static_vector<int, 4> st_vec {2, 5, 7, 3};
    std::vector<int>           vec {2, 5, 7, 2};

    EXPECT_FALSE(std::ranges::equal(st_vec, vec));
    EXPECT_FALSE(std::ranges::equal(vec, st_vec));

    std::ranges::any_of(st_vec, [] (int i) { return i > 6; });
}

TEST(StaticVector, CompareThreeWay)
{
    skl::static_vector<int, 10> a {1, 2, 3, 5, 7};
    skl::static_vector<int, 10> b {1, 2, 3, 4, 5};
    skl::static_vector<int, 10> c {1, 2, 3, 4, 5};

    EXPECT_EQ(a <=> b, std::strong_ordering::greater);
    EXPECT_EQ(b <=> a, std::strong_ordering::less);
    EXPECT_EQ(b <=> c, std::strong_ordering::equal);

    EXPECT_NE(a <=> b, std::strong_ordering::equivalent);
}

TEST(StaticVector, StdErase)
{
    skl::static_vector<int, 10> st_vec {1, 3, 4, 5, 7, 7, 9, 1, 3};
    std::vector<int>            std_vec {1, 3, 4, 5, 7, 7, 9, 1, 3};

    std::erase(st_vec, 1);
    std::erase(std_vec, 1);

    EXPECT_TRUE(std::ranges::equal(st_vec, std_vec));

    std::erase_if(st_vec, [] (int i) { return i >= 5; });
    std::erase_if(std_vec, [] (int i) { return i >= 5; });

    EXPECT_TRUE(std::ranges::equal(st_vec, std_vec));

    EXPECT_TRUE(std::ranges::equal(st_vec, std::initializer_list<int> {3, 4, 3}));

    std::vector<int> a;
}
#endif
