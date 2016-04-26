#include "resourcemanager.h"
#include <QtDebug>

ResourceManager::ResourceManager()
    : m_defaultShaderProgram(NULL)
    , m_activeShaderProgram(NULL)
    , m_activeTexture(NULL)
    , m_activeVertexBuffer(NULL)
    , m_activeIndexBuffer(NULL)
{
}

bool ResourceManager::init()
{
    bool success = true;
    success = initializeOpenGLFunctions();
    m_defaultShaderProgram = createShaderProgram();
    return success;
}

QOpenGLShaderProgram *ResourceManager::defaultShaderProgram()
{
    return m_defaultShaderProgram;
}

ResourceManager::~ResourceManager()
{
    qDeleteAll(m_shaderHash);
    m_shaderHash.clear();

    qDeleteAll(m_textureHash);
    m_textureHash.clear();

    qDeleteAll(m_vertexBuffers);
    m_vertexBuffers.clear();

    qDeleteAll(m_indexBuffers);
    m_indexBuffers.clear();

}

QOpenGLShaderProgram *ResourceManager::createShaderProgram(QString fragmentShader, QString vertexShader, QString geometryShader)
{
    if (m_shaderHash.contains(fragmentShader + vertexShader)) {
        qDebug() << QStringLiteral("Shader program already linked.");
        return m_shaderHash.value(fragmentShader + vertexShader);
    }

    QOpenGLShaderProgram *program = new QOpenGLShaderProgram();
    m_shaderHash.insert(fragmentShader + vertexShader, program);

    // Override system locale until shaders are compiled
    setlocale(LC_NUMERIC, "C");

    // Compile vertex shader
    if (program->addShaderFromSourceFile(QOpenGLShader::Vertex, vertexShader)) {
        qDebug() << QStringLiteral("Succesfully added vertex shader '") + vertexShader + QStringLiteral("'.");
    } else {
        QString error = QStringLiteral("Failed to add vertex shader '") + vertexShader + QStringLiteral("'!");
        qFatal(error.toLocal8Bit().data());
    }

    // Compile geometry shader
    if(geometryShader != "")
    {
        if (program->addShaderFromSourceFile(QOpenGLShader::Geometry, geometryShader)) {
            qDebug() << QStringLiteral("Succesfully added geometry shader '") + geometryShader + QStringLiteral("'.");
        } else {
            QString error = QStringLiteral("Failed to add geometry shader '") + geometryShader + QStringLiteral("'!");
            qFatal(error.toLocal8Bit().data());
        }
    }

    // Compile fragment shader
    if (program->addShaderFromSourceFile(QOpenGLShader::Fragment, fragmentShader)) {
        qDebug() << QStringLiteral("Succesfully added fragment shader '") + fragmentShader + QStringLiteral("'.");
    } else {
        QString error = QStringLiteral("Failed to add fragment shader '") + fragmentShader + QStringLiteral("'!");
        qFatal(error.toLocal8Bit().data());
    }

    // Link shader pipeline
    if (program->link()) {
        qDebug() << QStringLiteral("Succesfully linked shader program.");
    } else {
        qFatal("Failed to link shader program!");
    }

    // Restore system locale
    setlocale(LC_ALL, "");

    return program;
}

bool ResourceManager::bindShaderProgram(QOpenGLShaderProgram *shaderProgram)
{
    bool result = false;
    if(shaderProgram != m_activeShaderProgram) {
        result = shaderProgram->bind();
        if(result)
            m_activeShaderProgram = shaderProgram;
    }
//    else qDebug() << QStringLiteral("Shader program already active.");

    return result;
}

void ResourceManager::bindDefaultShaderProgram()
{
    bindShaderProgram(m_defaultShaderProgram);
}

void ResourceManager::releaseShaderProgram(QOpenGLShaderProgram *shaderProgram)
{
    shaderProgram->release();
    m_activeShaderProgram = NULL;
}

void ResourceManager::releaseDefaultShaderProgram()
{
    releaseShaderProgram(m_defaultShaderProgram);
}

QOpenGLTexture *ResourceManager::createTexture(QString textureName)
{
    if(m_textureHash.contains(textureName)) {
        qDebug() << QStringLiteral("Texture already linked.");
        return m_textureHash.value(textureName);
    }

    QOpenGLTexture *texture = new QOpenGLTexture(QImage(textureName).mirrored());

    m_textureHash.insert(textureName, texture);

    return texture;
}

void ResourceManager::bindTexture(QOpenGLTexture *texture, GLenum textureUnit)
{
    if(texture != m_activeTexture) {
        texture->bind(textureUnit - GL_TEXTURE0);
        m_activeTexture = texture;
    }
//    else qDebug() << QStringLiteral("Texture is already bound.");
}

QOpenGLBuffer *ResourceManager::createVertexBuffer(int size, const void *data)
{
    QOpenGLBuffer *vertexBuffer;
    vertexBuffer = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    vertexBuffer->create();
    vertexBuffer->bind();
    vertexBuffer->allocate(data, size);
    vertexBuffer->release();

    m_vertexBuffers.append(vertexBuffer);

    return vertexBuffer;
}

QOpenGLBuffer *ResourceManager::createVertexBuffer(QVector<VertexData> vertexData)
{
    return createVertexBuffer(vertexData.size() * sizeof(VertexData), vertexData.data());
}

void ResourceManager::bindVertexBuffer(QOpenGLBuffer *vertexBuffer)
{
    if(vertexBuffer != m_activeVertexBuffer) {
        vertexBuffer->bind();
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (const void *)offsetof(VertexData, vertexCoords));
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (const void *)offsetof(VertexData, textureCoords));
        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(VertexData), (const void *)offsetof(VertexData, color));
        m_activeVertexBuffer = vertexBuffer;
    }
}

QOpenGLBuffer *ResourceManager::createIndexBuffer(int size, const void *data)
{
    QOpenGLBuffer *indexBuffer;
    indexBuffer = new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
    indexBuffer->create();
    indexBuffer->bind();
    indexBuffer->allocate(data, size);
    indexBuffer->release();

    m_indexBuffers.append(indexBuffer);

    return indexBuffer;
}

QOpenGLBuffer *ResourceManager::createIndexBuffer(QVector<GLuint> indexData)
{
    return createIndexBuffer(indexData.size() * sizeof(GLuint), indexData.data());
}

void ResourceManager::bindIndexBuffer(QOpenGLBuffer *indexBuffer)
{
    if(indexBuffer != m_activeIndexBuffer) {
        indexBuffer->bind();
        m_activeIndexBuffer = indexBuffer;
    }
}

void ResourceManager::releaseAllBuffers()
{
    if(m_activeVertexBuffer) {
        m_activeVertexBuffer->release();
        m_activeVertexBuffer = NULL;
    }
    if(m_activeIndexBuffer) {
        m_activeIndexBuffer->release();
        m_activeIndexBuffer = NULL;
    }
}

void ResourceManager::setupGLState()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
}

void ResourceManager::restoreGLState()
{
    if(m_activeShaderProgram) {
        m_activeShaderProgram->release();
        m_activeShaderProgram = NULL;
    }
    m_activeTexture = NULL;

    releaseAllBuffers();
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);

    glDepthMask(GL_FALSE);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_BLEND);
}
