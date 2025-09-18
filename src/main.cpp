#include "skip_list.hpp"
#include <iostream>
#include <cassert>

int main() {
    skip_list<int> list;

    assert(list.insert(10));
    assert(list.insert(20));
    assert(!list.insert(10));

    assert(list.contains(20));
    assert(!list.contains(30));

    assert(list.erase(10));
    assert(!list.erase(10));

    for (int x : list)
        std::cout << x << " ";

    return 0;
}
