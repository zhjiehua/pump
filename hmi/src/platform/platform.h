#ifndef PLATFORM_H
#define PLATFORM_H

#include <QWidget>

class Platform
{
public:
    virtual ~Platform() = default;
    virtual void applyWindowMode(QWidget *window) = 0;
    virtual bool isEmbedded() const = 0;
};

Platform *createPlatform();

#endif
