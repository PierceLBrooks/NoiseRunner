#include "NoiseMessage.hpp"
#include "MainState.hpp"

NoiseMessage::NoiseMessage(std::string newMessage, TGE::Font* font, Noise* newNoise, unsigned int newDelayMax, unsigned int size, bool newOrder, bool newFlip, bool newKeepPrev)
{
    delayMax = newDelayMax;
    delay = delayMax;
    flip = newFlip;
    order = newOrder;
    keepPrev = newKeepPrev;
    noise = newNoise;
    message = newMessage;
    if (message.size() == 1)
    {
        if ((message.back() == ',') || (message.back() == '-') || (message.back() == '.') || (message.back() == '*') || (message.back() == '+') || (message.back() == '#') || (message.back() == '_'))
        {
            message.push_back(' ');
        }
    }
    TGE::Text* text = new TGE::Text(TGE::String(message),*font,size);
    if (message.size() == 2)
    {
        if (message.back() == ' ')
        {
            text->move(text->getGlobalBounds().width/4,-text->getGlobalBounds().height/4);
        }
    }
    text->setColor(TGE::Color::White);
    TGE::RenderTexture* renderTexture = new TGE::RenderTexture();
    renderTexture->create(text->getGlobalBounds().width,text->getGlobalBounds().height);
    renderTexture->clear(TGE::Color::Transparent);
    renderTexture->draw(*text);
    renderTexture->display();
    image = new TGE::Image(renderTexture->getTexture().copyToImage());
    texture = new TGE::Texture();
    texture->loadFromImage(*image);
    sprite = new TGE::Sprite(*texture);
    sprite->setColor(TGE::Color(COLOR_ADJUST,COLOR_ADJUST,COLOR_ADJUST));
    sprite->setPosition(TGE::Game::getInstance()->getWindow()->getSize().x/2,TGE::Game::getInstance()->getWindow()->getSize().y/2);
    scale.x = 1;
    scale.y = 1;
    if ((texture->getSize().x != 0) && (texture->getSize().y != 0))
    {
        if (texture->getSize().x > noise->getWorldSpeedFactor())
        {
            scale.x = noise->getWorldSpeedFactor()/texture->getSize().x;
            scale.x /= noise->getScale();
            scale.x *= MainState::soundCompensation;
        }
        else
        {
            scale.x = 1;
        }
        if (texture->getSize().y > noise->getWorldSpeedFactor())
        {
            scale.y = noise->getWorldSpeedFactor()/texture->getSize().y;
            scale.y /= noise->getScale();
            scale.y *= MainState::soundCompensation*1.5f;
        }
        else
        {
            scale.y = 1;
        }
        scale.x *= MainState::soundCompensation;
        scale.y *= sqrtf(MainState::soundCompensation);
        sprite->setScale(scale.x,scale.y);
    }
    else
    {
        scale.x = 1;
        scale.y = 1;
    }
    delete text;
    delete renderTexture;
    index = 0;
    pixel = 0;
    count = 0;
    color = noise->getWorldColor();
    colorPrevious = color;
    for (unsigned int i = 0; i < image->getSize().x; i++)
    {
        for (unsigned int ii = 0; ii < image->getSize().y; ii++)
        {
            if (image->getPixel(i,ii) != TGE::Color::Transparent)
            {
                count++;
            }
        }
    }
    count *= count/(image->getSize().x*image->getSize().y);
    sprite->setOrigin(texture->getSize().x/2,texture->getSize().y/2);
}

NoiseMessage::~NoiseMessage()
{
    delete sprite;
    delete texture;
    delete image;
}

TGE::Sprite* NoiseMessage::getSprite()
{
    return sprite;
}

std::string NoiseMessage::getMessage()
{
    return message;
}

bool NoiseMessage::getIsRendered()
{
    return ((getWidth() != 0.0f) && (getHeight() != 0.0f) && (count != 0));
}

float NoiseMessage::getWidth()
{
    if (sprite == nullptr)
    {
        return 0;
    }
    return scale.x*sprite->getTexture()->getSize().x;
}

float NoiseMessage::getHeight()
{
    if (sprite == nullptr)
    {
        return 0;
    }
    return scale.y*sprite->getTexture()->getSize().y;
}

void NoiseMessage::update(bool draw)
{
    if (noise == nullptr)
    {
        return;
    }
    if (!draw)
    {
        //index += sqrtf(message.size()*noise->getWorldSpeed());
        index += sqrtf(count/(noise->getWorldSpeed()*message.size()))/(delayMax*2);
        if (index >= noise->getWindowSize())
        {
            index -= noise->getWindowSize();
        }
        pixel = 0;
        return;
    }
    if (!order)
    {
        if (flip)
        {
            for (unsigned int i = 0; i < image->getSize().x; i++)
            {
                for (unsigned int ii = 0; ii < image->getSize().y; ii++)
                {
                    updatePixel(i,ii);
                }
            }
        }
        else
        {
            for (unsigned int i = image->getSize().x; i > 0; i--)
            {
                for (unsigned int ii = image->getSize().y; ii > 0; ii--)
                {
                    updatePixel(i-1,ii-1);
                }
            }
        }
    }
    else
    {
        if (flip)
        {
            for (unsigned int i = 0; i < image->getSize().y; i++)
            {
                for (unsigned int ii = 0; ii < image->getSize().x; ii++)
                {
                    updatePixel(ii,i);
                }
            }
        }
        else
        {
            for (unsigned int i = image->getSize().y; i > 0; i--)
            {
                for (unsigned int ii = image->getSize().x; ii > 0; ii--)
                {
                    updatePixel(ii-1,i-1);
                }
            }
        }
    }
    texture->loadFromImage(*image);
    sprite->setTexture(*texture);
}

void NoiseMessage::updatePixel(unsigned int x, unsigned int y)
{
    if (image->getPixel(x,y) != TGE::Color::Transparent)
    {
        color = noise->getWorldColor();
        for (unsigned int i = 0; i < noise->getColorChannelCount(); i++)
        {
            switch (noise->getColorChannelMapping(i))
            {
            case 0:
                color.r = 255-noise->getByteColor(index+pixel+i);
                break;
            case 1:
                color.g = 255-noise->getByteColor(index+pixel+i);
                break;
            case 2:
                color.b = 255-noise->getByteColor(index+pixel+i);
                break;
            case 3:
                if (noise->getColorApplyAlpha())
                {
                    color.a = noise->getByteColor(index+pixel+i);
                }
                break;
            }
        }
        if ((keepPrev) && (color == noise->getWorldColor()))
        {
            color = colorPrevious;
        }
        image->setPixel(x,y,color);
        colorPrevious = color;
        delay--;
        if (delay == 0)
        {
            delay = delayMax*noise->getColorChannelCount();
            pixel += noise->getColorChannelCount();
        }
    }
}
