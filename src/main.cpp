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

    // Start game engine when user clicks "开始游戏" on menu
    QObject::connect(&window, &MainWindow::gameStarted, [&engine]() {
        engine.start();
    });

    window.show();

    return app.exec();
}
