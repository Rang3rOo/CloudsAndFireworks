#ifndef CLOUDS_H
#define CLOUDS_H

#include <QImage>
#include <QVector>

class Cloud
{
public:
    Cloud();
    QImage createCloud(int size, float persistence, float frequency, float amplitude);

private:
    float getNoise2D(int x, int y);
    float makeCosineInterpolation2D(float a, float b, float x);
    float getSmoothedNoise2D(float x, float y);
    float compileNoise2D(float x, float y);
    uchar getPerlinNoise2D(float x, float y, float factor, float persistence, float frequency, float amplitude);
    QVector<uchar> generateNoise2D(int size, float persistence, float frequency, float amplitude);

    int m_numOctaves;

};

#endif // CLOUDS_H
