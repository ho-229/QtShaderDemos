#include "widget.h"

#include <QMatrix4x4>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QSequentialAnimationGroup>

Widget::Widget(QWidget *parent)
    : QOpenGLWidget(parent)
    , m_ebo(QOpenGLBuffer::IndexBuffer)
    , m_texWall(QOpenGLTexture::Target2D)
    , m_imageWall(":/wall.jpg")
    , m_texFace(QOpenGLTexture::Target2D)
    , m_imageFace(":/awesomeface.png")
    , m_animation(new QParallelAnimationGroup(this))
{
    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setSamples(8);
    format.setProfile(QSurfaceFormat::CoreProfile);

    this->setFormat(format);

    auto scaleAnimation = new QSequentialAnimationGroup;
    auto scale1 = new QPropertyAnimation(this, "scale");
    scale1->setStartValue(1);
    scale1->setEndValue(0.5);
    scale1->setDuration(1500);
    scaleAnimation->addAnimation(scale1);

    auto scale2 = new QPropertyAnimation(this, "scale");
    scale2->setStartValue(0.5);
    scale2->setEndValue(1);
    scale2->setDuration(1500);
    scaleAnimation->addAnimation(scale2);

    auto routeAnimation = new QVariantAnimation;
    routeAnimation->setStartValue(0);
    routeAnimation->setEndValue(360);
    routeAnimation->setDuration(3000);
    QObject::connect(routeAnimation, &QVariantAnimation::valueChanged, this, [this](const QVariant &value) {
        m_routeAngle = value.toInt();
        this->repaint();
    });

    m_animation->addAnimation(scaleAnimation);
    m_animation->addAnimation(routeAnimation);
    m_animation->setLoopCount(-1);
}

Widget::~Widget()
{

}

void Widget::closeEvent(QCloseEvent *)
{
    m_animation->stop();

    m_vao.destroy();
    m_vbo.destroy();
    m_ebo.destroy();
    m_texWall.destroy();
    m_texFace.destroy();
}

void Widget::initializeGL()
{
    this->initializeOpenGLFunctions();

    glClearColor(.1f, .1f, .1f, .1f);
    glEnable(GL_MULTISAMPLE);

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

    GLfloat vertices[] = {
        0.5f, 0.5f,     // right top
        0.5f, -0.5f,    // right bottom
        -0.5f, -0.5f,   // left bottom
        -0.5f, 0.5f,    // left top

        1, 0, 0,        // red
        0, 1, 0,        // green
        0, 0, 1,        // blue
        1, 1, 0,        // red

        // texCoord
        1, 0,
        1, 1,
        0, 1,
        0, 0,
    };
    m_vbo.create();
    m_vbo.bind();
    m_vbo.allocate(vertices, sizeof(vertices));
    m_vbo.setUsagePattern(QOpenGLBuffer::StaticDraw);

    m_vao.create();
    m_vao.bind();

    // vertex
    m_program.setAttributeBuffer(0, GL_FLOAT, 0, 2);
    m_program.enableAttributeArray(0);

    // color
    m_program.setAttributeBuffer(1, GL_FLOAT, 8 * sizeof(GLfloat), 3);
    m_program.enableAttributeArray(1);

    // texCoord
    m_program.setAttributeBuffer(2, GL_FLOAT, 20 * sizeof(GLfloat), 2);
    m_program.enableAttributeArray(2);

    m_vao.release();
    m_vbo.release();

    unsigned int indices[] = {
        // first triangle
        0,     // right top
        1,     // left top
        3,     // left bottom

        // second triangle
        1,     // left bottom
        2,     // right bottom
        3,     // right top
    };
    m_ebo.create();
    m_ebo.bind();
    m_ebo.allocate(indices, sizeof(indices));
    m_ebo.release();

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

    m_animation->start();
}

void Widget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);

    m_program.bind();
    m_vao.bind();
    m_ebo.bind();
    m_texWall.bind(0);
    m_texFace.bind(1);

    QMatrix4x4 trans;
    trans.optimize();
    trans.scale(m_scale);
    trans.rotate(m_routeAngle, 1, 1, 1);
    m_program.setUniformValue(2, trans);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    m_vao.release();
    m_ebo.release();
    m_texWall.release(0);
    m_texFace.release(1);
    m_program.release();
}
