// Validation.h
// Input validation helpers. They use only the C++ standard library so the
// same rules can be unit-tested without starting the GUI.
#pragma once
#include <cerrno>
#include <cstdlib>
#include <string>
#include "Student.h"

namespace validation {

// Removes leading/trailing whitespace.
inline std::string trim(const std::string& s) {
    const auto first = s.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    const auto last = s.find_last_not_of(" \t\r\n");
    return s.substr(first, last - first + 1);
}

// Converts text to a number. Returns false if the whole text is not a number.
inline bool parseGpa(const std::string& text, double& out) {
    if (text.empty()) return false;
    char* end = nullptr;
    errno = 0;
    const double v = std::strtod(text.c_str(), &end);
    if (errno != 0 || *end != '\0' || v != v) return false;  // v != v -> NaN
    out = v;
    return true;
}

// Checks one text field. Returns "" when valid, otherwise an error message.
inline std::string checkText(const std::string& label, const std::string& value) {
    if (value.empty())
        return label + " cannot be empty.";
    if (value.find(',') != std::string::npos)
        return label + " must not contain a comma (commas separate the fields in the file).";
    if (value.find_first_of("\r\n") != std::string::npos)
        return label + " must be on a single line.";
    return "";
}

// Validates all six form fields and fills 'out'.
// Returns "" when everything is valid, otherwise the first error found.
inline std::string validateFields(const std::string& name, const std::string& roll,
                                  const std::string& father, const std::string& caste,
                                  const std::string& district, const std::string& gpaText,
                                  Student& out) {
    Student s;
    s.name       = trim(name);
    s.rollNo     = trim(roll);
    s.fatherName = trim(father);
    s.caste      = trim(caste);
    s.district   = trim(district);

    std::string err;
    if (!(err = checkText("Name", s.name)).empty())            return err;
    if (!(err = checkText("Roll Number", s.rollNo)).empty())   return err;
    if (!(err = checkText("Father's Name", s.fatherName)).empty()) return err;
    if (!(err = checkText("Caste", s.caste)).empty())          return err;
    if (!(err = checkText("District", s.district)).empty())    return err;

    if (!parseGpa(trim(gpaText), s.gpa))
        return "GPA must be a number (for example 3.25).";
    if (s.gpa < 0.0 || s.gpa > 4.0)
        return "GPA must be between 0.0 and 4.0.";

    out = s;
    return "";
}

}  // namespace validation
