#ifndef FIREWORK_H
#define FIREWORK_H

#include "resourcemanager.h"

enum class FireworkTypes
{
    Blinks,
    Snakes
};

enum class FireworkStates
{
    Launched,
    Exploded,
    Faded,
    Finished
};

enum class FireworkModes
{
    Explosion,
    Blinks,
    Snakes
};

class Particle
{
public:
    Particle(float posX = 0.0f, float posY = 0.0f);

    QVector2D getPosition() const;
    void setPosition(float posX, float posY);
    void setPosition(const QVector2D &position);

    QVector2D getVelocity() const;
    void setVelocity(float velX, float velY);
    void setVelocity(const QVector2D &velocity);

    GLColor getColor() const;
    void setColor(const GLColor &color);

private:
    QVector2D m_position;
    QVector2D m_velocity;
    GLColor m_color;
};

class Firework
{
public:
    Firework(int posX = 0, int posY = 0);
    void launchRocket();
    void moveRocket();
    void destroyRocketTail();
    void explodeFirework();
    void moveFireworkParticles();
    void destroyParticlesTails();

    FireworkStates getCurrentFireworkState() const;

    uint getRocketParticlesQuantity() const;
    GLfloat getRocketSize() const;
    GLColor getRocketColor(int index) const;
    QVector2D getRocketPosition(int index) const;

    uint getParticlesQuantity() const;
    GLfloat getParticlesSize() const;
    QVector<Particle> getParticle(int index);

    QVector2D getExplosionPosition() const;
    QVector2D calculateSpriteOffset() const;
    bool isExplosionFinished() const;

    FireworkTypes getType() const;

private:
    QVector2D m_mouseClickedPosition;
    FireworkTypes m_fireworkType;
    bool m_fizzPlayed;
    FireworkStates m_fireworkState;
    uint m_fireworkLevel;

    QVector<Particle> m_rocket;
    GLfloat m_rocketSize;
    uint m_rocketMaxTail;
    uint m_rocketCurrentTail;

    QVector< QVector<Particle> > m_particles;
    GLfloat m_particlesSize;
    uint m_particlesAngleOffset;
    uint m_particlesFlightDuration;
    uint m_particlesMaxTail;
    uint m_particlesCurrentTail;

    uint m_curExplosionDuration;
    QVector2D m_explosionPosition;

    static const float GRAVITY;
    static const float LAUNCH_POSITION_Y;
    static const GLColor FIREWORK_COLORS[];
    static const uint FIREWORK_COLORS_SIZE;
    static const uint EXPLOSION_DURATION;
};

#endif // FIREWORK_H
