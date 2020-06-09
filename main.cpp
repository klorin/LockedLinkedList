#include <iostream>
#include "ThreadSafeList.h"
int main() {
    //std::cout << "Hello, World!" << std::endl;
    List<int> l1;
    l1.insert(6);
    //l1.remove(6);

    l1.insert(25);
    l1.insert(500);
    l1.insert(100);
    l1.remove(500);

    l1.insert(27);
    l1.insert(1);
    l1.insert(3);
    l1.insert(6);
    l1.remove(1);
    l1.remove(8);
    unsigned int checker = l1.getSize();
    l1.print();
    return 0;
}