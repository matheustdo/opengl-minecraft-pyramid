#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "math.h"
#include "QMouseEvent"
#include "QWheelEvent"

struct pos {
    int x;
    int y;
};

pos lastPos;
bool mouseReleased = true;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {
    ui->setupUi(this);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::mouseMoveEvent(QMouseEvent *event) {
    if(mouseReleased) {
        lastPos.x = event->x();
        lastPos.y = event->y();
    }
    mouseReleased = false;

    int dx = event->x() - lastPos.x;
    int dy = event->y() - lastPos.y;
    float horizontalAngle = this->ui->widget->horizontalAngle;
    float verticalAngle = this->ui->widget->verticalAngle;

    if(event->buttons() == Qt::RightButton){
        this->ui->widget->horizontalAngle = (horizontalAngle*180.0f/M_PI + dx)*M_PI/180.0f;

        if((verticalAngle*180.0f/M_PI + dy)*M_PI/180.0f > 0 && (verticalAngle*180.0f/M_PI + dy)*M_PI/180.0f < M_PI/2) {
            this->ui->widget->verticalAngle = (verticalAngle*180.0f/M_PI + dy)*M_PI/180.0f;
        }
    }
    if(event->buttons() == Qt::LeftButton){
        this->ui->widget->positionZ += -dy/6.5f;
        this->ui->widget->positionX += -dx/6.5f;
    }

    lastPos.x = event->x();
    lastPos.y = event->y();
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event) {
    mouseReleased = true;
}

void MainWindow::wheelEvent(QWheelEvent *event) {
    float zoomFactor = 5;
    float zoomAmount = this->ui->widget->zoomAmount;

    if(event->delta() > 0 && zoomAmount > 10) {
        this->ui->widget->zoomAmount = zoomAmount - zoomFactor;
    } else if (event->delta() < 0 && zoomAmount < 200) {
        this->ui->widget->zoomAmount = zoomAmount + zoomFactor;
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    switch (event->key()) {
    case Qt::Key_Escape:
        close(); // Quit on Escape
        break;
    case Qt::Key_W:
        this->ui->widget->positionZ -= 1;
        break;
    case Qt::Key_S:
        this->ui->widget->positionZ += 1;
        break;
    case Qt::Key_A:
        this->ui->widget->positionX -= 1;
        break;
    case Qt::Key_D:
        this->ui->widget->positionX += 1;
        break;
    case Qt::Key_Plus:
        if(this->ui->widget->levelAmount < 45) {
            this->ui->widget->levelAmount += 1; // Increment level amount number
        }
        break;
    case Qt::Key_Minus:
        if(this->ui->widget->levelAmount > 0) {
            this->ui->widget->levelAmount -= 1; // Decrement level amount number
        }
        break;
    case Qt::Key_F2:
        this->ui->widget->wired = !this->ui->widget->wired; // Toggle wired mode
        break;
    case Qt::Key_F11:
        setWindowState(windowState() ^ Qt::WindowFullScreen); // Toggle fullscreen on F11
        break;
    }
}
