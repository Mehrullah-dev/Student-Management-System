cmake_minimum_required(VERSION 3.16)
project(StudentRecordManager LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

# Works with Qt 6 or Qt 5
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

add_executable(StudentRecordManager
    main.cpp
    MainWindow.cpp
    MainWindow.h
    StudentRepository.cpp
    StudentRepository.h
    Student.h
    Validation.h
)
target_link_libraries(StudentRecordManager PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# students.txt is saved in the project folder (next to main.cpp)
target_compile_definitions(StudentRecordManager PRIVATE DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}")

if(WIN32)
    set_target_properties(StudentRecordManager PROPERTIES WIN32_EXECUTABLE ON)
endif()
