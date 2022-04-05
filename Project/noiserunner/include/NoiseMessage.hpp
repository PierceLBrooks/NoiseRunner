#ifndef NOISE_MESSAGE_HPP
#define NOISE_MESSAGE_HPP

#include "Noise.hpp"
#include "Tyrant/Framework.hpp"

class NoiseMessage
{
    public:
        NoiseMessage(std::string newMessage, TGE::Font* font, Noise* newNoise, unsigned int newDelayMax, unsigned int size, bool newOrder = true, bool newFlip = true, bool newKeepPrev = true);
        ~NoiseMessage();
        TGE::Sprite* getSprite();
        std::string getMessage();
        float getWidth();
        float getHeight();
        bool getIsRendered();
        void update(bool draw);
        void updatePixel(unsigned int x, unsigned int y);
    private:
        bool order, flip, keepPrev;
        unsigned int index, pixel, delay, delayMax;
        float count;
        std::string message;
        Noise* noise;
        TGE::Image* image;
        TGE::Texture* texture;
        TGE::Sprite* sprite;
        TGE::Color color, colorPrevious;
        TGE::Vector2f scale;
};

#endif
