#include "gtest/gtest.h"
#include "skip_list.hpp" 
#include <vector>
#include <numeric>
#include <algorithm>
#include <string>

TEST(SkipListInitializationTest, DefaultConstructor) {
    skip_list<int> list;
    EXPECT_EQ(list.size(), 0);
    EXPECT_TRUE(list.empty());
    EXPECT_EQ(list.begin(), list.end());
}

class SkipListBasicOpsTest : public ::testing::Test {
protected:
    skip_list<int> list;
};

TEST_F(SkipListBasicOpsTest, InsertSingleElement) {
    EXPECT_TRUE(list.insert(10));
    EXPECT_EQ(list.size(), 1);
    EXPECT_FALSE(list.empty());
    EXPECT_TRUE(list.contains(10));
}

TEST_F(SkipListBasicOpsTest, InsertMultipleElements) {
    ASSERT_TRUE(list.insert(20));
    ASSERT_TRUE(list.insert(10));
    ASSERT_TRUE(list.insert(30));
    
    EXPECT_EQ(list.size(), 3);
    EXPECT_TRUE(list.contains(10));
    EXPECT_TRUE(list.contains(20));
    EXPECT_TRUE(list.contains(30));
    EXPECT_FALSE(list.contains(40)); // Check for a non-existent element
}

TEST_F(SkipListBasicOpsTest, InsertDuplicateElement) {
    ASSERT_TRUE(list.insert(50));
    EXPECT_EQ(list.size(), 1);
    
    // Attempt to insert a duplicate
    EXPECT_FALSE(list.insert(50));
    EXPECT_EQ(list.size(), 1); 
}

TEST_F(SkipListBasicOpsTest, ContainsOnEmptyList) {
    EXPECT_FALSE(list.contains(100));
}

class SkipListEraseTest : public ::testing::Test {
protected:
    void SetUp() override {
        list.insert(10);
        list.insert(20);
        list.insert(30);
        list.insert(40);
        list.insert(50);
    }

    skip_list<int> list;
};

TEST_F(SkipListEraseTest, EraseExistingElementFromMiddle) {
    ASSERT_TRUE(list.contains(30));
    EXPECT_TRUE(list.erase(30));
    EXPECT_EQ(list.size(), 4);
    EXPECT_FALSE(list.contains(30));
}

TEST_F(SkipListEraseTest, EraseFirstElement) {
    ASSERT_TRUE(list.contains(10));
    EXPECT_TRUE(list.erase(10));
    EXPECT_EQ(list.size(), 4);
    EXPECT_FALSE(list.contains(10));
    EXPECT_TRUE(list.contains(20)); 
}

TEST_F(SkipListEraseTest, EraseLastElement) {
    ASSERT_TRUE(list.contains(50));
    EXPECT_TRUE(list.erase(50));
    EXPECT_EQ(list.size(), 4);
    EXPECT_FALSE(list.contains(50));
    EXPECT_TRUE(list.contains(40));
}

TEST_F(SkipListEraseTest, EraseNonExistingElement) {
    EXPECT_FALSE(list.erase(99));
    EXPECT_EQ(list.size(), 5); 
}

TEST_F(SkipListEraseTest, EraseFromEmptyList) {
    skip_list<int> empty_list;
    EXPECT_FALSE(empty_list.erase(10));
}

class SkipListIteratorTest : public ::testing::Test {
protected:
    void SetUp() override {
        values = {11, 22, 33, 44, 55};
        for(int val : values) {
            list.insert(val);
        }
    }
    
    skip_list<int> list;
    std::vector<int> values;
};

TEST_F(SkipListIteratorTest, BeginNotEqualToEndForNonEmptyList) {
    EXPECT_NE(list.begin(), list.end());
}

TEST_F(SkipListIteratorTest, TraversalIsInCorrectOrder) {
    std::vector<int> traversed_values;
    for(auto it = list.begin(); it != list.end(); ++it) {
        traversed_values.push_back(*it);
    }
    EXPECT_EQ(traversed_values, values);
}

TEST_F(SkipListIteratorTest, RangeBasedForLoop) {
    std::vector<int> traversed_values;
    for (const auto& val : list) {
        traversed_values.push_back(val);
    }
    EXPECT_EQ(traversed_values, values);
}


TEST_F(SkipListIteratorTest, FindExistingElement) {
    auto it = list.find(33);
    ASSERT_NE(it, list.end());
    EXPECT_EQ(*it, 33);
}

TEST_F(SkipListIteratorTest, FindNonExistingElement) {
    auto it = list.find(99);
    EXPECT_EQ(it, list.end());
}


TEST(SkipListAdvancedOpsTest, ClearList) {
    skip_list<std::string> list;
    list.insert("hello");
    list.insert("world");
    list.insert("test");
    ASSERT_EQ(list.size(), 3);
    
    list.clear();
    
    EXPECT_EQ(list.size(), 0);
    EXPECT_TRUE(list.empty());
    EXPECT_EQ(list.begin(), list.end());
    EXPECT_FALSE(list.contains("world")); 
}

TEST(SkipListAdvancedOpsTest, ClearEmptyList) {
    skip_list<int> list;
    ASSERT_TRUE(list.empty());
    list.clear();
    EXPECT_TRUE(list.empty());
    EXPECT_EQ(list.size(), 0);
}

TEST(SkipListStressTest, InsertAndEraseManyElements) {
    skip_list<int> list;
    const int num_elements = 1000;
    std::vector<int> elements(num_elements);
    std::iota(elements.begin(), elements.end(), 0); 

    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(elements.begin(), elements.end(), g);

    for(int i = 0; i < num_elements; ++i) {
        ASSERT_TRUE(list.insert(elements[i]));
    }
    ASSERT_EQ(list.size(), num_elements);
    
    std::sort(elements.begin(), elements.end());
    for(int i = 0; i < num_elements; ++i) {
        ASSERT_TRUE(list.contains(elements[i]));
    }
    
    auto it = list.begin();
    for(int i = 0; i < num_elements; ++i) {
        ASSERT_NE(it, list.end());
        ASSERT_EQ(*it, elements[i]);
        ++it;
    }
    ASSERT_EQ(it, list.end());

    std::shuffle(elements.begin(), elements.end(), g);
    for(int i = 0; i < num_elements; ++i) {
        ASSERT_TRUE(list.erase(elements[i]));
    }
    
    ASSERT_TRUE(list.empty());
    ASSERT_EQ(list.size(), 0);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
