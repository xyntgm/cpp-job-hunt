#include <QApplication>
#include <QFont>

#include "MainWindow.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("devicehmi"));
    app.setOrganizationName(QStringLiteral("cpp-job-hunt"));

    hmi::MainWindow window;
    window.show();

    return app.exec();
}
