#include <gtest/gtest.h>

#include "static_vector.hxx"

using namespace testing;
using namespace wbr;

// Test assign with count and value
TEST(StaticVectorAdapterTest, AssignCountValue)
{
    std::array<int, 5>    data_array = {10, 20, 30, 40, 50};
    std::size_t           element_count {3};
    static_vector_adapter adapter {data_array, element_count};  // Initialized with only 3 elements (10, 20, 30)

    // Assign a single value to all elements
    adapter.assign(3, 99);

    const std::array<int, 5> expected_data = {99, 99, 99, 40, 50};  // Only first 3 elements should be 99

    for ( std::size_t i = 0; i < data_array.size( ); ++i ) {
        EXPECT_EQ(data_array[i], expected_data[i]);
    }
    EXPECT_TRUE(std::equal(data_array.cbegin( ), data_array.cend( ), expected_data.cbegin( )));

    EXPECT_EQ(element_count, 3);
}

// Test assign with initializer list
TEST(StaticVectorAdapterTest, AssignInitializerList)
{
    std::array<int, 5>    data_array = {10, 20, 30, 40, 50};
    std::size_t           element_count {3};
    static_vector_adapter adapter {data_array, element_count};  // Initialized with only 3 elements (10, 20, 30)

    // Assign values from an initializer list
    adapter.assign({100, 200, 300});

    const std::array<int, 5> expected_data = {100, 200, 300, 40, 50};  // Only first 3 elements should be updated

    for ( std::size_t i = 0; i < data_array.size( ); ++i ) {
        EXPECT_EQ(data_array[i], expected_data[i]);
    }
    EXPECT_TRUE(std::equal(data_array.cbegin( ), data_array.cend( ), expected_data.cbegin( )));

    EXPECT_EQ(element_count, 3);
}

// Test assign with input iterators
TEST(StaticVectorAdapterTest, AssignInputIterators)
{
    std::array<int, 5>    data_array = {10, 20, 30, 40, 50};
    std::size_t           element_count {3};
    static_vector_adapter adapter {data_array, element_count};  // Initialized with only 3 elements (10, 20, 30)

    // Assign values from a different array using input iterators
    const int other_data[] = {1, 2, 3};
    adapter.assign(other_data, other_data + 3);

    const std::array<int, 5> expected_data = {1, 2, 3, 40, 50};  // Only first 3 elements should be updated

    for ( std::size_t i = 0; i < data_array.size( ); ++i ) {
        EXPECT_EQ(data_array[i], expected_data[i]);
    }
    EXPECT_TRUE(std::equal(data_array.cbegin( ), data_array.cend( ), expected_data.cbegin( )));

    EXPECT_EQ(element_count, 3);
}

// Test assign with initializer list exceeding capacity
TEST(StaticVectorAdapterTest, AssignInitializerListExceedingCapacity)
{
    std::array<int, 5> data_array = {10, 20, 30, 40, 50};
    std::size_t        element_count {3};
    auto               adapter = make_adapter<BoundCheckStrategy::UB>(data_array, element_count);  // Initialized with only 3 elements (10, 20, 30)

    std::initializer_list<int> ilist {100, 200, 300, 400, 500, 600};

    // default object's bound check strategy (the one supplied upon object creation)
    // EXPECT_NO_THROW(adapter.assign(ilist));  // UB!

    // custom bound check strategy: throw exception
    EXPECT_THROW(adapter.assign<BoundCheckStrategy::Exception>(ilist), std::overflow_error);

    // custom bound check strategy: adjust input data to prevent overflow
    EXPECT_NO_THROW(adapter.assign<BoundCheckStrategy::LimitToBound>(ilist));
    EXPECT_EQ(adapter.size( ), 5);
    EXPECT_EQ(element_count, 5);

#if !defined(NDEBUG)
    EXPECT_EXIT(adapter.assign<BoundCheckStrategy::Assert>(ilist), KilledBySignal(SIGABRT), "");
#endif
}

// Test assign with count exceeding capacity
TEST(StaticVectorAdapterTest, AssignCountExceedingCapacity)
{
    std::array<int, 5> data_array = {10, 20, 30, 40, 50};
    std::size_t        element_count {3};
    auto               adapter = make_adapter<BoundCheckStrategy::UB>(data_array, element_count);  // Initialized with only 3 elements (10, 20, 30)'

    // default object's bound check strategy (the one supplied upon object creation)
    // EXPECT_NO_THROW(adapter.assign(6, 99));  // UB!

    // custom bound check strategy: throw exception
    EXPECT_THROW(adapter.assign<BoundCheckStrategy::Exception>(6, 99), std::out_of_range);

    // custom bound check strategy: adjust input data to prevent overflow
    EXPECT_NO_THROW(adapter.assign<BoundCheckStrategy::LimitToBound>(6, 99));
    EXPECT_EQ(adapter.size( ), 5);
    EXPECT_EQ(element_count, 5);

    // custom bound check strategy: assert
#if !defined(NDEBUG)
    EXPECT_EXIT(adapter.assign<BoundCheckStrategy::Assert>(6, 99), KilledBySignal(SIGABRT), "");
#endif
}

// Test assign with input iterators exceeding capacity
TEST(StaticVectorAdapterTest, AssignInputIteratorsExceedingCapacity)
{
    std::array<int, 5> data_array = {10, 20, 30, 40, 50};
    std::size_t        element_count {3};
    auto               adapter = make_adapter<BoundCheckStrategy::UB>(data_array, element_count);  // Initialized with only 3 elements (10, 20, 30)

    int other_data[] = {1, 2, 3, 4, 5, 6};

    // default object's bound check strategy (the one supplied upon object creation)
    // EXPECT_NO_THROW(adapter.assign(other_data, other_data + 6));  // UB!

    // custom bound check strategy: throw exception
    EXPECT_THROW(adapter.assign<BoundCheckStrategy::Exception>(other_data, other_data + 6), std::out_of_range);

    // custom bound check strategy: adjust input data to prevent overflow
    EXPECT_NO_THROW(adapter.assign<BoundCheckStrategy::LimitToBound>(other_data, other_data + 6));
    EXPECT_EQ(adapter.size( ), 5);
    EXPECT_EQ(element_count, 5);

    // custom bound check strategy: assert
#if !defined(NDEBUG)
    EXPECT_EXIT(adapter.assign<BoundCheckStrategy::Assert>(other_data, other_data + 6), KilledBySignal(SIGABRT), "");
#endif
}

TEST(StaticVectorAdapterTest, AssignSwappedInputIterators)
{
    std::array<int, 5> data_array = {10, 20, 30, 40, 50};
    std::size_t        element_count {3};
    auto               adapter = make_adapter<BoundCheckStrategy::UB>(data_array, element_count);  // Initialized with only 3 elements (10, 20, 30)

    int other_data[] = {1, 2, 3, 4, 5, 6};

    // default object's bound check strategy (the one supplied upon object creation)
    // EXPECT_NO_THROW(adapter.assign(other_data, other_data + 6));  // UB!

    // custom bound check strategy: throw exception
    EXPECT_THROW(adapter.assign<BoundCheckStrategy::Exception>(other_data + 6, other_data), std::runtime_error);

    // custom bound check strategy: adjust input data to prevent overflow
    EXPECT_NO_THROW(adapter.assign<BoundCheckStrategy::LimitToBound>(other_data + 6, other_data));
    EXPECT_EQ(adapter.size( ), 0);
    EXPECT_EQ(element_count, 0);

#if !defined(NDEBUG)
    // custom bound check strategy: assert
    EXPECT_EXIT(adapter.assign<BoundCheckStrategy::Assert>(other_data + 6, other_data), KilledBySignal(SIGABRT), "");
#endif
}

// Test for push_back method
TEST(StaticVectorAdapterTest, PushBack)
{
    std::array<int, 5>    data_array = {10, 20, 30, 40, 50};
    std::size_t           element_count {3};
    static_vector_adapter adapter {data_array, element_count};  // Initialized with only 3 elements (10, 20, 30)

    // Initial state
    EXPECT_EQ(adapter.size( ), 3);
    EXPECT_FALSE(adapter.empty( ));

    // Push new element to the back
    adapter.push_back(60);

    // Verify size is increased by 1 and last element is correct
    EXPECT_EQ(adapter.size( ), 4);
    EXPECT_EQ(adapter[3], 60);  // 4th element should be 60 (indexing starts from 0)

    // Push another element to the back
    adapter.push_back(70);

    // Verify size is increased again and last two elements are correct
    EXPECT_EQ(adapter.size( ), 5);
    EXPECT_EQ(adapter[3], 60);  // 4th element should still be 60
    EXPECT_EQ(adapter[4], 70);  // 5th element should be 70

#if wbr_STATIC_VECTOR_DO_RANGE_CHECKS
    #if wbr_THROW_EXCEPTION_ON_ERROR
    // Ensure we can't push beyond capacity
    EXPECT_THROW(adapter.push_back(80), std::overflow_error);
    #endif
#endif

    const std::array<int, 5> expected_data = {10, 20, 30, 60, 70};  // Only first 3 elements should be updated
    for ( std::size_t i = 0; i < data_array.size( ); ++i ) {
        EXPECT_EQ(data_array[i], expected_data[i]);
    }
    EXPECT_TRUE(std::equal(data_array.cbegin( ), data_array.cend( ), expected_data.cbegin( )));

    EXPECT_EQ(element_count, 5);
}

// Test for pop_back method
TEST(StaticVectorAdapterTest, PopBack)
{
    std::array<int, 5>    data_array = {10, 20, 30, 40, 50};
    std::size_t           element_count {3};
    static_vector_adapter adapter {data_array, element_count};  // Initialized with only 3 elements (10, 20, 30)
    // Initial state
    EXPECT_EQ(adapter.size( ), 3);

    // Pop the last element off the back
    adapter.pop_back( );

    // Verify size is decreased by 1 and last element is correct
    EXPECT_EQ(adapter.size( ), 2);
    EXPECT_EQ(adapter[1], 20);  // 2nd element should be 20

    // Pop another element off the back
    adapter.pop_back( );

    // Verify size is decreased again and last element is correct
    EXPECT_EQ(adapter.size( ), 1);
    EXPECT_EQ(adapter[0], 10);  // Only first element remains (indexing starts from 0)

    // Ensure we can't pop on an empty container
    adapter.pop_back( );
    EXPECT_TRUE(adapter.empty( ));

#if wbr_STATIC_VECTOR_DO_RANGE_CHECKS
    #if wbr_THROW_EXCEPTION_ON_ERROR
    // Popping again should throw
    EXPECT_THROW(adapter.pop_back( ), std::underflow_error);
    #endif
#endif

    EXPECT_EQ(element_count, 0);
}

// Test for emplace_back method
TEST(StaticVectorAdapterTest, EmplaceBack)
{
    std::array<int, 5>    data_array = {10, 20, 30, 40, 50};
    std::size_t           element_count {3};
    static_vector_adapter adapter {data_array, element_count};  // Initialized with only 3 elements (10, 20, 30)

    // Initial state
    EXPECT_EQ(adapter.size( ), 3);

    // Use emplace_back to add a new element with initial value of 60
    adapter.emplace_back(60);

    // Verify size is increased by 1 and last element is correct
    EXPECT_EQ(adapter.size( ), 4);
    EXPECT_EQ(adapter[3], 60);  // 4th element should be 60 (indexing starts from 0)

    // Use emplace_back to add another new element with initial value of 70
    adapter.emplace_back(70);

    // Verify size is increased again and last two elements are correct
    EXPECT_EQ(adapter.size( ), 5);
    EXPECT_EQ(adapter[3], 60);  // 4th element should still be 60
    EXPECT_EQ(adapter[4], 70);  // 5th element should be 70

#if wbr_STATIC_VECTOR_DO_RANGE_CHECKS
    #if wbr_THROW_EXCEPTION_ON_ERROR
    // Ensure we can't emplace_back beyond capacity
    EXPECT_THROW(adapter.emplace_back(80), std::overflow_error);
    #endif
#endif

    const std::array<int, 5> expected_data = {10, 20, 30, 60, 70};  // Only first 3 elements should be updated
    for ( std::size_t i = 0; i < data_array.size( ); ++i ) {
        EXPECT_EQ(data_array[i], expected_data[i]);
    }
    EXPECT_TRUE(std::equal(data_array.cbegin( ), data_array.cend( ), expected_data.cbegin( )));
    EXPECT_EQ(element_count, 5);
}

TEST(StaticVectorAdapterTest, EraseSingleMethod)
{
    int                   data_c_array[5] = {10, 20, 30, 40, 50};
    size_t                element_count   = std::size(data_c_array);
    static_vector_adapter adapter {data_c_array, element_count};

    // Test erase from middle
    auto it = adapter.erase(adapter.begin( ) + 2);  // Erase the element with value 30
    EXPECT_EQ(*it, 40);                             // Next element should be 40
    EXPECT_EQ(adapter.size( ), 4);
    EXPECT_EQ(element_count, 4);

    // Verify elements after erase
    int expected[] = {10, 20, 40, 50};
    for ( size_t i = 0; i < adapter.size( ); ++i ) {
        EXPECT_EQ(adapter[i], expected[i]);
    }

    // Test erase from beginning
    it = adapter.erase(adapter.begin( ));  // Erase the first element (10)
    EXPECT_EQ(*it, 20);                    // Next element should be 20
    EXPECT_EQ(adapter.size( ), 3);
    EXPECT_EQ(element_count, 3);

    // Verify elements after second erase
    int expected_second[] = {20, 40, 50};
    for ( size_t i = 0; i < adapter.size( ); ++i ) {
        EXPECT_EQ(adapter[i], expected_second[i]);
    }

    // Test erase from end
    it = adapter.erase(adapter.end( ) - 1);  // Erase the last element (50)
    EXPECT_EQ(*(adapter.end( ) - 1), 40);    // Next element should be 40
    EXPECT_EQ(adapter.size( ), 2);
    EXPECT_EQ(element_count, 2);

    // Verify elements after third erase
    int expected_third[] = {20, 40};
    for ( size_t i = 0; i < adapter.size( ); ++i ) {
        EXPECT_EQ(adapter[i], expected_third[i]);
    }

    adapter.erase(adapter.begin( ));
    EXPECT_EQ(adapter.size( ), 1);

    adapter.erase(adapter.begin( ));
    EXPECT_EQ(adapter.size( ), 0);

    // Test erase on empty container should return end()
    it = adapter.erase(adapter.begin( ));
    EXPECT_EQ(it, adapter.end( ));

    // Verify the container is empty
    EXPECT_TRUE(adapter.empty( ));
    EXPECT_EQ(element_count, 0);
}

TEST(StaticVectorAdapterTest, EraseMultiMethod)
{
    int                   data_c_array[] = {10, 20, 30, 40, 50};
    size_t                element_count  = std::size(data_c_array);
    static_vector_adapter adapter {data_c_array, element_count};

    // Test erase single element from middle
    auto it = adapter.erase(adapter.begin( ) + 1, adapter.begin( ) + 2);  // Erase the element with value 20
    EXPECT_EQ(*it, 30);                                                   // Next element should be 30
    EXPECT_EQ(adapter.size( ), 4);
    EXPECT_EQ(element_count, 4);

    // Verify elements after erase
    int expected[] = {10, 30, 40, 50};
    for ( size_t i = 0; i < adapter.size( ); ++i ) {
        EXPECT_EQ(adapter[i], expected[i]);
    }

    // Test erase multiple elements from middle
    it = adapter.erase(adapter.begin( ) + 1, adapter.begin( ) + 3);  // Erase elements with values 30 and 40
    EXPECT_EQ(*it, 50);                                              // Next element should be 50
    EXPECT_EQ(adapter.size( ), 2);
    EXPECT_EQ(element_count, 2);

    // Verify elements after erase
    int expected_second[] = {10, 50};
    for ( size_t i = 0; i < adapter.size( ); ++i ) {
        EXPECT_EQ(adapter[i], expected_second[i]);
    }

    // Test erase from beginning to end
    it = adapter.erase(adapter.begin( ), adapter.end( ));  // Erase all elements
    EXPECT_EQ(it, adapter.end( ));

    // Verify the container is empty
    EXPECT_TRUE(adapter.empty( ));
    EXPECT_EQ(element_count, 0);
}

TEST(StaticVectorAdapterTest, At_Method_InBounds)
{
    int                   data_c_array[] = {10, 20, 30, 40, 50};
    size_t                element_count  = std::size(data_c_array);
    static_vector_adapter adapter {data_c_array, element_count};

    for ( size_t i = 0; i < adapter.size( ); ++i ) {
        adapter[i] = static_cast<int>(i);
    }

    // Act & Assert
    for ( size_t i = 0; i < adapter.size( ); ++i ) {
        EXPECT_EQ(adapter.at(i), static_cast<int>(i));
    }
}

TEST(StaticVectorAdapterTest, At_Method_OutOfBounds)
{
    int                   data_c_array[] = {10, 20, 30, 40, 50};
    size_t                element_count  = std::size(data_c_array);
    static_vector_adapter adapter {data_c_array, element_count};

    for ( size_t i = 0; i < adapter.size( ); ++i ) {
        adapter[i] = static_cast<int>(i);
    }

    // Act & Assert: Expect an out_of_range exception when accessing beyond the vector's size
    EXPECT_THROW(std::ignore = adapter.at(adapter.capacity( )), std::out_of_range);
}

TEST(StaticVectorAdapterTest, Front_Method)
{
    int                   data_c_array[] = {10, 20, 30, 40, 50};
    size_t                element_count  = std::size(data_c_array);
    static_vector_adapter adapter {data_c_array, element_count};

    // Act & Assert: Test front element when not empty
    EXPECT_EQ(adapter.front( ), 10);

    // Act: Empty the adapter and test front method
    adapter.clear( );

    // Assert: Expected behavior with an empty container
    // Note: Behavior depends on your implementation, might throw or return undefined value
    // For this example, we assume it throws.
    EXPECT_THROW(std::ignore = adapter.front<BoundCheckStrategy::Exception>( ), std::out_of_range);
#if !defined(NDEBUG)
    EXPECT_EXIT(std::ignore = adapter.front<BoundCheckStrategy::Assert>( ), KilledBySignal(SIGABRT), "");
#endif
}

TEST(StaticVectorAdapterTest, Back_Method)
{
    int                   data_c_array[] = {10, 20, 30, 40, 50};
    size_t                element_count  = 4;
    static_vector_adapter adapter {data_c_array, element_count};

    // Act & Assert: Test back element when not empty
    EXPECT_EQ(adapter.back( ), 40);

    // Act: Empty the adapter and test back method
    adapter.clear( );

    // Assert: Expected behavior with an empty container
    // Note: Behavior depends on your implementation, might throw or return undefined value
    // For this example, we assume it throws.
    EXPECT_THROW(std::ignore = adapter.back<BoundCheckStrategy::Exception>( ), std::out_of_range);
#if !defined(NDEBUG)
    EXPECT_EXIT(std::ignore = adapter.back<BoundCheckStrategy::Assert>( ), KilledBySignal(SIGABRT), "");
#endif
}

TEST(StaticVectorAdapterTest, BasicInsertion)
{
    int                   data_c_array[10] = {10, 20, 30, 40, 50};
    size_t                element_count    = 5;
    static_vector_adapter adapter {data_c_array, element_count};
    const int             expected_c_array[6] = {10, 20, 99, 30, 40, 50};

    auto it = adapter.insert(adapter.begin( ) + 2, 99);  // Insert at position 2

    EXPECT_EQ(*it, 99);
    EXPECT_EQ(it, adapter.begin( ) + 2);
    EXPECT_EQ(adapter.size( ), 6);  // Size should be increased by one
    EXPECT_EQ(element_count, 6);
    EXPECT_TRUE(std::equal(adapter.cbegin( ), adapter.cend( ), std::begin(expected_c_array)));
}

TEST(StaticVectorAdapterTest, InsertAtBeginning)
{
    int                   data_c_array[10] = {10, 20, 30, 40, 50};
    size_t                element_count    = 5;
    static_vector_adapter adapter {data_c_array, element_count};
    const int             expected_c_array[6] = {100, 10, 20, 30, 40, 50};

    auto it = adapter.insert(adapter.begin( ), 100);  // Insert at the beginning

    EXPECT_EQ(*it, 100);
    EXPECT_EQ(it, adapter.begin( ));
    EXPECT_EQ(adapter.size( ), 6);
    EXPECT_EQ(element_count, 6);
    EXPECT_TRUE(std::equal(adapter.cbegin( ), adapter.cend( ), std::begin(expected_c_array)));
}

TEST(StaticVectorAdapterTest, InsertAtEnd)
{
    int                   data_c_array[10] = {10, 20, 30, 40, 50};
    size_t                element_count    = 5;
    static_vector_adapter adapter {data_c_array, element_count};
    const int             expected_c_array[6] = {10, 20, 30, 40, 50, 200};

    auto it = adapter.insert(adapter.end( ), 200);  // Insert at the end
    EXPECT_EQ(*it, 200);
    EXPECT_EQ(it, adapter.end( ) - 1);
    EXPECT_EQ(adapter.size( ), 6);
    EXPECT_EQ(element_count, 6);
    EXPECT_TRUE(std::ranges::equal(adapter, expected_c_array));
}

TEST(StaticVectorAdapterTest, InsertOutsideOfRangePosition)
{
    int                   data_c_array[10] = {10, 20, 30, 40, 50};
    size_t                element_count    = 5;
    static_vector_adapter adapter {data_c_array, element_count};

    EXPECT_THROW(adapter.insert<BoundCheckStrategy::Exception>(adapter.end( ) + 1, 333), std::out_of_range);
#if !defined(NDEBUG)
    EXPECT_EXIT(adapter.insert<BoundCheckStrategy::Assert>(adapter.end( ) + 1, 333), KilledBySignal(SIGABRT), "");
#endif

    const int expected_c_array[6] = {10, 20, 30, 40, 50, 333};
    auto      it                  = adapter.begin( );
    EXPECT_NO_THROW(it = adapter.insert<BoundCheckStrategy::LimitToBound>(adapter.end( ) + 1, 333));
    EXPECT_EQ(it, adapter.end( ) - 1);
    EXPECT_EQ(adapter.size( ), 6);
    EXPECT_TRUE(std::ranges::equal(adapter, expected_c_array));

    EXPECT_THROW(adapter.insert<BoundCheckStrategy::Exception>(adapter.begin( ) - 1, 222), std::out_of_range);

#if !defined(NDEBUG)
    EXPECT_EXIT(adapter.insert<BoundCheckStrategy::Assert>(adapter.begin( ) - 1, 222), KilledBySignal(SIGABRT), "");
#endif

    const int expected_c_array_2[7] = {222, 10, 20, 30, 40, 50, 333};
    EXPECT_NO_THROW(it = adapter.insert<BoundCheckStrategy::LimitToBound>(adapter.begin( ) - 1, 222));
    EXPECT_EQ(it, adapter.begin( ));
    EXPECT_EQ(adapter.size( ), 7);
    EXPECT_TRUE(std::ranges::equal(adapter, expected_c_array_2));
}

TEST(StaticVectorAdapterTest, InsertWithFullCapacity)
{
    int                   data_c_array[5] = {10, 20, 30, 40, 50};
    size_t                element_count   = 5;
    static_vector_adapter adapter {data_c_array, element_count};

    EXPECT_THROW(adapter.insert<BoundCheckStrategy::Exception>(adapter.end( ), 400), std::overflow_error);

#if !defined(NDEBUG)
    EXPECT_EXIT(adapter.insert<BoundCheckStrategy::Assert>(adapter.end( ), 400), KilledBySignal(SIGABRT), "");
#endif

    decltype(adapter)::iterator iter;
    EXPECT_NO_THROW(adapter.insert<BoundCheckStrategy::LimitToBound>(adapter.end( ), 400));
    EXPECT_EQ(adapter.size( ), 5);
    const int expected_data_c_array[5] = {10, 20, 30, 40, 50};
    EXPECT_TRUE(std::ranges::equal(adapter, expected_data_c_array));
}
