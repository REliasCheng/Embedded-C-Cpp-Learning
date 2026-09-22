#include "student.hpp"

#include <cassert>
#include <iostream>
#include <stdexcept>

int main()
{
    Student student("Zhang", 18);
    assert(student.name() == "Zhang" && student.age() == 18);
    assert(!student.set_age(-1) && student.age() == 18);
    assert(student.set_age(19) && student.age() == 19);

    bool rejected = false;
    try {
        Student invalid("", -1);
    } catch (const std::invalid_argument &) {
        rejected = true;
    }
    assert(rejected);
    std::cout << student.name() << " age=" << student.age() << '\n';
}
