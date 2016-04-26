#include "window.h"
#include <QSurfaceFormat>
#include <QMouseEvent>

Window::Window(uint width, uint height)
    : QOpenGLWidget()
    , m_windowWidth(width)
    , m_windowHeight(height)
    , m_frameCount(0)
{
    setWindowTitle("Clouds and Fireworks");
    setMinimumSize(width, height);
    setMaximumSize(width, height);

    qDebug() << this->format();

    startTimer(17); 
}

Window::~Window()
{
    m_vao.destroy();
    qDeleteAll(m_fbos);
    qDeleteAll(m_cloudTextures);
}

void Window::initializeGL()
{
    initializeOpenGLFunctions();

    glDisable(GL_DEPTH_TEST);

    m_resourceManager.init();

    m_vao.create();
    m_vao.bind();

    m_vertexBuffer = m_resourceManager.createVertexBuffer(4 * sizeof(VertexData));

    m_resourceManager.bindVertexBuffer(m_vertexBuffer);

    m_vertexData.clear();
    m_vertexData << VertexData(QVector3D(0.0f, 0.0f, 0.0f), QVector2D(0.0f, 0.0f))
                 << VertexData(QVector3D(1.0f, 0.0f, 0.0f), QVector2D(1.0f, 0.0f))
                 << VertexData(QVector3D(0.0f, 1.0f, 0.0f), QVector2D(0.0f, 1.0f))
                 << VertexData(QVector3D(1.0f, 1.0f, 0.0f), QVector2D(1.0f, 1.0f));

    m_vertexBuffer->write(0, m_vertexData.data(), m_vertexData.size() * sizeof(VertexData));

    m_backgroundTexture = m_resourceManager.createTexture(":/images/background.png");

    QOpenGLTexture *cloudTexture;

    for(int i = 32; i < 1024; i *= 2) {
        cloudTexture = new QOpenGLTexture(m_cloud.createCloud(i, 0.7, 0.05, 0.9));
        cloudTexture->setWrapMode(QOpenGLTexture::MirroredRepeat);
        m_cloudTextures << cloudTexture;
    }

    m_cloudProgram = m_resourceManager.createShaderProgram(":/shaders/clouds.frag", ":/shaders/default.vert");

    m_circleParticleTexture = m_resourceManager.createTexture(":/images/circleParticle.png");
    m_starParticleTexture = m_resourceManager.createTexture(":/images/starParticle.png");

    QOpenGLFramebufferObjectFormat fboFormat;
    fboFormat.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);

    for(uint i = 0; i < 2; ++i) {
        m_fbos << new QOpenGLFramebufferObject(m_windowWidth, m_windowHeight, fboFormat);
        m_fbos.last()->bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        m_fbos.last()->release();
    }

    m_explosionTexture = m_resourceManager.createTexture(":/images/explosion.png");

    m_fireworkProgram = m_resourceManager.createShaderProgram(":/shaders/fireworks.frag", ":/shaders/default.vert");
    m_waterProgram = m_resourceManager.createShaderProgram(":/shaders/water.frag", ":/shaders/default.vert");
}

void Window::paintGL()
{
    m_resourceManager.setupGLState();


    m_fbos[0]->bind();

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    drawBackground();

    if(m_fireworks.size())
        drawFireworks();

    m_fbos[0]->release();


    m_fbos[1]->bind();

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    drawClouds();

    m_fbos[1]->release();


    m_resourceManager.bindDefaultShaderProgram();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_fbos.at(0)->texture());
    m_resourceManager.defaultShaderProgram()->setUniformValue("tex", 0);

    m_matrixStack.push(Model);
    m_matrixStack.model().translate(0.0f, m_windowHeight/6.0f);
    m_matrixStack.model().scale(m_windowWidth, m_windowHeight/1.1667f);

    m_resourceManager.defaultShaderProgram()->setUniformValue("colorMode", (uint)DefaultShaderModes::Texture);
    m_resourceManager.defaultShaderProgram()->setUniformValue("modelViewProjectionMatrix", m_matrixStack.getCopy(ModelViewProjection));

    glDrawArrays(GL_TRIANGLE_STRIP, 0, m_vertexData.size());

    m_matrixStack.pop(Model);

    drawClouds();
    drawWater();

}
void Window::resizeGL(int width, int height)
{
    if(width >= 0 && height >= 0) {
        glViewport(0, 0, width, height);
        m_matrixStack.projection().ortho(0.0f, width, 0.0f, height, -1.0f, 1.0f);

        m_windowWidth = (uint)width;
        m_windowHeight = (uint)height;
    }
}

void Window::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton) {
        m_fireworks << Firework(event->pos().x(), m_windowHeight - event->pos().y() * 1.1667f - 28.0f);
    }

}

void Window::timerEvent(QTimerEvent */*event*/)
{
    ++m_frameCount;
    repaint();
}

void Window::drawBackground()
{
    m_resourceManager.bindDefaultShaderProgram();

    m_matrixStack.push(Model);
    m_matrixStack.model().scale(m_windowWidth, m_windowHeight);

    m_resourceManager.bindTexture(m_backgroundTexture);
    m_resourceManager.defaultShaderProgram()->setUniformValue("tex", 0);

    m_resourceManager.defaultShaderProgram()->setUniformValue("colorMode", (uint)DefaultShaderModes::Texture);
    m_resourceManager.defaultShaderProgram()->setUniformValue("modelViewProjectionMatrix", m_matrixStack.getCopy(ModelViewProjection));

    glDrawArrays(GL_TRIANGLE_STRIP, 0, m_vertexData.size());

    m_matrixStack.pop(Model);
}

void Window::drawClouds()
{
    m_resourceManager.bindShaderProgram(m_cloudProgram);

    m_matrixStack.push(Model);
    m_matrixStack.model().translate(0.0f, m_windowHeight - m_windowHeight/1.7f);
    m_matrixStack.model().scale(m_windowWidth, m_windowHeight/1.7f);

    m_resourceManager.bindTexture(m_cloudTextures[0], GL_TEXTURE0);
    m_cloudProgram->setUniformValue("tex1", 0);
    m_resourceManager.bindTexture(m_cloudTextures[1], GL_TEXTURE1);
    m_cloudProgram->setUniformValue("tex2", 1);
    m_resourceManager.bindTexture(m_cloudTextures[2], GL_TEXTURE2);
    m_cloudProgram->setUniformValue("tex3", 2);
    m_resourceManager.bindTexture(m_cloudTextures[3], GL_TEXTURE3);
    m_cloudProgram->setUniformValue("tex4", 3);
    m_resourceManager.bindTexture(m_cloudTextures[4], GL_TEXTURE4);
    m_cloudProgram->setUniformValue("tex5", 4);

    m_cloudProgram->setUniformValue("offset", (float)m_frameCount/(float)m_windowWidth);

    m_cloudProgram->setUniformValue("modelViewProjectionMatrix", m_matrixStack.getCopy(ModelViewProjection));

    glDrawArrays(GL_TRIANGLE_STRIP, 0, m_vertexData.size());

    m_matrixStack.pop(Model);
}

void Window::drawFireworks()
{
    m_resourceManager.bindShaderProgram(m_fireworkProgram);
    m_fireworkProgram->setUniformValue("tex", 0);

    GLfloat size, angle;
    QVector2D position, velocity, offset;
    GLColor color;
    QVector<Particle> particle;
    FireworkTypes type;

    for (int i = 0; i < m_fireworks.size(); ++i) {

        m_resourceManager.bindTexture(m_circleParticleTexture);
        size = m_fireworks.at(i).getRocketSize();
        for (uint j = 0; j < m_fireworks.at(i).getRocketParticlesQuantity(); ++j) {

            position = m_fireworks.at(i).getRocketPosition(j);
            color = m_fireworks.at(i).getRocketColor(j);

            m_matrixStack.push(Model);
            m_matrixStack.model().translate(position.x(), position.y());
            m_matrixStack.model().scale(size / 2.0f, size * 2.0f);

            m_fireworkProgram->setUniformValue("mode", (uint)FireworkModes::Snakes);
            m_fireworkProgram->setUniformValue("solidColor", color.red, color.green, color.blue, color.alpha);
            m_fireworkProgram->setUniformValue("modelViewProjectionMatrix", m_matrixStack.getCopy(ModelViewProjection));
            glDrawArrays(GL_TRIANGLE_STRIP, 0, m_vertexData.size());
            m_matrixStack.pop(Model);
        }

        if(m_fireworks.at(i).getParticlesQuantity() && !(m_fireworks.at(i).isExplosionFinished())) {

            m_resourceManager.bindTexture(m_explosionTexture);

            position = m_fireworks.at(i).getExplosionPosition();
            offset = m_fireworks.at(i).calculateSpriteOffset();

            m_matrixStack.push(Model);
            m_matrixStack.model().translate(position.x(), position.y());
            m_matrixStack.model().scale(70.0f, 70.0f);
            m_matrixStack.model().translate(-0.5f, -0.5f);

            m_fireworkProgram->setUniformValue("mode", (uint)FireworkModes::Explosion);
            m_fireworkProgram->setUniformValue("spriteOffset", offset);
            m_fireworkProgram->setUniformValue("modelViewProjectionMatrix", m_matrixStack.getCopy(ModelViewProjection));
            glDrawArrays(GL_TRIANGLE_STRIP, 0, m_vertexData.size());

            m_matrixStack.pop(Model);
        }

        m_resourceManager.bindTexture(m_starParticleTexture);

        size = m_fireworks.at(i).getParticlesSize();
        type = m_fireworks.at(i).getType();
        for (uint j = 0; j < m_fireworks.at(i).getParticlesQuantity(); ++j) {

            particle = m_fireworks[i].getParticle(j);
            for (int k = 0; k < particle.size(); ++k) {

                position = particle.at(k).getPosition();
                velocity = particle.at(k).getVelocity();
                color = particle.at(k).getColor();

                if(type != FireworkTypes::Blinks) {
                    angle = calculateRotationAngle(velocity, QVector2D(0.0f, 1.0f));
                    m_fireworkProgram->setUniformValue("mode", (uint)FireworkModes::Snakes);
                }
                else {
                    angle = 0.0f;
                    m_fireworkProgram->setUniformValue("mode", (uint)FireworkModes::Blinks);
                }

                m_matrixStack.push(Model);
                m_matrixStack.model().translate(position.x(), position.y());
                m_matrixStack.model().scale(size, size);
                m_matrixStack.model().rotate(angle, 0.0f, 0.0f, 1.0f);
                m_matrixStack.model().translate(-0.5f, -0.5f);

                m_fireworkProgram->setUniformValue("solidColor", color.red, color.green, color.blue, color.alpha);
                m_fireworkProgram->setUniformValue("modelViewProjectionMatrix", m_matrixStack.getCopy(ModelViewProjection));
                glDrawArrays(GL_TRIANGLE_STRIP, 0, m_vertexData.size());
                m_matrixStack.pop(Model);
            }
        }

        switch(m_fireworks.at(i).getCurrentFireworkState()) {
        case FireworkStates::Launched:
            m_fireworks[i].moveRocket();
            break;
        case FireworkStates::Exploded:
            m_fireworks[i].destroyRocketTail();
            m_fireworks[i].moveFireworkParticles();
            break;
        case FireworkStates::Faded:
            m_fireworks[i].destroyParticlesTails();
            break;
        case FireworkStates::Finished:
            m_fireworks.removeAt(i);
            break;
        }
    }
}

void Window::drawWater()
{
    m_resourceManager.bindShaderProgram(m_waterProgram);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_fbos.at(0)->texture());
    m_resourceManager.defaultShaderProgram()->setUniformValue("tex", 0);

    m_waterProgram->setUniformValue("angle", (float)m_frameCount * 5.0f);

    m_matrixStack.push(Model);
    m_matrixStack.model().translate(0.0f, m_windowHeight/6.0f);
    m_matrixStack.model().scale(m_windowWidth, m_windowHeight/6.0f);
    m_matrixStack.model().scale(1.0f, -1.0f);

    m_waterProgram->setUniformValue("modelViewProjectionMatrix", m_matrixStack.getCopy(ModelViewProjection));

    glDrawArrays(GL_TRIANGLE_STRIP, 0, m_vertexData.size());


    m_matrixStack.pop(Model);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_fbos.at(1)->texture());
    m_matrixStack.push(Model);
    m_matrixStack.model().translate(0.0f, m_windowHeight/3.0f);
    m_matrixStack.model().scale(m_windowWidth, m_windowHeight/3.0f);
    m_matrixStack.model().scale(1.0f, -1.0f);

    m_waterProgram->setUniformValue("modelViewProjectionMatrix", m_matrixStack.getCopy(ModelViewProjection));

    glDrawArrays(GL_TRIANGLE_STRIP, 0, m_vertexData.size());

    m_matrixStack.pop(Model);
}

GLfloat Window::calculateRotationAngle(QVector2D vector1, QVector2D vector2)
{
    GLfloat angle = 0.0f;

    if(!vector1.length() || !vector2.length())
        return 0.0f;

    float cos = QVector2D::dotProduct(vector1, vector2)/(vector1.length() * vector2.length());
    fixBoundary(-1.0f, cos, 1.0f);
    angle = acos(cos) * 180.0 / M_PI;

    if(vector1.x() > 0)
        angle = -angle;

    return angle;
}

float Window::fixBoundary(const float &min, float &value, const float &max)
{
    if (value < min) {
        value = min;
    } else if (value > max) {
        value = max;
    }
    return value;
}

