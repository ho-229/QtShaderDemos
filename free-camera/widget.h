#ifndef WIDGET_H
#define WIDGET_H

#include <QVector3D>
#include <QOpenGLWidget>
#include <QOpenGLTexture>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions_4_4_Core>
#include <QOpenGLVertexArrayObject>

class Widget : public QOpenGLWidget, private QOpenGLFunctions_4_4_Core
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    virtual ~Widget() override;

protected:
    void closeEvent(QCloseEvent *) override;
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    void keyPressEvent(QKeyEvent *) override;
    void mouseMoveEvent(QMouseEvent *) override;
    void mousePressEvent(QMouseEvent *) override;
    void wheelEvent(QWheelEvent *) override;

private:
    QOpenGLShaderProgram m_program;

    QOpenGLVertexArrayObject m_vao;

    QOpenGLBuffer m_vbo;

    QOpenGLTexture m_texWall;
    QImage m_imageWall;

    QOpenGLTexture m_texFace;
    QImage m_imageFace;

    QVector3D m_cameraFront = {0, 0, -1};
    QVector3D m_cameraPosition = {0, 0, 3};

    float m_yaw = -90;
    float m_pitch = 0;
    int m_fov = 45;

    QPoint m_center;
};
#endif // WIDGET_H
