#include <QApplication>
#include <QMainWindow>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QMainWindow window;
    window.setWindowTitle("Qt Monopoly");
    window.resize(800, 800); 
    window.show();

    return app.exec();
}