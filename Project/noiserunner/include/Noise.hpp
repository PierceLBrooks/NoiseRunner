#ifndef NOISE_HPP
#define NOISE_HPP

#include <iostream>
#include <fstream>
#include <cmath>
#include "Utilities.hpp"
#include "Tyrant/Framework.hpp"
#include "Tyrant/System/Random.hpp"

#define DEFAULT_COLOR_CHANNEL_COUNT 3
#define DEFAULT_SIZE 1000000
#define SOUND_SAMPLE_MAX ((powf(2.0f,16.0f)/2.0f)-1.0f)
#define DEFAULT_SOUND_SAMPLE_RATE 44100
#define DEFAULT_SOUND_CHANNEL_COUNT 1
#define COLOR_ADJUST 225

class Noise
{
    public:
        /*struct World
        {
            public:
                struct Sector
                {
                    public:
                        Sector(World* newWorld);
                        ~Sector;
                };
                World(std::string file, std::vector<TGE::Drawable*>* newDrawableStack);
                ~World();
                void eraseDrawable(TGE::Drawable* drawable);
                void processBytes();
                void getBytes(std::string file, char* buffer, unsigned int bufferLength, unsigned int offset);
                unsigned int getSize(std::string file);
                unsigned int getByte(unsigned int index);
                std::vector<TGE::Drawable*>* drawableStack;
        };*/
        Noise(std::string file, std::vector<TGE::Drawable*>* newDrawableStack, unsigned int newWorldScale = 2, unsigned int tileSizeTarget = 32, unsigned int worldWindowFactor = 2, unsigned int worldSpeedFactor = 3, unsigned int tileFramePixelMaxFactor = 3,
              unsigned int newColorChannelCount = DEFAULT_COLOR_CHANNEL_COUNT, unsigned int colorChannelMapRed = 0, unsigned int colorChannelMapGreen = 1, unsigned int colorChannelMapBlue = 2, unsigned int colorChannelMapAlpha = 3, bool newSoundEnabled = true);
        ~Noise();
        void setSoundSampleRate(unsigned int newSoundSampleRate);
        void setSoundChannelCount(unsigned int newSoundChannelCount);
        void initialize(bool newSoundEnabled = false);
        void update(float deltaTime);
        void handleSound();
        void handleConvolution(TGE::Int16* buffer, unsigned int index, unsigned int degree);
        void processBytes();
        void getBytes(std::string file, char* buffer, unsigned int bufferLength, unsigned int offset);
        char getByte(unsigned int index);
        unsigned char getByteColor(unsigned int index);
        char getNewestByte();
        unsigned int getIntFromChar(char c);
        unsigned int getWindowByteCount();
        unsigned int getSize(std::string file);
        unsigned int getWindowSize();
        unsigned int getWorldIndex();
        unsigned int getWorldOffset();
        unsigned int getWorldSeed();
        unsigned int getScale();
        unsigned int getColorChannelCount();
        unsigned int getColorChannelMapping(unsigned int channel);
        int getWorldDirection();
        int getIntSine(int angle);
        float getWorldSpeed();
        float getWorldSpeedFactor();
        float getTimeBuffer();
        bool getColorApplyAlpha();
        bool getIsNewIndex();
        bool getEnemy(unsigned int* enemy, bool second, bool bytes = false);
        unsigned int getPlayer(bool type = false);
        TGE::Color getWorldColor();
        TGE::Color getBytesAsColor(bool inverse = false);
        TGE::Sprite* getSprite();
        std::vector<TGE::Drawable*>* getDrawableStack();
        unsigned int getByteCount();

    private:
        std::vector<TGE::Drawable*>* drawableStack;
        float tileFrame, tileSpeed, timeBuffer, timeBufferPrev, timeBufferSpeed, timeBufferMax, worldPosition, worldSpeed;
        TGE::Vector2f windowSize;
        TGE::RNG* rng;
        TGE::RenderTexture* tileTexture;
        TGE::Sprite* tileSprite;
        TGE::RenderTexture* worldTexture;
        TGE::Sprite* worldSprite;
        TGE::RenderTexture* texture;
        TGE::Sprite* sprite;
        TGE::Int16* soundSampleBuffer;
        TGE::Int16* soundSampleBufferAnti;
        TGE::SoundBuffer* soundBuffer;
        TGE::SoundBuffer* soundBufferAnti;
        TGE::Sound* sound;
        TGE::Sound* soundAnti;
        TGE::Color worldColor;
        std::string worldFile;
        char* worldBuffer;
        bool colorApplyAlpha, worldDirection, worldFlip, soundEnabled, initialized;
        unsigned int powers[8];
        unsigned int worldSeed, worldIndex, worldIndexPrev, worldIndexSpeed, worldScale, worldSize, worldWindowSize, worldWindowSizeReal, worldWindowSizeSpeed, worldWindowDivision, worldWindowDivisionCount, worldWindowIndex, worldWindowIndexCount;
        unsigned int tileSize, tileSizeReal, tileFrameCount, tileFrameWidth, tileFrameHeight, tileFrameRowMax, tileFramePixelCount, tileFramePixelMax, tileColumnCount;
        unsigned int soundSampleRate, soundSampleCount, soundSampleCountMax, soundSampleMod, soundChannelCount;
        unsigned int colorChannelCount;
        unsigned int* colorChannelMap;

};

#endif
