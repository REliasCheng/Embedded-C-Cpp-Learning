#include "student.hpp"

#include <stdexcept>
#include <utility>

Student::Student(std::string name, int age)
    : name_(std::move(name)), age_(age)
{
    if (name_.empty() || age_ < 0) {
        throw std::invalid_argument("invalid student");
    }
}

const std::string &Student::name() const
{
    return name_;
}

int Student::age() const
{
    return age_;
}

bool Student::set_age(int age)
{
    if (age < 0) {
        return false;
    }
    age_ = age;
    return true;
}
