// StudentRepository.cpp
#include "StudentRepository.h"

#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <utility>

#include "Validation.h"

StudentRepository::StudentRepository(std::string filePath)
    : filePath_(std::move(filePath)) {}

// ---------------------------------------------------------------- load ----
bool StudentRepository::load() {
    students_.clear();
    skippedLines_ = 0;

    std::ifstream in(filePath_);
    if (!in.is_open()) return true;           // no file yet -> empty database

    std::string line;
    while (std::getline(in, line)) {
        line = validation::trim(line);
        if (line.empty()) continue;

        // Split the line at every comma.
        std::vector<std::string> f;
        std::stringstream ss(line);
        std::string token;
        while (std::getline(ss, token, ',')) f.push_back(validation::trim(token));

        double gpa = 0.0;
        if (f.size() != 6 || !validation::parseGpa(f[5], gpa)) {
            ++skippedLines_;                  // damaged line: ignore it
            continue;
        }
        students_.push_back({f[0], f[1], f[2], f[3], f[4], gpa});
    }
    return true;
}

// ----------------------------------------------------------------- find ---
int StudentRepository::indexOf(const std::string& rollNo) const {
    for (int i = 0; i < count(); ++i)
        if (students_[i].rollNo == rollNo) return i;
    return -1;
}

// ------------------------------------------------------------------ add ---
bool StudentRepository::add(const Student& s, std::string& error) {
    if (indexOf(s.rollNo) != -1) {
        error = "A student with Roll Number " + s.rollNo + " already exists.";
        return false;
    }
    students_.push_back(s);
    if (!save(error)) {
        students_.pop_back();                 // undo in memory if the file failed
        return false;
    }
    return true;
}

// --------------------------------------------------------------- update ---
bool StudentRepository::update(const Student& s, std::string& error) {
    const int i = indexOf(s.rollNo);
    if (i == -1) {
        error = "No student with Roll Number " + s.rollNo + " was found.";
        return false;
    }
    const Student backup = students_[i];
    students_[i] = s;
    if (!save(error)) {
        students_[i] = backup;
        return false;
    }
    return true;
}

// --------------------------------------------------------------- remove ---
bool StudentRepository::remove(const std::string& rollNo, std::string& error) {
    const int i = indexOf(rollNo);
    if (i == -1) {
        error = "No student with Roll Number " + rollNo + " was found.";
        return false;
    }
    const Student backup = students_[i];
    students_.erase(students_.begin() + i);
    if (!save(error)) {
        students_.insert(students_.begin() + i, backup);
        return false;
    }
    return true;
}

// ----------------------------------------------------------------- save ---
// Writes to a temporary file first, then replaces the real file. If the
// program crashes half-way, the old data file is still intact.
bool StudentRepository::save(std::string& error) const {
    const std::string tmpPath = filePath_ + ".tmp";
    {
        std::ofstream out(tmpPath, std::ios::trunc);
        if (!out.is_open()) {
            error = "Could not open the data file for writing.";
            return false;
        }
        for (const Student& s : students_) {
            out << s.name << ',' << s.rollNo << ',' << s.fatherName << ','
                << s.caste << ',' << s.district << ','
                << std::fixed << std::setprecision(2) << s.gpa << '\n';
        }
        out.flush();
        if (!out) {
            error = "Error while writing to the data file.";
            return false;
        }
    }
    std::error_code ec;
    std::filesystem::rename(tmpPath, filePath_, ec);
    if (ec) {
        error = "Could not replace the data file: " + ec.message();
        return false;
    }
    return true;
}
