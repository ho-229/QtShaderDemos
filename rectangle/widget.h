#ifndef WIDGET_H
#define WIDGET_H

#include <QOpenGLWidget>
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
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

private:
    QOpenGLShaderProgram m_program;

    QOpenGLVertexArrayObject m_vao;

    QOpenGLBuffer m_vbo;
    QOpenGLBuffer m_ebo;
};
#endif // WIDGET_H
