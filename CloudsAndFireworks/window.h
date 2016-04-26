#ifndef WINDOW_H
#define WINDOW_H

#include <QOpenGLWidget>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLFramebufferObject>
#include <QElapsedTimer>
#include "resourcemanager.h"
#include "glmatrixstack.h"
#include "cloud.h"
#include "firework.h"

class Window : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core
{
    Q_OBJECT

public:
    explicit Window(uint width = 1280, uint height = 800);
    ~Window();

private:
    void initializeGL() Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;
    void resizeGL(int width, int height) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void timerEvent(QTimerEvent *) Q_DECL_OVERRIDE;
    void drawBackground();
    void drawClouds();
    void drawFireworks();
    void drawWater();
    GLfloat calculateRotationAngle(QVector2D vector1, QVector2D vector2);
    float fixBoundary(const float &min, float &value, const float &max);

    uint m_windowWidth;
    uint m_windowHeight;
    uint m_frameCount;

    ResourceManager m_resourceManager;
    QOpenGLShaderProgram *m_cloudProgram;
    QOpenGLShaderProgram *m_fireworkProgram;
    QOpenGLShaderProgram *m_waterProgram;

    QOpenGLTexture *m_backgroundTexture;
    QVector<QOpenGLTexture*> m_cloudTextures;
    QOpenGLTexture *m_circleParticleTexture;
    QOpenGLTexture *m_starParticleTexture;
    QOpenGLTexture *m_explosionTexture;

    QOpenGLVertexArrayObject m_vao;
    QVector<VertexData> m_vertexData;
    QOpenGLBuffer *m_vertexBuffer;

    GLMatrixStack m_matrixStack;

    Cloud m_cloud;

    QVector<Firework> m_fireworks;

    QVector<QOpenGLFramebufferObject*> m_fbos;
};

#endif // WINDOW_H
