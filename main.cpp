// main.cpp - program entry point
#include <QApplication>
#include <QDir>

#include "MainWindow.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    // Where to keep students.txt:
    //  * normally in the project folder (next to main.cpp), so it is easy to find;
    //  * if that folder does not exist (e.g. the exe was copied to another PC),
    //    next to the executable instead.
    QString dataDir = QCoreApplication::applicationDirPath();
#ifdef DATA_DIR
    if (QDir(QString::fromUtf8(DATA_DIR)).exists())
        dataDir = QString::fromUtf8(DATA_DIR);
#endif

    MainWindow window(QDir(dataDir).filePath("students.txt"));
    window.show();
    return app.exec();
}
