# Student Record Management System (C++ / Qt)

GUI application for adding, updating, deleting and browsing student records.
Records are stored in `students.txt` (created in the project folder next to `main.cpp`; click **Open in Notepad** in the app to view it), one per line:

    Name,RollNo,FatherName,Caste,District,GPA

## Files
| File | Purpose |
|---|---|
| `main.cpp` | Starts the Qt application |
| `MainWindow.h/.cpp` | GUI form, buttons and event handlers |
| `StudentRepository.h/.cpp` | All file reading/writing (add, update, delete, load) |
| `Validation.h` | Input checks (empty fields, commas, GPA range) |
| `Student.h` | Student record structure |
| `tests/test_repository.cpp` | Console tests for file handling + validation |

## Build (Qt 6 or Qt 5 + CMake)
**Qt Creator:** File > Open File or Project > select `CMakeLists.txt` > Configure > Run.

**Command line:**

    cmake -S . -B build
    cmake --build build
    ./build/StudentRecordManager        (Windows: build\StudentRecordManager.exe)

## Run the console tests (no Qt needed)

    cd tests
    g++ -std=c++17 -I.. test_repository.cpp ../StudentRepository.cpp -o test_repo
    ./test_repo

## Behaviour summary
* Start-up: Add enabled; Update, Delete, Previous, New Record disabled.
* **Next**: shows the next stored record (wraps to the first). Add is disabled; Update/Delete enabled; Roll Number becomes read-only because it is the unique ID.
* **Open in Notepad**: opens `students.txt`; the full path is also shown in the status bar.
* **New Record**: returns to Add mode with an empty form.
* Add rejects duplicate roll numbers; every button validates input first.
