// StudentRepository.h
// Handles ALL file input/output. The GUI never touches the file directly.
//
// File format (students.txt) - one record per line, fields separated by commas:
//     Name,RollNo,FatherName,Caste,District,GPA
#pragma once
#include <string>
#include <vector>
#include "Student.h"

class StudentRepository {
public:
    explicit StudentRepository(std::string filePath);

    // Reads the file into memory. A missing file is fine (empty database).
    // Lines that are damaged are skipped and counted in skippedLines().
    bool load();

    int  count() const { return static_cast<int>(students_.size()); }
    int  skippedLines() const { return skippedLines_; }
    const Student& at(int index) const { return students_.at(index); }
    int  indexOf(const std::string& rollNo) const;   // -1 if not found

    // Each of these changes the file. They return false and fill 'error' on failure.
    bool add(const Student& s, std::string& error);
    bool update(const Student& s, std::string& error);          // matched by roll number
    bool remove(const std::string& rollNo, std::string& error);

private:
    bool save(std::string& error) const;   // rewrites the whole file safely

    std::string filePath_;
    std::vector<Student> students_;
    int skippedLines_ = 0;
};
