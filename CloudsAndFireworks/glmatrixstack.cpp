#include "glmatrixstack.h"

GLMatrixStack::GLMatrixStack()
{
    m_model.push(QMatrix4x4());
    m_view.push(QMatrix4x4());
    m_projection.push(QMatrix4x4());
}

void GLMatrixStack::push(MatrixType matrix)
{
    switch(matrix) {
    case Model:
        m_model.push(m_model.top());
        break;

    case View:
        m_view.push(m_view.top());
        break;

    case Projection:
        m_projection.push(m_projection.top());
        break;

    default:
        qDebug("GLMatrixStack::push: wrong matrix type!");
        break;
    }
}

void GLMatrixStack::pushAll()
{
    m_model.push(m_model.top());
    m_view.push(m_view.top());
    m_projection.push(m_projection.top());
}

void GLMatrixStack::pop(MatrixType matrix)
{
    switch(matrix) {
    case Model:
        m_model.pop();
        break;

    case View:
        m_view.pop();
        break;

    case Projection:
        m_projection.pop();
        break;

    default:
        qDebug("GLMatrixStack::pop: wrong matrix type!");
        break;
    }
}

void GLMatrixStack::popAll()
{
    m_model.pop();
    m_view.pop();
    m_projection.pop();
}

QMatrix4x4 GLMatrixStack::getCopy(MatrixType matrix)
{
    QMatrix4x4 result;
    switch(matrix) {
    case Model:
        result = m_model.top();
        break;

    case View:
        result = m_view.top();
        break;

    case Projection:
        result = m_projection.top();
        break;

    case ModelView:
        result = m_view.top() * m_model.top();
        break;

    case ModelViewProjection:
        result = m_projection.top() * m_view.top() * m_model.top();
        break;

    default:
        qDebug("GLMatrixStack::get: wrong matrix type!");
        break;
    }
    return result;
}

QMatrix4x4 &GLMatrixStack::model()
{
    return m_model.top();
}

QMatrix4x4 &GLMatrixStack::view()
{
    return m_view.top();
}

QMatrix4x4 &GLMatrixStack::projection()
{
    return m_projection.top();
}

void GLMatrixStack::load(MatrixType type, const QMatrix4x4 &matrix)
{
    switch(type) {
    case Model:
        m_model.last() = matrix;
        break;

    case View:
        m_view.last() = matrix;
        break;

    case Projection:
        m_projection.last() = matrix;
        break;

    default:
        qDebug("GLMatrixStack::load: wrong matrix type!");
        break;
    }
}

void GLMatrixStack::reset()
{
    m_model.clear();
    m_model.push(QMatrix4x4());

    m_view.clear();
    m_view.push(QMatrix4x4());

    m_projection.clear();
    m_projection.push(QMatrix4x4());

}

void GLMatrixStack::setAllToIdentity()
{
    model().setToIdentity();
    view().setToIdentity();
    projection().setToIdentity();
}
