#ifndef PRACTICE_STUDENT_HPP
#define PRACTICE_STUDENT_HPP

#include <string>

class Student {
public:
    Student(std::string name, int age);
    const std::string &name() const;
    int age() const;
    bool set_age(int age);

private:
    std::string name_;
    int age_;
};

#endif
