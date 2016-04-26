#ifndef GLMATRIXSTACK_H
#define GLMATRIXSTACK_H
#include <QStack>
#include <QMatrix4x4>

enum MatrixType {
    Model,
    View,
    Projection,
    ModelView,
    ModelViewProjection
};

class GLMatrixStack
{
public:
    GLMatrixStack();
    void push(MatrixType matrix);
    void pop(MatrixType matrix);
    void pushAll();
    void popAll();
    void setAllToIdentity();
    QMatrix4x4 getCopy(MatrixType matrix);
    QMatrix4x4 &model();
    QMatrix4x4 &view();
    QMatrix4x4 &projection();
    void load(MatrixType type, const QMatrix4x4 &matrix);
    void reset();

private:
    QStack<QMatrix4x4> m_model;
    QStack<QMatrix4x4> m_view;
    QStack<QMatrix4x4> m_projection;
};

#endif // GLMATRIXSTACK_H
