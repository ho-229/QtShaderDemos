#include "widget.h"

#include <QVector3D>
#include <QMatrix4x4>
#include <QVariantAnimation>

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

constexpr GLfloat cubePositions[][3] = {
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

Widget::Widget(QWidget *parent)
    : QOpenGLWidget(parent)
    , m_texWall(QOpenGLTexture::Target2D)
    , m_imageWall(":/wall.jpg")
    , m_texFace(QOpenGLTexture::Target2D)
    , m_imageFace(":/awesomeface.png")
    , m_animation(new QVariantAnimation(this))
{
    QSurfaceFormat format;
    format.setSamples(8);
    format.setProfile(QSurfaceFormat::CoreProfile);

    this->setFormat(format);

    m_animation->setStartValue(0);
    m_animation->setEndValue(360);
    m_animation->setDuration(3000);
    m_animation->setLoopCount(-1);
    QObject::connect(m_animation, &QVariantAnimation::valueChanged, this, [this](const QVariant &value) {
        m_routeAngle = value.toInt();
        this->repaint();
    });
}

Widget::~Widget()
{

}

void Widget::closeEvent(QCloseEvent *)
{
    m_animation->stop();

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

    QMatrix4x4 view;
    view.optimize();
    view.translate(0, 0, -5);
    m_program.setUniformValue(3, view);

    m_program.release();

    m_animation->start();
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
}

void Widget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_program.bind();
    m_vao.bind();
    m_texWall.bind(0);
    m_texFace.bind(1);

    for(size_t i = 0; i < 10; ++i)
    {
        QMatrix4x4 model;
        model.optimize();
        model.translate(cubePositions[i][0], cubePositions[i][1], cubePositions[i][2]);
        model.rotate(m_routeAngle + 20 * i, 1, 1, 1);
        m_program.setUniformValue(2, model);

        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    m_vao.release();
    m_texWall.release(0);
    m_texFace.release(1);
    m_program.release();
}
