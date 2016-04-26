#include "firework.h"
#include <cmath>
#include <QSound>

const float Firework::GRAVITY = 0.02f;
const float Firework::LAUNCH_POSITION_Y = 0.0f;
const GLColor Firework::FIREWORK_COLORS[] = { GLColor(1.0f, 0.0f, 0.0f, 1.0f),
                                              GLColor(1.0f, 0.5f, 0.0f, 1.0f),
                                              GLColor(1.0f, 1.0f, 0.0f, 1.0f),
                                              GLColor(1.0f, 0.0f, 0.5f, 1.0f),
                                              GLColor(1.0f, 0.0f, 1.0f, 1.0f),
                                              GLColor(0.5f, 1.0f, 0.0f, 1.0f),
                                              GLColor(0.5f, 0.0f, 1.0f, 1.0f),
                                              GLColor(0.0f, 1.0f, 0.0f, 1.0f),
                                              GLColor(0.0f, 1.0f, 0.5f, 1.0f),
                                              GLColor(0.0f, 1.0f, 1.0f, 1.0f),
                                              GLColor(0.0f, 0.5f, 1.0f, 1.0f) };

const uint Firework::FIREWORK_COLORS_SIZE = 11;
const uint Firework::EXPLOSION_DURATION = 64;

Firework::Firework(int posX, int posY)
    : m_mouseClickedPosition((float)posX, (float)posY)
    , m_fizzPlayed(false)
    , m_rocketSize(6.0f)
    , m_rocketMaxTail(30)
    , m_rocketCurrentTail(0)
    , m_particlesFlightDuration(120)
    , m_particlesMaxTail(30)
    , m_particlesCurrentTail(0)
    , m_curExplosionDuration(0)
{
    m_fireworkLevel = 4 + (qrand() % 4);
    m_particlesAngleOffset = ((float)qrand() / (float)RAND_MAX) * 360.0f;
    m_particlesSize = 4.0f + ((float)qrand() / (float)RAND_MAX) * 2.0f;

    int type = qrand() % 3;

    if(!type)
        m_fireworkType = FireworkTypes::Blinks;
    else
        m_fireworkType = FireworkTypes::Snakes;

    if(m_fireworkType == FireworkTypes::Blinks) {
        m_particlesMaxTail = 0;
        m_particlesFlightDuration = 80;
        m_particlesSize += 3.0f;
    }

    launchRocket();
}

void Firework::launchRocket()
{
    float verticalVelocity = 4.0f + ((float)qrand() / (float)RAND_MAX) * 2.0f;

    m_rocket << Particle(m_mouseClickedPosition.x(), LAUNCH_POSITION_Y);
    m_rocket.last().setVelocity(0.0f, verticalVelocity);
    m_rocket.last().setColor(GLColor(1.0f, 1.0f, 0.0f, 1.0f));  

    m_fireworkState = FireworkStates::Launched;
}

void Firework::moveRocket()
{
    QVector2D position;
    GLColor color;

    for(int i = 1; i < m_rocket.size(); ++i) {
        color = m_rocket.at(i).getColor();
        color.green -= (1.0 / (float)m_rocketMaxTail) * 1.3f;
        color.alpha -= (1.0 / (float)m_rocketMaxTail);

        if(color.alpha > 0.0)
            m_rocket[i].setColor(color);
        else {
            m_rocket.removeAt(i);
            --m_rocketCurrentTail;
            --i;
        }
    }

    position = m_rocket.first().getPosition();

    if(m_rocketCurrentTail < m_rocketMaxTail) {
        color = m_rocket.first().getColor();

        m_rocket << Particle(position.x(), position.y());
        m_rocket.last().setColor(color);

        ++m_rocketCurrentTail;
    }

    m_mouseClickedPosition.setX(position.x());
    m_rocket.first().setPosition(position.x() + (-0.75f + ((float)qrand() / (float)RAND_MAX) * 1.5f), position.y() + m_rocket.first().getVelocity().y());

    if (m_rocket.first().getPosition().y() >= m_mouseClickedPosition.y()) {
        m_rocket.removeFirst();

        explodeFirework();

        m_fireworkState = FireworkStates::Exploded;
    }
}

void Firework::destroyRocketTail()
{
    GLColor color;

    for(int i = 0; i < m_rocket.size(); ++i) {
        color = m_rocket.at(i).getColor();
        color.green -= (1.0 / (float)m_rocketMaxTail) * 1.3f;
        color.alpha -= (1.0 / (float)m_rocketMaxTail);

        if(color.alpha > 0.0)
            m_rocket[i].setColor(color);
        else {
            m_rocket.removeAt(i);
            --m_rocketCurrentTail;
            --i;
        }
    }
}

void Firework::explodeFirework()
{
    GLColor particleColor = FIREWORK_COLORS[qrand() % FIREWORK_COLORS_SIZE];
    m_explosionPosition = m_mouseClickedPosition;

    if(m_fireworkType == FireworkTypes::Blinks) {
        if(!(qrand() % 3))
            particleColor = GLColor(1.0f, 1.0f, 1.0f, 0.0f);
        else
            particleColor.alpha = 0.0f;
    }

    uint size, particlesOnLevel;
    float angle, speed, verticalVelocity, horizontalVelocity;

    for (uint i = 0; i < m_fireworkLevel; ++i) {

        particlesOnLevel = 1 + 3 * i + (qrand() % 2) * i;
        speed = (0.1f + (float)i) + ((float)qrand() / (float)RAND_MAX) * 0.1f;
        size = m_particles.size();

        for (uint j = 0; j < particlesOnLevel; ++j) {

            m_particles << QVector<Particle>();
            m_particles[j + size] << Particle();
            m_particles[j + size][0].setPosition(m_mouseClickedPosition);

            angle = (360.0f / particlesOnLevel) * (float)j + (360.0f / particlesOnLevel / 2.0f) * (i % 2) + m_particlesAngleOffset;

            horizontalVelocity = speed * cosf(angle / 180.0f * M_PI) * (1.0f + 0.2f * ((float)qrand() / (float)RAND_MAX));
            verticalVelocity = speed * sinf(angle / 180.0f * M_PI) * (1.0f + 0.2f * ((float)qrand() / (float)RAND_MAX));

            if(fabs(horizontalVelocity) < 0.001)
                horizontalVelocity = 0.0f;

            if(fabs(verticalVelocity) < 0.001)
                verticalVelocity = 0.0f;


            m_particles[j + size][0].setVelocity(verticalVelocity, horizontalVelocity);

            m_particles[j + size][0].setColor(particleColor);
        }
    }

    QSound::play(":/sounds/explosion.wav");
}

void Firework::moveFireworkParticles()
{
    QVector2D position, velocity, originalPosition, originalVelocity;
    GLColor color;

    for (int i = 0; i < m_particles.size(); ++i) {

        originalPosition = m_particles[i].last().getPosition();
        originalVelocity = m_particles[i].last().getVelocity();

        for(int j = 0; j < m_particles.at(i).size(); ++j) {
            position = m_particles[i][j].getPosition();
            velocity = m_particles[i][j].getVelocity();

            if((position.y() + velocity.y()) >= LAUNCH_POSITION_Y)
                m_particles[i][j].setPosition(position + velocity);
            else
                m_particles[i][j].setPosition(position.x() + velocity.x(), LAUNCH_POSITION_Y);

            velocity.setX(velocity.x() * 0.96f);
            velocity.setY(velocity.y() * 0.96f - GRAVITY);
            m_particles[i][j].setVelocity(velocity);

            if((m_fireworkType == FireworkTypes::Blinks) &&(m_particlesFlightDuration < 60) && !(m_particlesFlightDuration % 5)) {
                color = m_particles[i].first().getColor();
                color.alpha = (float)(qrand() % 2);
                m_particles[i].first().setColor(color);

                if(!m_fizzPlayed) {
                    QSound::play(":/sounds/fizz.wav");
                    m_fizzPlayed = true;
                }
            }
        }

        if(m_particlesCurrentTail < m_particlesMaxTail) {
            color = m_particles[i].last().getColor();

            m_particles[i] << Particle(originalPosition.x(), originalPosition.y());
            m_particles[i].last().setVelocity(originalVelocity);
            color.alpha -= (1.0 / (float)m_particlesMaxTail);
            m_particles[i].last().setColor(color);
        }
    }

    if(m_particlesCurrentTail < m_particlesMaxTail)
        ++m_particlesCurrentTail;

    if(!(--m_particlesFlightDuration))
    {
        for (int i = 0; i < m_particles.size(); ++i)
            m_particles[i].removeFirst();

        m_fireworkState = FireworkStates::Faded;
    }

    if(m_curExplosionDuration < EXPLOSION_DURATION)
        ++m_curExplosionDuration;
}

void Firework::destroyParticlesTails()
{
    QVector2D position, velocity;

    for (int i = 0; i < m_particles.size(); ++i) {

        for(int j = 0; j < m_particles[i].size(); ++j) {
            position = m_particles[i][j].getPosition();
            velocity = m_particles[i][j].getVelocity();

            if((position.y() + velocity.y()) >= LAUNCH_POSITION_Y)
                m_particles[i][j].setPosition(position + velocity);
            else
                m_particles[i][j].setPosition(position.x() + velocity.x(), LAUNCH_POSITION_Y);

            velocity.setX(velocity.x() * 0.96f);
            velocity.setY(velocity.y() * 0.96f - GRAVITY);
            m_particles[i][j].setVelocity(velocity);
        }

        if(m_particlesCurrentTail)
            m_particles[i].removeFirst();
    }

    if(m_particlesCurrentTail)
        --m_particlesCurrentTail;
    else
        m_fireworkState = FireworkStates::Finished;

    if(m_curExplosionDuration < EXPLOSION_DURATION)
        ++m_curExplosionDuration;
}

FireworkStates Firework::getCurrentFireworkState() const
{
    return m_fireworkState;
}

uint Firework::getRocketParticlesQuantity() const
{
    return m_rocket.size();
}

GLfloat Firework::getRocketSize() const
{
    return m_rocketSize;
}

GLColor Firework::getRocketColor(int index) const
{
    return m_rocket.at(index).getColor();
}

QVector2D Firework::getRocketPosition(int index) const
{
    return m_rocket.at(index).getPosition();
}

uint Firework::getParticlesQuantity() const
{
    return m_particles.size();
}

GLfloat Firework::getParticlesSize() const
{
    return m_particlesSize;
}

QVector<Particle> Firework::getParticle(int index)
{
    return m_particles.at(index);
}

QVector2D Firework::getExplosionPosition() const
{
    return m_explosionPosition;
}

QVector2D Firework::calculateSpriteOffset() const
{
    return QVector2D((m_curExplosionDuration / 2 % 6), (m_curExplosionDuration / 2 / 6));
}

bool Firework::isExplosionFinished() const
{
    return (m_curExplosionDuration >= EXPLOSION_DURATION);
}

FireworkTypes Firework::getType() const
{
    return m_fireworkType;
}

Particle::Particle(float posX, float posY)
    : m_position(posX, posY)
{
}

void Particle::setPosition(float posX, float posY)
{
    m_position.setX(posX);
    m_position.setY(posY);
}

void Particle::setPosition(const QVector2D &position)
{
    setPosition(position.x(), position.y());
}

void Particle::setVelocity(float velX, float velY)
{
    m_velocity.setX(velX);
    m_velocity.setY(velY);
}

void Particle::setVelocity(const QVector2D &velocity)
{
    setVelocity(velocity.x(), velocity.y());
}

QVector2D Particle::getPosition() const
{
    return m_position;
}

QVector2D Particle::getVelocity() const
{
    return m_velocity;
}

GLColor Particle::getColor() const
{
    return m_color;
}

void Particle::setColor(const GLColor &color)
{
    m_color = color;
}
