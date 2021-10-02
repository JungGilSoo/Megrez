
#include <opencv2/core/utils/logger.hpp>

MainWindow* theMainWindow = 0;

void ensureMakeDir(const QString& filepath)
{

}


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    theMainWindow = this;

    ui->setupUi(this);
    // To display OpenCV debug on the console
    cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_ERROR);

    // Define the type & condition of window(display)
    Qt::WindowFlags flags;
    flags = Qt::Dialog | Qt::FramelessWindowHint;
    setWindowFlags(flags);
    this->move(0,0);

    // from here
    m_appPath = QDir::currentPath();
}