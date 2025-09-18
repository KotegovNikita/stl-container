/**
 * @file stl-container.hpp
 * @brief Provides a template-based implementation of the Skip List data structure.
 *
 * This file contains the declaration and definition of the FastList class,
 * a probabilistic data structure that allows for efficient search, insertion,
 * and deletion operations.
 *
 */

#ifndef SKIP_LIST_HPP
#define SKIP_LIST_HPP

#include <vector>
#include <memory>
#include <random>
#include <stdexcept>
#include <iterator>
#include <algorithm>
#include <type_traits>

/**
 * @class FastList
 * @brief A template class that implements a Skip List.
 *
 * This class provides logarithmic time complexity on average for search,
 * insertion, and removal operations. It is an alternative to balanced trees.
 *
 * @tparam KeyType The type of elements stored in the list. It must support copy construction and comparison operators.
 */
template<typename KeyType>
class skip_list {
    static_assert(std::is_copy_constructible_v<KeyType>, "KeyType must be copy-constructible.");

private:
    /**
     * @struct SkipNode
     * @brief Represents a node within the FastList.
     *
     * Each node stores a value and an array of pointers to subsequent nodes at different levels,
     * enabling the skip-ahead functionality of the list.
     */
    struct SkipNode {
        KeyType value; ///< The data payload of the node.
        std::vector<SkipNode*> forward; ///< An array of forward pointers for each level.

        /**
         * @brief Constructs a new SkipNode.
         * @param level The number of forward pointers (height of the node).
         * @param val The value to be stored in this node.
         */
        SkipNode(int level, const KeyType& val) : value(val), forward(level, nullptr) {}
    };

    static constexpr int MAX_HEIGHT = 16; ///< Defines the maximum possible height for any node.
    const float promotion_probability_ = 0.5f; ///< The probability used to determine a new node's height.

    SkipNode* sentinel_head_; ///< A sentinel node that marks the beginning of the list.
    int current_height_;      ///< The current maximum height among all nodes in the list.
    size_t element_count_;    ///< The total number of elements currently in the list.
    
    std::mt19937 random_engine_; ///< Mersenne Twister engine for random number generation.
    std::uniform_real_distribution<float> distribution_; ///< Uniform distribution for probability checks.

    /**
     * @brief Determines a random height for a new node.
     *
     * The height is generated probabilistically. It starts at 1 and increases
     * with a fixed probability until it reaches the maximum allowed height.
     * @return A randomly generated height between 1 and MAX_HEIGHT.
     */
    int generateRandomHeight() {
        int height = 1;
        while (distribution_(random_engine_) < promotion_probability_ && height < MAX_HEIGHT) {
            ++height;
        }
        return height;
    }

public:
    /**
     * @brief Default constructor for FastList.
     *
     * Initializes an empty list with a sentinel head node.
     */
    skip_list()
        : sentinel_head_(new SkipNode(MAX_HEIGHT, KeyType{})), 
          current_height_(0), 
          element_count_(0),
          random_engine_(std::random_device{}()), 
          distribution_(0.0f, 1.0f) {}

    /**
     * @brief Destructor for FastList.
     *
     * Deallocates all nodes and frees memory.
     */
    ~skip_list() {
        clear();
        delete sentinel_head_;
    }

    /**
     * @brief Removes all elements from the list.
     *
     * After this operation, the list becomes empty.
     */
    void clear() {
        SkipNode* current = sentinel_head_->forward[0];
        while (current) {
            SkipNode* next = current->forward[0];
            delete current;
            current = next;
        }

        std::fill(sentinel_head_->forward.begin(), sentinel_head_->forward.end(), nullptr);
        current_height_ = 0;
        element_count_ = 0;
    }

    /**
     * @brief Inserts a new value into the list.
     *
     * If the value already exists, the insertion is aborted.
     * @param value The value to be inserted.
     * @return `true` if the insertion was successful, `false` if the value was already present.
     */
    bool insert(const KeyType& value) {
        std::vector<SkipNode*> update_path(MAX_HEIGHT, nullptr);
        SkipNode* current = sentinel_head_;

        for (int i = current_height_ - 1; i >= 0; --i) {
            while (current->forward[i] && current->forward[i]->value < value) {
                current = current->forward[i];
            }
            update_path[i] = current;
        }

        current = current->forward[0];

        if (current && current->value == value) {
            return false; // Element already exists
        }

        int newHeight = generateRandomHeight();
        if (newHeight > current_height_) {
            for (int i = current_height_; i < newHeight; ++i) {
                update_path[i] = sentinel_head_;
            }
            current_height_ = newHeight;
        }

        SkipNode* newNode = new SkipNode(newHeight, value);
        for (int i = 0; i < newHeight; ++i) {
            newNode->forward[i] = update_path[i]->forward[i];
            update_path[i]->forward[i] = newNode;
        }

        ++element_count_;
        return true;
    }

    /**
     * @brief Removes a value from the list.
     *
     * @param value The value to be removed.
     * @return `true` if the value was found and removed, `false` otherwise.
     */
    bool erase(const KeyType& value) {
        if (empty()) {
            return false;
        }

        std::vector<SkipNode*> update_path(MAX_HEIGHT, nullptr);
        SkipNode* current = sentinel_head_;

        for (int i = current_height_ - 1; i >= 0; --i) {
            while (current->forward[i] && current->forward[i]->value < value) {
                current = current->forward[i];
            }
            update_path[i] = current;
        }

        current = current->forward[0];

        if (!current || current->value != value) {
            return false; // Element not found
        }

        for (int i = 0; i < current_height_; ++i) {
            if (update_path[i]->forward[i] != current) break;
            update_path[i]->forward[i] = current->forward[i];
        }

        delete current;

        while (current_height_ > 0 && sentinel_head_->forward[current_height_ - 1] == nullptr) {
            --current_height_;
        }

        --element_count_;
        return true;
    }

    /**
     * @brief Searches for a value in the list.
     *
     * @param value The value to search for.
     * @return `true` if the value is found, `false` otherwise.
     */
    bool contains(const KeyType& value) const {
        SkipNode* current = sentinel_head_;
        for (int i = current_height_ - 1; i >= 0; --i) {
            while (current->forward[i] && current->forward[i]->value < value) {
                current = current->forward[i];
            }
        }
        current = current->forward[0];
        return current && current->value == value;
    }
    
    /**
     * @class iterator
     * @brief A forward iterator for the FastList.
     *
     * This iterator allows traversal of the list in one direction.
     */
    class iterator {
        SkipNode* current_node_;

    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type        = KeyType;
        using reference         = KeyType&;
        using pointer           = KeyType*;
        using difference_type   = std::ptrdiff_t;

        /**
         * @brief Constructs an iterator.
         * @param node A pointer to the node the iterator should point to.
         */
        explicit iterator(SkipNode* node = nullptr) : current_node_(node) {}

        /// @brief Dereferences the iterator to access the node's value.
        reference operator*() const { return current_node_->value; }

        /// @brief Accesses a member of the node's value.
        pointer operator->() const { return &current_node_->value; }

        /// @brief Advances the iterator to the next node (prefix).
        iterator& operator++() {
            if (current_node_) current_node_ = current_node_->forward[0];
            return *this;
        }

        /// @brief Advances the iterator to the next node (postfix).
        iterator operator++(int) {
            iterator temp = *this;
            ++(*this);
            return temp;
        }

        /// @brief Compares two iterators for equality.
        friend bool operator==(const iterator& a, const iterator& b) { return a.current_node_ == b.current_node_; }
        
        /// @brief Compares two iterators for inequality.
        friend bool operator!=(const iterator& a, const iterator& b) { return a.current_node_ != b.current_node_; }
    };

    /**
     * @brief Finds an element with a specific value.
     * * @param value The value to find.
     * @return An iterator to the found element, or `end()` if the element is not found.
     */
    iterator find(const KeyType& value) const {
        SkipNode* current = sentinel_head_;
        for (int i = current_height_ - 1; i >= 0; --i) {
            while (current->forward[i] && current->forward[i]->value < value) {
                current = current->forward[i];
            }
        }
        current = current->forward[0];
        if (current && current->value == value) {
            return iterator(current);
        }
        return end();
    }

    /**
     * @brief Retrieves the total number of elements in the list.
     * @return The current size of the list.
     */
    size_t size() const { return element_count_; }

    /**
     * @brief Checks if the list contains any elements.
     * @return `true` if the list is empty, `false` otherwise.
     */
    bool empty() const { return element_count_ == 0; }

    /**
     * @brief Returns an iterator to the first element of the list.
     */
    iterator begin() const { return iterator(sentinel_head_->forward[0]); }
    
    /**
     * @brief Returns an iterator pointing past the last element of the list.
     */
    iterator end() const { return iterator(nullptr); }
};

#endif // SKIP_LIST_HPP
