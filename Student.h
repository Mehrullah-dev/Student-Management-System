// Student.h
// Plain data structure that represents one student record.
#pragma once
#include <string>

struct Student {
    std::string name;        // Student's full name
    std::string rollNo;      // Unique identifier (primary key)
    std::string fatherName;  // Father's name
    std::string caste;       // Caste
    std::string district;    // District
    double      gpa = 0.0;   // GPA on a 0.0 - 4.0 scale
};
