// MainWindow.h
// The only window of the application (Qt Widgets).
#pragma once
#include <QMainWindow>
#include <QString>

#include "StudentRepository.h"

class QLabel;
class QLineEdit;
class QPushButton;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(const QString& dataFilePath, QWidget* parent = nullptr);

private slots:          // one slot per button = event-driven programming
    void onAdd();
    void onUpdate();
    void onDelete();
    void onNext();
    void onPrevious();
    void onNewRecord();
    void onOpenFile();          // opens students.txt in Notepad

private:
    // Add mode    : empty form, Add enabled, Update/Delete disabled.
    // Browse mode : a stored record is shown, Add disabled, Update/Delete enabled.
    enum class Mode { Add, Browse };

    void buildUi();
    void setMode(Mode mode);
    void showRecord(int index);
    void clearFields();
    void refreshStatus();
    bool readForm(Student& out);   // reads + validates the text fields

    QString dataFilePath_;         // full path of students.txt (shown to the user)
    StudentRepository repo_;
    Mode mode_ = Mode::Add;
    int  current_ = -1;            // index of the record on screen (Browse mode)

    QLineEdit *nameEdit_ = nullptr, *rollEdit_ = nullptr, *fatherEdit_ = nullptr,
              *casteEdit_ = nullptr, *districtEdit_ = nullptr, *gpaEdit_ = nullptr;
    QPushButton *addBtn_ = nullptr, *updateBtn_ = nullptr, *deleteBtn_ = nullptr,
                *nextBtn_ = nullptr, *prevBtn_ = nullptr, *newBtn_ = nullptr, *openBtn_ = nullptr;
    QLabel *counterLabel_ = nullptr;
};
