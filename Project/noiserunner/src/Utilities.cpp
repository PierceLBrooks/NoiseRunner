#include "Utilities.hpp"

namespace util
{
    std::string getDataDir()
    {
        return "data/";
    }

    std::string numToString(float num)
    {
        std::ostringstream toStr;
        toStr << num;
        return toStr.str();
    }

    float stringToNum(std::string str)
    {
        return atof(str.c_str());
    }

    float numRound(float num, bool up)
    {
        if (up)
        {
            return (int)(num+((num-(int)num > 0)?1:0));
        }
        return ((float)((int)((num*10.0f)+0.5f)))/10.0f;
    }

    int getGCD(int x, int y)
    {
        int temp;
        while (y != 0)
        {
            temp = y;
            y = x%y;
            x = temp;
        }
        return x;
    }

    float getDistance(TGE::Vector2f point1, TGE::Vector2f point2)
    {
        return sqrtf(powf(point1.x-point2.x,2.0f)+powf(point1.y-point2.y,2.0f));
    }

    float getDirection(TGE::Vector2f self, TGE::Vector2f target)
    {
        return atan2f(target.y-self.y,target.x-self.x);
    }

    TGE::Vector2f getMidpoint(TGE::Vector2f point1, TGE::Vector2f point2)
    {
        return TGE::Vector2f((point1.x+point2.x)/2,(point1.y+point2.y)/2);
    }

    bool getIsLeft(TGE::Vector2f linePoint1, TGE::Vector2f linePoint2, TGE::Vector2f point)
    {
        return ((((linePoint2.x-linePoint1.x)*(point.y-linePoint1.y))-((linePoint2.y-linePoint1.y)*(point.x-linePoint1.x)))>0);
    }

    bool getIsVowel(char letter)
    {
        char vowels[6] = {'a', 'e', 'i', 'o', 'u', 'y'};
        for (int i = 0; i < 6; ++i)
        {
            if (letter == vowels[i])
            {
                return true;
            }
        }
        return false;
    }


    TGE::Color hsvToRGB(TGE::Vector3f color)
    {
        int i = 0;
        float f = 0, p = 0, q = 0, t = 0;
        TGE::Color colorNew;
        if (color.y == 0)
        {
            colorNew.r = color.z*255;
            colorNew.g = color.z*255;
            colorNew.b = color.z*255;
        }
        else
        {
            color.x /= 60;
            i = floor(color.x);
            f = color.x-i;
            p = color.z*(1.0f-color.y);
            q = color.z*(1.0f-(color.y*f));
            t = color.z*(1.0f-(color.y*(1.0f-f)));
            switch (i)
            {
            case 0:
                colorNew.r = color.z*255;
                colorNew.g = t*255;
                colorNew.b = p*255;
                break;
            case 1:
                colorNew.r = q*255;
                colorNew.g = color.z*255;
                colorNew.b = p*255;
                break;
            case 2:
                colorNew.r = p*255;
                colorNew.g = color.z*255;
                colorNew.b = t*255;
                break;
            case 3:
                colorNew.r = p*255;
                colorNew.g = q*255;
                colorNew.b = color.z*255;
                break;
            case 4:
                colorNew.r = t*255;
                colorNew.g = p*255;
                colorNew.b = color.z*255;
                break;
            case 5:
                colorNew.r = color.z*255;
                colorNew.g = p*255;
                colorNew.b = q*255;
                break;
            }
        }
        return colorNew;
    }

    TGE::Vector3f rgbToHSV(TGE::Color color, bool normalize)
    {
        float rgbMax, rgbDif;
        TGE::Vector3f newColor;
        TGE::Vector3f oldColor(((float)color.r)/255.0f,((float)color.g)/255.0f,((float)color.b)/255.0f);
        rgbMax = std::max(oldColor.x,std::max(oldColor.y,oldColor.z));
        newColor.z = rgbMax;
        rgbDif = rgbMax-std::min(oldColor.x,std::min(oldColor.y,oldColor.z));
        if (rgbMax != 0)
        {
            newColor.y = rgbDif/rgbMax;
        }
        else
        {
            newColor.y = 0;
            newColor.x = 0;
            return newColor;
        }
        if (oldColor.x == rgbMax)
        {
            newColor.x = (oldColor.y-oldColor.z)/rgbDif;
        }
        else
        {
            if (oldColor.y == rgbMax)
            {
                newColor.x = 2+((oldColor.z-oldColor.x)/rgbDif);
            }
            else
            {
                newColor.x = 4+((oldColor.x-oldColor.y)/rgbDif);
            }
        }
        newColor.x *= 60;
        if (newColor.x < 0)
        {
            newColor.x += 360;
        }
        if (normalize)
        {
            newColor.x /= 360;
            return newColor;
        }
        return newColor;
    }

    void eraseDrawable(TGE::Drawable* drawable, std::vector<TGE::Drawable*>* drawableStack)
    {
        if (drawable != nullptr)
        {
            for (unsigned int i = 0; i != drawableStack->size(); ++i)
            {
                if ((&(*drawableStack->at(i))) == (&(*drawable)))
                {
                    drawableStack->erase(drawableStack->begin()+i);
                    break;
                }
            }
        }
    }

    void eraseSprites(std::vector<TGE::Sprite*>* sprites, std::vector<TGE::Drawable*>* drawableStack)
    {
        if (sprites != nullptr)
        {
            for (unsigned int i = 0; i != sprites->size(); ++i)
            {
                eraseDrawable(sprites->at(i), drawableStack);
            }
        }
    }

    float sqr(float val)
    {
        return val*val;
    }

    unsigned int hashRotate(unsigned int x, unsigned int y)
    {
        /*
        Source: http://burtleburtle.net/bob/c/lookup3.c
        Public domain, no warranty
        Original source code has been altered.
        */
        return ((x<<y)|(x>>(32-y)));
    }

    void hashMix(unsigned int& a, unsigned int& b, unsigned int& c)
    {
        /*
        Source: http://burtleburtle.net/bob/c/lookup3.c
        Public domain, no warranty
        Original source code has been altered.
        */
        a -= c;
        a ^= hashRotate(c,4);
        c += b;
        b -= a;
        b ^= hashRotate(a,6);
        a += c;
        c -= b;
        c ^= hashRotate(b,8);
        b += a;
        a -= c;
        a ^= hashRotate(c,16);
        c += b;
        b -= a;
        b ^= hashRotate(a,19);
        a += c;
        c -= b;
        c ^= hashRotate(b,4);
        b += a;
    }

    void hashFinalMix(unsigned int& a, unsigned int& b, unsigned int& c)
    {
        /*
        Source: http://burtleburtle.net/bob/c/lookup3.c
        Public domain, no warranty
        Original source code has been altered.
        */
        c ^= b;
        c -= hashRotate(b,14);
        a ^= c;
        a -= hashRotate(c,11);
        b ^= a;
        b -= hashRotate(a,25);
        c ^= b;
        c -= hashRotate(b,16);
        a ^= c;
        a -= hashRotate(c,4);
        b ^= a;
        b -= hashRotate(a,14);
        c ^= b;
        c -= hashRotate(b,24);
    }

    unsigned int hashFinalMixAlt(unsigned int a, unsigned int b, unsigned int c)
    {
        /*
        Source: http://burtleburtle.net/bob/c/lookup3.c
        Public domain, no warranty
        Original source code has been altered.
        */
        c ^= b;
        c -= hashRotate(b,14);
        a ^= c;
        a -= hashRotate(c,11);
        b ^= a;
        b -= hashRotate(a,25);
        c ^= b;
        c -= hashRotate(b,16);
        a ^= c;
        a -= hashRotate(c,4);
        b ^= a;
        b -= hashRotate(a,14);
        c ^= b;
        c -= hashRotate(b,24);
        return c;
    }

    unsigned int hashString(std::string str, unsigned int previousHashAsSeed)
    {
        /*
        Source: http://burtleburtle.net/bob/c/lookup3.c
        Public domain, no warranty
        Original source code has been altered.
        */
        unsigned int a, b, c, i = 0, length = str.size();
        a = 0xdeadbeef+(length<<2)+previousHashAsSeed;
        b = a;
        c = a;
        while (length > 3)
        {
            a += (unsigned int)str[(i*3)+0];
            b += (unsigned int)str[(i*3)+1];
            c += (unsigned int)str[(i*3)+2];
            hashMix(a,b,c);
            length -= 3;
            i++;
        }
        switch (length)
        {
        case 3:
            c += (unsigned int)str[2];
        case 2:
            b += (unsigned int)str[1];
        case 1:
            a += (unsigned int)str[0];
            hashFinalMix(a,b,c);
        case 0:
            break;
        }
        return c;
    }

    float smoothStep(float front, float back, float value)
    {
        float temp = clamp(0,1,(value-front)/(back-front));
        return sqr(temp)*(3.0f-(temp*2));
    }

    float clamp(float front, float back, float value)
    {
        return std::min(std::max(value,front),back);
    }
}
