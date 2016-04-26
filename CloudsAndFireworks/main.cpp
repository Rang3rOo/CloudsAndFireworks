#include "window.h"
#include <QApplication>

const uint SCREEN_WIDTH = 1280;
const uint SCREEN_HEIGHT = 800;

int main(int argc, char *argv[])
{
    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setMajorVersion(3);
    format.setMinorVersion(3);
    QSurfaceFormat::setDefaultFormat(format);

    QApplication app(argc, argv);
    Window window(SCREEN_WIDTH, SCREEN_HEIGHT);
    window.show();
    return app.exec();
}
