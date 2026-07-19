#include "core/GameEngine.h"
#include "ui/MainWindow.h"

#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("Farewell_Trip");
    app.setApplicationVersion("1.0.0");

    MainWindow window;
    GameEngine engine(&window);

    window.show();
    engine.start();

    return app.exec();
}
