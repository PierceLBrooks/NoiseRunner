#include "Noise.hpp"
#include "MainState.hpp"

Noise::Noise(std::string file, std::vector<TGE::Drawable*>* newDrawableStack, unsigned int newWorldScale, unsigned int tileSizeTarget, unsigned int worldWindowFactor, unsigned int worldSpeedFactor, unsigned int tileFramePixelMaxFactor,
             unsigned int newColorChannelCount, unsigned int colorChannelMapRed, unsigned int colorChannelMapGreen, unsigned int colorChannelMapBlue, unsigned int colorChannelMapAlpha, bool newSoundEnabled)
{
    worldSeed = 0;
    if (file.size() == 0)
    {
        rng = new TGE::RNG();
        file = "RANDOM_NULL_SEED";
    }
    else
    {
        if (file.front() == '?')
        {
            worldSeed = util::hashString(file);
            rng = new TGE::RNG();
            rng->seed(worldSeed);
            if (file.size() != 1)
            {
                file = "RANDOM_SEED_"+file.substr(1);
            }
            else
            {
                file = "RANDOM_NO_SEED";
            }
        }
        else
        {
            rng = nullptr;
        }
    }
    drawableStack = newDrawableStack;
    soundEnabled = newSoundEnabled;
    windowSize = TGE::Vector2f(TGE::Game::getInstance()->getWindow()->getSize());
    unsigned int tileCount = 0;
    colorApplyAlpha = false;
    colorChannelCount = newColorChannelCount;
    colorChannelMap = new unsigned int[colorChannelCount];
    colorChannelMap[0] = colorChannelMapRed;
    colorChannelMap[1] = colorChannelMapGreen;
    colorChannelMap[2] = colorChannelMapBlue;
    colorChannelMap[3] = colorChannelMapAlpha;
    worldSpeed = powf(2,worldSpeedFactor);
    timeBuffer = 0;
    timeBufferPrev = 0;
    timeBufferSpeed = worldSpeed;
    timeBufferMax = 1;
    tileSpeed = worldSpeed;
    tileFrame = 0;
    tileFrameCount = 0;
    tileSize = 0;
    tileFramePixelMax = powf(2,tileFramePixelMaxFactor);
    worldColor = TGE::Color::Black;
    worldDirection = true;
    worldFlip = false;
    worldScale = newWorldScale;
    worldIndex = 0;
    worldIndexPrev = 0;
    worldIndexSpeed = 1;
    worldWindowIndex = 0;
    worldWindowIndexCount = 1;
    worldWindowSizeSpeed = 32;
    worldPosition = 0;
    worldFile = file;
    worldSize = getSize(worldFile);
    do
    {
        tileCount++;
        tileSize = sqrtf((float)((worldSize/tileCount)/colorChannelCount));
    } while (tileSize > tileSizeTarget);
    tileSizeReal = tileSize*worldScale;
    worldWindowSize = tileSize;
    for (unsigned int i = 0; i <= (unsigned int)(powf(2,worldWindowFactor)); i++)
    {
        worldWindowSize *= tileSize;
        if (worldWindowSize > worldSize)
        {
            worldWindowSize = worldSize;
            break;
        }
    }
    tileFramePixelCount = tileSize*tileSize*colorChannelCount;
    worldWindowDivision = 0;
    worldWindowDivisionCount = 0;
    do
    {
        worldWindowDivisionCount++;
        tileFrameCount = (worldWindowSize/tileFramePixelCount)/worldWindowDivisionCount;
        tileFrameRowMax = 0;
        do
        {
            tileFrameRowMax++;
            tileFrameWidth = std::min(tileFrameRowMax,tileFrameCount)*tileSize;
            tileFrameHeight = ((tileFrameCount/tileFrameRowMax)+1)*tileSize;
        } while (tileFrameHeight > TGE::Texture::getMaximumSize()/tileFramePixelMax);
    } while (tileFrameWidth > TGE::Texture::getMaximumSize()/tileFramePixelMax);
    worldWindowSizeReal = worldWindowSize/worldWindowDivisionCount;
    tileColumnCount = (windowSize.y/8)/tileSizeReal;
    //std::cout << worldWindowSize << "  " << worldWindowSizeReal << "  " << worldWindowDivisionCount << "  " << tileFrameWidth << "  " << tileFrameHeight << "  " << worldSpeed << "\n";
    worldBuffer = new char[worldWindowSizeReal];
    worldSprite = nullptr;
    tileSprite = nullptr;
    tileTexture = nullptr;
    sprite = nullptr;
    soundSampleBuffer = new TGE::Int16[1];
    soundSampleBufferAnti = new TGE::Int16[1];
    soundBuffer = nullptr;
    soundBufferAnti = nullptr;
    sound = nullptr;
    soundAnti = nullptr;
    soundSampleCount = 0;
    soundSampleCountMax = 0;
    soundSampleMod = SOUND_SAMPLE_MAX/127.0f;
    if (soundEnabled)
    {
        soundChannelCount = DEFAULT_SOUND_CHANNEL_COUNT;
        soundSampleRate = DEFAULT_SOUND_SAMPLE_RATE;
    }
    else
    {
        soundChannelCount = 0;
        soundSampleRate = 0;
    }
    for (unsigned int i = 0; i < 8; i++)
    {
        powers[i] = powf(2,i)-1;
    }
    initialized = false;
}

Noise::~Noise()
{
    if (soundEnabled)
    {
        sound->stop();
    }
    delete sound;
    delete soundAnti;
    delete soundBuffer;
    delete soundBufferAnti;
    delete[] soundSampleBuffer;
    delete[] soundSampleBufferAnti;
    delete[] colorChannelMap;
    delete[] worldBuffer;
    util::eraseDrawable(sprite,drawableStack);
    delete tileSprite;
    delete tileTexture;
    delete worldSprite;
    delete worldTexture;
    delete sprite;
    delete texture;
}

void Noise::setSoundSampleRate(unsigned int newSoundSampleRate)
{
    if (!soundEnabled)
    {
        return;
    }
    soundSampleRate = newSoundSampleRate;
}

void Noise::setSoundChannelCount(unsigned int newSoundChannelCount)
{
    if (!soundEnabled)
    {
        return;
    }
    soundChannelCount = newSoundChannelCount;
}

void Noise::initialize(bool newSoundEnabled)
{
    worldIndex = 0;
    worldIndexPrev = 0;
    worldWindowIndex = 0;
    worldWindowDivision = 0;
    timeBuffer = 0;
    timeBufferPrev = 0;
    if (initialized)
    {
        delete worldTexture;
        delete texture;
        delete[] soundSampleBuffer;
        delete[] soundSampleBufferAnti;
    }
    soundEnabled = newSoundEnabled;
    if (soundEnabled)
    {
        soundSampleCountMax = 1;
        while (worldWindowSizeReal/soundSampleCountMax > soundSampleRate*soundChannelCount)
        {
            soundSampleCountMax++;
        }
        soundSampleCountMax = sqrtf(soundSampleCountMax);
        soundSampleBuffer = new TGE::Int16[worldWindowSizeReal];
        soundSampleBufferAnti = new TGE::Int16[worldWindowSizeReal];
    }
    processBytes();
    worldTexture = new TGE::RenderTexture();
    worldTexture->create(windowSize.x+(timeBufferMax*tileSizeReal),windowSize.y);
    worldTexture->clear(worldColor);
    texture = new TGE::RenderTexture();
    texture->create(windowSize.x,windowSize.y);
    texture->clear(worldColor);
    initialized = true;
}

void Noise::update(float deltaTime)
{
    if (!initialized)
    {
        return;
    }
    worldIndexPrev = worldIndex;
    timeBufferPrev = timeBuffer;
    timeBuffer += deltaTime*timeBufferSpeed;
    tileFrame += deltaTime*tileSpeed;
    if ((unsigned int)tileFrame >= tileFrameCount)
    {
        tileFrame = 0;
    }
    if ((int)timeBuffer != (int)timeBufferPrev)
    {
        if (worldIndex < worldWindowSize)
        {
            worldIndex += worldIndexSpeed;
            if (worldIndex > worldWindowSize)
            {
                worldIndex = 0;
                worldWindowDivision++;
                if (worldWindowDivision == worldWindowDivisionCount)
                {
                    worldWindowDivision = 0;
                    worldWindowIndex++;
                    if (worldWindowIndex == worldWindowIndexCount)
                    {
                        worldWindowIndex = 0;
                    }
                }
                processBytes();
            }
        }
    }
    if (soundEnabled)
    {
        if (sound->getStatus() == TGE::SoundSource::Stopped)
        {
            soundSampleCount++;
            handleSound();
        }
    }
    worldPosition = (timeBuffer*getWorldSpeedFactor())*getWorldDirection();
    if (timeBuffer > timeBufferMax)
    {
        timeBuffer -= timeBufferMax;
        timeBufferPrev -= timeBufferMax;
        worldPosition += (timeBufferMax*getWorldSpeedFactor())*(worldDirection?1:-1);
    }
    //tileSprite->setColor(TGE::Color(255,255,255));
    tileSprite->setTextureRect(TGE::IntRect((((unsigned int)tileFrame)%tileFrameRowMax)*tileSize,(((unsigned int)tileFrame)/tileFrameRowMax)*tileSize,tileSize,tileSize));
    worldTexture->clear(TGE::Color::Transparent);
    // work needed in this loop
    for (unsigned int i = 0; i < worldTexture->getSize().x/tileSizeReal; i++)
    {
        if (worldIndex+i >= worldWindowSizeReal)
        {
            break;
        }
        for (unsigned int ii = 0; ii < 8; ii++)
        {
            if (worldBuffer[worldIndex+i]&powers[ii])
            {
                for (unsigned int iii = 0; iii < tileColumnCount; iii++)
                {
                    tileSprite->setPosition(i*tileSizeReal,((ii*tileColumnCount)+iii)*tileSizeReal);
                    worldTexture->draw(*tileSprite);
                }
            }
        }
        for (unsigned int ii = (worldTexture->getSize().y/tileSizeReal)+1; ii > 0; ii--)
        {
            if (worldBuffer[worldIndex+i] > ii-1)// especially here
            {
                tileSprite->setPosition(i*tileSizeReal,(ii-1)*tileSizeReal);
                worldTexture->draw(*tileSprite);
            }
        }
    }
    worldTexture->display();
    delete worldSprite;
    worldSprite = new TGE::Sprite(worldTexture->getTexture());
    worldSprite->setPosition((windowSize.x/2)+worldPosition,windowSize.y/2);
    worldSprite->setOrigin(windowSize.x/2,windowSize.y/2);
    worldSprite->setScale(worldDirection?1:-1,worldFlip?-1:1);
    if (sprite != nullptr)
    {
        sprite->setColor(TGE::Color(250,250,250));
        texture->draw(*sprite);
    }
    texture->draw(*worldSprite);
    texture->display();
    util::eraseDrawable(sprite,drawableStack);
    delete sprite;
    sprite = new TGE::Sprite(texture->getTexture());
    sprite->setDepth(1);
    sprite->setColor(TGE::Color(COLOR_ADJUST,COLOR_ADJUST,COLOR_ADJUST));
    drawableStack->push_back(sprite);
    //std::cout << worldIndex << "\n";
}

void Noise::handleSound()
{
    if (!soundEnabled)
    {
        return;
    }
    delete sound;
    delete soundAnti;
    delete soundBuffer;
    delete soundBufferAnti;
    TGE::Int16 temp;
    if (soundSampleCount == 0)
    {
        if (MainState::soundCompensation == 1.0f)
        {
            for (unsigned int i = 0; i < worldWindowSizeReal; i++)
            {
                soundSampleBuffer[i] = worldBuffer[i];
            }
        }
        else
        {
            temp = 0;
            TGE::Int16 tempOther, counter = 0;
            //TGE::Int16* buffer = new TGE::Int16[worldWindowSizeReal];
            TGE::Int16* bufferOther = new TGE::Int16[worldWindowSizeReal];
            for (unsigned int i = 0; i < worldWindowSizeReal; i++)
            {
                soundSampleBuffer[i] = (((soundSampleMod*worldBuffer[i]*(i%2))/2)^i)/2;
                soundSampleBufferAnti[i] = ~soundSampleBuffer[i];
                bufferOther[i] = soundSampleBuffer[i];
                //buffer[i] = soundSampleBuffer[i];
                if (temp == 0)
                {
                    temp = abs(soundSampleBuffer[i]/soundSampleMod);
                    counter++;
                }
                else
                {
                    temp--;
                }
                tempOther = i;
                /*if ((tempOther < 0) != (counter%2 == 0))
                {
                    buffer[i] &= buffer[i]^-getIntSine(tempOther);
                }
                else
                {
                    buffer[i] &= buffer[i]^getIntSine(tempOther);
                }*/
                if ((tempOther%2 == 0) == (counter%2 == 0))
                {
                    bufferOther[i] ^= temp^-tempOther;
                }
                else
                {
                    bufferOther[i] ^= temp^tempOther;
                }
                handleConvolution(bufferOther,i,2);
            }
            for (unsigned int i = 1; i < 1+((abs(worldBuffer[0])+abs(worldBuffer[worldWindowSizeReal-1]))/2); i++)
            {
                for (unsigned int ii = 0; ii < worldWindowSizeReal; ii++)
                {
                    soundSampleBuffer[ii] &= soundSampleBuffer[ii]^getIntSine(soundSampleBuffer[ii]);
                    /*for (unsigned int iii = 0; iii < 8; iii++)
                    {
                        if ((!(worldBuffer[ii]&powers[iii])) && (ii+iii+1 < worldWindowSizeReal))
                        {
                            buffer[ii+iii+1] |= buffer[ii+iii+1]^(ii+iii+1);
                        }
                    }*/
                }
                for (unsigned int ii = worldWindowSizeReal; ii > 0; ii--)
                {
                    temp = (soundSampleBuffer[ii-1]^(ii%i));
                    soundSampleBuffer[ii-1] |= (soundSampleBuffer[worldWindowSizeReal-ii]^(ii%i));
                    soundSampleBuffer[worldWindowSizeReal-ii] |= temp;
                }
            }
            for (unsigned int i = 0; i < worldWindowSizeReal; i++)
            {
                soundSampleBufferAnti[i] = ~soundSampleBuffer[i]+worldBuffer[i];
                soundSampleBufferAnti[i] *= i%2;
                soundSampleBufferAnti[i] &= getIntSine(i);
                //soundSampleBuffer[i] |= buffer[i];
                soundSampleBuffer[i] |= bufferOther[i];
                //soundSampleBuffer[i] /= 2;
                handleConvolution(soundSampleBuffer,i,2);
                //soundSampleBufferAnti[i] = ~soundSampleBuffer[i]+worldBuffer[i];
                //soundSampleBufferAnti[i] *= i%2;
            }
            //delete[] buffer;
            delete[] bufferOther;
        }
    }
    else
    {
        for (unsigned int i = worldWindowSizeReal; i > 0; i--)
        {
            /*temp = (soundSampleBuffer[i-1]^(i%soundSampleCount));
            soundSampleBuffer[i-1] |= (soundSampleBuffer[worldWindowSizeReal-i]^(i%soundSampleCount));
            soundSampleBuffer[worldWindowSizeReal-i] |= temp;
            temp = (soundSampleBufferAnti[i-1]^(i%soundSampleCount));
            soundSampleBufferAnti[i-1] |= (soundSampleBufferAnti[worldWindowSizeReal-i]^(i%soundSampleCount));
            soundSampleBufferAnti[worldWindowSizeReal-i] |= temp;*/
            temp = soundSampleBuffer[i-1];
            soundSampleBuffer[i-1] = soundSampleBuffer[worldWindowSizeReal-i];
            soundSampleBuffer[worldWindowSizeReal-i] = temp;
            temp = soundSampleBufferAnti[i-1];
            soundSampleBufferAnti[i-1] = soundSampleBufferAnti[worldWindowSizeReal-i];
            soundSampleBufferAnti[worldWindowSizeReal-i] = temp;
        }
    }
    soundBuffer = new TGE::SoundBuffer();
    soundBufferAnti = new TGE::SoundBuffer();
    soundBuffer->loadFromSamples(soundSampleBuffer,worldWindowSizeReal,soundChannelCount,soundSampleRate*soundSampleCountMax);
    soundBufferAnti->loadFromSamples(soundSampleBufferAnti,worldWindowSizeReal,soundChannelCount,soundSampleRate*soundSampleCountMax);
    sound = new TGE::Sound();
    soundAnti = new TGE::Sound();
    sound->setBuffer(*soundBuffer);
    soundAnti->setBuffer(*soundBufferAnti);
    sound->play();
    soundAnti->play();
    sound->setLoop(true);
    soundAnti->setLoop(true);
}

void Noise::handleConvolution(TGE::Int16* buffer, unsigned int index, unsigned int degree)
{
    if (degree == 0)
    {
        return;
    }
    if ((worldWindowSizeReal < degree) || (index < degree-1) || (index >= worldWindowSizeReal))
    {
        return;
    }
    TGE::Int16 temp = 0;
    for (unsigned int i = index-(degree-1); i < index+1; i++)
    {
        temp += buffer[i];
    }
    temp /= degree;
    buffer[index] &= temp;
}

void Noise::processBytes()
{
    bool worldWindowSizeDivide;
    unsigned int tileFrameX = 0, tileFrameY = 0, tileFramePixelX = 0, tileFramePixelY = 0, worldWindowSizeIndex = 0;
    unsigned int worldOffset = getWorldOffset();
    //std::cout << "hi  " << worldWindowIndex << "  " << worldWindowDivision << "  " << worldOffset << "  " << worldSize << "\n";
    delete[] worldBuffer;
    worldBuffer = new char[worldWindowSizeReal];
    getBytes(worldFile,worldBuffer,worldWindowSizeReal,worldOffset);
    delete tileSprite;
    delete tileTexture;
    tileSprite = nullptr;
    TGE::Image* tileFrameImage = nullptr;
    TGE::Texture* tileFrameTexture = nullptr;
    tileTexture = new TGE::RenderTexture();
    TGE::Color color = worldColor;
    tileTexture->create(tileFrameWidth,tileFrameHeight);
    tileTexture->clear(color);
    for (unsigned int i = 0; i < worldWindowSizeReal; i++)
    {
        if (i%tileFramePixelCount == 0)
        {
            if (i != 0)
            {
                tileFrameTexture = new TGE::Texture();
                tileFrameTexture->loadFromImage(*tileFrameImage);
                tileSprite = new TGE::Sprite(*tileFrameTexture);
                tileSprite->setPosition(tileFrameX*tileSize,tileFrameY*tileSize);
                tileTexture->draw(*tileSprite);
                tileFrameX++;
                if (tileFrameX == tileFrameRowMax)
                {
                    tileFrameX = 0;
                    tileFrameY++;
                }
            }
            delete tileSprite;
            tileSprite = nullptr;
            delete tileFrameTexture;
            tileFrameTexture = nullptr;
            delete tileFrameImage;
            tileFrameImage = new TGE::Image();
            tileFrameImage->create(tileSize,tileSize);
            tileFramePixelX = 0;
            tileFramePixelY = 0;
            worldWindowSizeIndex++;
        }
        worldWindowSizeDivide = false;
        if (worldWindowSizeSpeed > 0)
        {
            if (i > (worldWindowSizeReal/worldWindowSizeSpeed)*worldWindowIndex)
            {
                i -= (worldWindowSizeReal/worldWindowSizeSpeed)*worldWindowIndex;
                worldWindowSizeDivide = true;
            }
        }
        switch (colorChannelMap[i%colorChannelCount])
        {
        case 0:
            color.r = getIntFromChar(worldBuffer[i]);
            break;
        case 1:
            color.g = getIntFromChar(worldBuffer[i]);
            break;
        case 2:
            color.b = getIntFromChar(worldBuffer[i]);
            break;
        case 3:
            if (colorApplyAlpha)
            {
                color.a = getIntFromChar(worldBuffer[i]);
            }
            break;
        }
        if (i%colorChannelCount == colorChannelCount-1)
        {
            tileFrameImage->setPixel(tileFramePixelX,tileFramePixelY,color);
            tileFramePixelX++;
            if (tileFramePixelX == tileSize)
            {
                tileFramePixelX = 0;
                tileFramePixelY++;
            }
            color = worldColor;
        }
        if (worldWindowSizeDivide)
        {
            i += (worldWindowSizeReal/worldWindowSizeSpeed)*worldWindowIndex;
        }
    }
    delete tileSprite;
    delete tileFrameTexture;
    delete tileFrameImage;
    tileSprite = new TGE::Sprite(tileTexture->getTexture());
    tileSprite->setTextureRect(TGE::IntRect(0,0,tileSize,tileSize));
    tileSprite->setScale(worldScale,worldScale);
    tileSprite->getTexture()->copyToImage().saveToFile(worldFile+"_TILE_ANIMATION.png");
    if (soundEnabled)
    {
        soundSampleCount = 0;
        handleSound();
    }
}

void Noise::getBytes(std::string file, char* buffer, unsigned int bufferLength, unsigned int offset)
{
    if (rng != nullptr)
    {
        for (unsigned int i = 0; i < bufferLength; i++)
        {
            buffer[i] = rng->geni(-128,127);
        }
        return;
    }
    std::ifstream stream(file,std::ios::binary|std::ios::ate);
    unsigned int length = stream.tellg();
    if (length == 0)
    {
        return;
    }
    stream.seekg(offset,std::ios::beg);
    stream.read(buffer,std::min(bufferLength,length));
    stream.close();
}

char Noise::getByte(unsigned int index)
{
    index += worldIndex;
    if (index >= worldWindowSizeReal)
    {
        index -= worldWindowSizeReal;
    }
    return worldBuffer[index];
}

unsigned char Noise::getByteColor(unsigned int index)
{
    index += worldIndex;
    index *= colorChannelCount;
    if (index >= worldWindowSizeReal)
    {
        index -= worldWindowSizeReal;
    }
    return getIntFromChar(worldBuffer[index]);
}

char Noise::getNewestByte()
{
    return getByte(getWindowByteCount());
}

unsigned int Noise::getIntFromChar(char c)
{
    unsigned int i = abs(c);
    if (c < 0)
    {
        i += 128;
    }
    return i;
}

unsigned int Noise::getWindowByteCount()
{
    return worldTexture->getSize().x/tileSizeReal;
}

unsigned int Noise::getSize(std::string file)
{
    if (rng != nullptr)
    {
        return DEFAULT_SIZE;
    }
    std::ifstream stream(file,std::ios::binary|std::ios::ate);
    unsigned int length = stream.tellg();
    return length;
}

unsigned int Noise::getWindowSize()
{
    return worldWindowSizeReal;
}

unsigned int Noise::getWorldIndex()
{
    return worldIndex;
}

unsigned int Noise::getWorldOffset()
{
    return (worldWindowIndex*(worldWindowSize*worldWindowDivisionCount))+(worldWindowDivision*worldWindowSizeReal);
}

unsigned int Noise::getWorldSeed()
{
    return worldSeed;
}

unsigned int Noise::getScale()
{
    return worldScale;
}

unsigned int Noise::getColorChannelCount()
{
    return colorChannelCount;
}

unsigned int Noise::getColorChannelMapping(unsigned int channel)
{
    if (channel >= colorChannelCount)
    {
        return colorChannelCount;
    }
    return colorChannelMap[channel];
}

int Noise::getWorldDirection()
{
    return worldDirection?-1:1;
}

int Noise::getIntSine(int angle)
{
    return (angle<<1)-((angle*abs(angle))>>14);
}

float Noise::getWorldSpeed()
{
    return worldSpeed;
}

float Noise::getWorldSpeedFactor()
{
    return tileSizeReal;
}

float Noise::getTimeBuffer()
{
    return timeBuffer;
}

bool Noise::getColorApplyAlpha()
{
    return colorApplyAlpha;
}

bool Noise::getIsNewIndex()
{
    return (worldIndex!=worldIndexPrev);
}

bool Noise::getEnemy(unsigned int* enemy, bool second, bool bytes)
{
    if (!getIsNewIndex())
    {
        return false;
    }
    if (worldWindowSizeReal < 3)
    {
        return false;
    }
    if (worldIndex > worldWindowSizeReal-3)
    {
        return false;
    }
    if (worldBuffer[worldIndex] > worldBuffer[worldWindowSizeReal-worldIndex-1])
    {
        return false;
    }
    if (bytes)
    {
        if (second)
        {
            *enemy = getIntFromChar(worldBuffer[(worldWindowSizeReal-worldIndex-1)]);
            *enemy |= getIntFromChar(worldBuffer[worldWindowSizeReal-worldIndex-2])<<8;
            *enemy |= getIntFromChar(worldBuffer[worldWindowSizeReal-worldIndex-3])<<16;
            return true;
        }
        if (worldIndex+2 >= worldWindowSizeReal)
        {
            return false;
        }
        *enemy = getIntFromChar(worldBuffer[worldIndex]);
        *enemy |= getIntFromChar(worldBuffer[worldIndex+1])<<8;
        *enemy |= getIntFromChar(worldBuffer[worldIndex+2])<<16;
    }
    else
    {
        if (second)
        {
            *enemy = 128+worldBuffer[(worldWindowSizeReal-worldIndex-1)];
            *enemy |= (128+worldBuffer[worldWindowSizeReal-worldIndex-2])<<8;
            *enemy |= (128+worldBuffer[worldWindowSizeReal-worldIndex-3])<<16;
            return true;
        }
        if (worldIndex+2 >= worldWindowSizeReal)
        {
            return false;
        }
        *enemy = 128+worldBuffer[worldIndex];
        *enemy |= (128+worldBuffer[worldIndex+1])<<8;
        *enemy |= (128+worldBuffer[worldIndex+2])<<16;
    }
    return true;
}

unsigned int Noise::getPlayer(bool type)
{
    if ((!getIsNewIndex()) || (worldIndex+2 >= worldWindowSizeReal))
    {
        return 0;
    }
    unsigned int player;
    if (type)
    {
        player = 128+worldBuffer[worldIndex];
        player |= (128+worldBuffer[worldIndex+1])<<8;
        player |= (128+worldBuffer[worldIndex+2])<<16;
    }
    else
    {
        player = getIntFromChar(worldBuffer[worldIndex]);
        player |= getIntFromChar(worldBuffer[worldIndex+1])<<8;
        player |= getIntFromChar(worldBuffer[worldIndex+2])<<16;
    }
    return player;
}

TGE::Color Noise::getWorldColor()
{
    return worldColor;
}

TGE::Color Noise::getBytesAsColor(bool inverse)
{
    unsigned int bytes;
    if (getEnemy(&bytes,inverse,true))
    {
        TGE::Color color;
        color.r = bytes&255;
        color.g = (bytes>>8)&255;
        color.b = (bytes>>16)&255;
        return color;
    }
    return worldColor;
}

TGE::Sprite* Noise::getSprite()
{
    return sprite;
}

std::vector<TGE::Drawable*>* Noise::getDrawableStack()
{
    return drawableStack;
}

unsigned int Noise::getByteCount()
{
    return getWorldOffset()+worldIndex;
}
