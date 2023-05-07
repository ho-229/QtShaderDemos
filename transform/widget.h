﻿#ifndef WIDGET_H
#define WIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLTexture>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions_4_4_Core>
#include <QOpenGLVertexArrayObject>

class QCloseEvent;
class QVariantAnimation;
class QSequentialAnimationGroup;
class QParallelAnimationGroup;

class Widget : public QOpenGLWidget, private QOpenGLFunctions_4_4_Core
{
    Q_OBJECT

    Q_PROPERTY(qreal scale MEMBER m_scale)

public:
    explicit Widget(QWidget *parent = nullptr);
    virtual ~Widget() override;

protected:
    void closeEvent(QCloseEvent *) override;
    void initializeGL() override;
    void paintGL() override;

private:
    QOpenGLShaderProgram m_program;

    QOpenGLVertexArrayObject m_vao;

    QOpenGLBuffer m_vbo;
    QOpenGLBuffer m_ebo;

    QOpenGLTexture m_texWall;
    QImage m_imageWall;

    QOpenGLTexture m_texFace;
    QImage m_imageFace;

    QParallelAnimationGroup *m_animation = nullptr;

    qreal m_scale = 0;
    int m_routeAngle = 0;
};
#endif // WIDGET_H
