// MainWindow.cpp
#include "MainWindow.h"

#include <QDesktopServices>
#include <QDir>
#include <QFileInfo>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QStatusBar>
#include <QUrl>
#include <QVBoxLayout>
#include <QWidget>
#include <algorithm>

#include "Validation.h"

namespace {
// Small helper: std::string -> QString
QString q(const std::string& s) { return QString::fromStdString(s); }
}  // namespace

// ------------------------------------------------------- constructor ------
MainWindow::MainWindow(const QString& dataFilePath, QWidget* parent)
    : QMainWindow(parent),
      dataFilePath_(QDir::toNativeSeparators(dataFilePath)),
      repo_(dataFilePath.toLocal8Bit().toStdString()) {
    repo_.load();
    buildUi();
    setMode(Mode::Add);                       // application start-up state
    statusBar()->showMessage("Data file: " + dataFilePath_);   // tells the user where records are saved

    if (repo_.skippedLines() > 0) {
        QMessageBox::warning(this, "Data file",
            QString("%1 damaged line(s) in the data file were ignored.")
                .arg(repo_.skippedLines()));
    }
}

// ------------------------------------------------------------ GUI ---------
void MainWindow::buildUi() {
    setWindowTitle("Student Record");
    setMinimumWidth(620);

    auto* central = new QWidget(this);
    setCentralWidget(central);

    auto* title = new QLabel("Student Record Management System");
    title->setObjectName("title");
    title->setAlignment(Qt::AlignCenter);

    counterLabel_ = new QLabel;
    counterLabel_->setObjectName("counter");
    counterLabel_->setAlignment(Qt::AlignCenter);

    // --- the six labelled text fields -------------------------------------
    nameEdit_     = new QLineEdit;
    rollEdit_     = new QLineEdit;
    fatherEdit_   = new QLineEdit;
    casteEdit_    = new QLineEdit;
    districtEdit_ = new QLineEdit;
    gpaEdit_      = new QLineEdit;
    nameEdit_->setPlaceholderText("e.g. Ali Khan");
    rollEdit_->setPlaceholderText("e.g. 26SW001 (must be unique)");
    fatherEdit_->setPlaceholderText("e.g. Ahmed Khan");
    casteEdit_->setPlaceholderText("e.g. Khan");
    districtEdit_->setPlaceholderText("e.g. Khairpur");
    gpaEdit_->setPlaceholderText("0.00 - 4.00");

    auto* form = new QFormLayout;
    form->setLabelAlignment(Qt::AlignRight);
    form->setVerticalSpacing(10);
    form->addRow("Student Name:", nameEdit_);
    form->addRow("Roll Number:",  rollEdit_);
    form->addRow("Father's Name:", fatherEdit_);
    form->addRow("Caste:",        casteEdit_);
    form->addRow("District:",     districtEdit_);
    form->addRow("GPA:",          gpaEdit_);

    // --- buttons ----------------------------------------------------------
    addBtn_    = new QPushButton("Add");
    updateBtn_ = new QPushButton("Update");
    deleteBtn_ = new QPushButton("Delete");
    nextBtn_   = new QPushButton("Next  \u25B6");
    prevBtn_   = new QPushButton("\u25C0  Previous");
    newBtn_    = new QPushButton("New Record");
    openBtn_   = new QPushButton("Open in Notepad");
    deleteBtn_->setObjectName("deleteButton");
    addBtn_->setDefault(true);

    auto* navRow = new QHBoxLayout;
    navRow->addWidget(prevBtn_);
    navRow->addWidget(nextBtn_);
    navRow->addStretch();
    navRow->addWidget(newBtn_);
    navRow->addWidget(openBtn_);

    auto* actionRow = new QHBoxLayout;
    actionRow->addWidget(addBtn_);
    actionRow->addWidget(updateBtn_);
    actionRow->addWidget(deleteBtn_);

    auto* layout = new QVBoxLayout(central);
    layout->setContentsMargins(24, 18, 24, 14);
    layout->setSpacing(12);
    layout->addWidget(title);
    layout->addWidget(counterLabel_);
    layout->addLayout(form);
    layout->addLayout(navRow);
    layout->addLayout(actionRow);

    // --- connect each button click to its handler -------------------------
    connect(addBtn_,    &QPushButton::clicked, this, &MainWindow::onAdd);
    connect(updateBtn_, &QPushButton::clicked, this, &MainWindow::onUpdate);
    connect(deleteBtn_, &QPushButton::clicked, this, &MainWindow::onDelete);
    connect(nextBtn_,   &QPushButton::clicked, this, &MainWindow::onNext);
    connect(prevBtn_,   &QPushButton::clicked, this, &MainWindow::onPrevious);
    connect(newBtn_,    &QPushButton::clicked, this, &MainWindow::onNewRecord);
    connect(openBtn_,   &QPushButton::clicked, this, &MainWindow::onOpenFile);

    setStyleSheet(R"(
        QMainWindow, QWidget { background: #eef2f7; font-size: 10.5pt; }
        QLabel#title   { font-size: 16pt; font-weight: bold; color: #1f3a5f; }
        QLabel#counter { color: #55657a; font-style: italic; }
        QLineEdit { background: white; border: 1px solid #b8c2d0;
                    border-radius: 4px; padding: 5px; }
        QLineEdit:focus     { border: 1px solid #2f6fdb; }
        QLineEdit:read-only { background: #e1e6ee; color: #55657a; }
        QPushButton { background: #2f6fdb; color: white; border: none;
                      border-radius: 4px; padding: 7px 18px; min-width: 80px; }
        QPushButton:hover    { background: #2459b3; }
        QPushButton:disabled { background: #b8c2d0; color: #eef2f7; }
        QPushButton#deleteButton       { background: #d64545; }
        QPushButton#deleteButton:hover { background: #b03636; }
        QPushButton#deleteButton:disabled { background: #b8c2d0; }
    )");
}

// ------------------------------------------------------- mode handling ----
void MainWindow::setMode(Mode mode) {
    mode_ = mode;
    const bool adding = (mode == Mode::Add);

    addBtn_->setEnabled(adding);
    updateBtn_->setEnabled(!adding);
    deleteBtn_->setEnabled(!adding);
    prevBtn_->setEnabled(!adding);
    newBtn_->setEnabled(!adding);

    // The roll number identifies the record, so it cannot be edited
    // while an existing record is being shown.
    rollEdit_->setReadOnly(!adding);

    refreshStatus();
}

void MainWindow::refreshStatus() {
    const int n = repo_.count();
    nextBtn_->setEnabled(n > 0);
    if (mode_ == Mode::Add)
        counterLabel_->setText(QString("New record  \u2022  %1 stored").arg(n));
    else
        counterLabel_->setText(QString("Record %1 of %2").arg(current_ + 1).arg(n));
}

void MainWindow::clearFields() {
    nameEdit_->clear();
    rollEdit_->clear();
    fatherEdit_->clear();
    casteEdit_->clear();
    districtEdit_->clear();
    gpaEdit_->clear();
}

void MainWindow::showRecord(int index) {
    const Student& s = repo_.at(index);
    current_ = index;
    nameEdit_->setText(q(s.name));
    rollEdit_->setText(q(s.rollNo));
    fatherEdit_->setText(q(s.fatherName));
    casteEdit_->setText(q(s.caste));
    districtEdit_->setText(q(s.district));
    gpaEdit_->setText(QString::number(s.gpa, 'f', 2));
    setMode(Mode::Browse);
}

// Reads the form, validates it and shows a message box if something is wrong.
bool MainWindow::readForm(Student& out) {
    const std::string error = validation::validateFields(
        nameEdit_->text().toStdString(),     rollEdit_->text().toStdString(),
        fatherEdit_->text().toStdString(),   casteEdit_->text().toStdString(),
        districtEdit_->text().toStdString(), gpaEdit_->text().toStdString(), out);
    if (!error.empty()) {
        QMessageBox::warning(this, "Invalid input", q(error));
        return false;
    }
    return true;
}

// ------------------------------------------------------------ slots -------
void MainWindow::onAdd() {
    Student s;
    if (!readForm(s)) return;

    std::string error;
    if (!repo_.add(s, error)) {
        QMessageBox::warning(this, "Cannot add", q(error));
        return;
    }
    QMessageBox::information(this, "Success",
        QString("Record of %1 (Roll No %2) was added successfully.\n\nSaved in:\n%3")
            .arg(q(s.name), q(s.rollNo), dataFilePath_));
    clearFields();
    nameEdit_->setFocus();
    refreshStatus();
}

void MainWindow::onUpdate() {
    Student s;
    if (!readForm(s)) return;

    std::string error;
    if (!repo_.update(s, error)) {
        QMessageBox::warning(this, "Cannot update", q(error));
        return;
    }
    QMessageBox::information(this, "Success",
        QString("Record with Roll No %1 was updated.").arg(q(s.rollNo)));
    refreshStatus();
}

void MainWindow::onDelete() {
    const std::string roll = rollEdit_->text().toStdString();
    const auto answer = QMessageBox::question(this, "Confirm delete",
        QString("Delete the record with Roll No %1?\nThis cannot be undone.")
            .arg(q(roll)));
    if (answer != QMessageBox::Yes) return;

    std::string error;
    if (!repo_.remove(roll, error)) {
        QMessageBox::warning(this, "Cannot delete", q(error));
        return;
    }
    QMessageBox::information(this, "Deleted", "The record was deleted.");

    if (repo_.count() == 0) {                 // nothing left to browse
        clearFields();
        current_ = -1;
        setMode(Mode::Add);
    } else {                                  // show the neighbouring record
        showRecord(std::min(current_, repo_.count() - 1));
    }
}

void MainWindow::onNext() {
    const int n = repo_.count();
    if (n == 0) {
        QMessageBox::information(this, "No records", "There are no stored records yet.");
        return;
    }
    const int index = (mode_ == Mode::Add) ? 0 : (current_ + 1) % n;   // wraps around
    showRecord(index);
}

void MainWindow::onPrevious() {
    const int n = repo_.count();
    if (n == 0) return;
    showRecord((current_ - 1 + n) % n);
}

void MainWindow::onNewRecord() {
    clearFields();
    current_ = -1;
    setMode(Mode::Add);
    nameEdit_->setFocus();
}

// Opens the data file with the default text editor (Notepad on Windows).
void MainWindow::onOpenFile() {
    if (!QFileInfo::exists(dataFilePath_)) {
        QMessageBox::information(this, "No file yet",
            "students.txt is created when you add the first record.\n\nExpected location:\n" + dataFilePath_);
        return;
    }
    if (!QDesktopServices::openUrl(QUrl::fromLocalFile(dataFilePath_)))
        QMessageBox::warning(this, "Cannot open", "Could not open the file:\n" + dataFilePath_);
}
