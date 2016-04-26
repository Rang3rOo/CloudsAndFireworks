#include "cloud.h"
#include <cmath>


Cloud::Cloud()
    : m_numOctaves(5)
{
}

float Cloud::getNoise2D(int x, int y)
{
    int n = x + y * 57;
    n = (n << 13) ^ n;
    return (1.0f - (float)((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0f);
}

float Cloud::makeCosineInterpolation2D(float a, float b, float x)
{
    float ft = x * M_PI;
    float f = (1 - cosf(ft)) * 0.5;
    return (a*(1-f) + b*f);
}

float Cloud::getSmoothedNoise2D(float x, float y)
{
    float corners = (getNoise2D(x-1, y-1) + getNoise2D(x+1, y-1) + getNoise2D(x-1, y+1) + getNoise2D(x+1, y+1)) / 16;
    float sides   = (getNoise2D(x-1, y) + getNoise2D(x+1, y) + getNoise2D(x, y-1) + getNoise2D(x, y+1)) /  8;
    float center  =  getNoise2D(x, y) / 4;
    return (corners + sides + center);
}

float Cloud::compileNoise2D(float x, float y)
{
    float xIntPart= int(x);
    float xFracPart = x - xIntPart;

    float yIntPart = int(y);
    float yFracPart = y - yIntPart;

    float v1 = getSmoothedNoise2D(xIntPart,     yIntPart);
    float v2 = getSmoothedNoise2D(xIntPart + 1, yIntPart);
    float v3 = getSmoothedNoise2D(xIntPart,     yIntPart + 1);
    float v4 = getSmoothedNoise2D(xIntPart + 1, yIntPart + 1);

    float i1 = makeCosineInterpolation2D(v1, v2, xFracPart);
    float i2 = makeCosineInterpolation2D(v3, v4, xFracPart);

    return makeCosineInterpolation2D(i1, i2, yFracPart);
}

uchar Cloud::getPerlinNoise2D(float x, float y, float factor, float persistence, float frequency, float amplitude)
{
    float total = 0;

    persistence = 0.7f;
    frequency = 0.01f;
    amplitude = 0.8f;

    x += (factor);
    y += (factor);

    for(int i = 0 ; i < m_numOctaves; ++i) {
        total += compileNoise2D(x*frequency, y*frequency) * amplitude;
        amplitude *= persistence;
        frequency *= 2;
    }

    total = fabsf(total);
    uchar res = uchar(total * 255.0f);

    return res;
}

QVector<uchar> Cloud::generateNoise2D(int size, float persistence, float frequency, float amplitude)
{
  QVector<uchar> data;
  data.reserve(size*size);

  float factor = (float)(qrand());

  for(int i = 0 ; i < size; ++i) {
    for(int j = 0; j < size; ++j) {
       data << getPerlinNoise2D(float(i), float(j), factor, persistence, frequency, amplitude);
    }
  }

  return data;
}

QImage Cloud::createCloud(int size, float persistence, float frequency, float amplitude)
{
    QVector<uchar> data = generateNoise2D(size, persistence, frequency, amplitude);

    QImage result(size, size, QImage::Format_Grayscale8);

    for(int line = 0; line < result.height(); ++line) {
        memcpy(result.scanLine(line), &data[line * result.width()], size_t(result.width()));
    }

    return result;
}
