#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>
#include <QTimer>

class GLWidget : public QGLWidget
{
    Q_OBJECT

public:
    explicit GLWidget(QWidget *parent = 0);
    ~GLWidget();

    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();
    void printCube(float offsetX, float offsetY, float offsetZ);
    void printLevel(int width, float y);
    void printPyramid(int height);
    void printFloor(int width);

    float horizontalAngle, verticalAngle, positionX, positionZ;
    int zoomAmount, levelAmount;
    bool wired;

private:
    QTimer timer;
};

#endif // GLWIDGET_H
