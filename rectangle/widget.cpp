#include "widget.h"

#define USE_VAO 1

Widget::Widget(QWidget *parent)
    : QOpenGLWidget(parent),
    m_ebo(QOpenGLBuffer::IndexBuffer)
{
    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setSamples(8);
    format.setProfile(QSurfaceFormat::CoreProfile);

    this->setFormat(format);
}

Widget::~Widget()
{

}

void Widget::closeEvent(QCloseEvent *)
{
    m_vao.destroy();
    m_vbo.destroy();
    m_ebo.destroy();
}

void Widget::initializeGL()
{
    this->initializeOpenGLFunctions();

    glClearColor(.1f, .1f, .1f, .1f);

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
        -0.5f, 0.5f,    // left top
        -0.5f, -0.5f,   // left bottom

        1, 0, 0,        // red
        0, 1, 0,        // green
        0, 0, 1,        // blue
        1, 1, 0,        // yellow
    };
    m_vbo.create();
    m_vbo.bind();
    m_vbo.allocate(vertices, sizeof(vertices) * sizeof(GLfloat));
    m_vbo.setUsagePattern(QOpenGLBuffer::StaticDraw);

#if USE_VAO
    m_vao.create();
    m_vao.bind();

    // vertex
    m_program.setAttributeBuffer(0, GL_FLOAT, 0, 2);
    m_program.enableAttributeArray(0);

    // color
    m_program.setAttributeBuffer(1, GL_FLOAT, 8 * sizeof(GLfloat), 3);
    m_program.enableAttributeArray(1);

    m_vao.release();
#endif
    m_vbo.release();

    unsigned int indices[] = {
        // first triangle
        0,     // right top
        2,     // left top
        3,     // left bottom

        // second triangle
        3,     // left bottom
        1,     // right bottom
        0,     // right top
    };
    m_ebo.create();
    m_ebo.bind();
    m_ebo.allocate(indices, sizeof(indices) * sizeof(unsigned));
    m_ebo.release();

    m_program.release();
}

void Widget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);

    m_program.bind();
#if USE_VAO
    m_vao.bind();
#else
    m_vbo.bind();

    // vertex
    m_program.setAttributeBuffer(0, GL_FLOAT, 0, 2);
    m_program.enableAttributeArray(0);

    // color
    m_program.setAttributeBuffer(1, GL_FLOAT, 8 * sizeof(GLfloat), 3);
    m_program.enableAttributeArray(1);
#endif
    m_ebo.bind();

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    m_vao.release();
    m_ebo.release();
    m_program.release();
}
