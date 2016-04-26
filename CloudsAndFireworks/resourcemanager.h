#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLBuffer>
#include <QOpenGLTexture>
#include <QHash>
#include <QImage>
#include <cstddef>

struct TextureBufferIDs
{
    GLuint textureID;
    GLuint tboID;

    TextureBufferIDs(GLuint textureID = 0, GLuint tboID = 0)
        : textureID(textureID)
        , tboID(tboID) {}
};

struct GLColor
{
     GLfloat red;
     GLfloat green;
     GLfloat blue;
     GLfloat alpha;

     GLColor(GLfloat red = 1.0f, GLfloat green = 1.0f, GLfloat blue = 1.0f, GLfloat alpha = 1.0f)
         : red(red)
         , green(green)
         , blue(blue)
         , alpha(alpha) {}
};

struct VertexData
{
    QVector3D vertexCoords;
    QVector2D textureCoords;
    GLColor color;

    VertexData(QVector3D vertexCoords = QVector3D(0.0f, 0.0f, 0.0f),
               QVector2D textureCoords = QVector2D(0.0f, 0.0f),
               GLColor color = GLColor(1.0f, 1.0f, 1.0f, 1.0f))
        : vertexCoords(vertexCoords)
        , textureCoords(textureCoords)
        , color(color) {}
};

enum class DefaultShaderModes
{
    Solid,
    Vertex,
    Texture,
    Tinted
};

/*!
  @brief Класс менеджера ресурсов.

  Отвечает за создание шейдеров, текстур и различных буфферов и работу с ними.
  */


class ResourceManager: protected QOpenGLFunctions_3_3_Core
{

public:
    /*! Конструктор класса ResourceManager. */
    ResourceManager();

    /*! Деструктор класса ResourceManager. */
    ~ResourceManager();

    /*!
     * Инициализирует внутренние объекты OpenGL. Возвращает <i>true</i> при успешной инициализации и <i>false</i> при неудачной.
     * Не вызывать до создания контекста!
     */
    bool init();

    /*!
     * Создаёт шейдерную программу, принимая в качестве фрагментного шейдера - <i>fragmentShader</i>, а в качестве вершинного - <i>vertexShader</i>.
     * Возвращает указатель на созданную шейдерную программу.
     */
    QOpenGLShaderProgram *createShaderProgram(QString fragmentShader = ":/shaders/default.frag", QString vertexShader = ":/shaders/default.vert", QString geometryShader  = "");

    /*!
     * Создаёт текстуру, используя картинку <i>textureName</i>.
     * Возвращает указатель на созданную текстуру.
     */
    QOpenGLTexture *createTexture(QString textureName);

    /*! Биндит текстуру <i>textureID</i>. */
    void bindTexture(QOpenGLTexture *texture, GLenum textureUnit = GL_TEXTURE0);

    /*!
     * Создаёт вершинный буффер размера <i>size</i> и записывает туда данные <i>data</i>.
     * Возвращает указатель на созданный вершинный буффер.
     */
    QOpenGLBuffer *createVertexBuffer(int size, const void *data = 0);

    /*!
     * Это перегруженная функция создания вершинного буффера. В качестве параметра принимает контейнер вершин <i>vertexData</i>.
     * Возвращает указатель на созданный вершинный буффер.
     */
    QOpenGLBuffer *createVertexBuffer(QVector<VertexData> vertexData);

    /*!
     * Создаёт индексный буффер размера <i>size</i> и записывает туда данные <i>data</i>.
     * Возвращает указатель на созданный индексный буффер.
     */
    QOpenGLBuffer *createIndexBuffer(int size, const void *data = 0);

    /*!
     * Это перегруженная функция создания индексного буффера. В качестве параметра принимает контейнер индексов <i>indexData</i>.
     * Возвращает указатель на созданный индексный буффер.
     */
    QOpenGLBuffer *createIndexBuffer(QVector<GLuint> indexData);

    /*! Возвращает указатель на шейдерную программу по умолчанию. */
    QOpenGLShaderProgram* defaultShaderProgram();

    /*! Биндит шейдерную программу <i>shaderProgram</i>. */
    bool bindShaderProgram(QOpenGLShaderProgram *shaderProgram);

    /*! Биндит стандартную шейдерную программу. */
    void bindDefaultShaderProgram();

    /*! Освобождает ранее забинденную шейдерную программу <i>shaderProgram</i>. */
    void releaseShaderProgram(QOpenGLShaderProgram *shaderProgram);

    /*! Освобождает стандартную шейдерную программу. */
    void releaseDefaultShaderProgram();

    /*! Биндит вершинный буффер <i>vertexBuffer</i>. */
    void bindVertexBuffer(QOpenGLBuffer *vertexBuffer);

    /*! Биндит индексный буффер <i>indexBuffer</i>. */
    void bindIndexBuffer(QOpenGLBuffer *indexBuffer);

    /*! Настраивает переменные OpenGL. */
    void setupGLState();

    /*! Возвращает контекст в исходное состояние. */
    void restoreGLState();

private:
    void releaseAllBuffers();

    QHash<QString, QOpenGLShaderProgram*> m_shaderHash;
    QOpenGLShaderProgram *m_defaultShaderProgram;
    QOpenGLShaderProgram *m_activeShaderProgram;

    QHash<QString, QOpenGLTexture*> m_textureHash;
    QOpenGLTexture *m_activeTexture;

    QVector<QOpenGLBuffer*> m_vertexBuffers;
    QOpenGLBuffer *m_activeVertexBuffer;

    QVector<QOpenGLBuffer*> m_indexBuffers;
    QOpenGLBuffer *m_activeIndexBuffer;
};

#endif // RESOURCEMANAGER_H
