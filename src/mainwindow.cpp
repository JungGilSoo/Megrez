

MainWindow* theMainWindow = 0;

void ensureMakeDir(const QString& filepath)
{

}


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    theMainWindow = this;

}