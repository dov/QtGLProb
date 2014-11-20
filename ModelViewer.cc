#include <QApplication>
#include "ModelViewerWidget.h"

using namespace std;

class MyApp : public QApplication {
public:
    MyApp(int argc, char *argv[]);

private:
    ModelViewerWidget *mv {nullptr};
};

MyApp::MyApp(int argc, char *argv[])
    : QApplication(argc, argv)
{
    mv = new ModelViewerWidget;
    mv->show();
}

int main(int argc, char **argv)
{
  MyApp app(argc, argv);
  return app.exec();
}

