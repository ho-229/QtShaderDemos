#include "widget.h"

#include <QtMath>
#include <QVector2D>
#include <QVector3D>
#include <QKeyEvent>
#include <QMatrix4x4>
#include <QMouseEvent>
#include <QGuiApplication>

#define PERSPECTIVE_ENABLE 1

constexpr GLfloat vertices[] =  {
    // vertices          texCoord
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
    0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
    0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
    0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};

constexpr QVector3D cubePositions[] = {
    { 0.0f,  0.0f,  0.0f },
    { 2.0f,  5.0f, -15.0f},
    {-1.5f, -2.2f, -2.5f},
    {-3.8f, -2.0f, -12.3f},
    { 2.4f, -0.4f, -3.5f},
    {-1.7f,  3.0f, -7.5f},
    { 1.3f, -2.0f, -2.5f},
    { 1.5f,  2.0f, -2.5f},
    { 1.5f,  0.2f, -1.5f},
    {-1.3f,  1.0f, -1.5f},
};

constexpr QVector3D upNormal = {0, 1, 0};

Widget::Widget(QWidget *parent)
    : QOpenGLWidget(parent)
    , m_texWall(QOpenGLTexture::Target2D)
    , m_imageWall(":/wall.jpg")
    , m_texFace(QOpenGLTexture::Target2D)
    , m_imageFace(":/awesomeface.png")
{
    QSurfaceFormat format;
    format.setSamples(8);
    format.setProfile(QSurfaceFormat::CoreProfile);

    this->setFormat(format);

    qInfo() << "Press ESC to escape.";
}

Widget::~Widget()
{
    if(this->hasMouseTracking())
        QGuiApplication::restoreOverrideCursor();
}

void Widget::closeEvent(QCloseEvent *)
{
    m_vao.destroy();
    m_vbo.destroy();
    m_texWall.destroy();
    m_texFace.destroy();
}

void Widget::initializeGL()
{
    this->initializeOpenGLFunctions();

    glClearColor(.1f, .1f, .1f, .1f);
    glEnable(GL_DEPTH_TEST);

    if(!m_program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader/fragment.fsh") ||
        !m_program.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader/vertex.vsh"))
    {
        qDebug() << m_program.log();
        return;
    }

    if(!m_program.link() || !m_program.bind())
    {
        qDebug() << m_program.log();
        return;
    }

    m_vbo.create();
    m_vbo.bind();
    m_vbo.allocate(vertices, sizeof(vertices));
    m_vbo.setUsagePattern(QOpenGLBuffer::StaticDraw);

    m_vao.create();
    m_vao.bind();

    // vertex
    m_program.setAttributeBuffer(0, GL_FLOAT, 0 * sizeof(GLfloat), 3, 5 * sizeof(GLfloat));
    m_program.enableAttributeArray(0);

    // texCoord
    m_program.setAttributeBuffer(1, GL_FLOAT, 3 * sizeof(GLfloat), 2, 5 * sizeof(GLfloat));
    m_program.enableAttributeArray(1);

    m_vao.release();
    m_vbo.release();

    m_texWall.create();
    m_texWall.setWrapMode(QOpenGLTexture::Repeat);
    m_texWall.setMinMagFilters(QOpenGLTexture::Linear, QOpenGLTexture::Linear);
    m_texWall.setData(m_imageWall);
    m_program.setUniformValue(0, 0);

    m_texFace.create();
    m_texFace.setWrapMode(QOpenGLTexture::Repeat);
    m_texFace.setMinMagFilters(QOpenGLTexture::Linear, QOpenGLTexture::Linear);
    m_texFace.setData(m_imageFace);
    m_program.setUniformValue(1, 1);

    m_program.release();
}

void Widget::resizeGL(int w, int h)
{
    m_program.bind();
    QMatrix4x4 projection;
#if PERSPECTIVE_ENABLE
    projection.perspective(45, static_cast<double>(this->width()) / this->height(), .1, 100);
#else
    qreal aspect = static_cast<double>(this->width()) / this->height();
    projection.ortho(-3 * aspect, 3 * aspect, -3, 3, .1, 100);
#endif
    m_program.setUniformValue(4, projection);
    m_program.release();

    m_center = {w / 2, h / 2};
}

void Widget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_program.bind();
    m_vao.bind();
    m_texWall.bind(0);
    m_texFace.bind(1);

    QMatrix4x4 view;
    view.lookAt(m_cameraPosition, m_cameraPosition + m_cameraFront, upNormal);
    m_program.setUniformValue(3, view);

    for(size_t i = 0; i < 10; ++i)
    {
        QMatrix4x4 model;
        model.optimize();
        model.translate(cubePositions[i]);
        model.rotate(20 * i, 1, 1, 1);
        m_program.setUniformValue(2, model);

        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    m_vao.release();
    m_texWall.release(0);
    m_texFace.release(1);
    m_program.release();
}

void Widget::keyPressEvent(QKeyEvent *key)
{
    constexpr float cameraSpeed = 0.3;
    switch (key->key())
    {
    case Qt::Key_W:
        m_cameraPosition += m_cameraFront * cameraSpeed;
        break;
    case Qt::Key_S:
        m_cameraPosition -= m_cameraFront * cameraSpeed;
        break;
    case Qt::Key_A:
        m_cameraPosition -= QVector3D::crossProduct(m_cameraFront, upNormal) * cameraSpeed;
        break;
    case Qt::Key_D:
        m_cameraPosition += QVector3D::crossProduct(m_cameraFront, upNormal) * cameraSpeed;
        break;
    case Qt::Key_Space:
        m_cameraPosition += upNormal * cameraSpeed;
        break;
    case Qt::Key_Shift:
        m_cameraPosition -= upNormal * cameraSpeed;
        break;
    case Qt::Key_Super_L:
    case Qt::Key_Escape:
        if(this->hasMouseTracking())
        {
            this->setMouseTracking(false);
            QGuiApplication::restoreOverrideCursor();
        }
        Q_FALLTHROUGH();
    default:
        return;
    }

    this->repaint();
}

void Widget::mouseMoveEvent(QMouseEvent *pos)
{
    QVector2D offset = {static_cast<float>(pos->x() - m_center.x()), static_cast<float>(m_center.y() - pos->y())};

    constexpr float sensitivity = 0.15;
    offset *= sensitivity;

    m_yaw += offset.x();
    m_pitch += offset.y();

    if(m_pitch > 89)
        m_pitch = 89;
    else if(m_pitch < -89)
        m_pitch = -89;

    m_cameraFront.setX(qCos(qDegreesToRadians(m_yaw)) * qCos(qDegreesToRadians(m_pitch)));
    m_cameraFront.setY(qSin(qDegreesToRadians(m_pitch)));
    m_cameraFront.setZ(qSin(qDegreesToRadians(m_yaw)) * qCos(qDegreesToRadians(m_pitch)));
    m_cameraFront.normalize();

    QCursor::setPos(this->mapToGlobal(m_center));
    this->repaint();
}

void Widget::mousePressEvent(QMouseEvent *)
{
    if(!this->hasMouseTracking())
    {
        QGuiApplication::setOverrideCursor(Qt::BlankCursor);
        QCursor::setPos(this->mapToGlobal(m_center));
        this->setMouseTracking(true);
    }
}
