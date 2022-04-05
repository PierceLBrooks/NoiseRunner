//TODO organize this in some way.

#ifndef UTILITIES_HPP
#define UTILITIES_HPP

#include "Tyrant/Framework.hpp"
#include "Tyrant/System/Random.hpp"
#include <cmath>
#include <sstream>
#include <string>
#include <iostream>
#include <vector>
#include <iterator>

#define DELTA_TIME_MAX 0.02f
#define MATH_E 2.718281828f
#define PI 4.0f*atanf(1.0f)
#define TWO_PI PI*2.0f
#define HALF_PI PI/2.0f
#define DEG_TO_RAD PI/180.0f
#define RAD_TO_DEG 180.0f/PI

namespace util
{
    std::string getDataDir();
    std::string numToString(float num);
    float stringToNum(std::string str);
    float numRound(float num, bool up = false);
    float getDistance(TGE::Vector2f point1, TGE::Vector2f point2);
    float getDirection(TGE::Vector2f self, TGE::Vector2f target);
    TGE::Vector2f getMidpoint(TGE::Vector2f point1, TGE::Vector2f point2);
    bool getIsLeft(TGE::Vector2f linePoint1, TGE::Vector2f linePoint2, TGE::Vector2f point);
    bool getIsVowel(char letter);
    TGE::Color hsvToRGB(TGE::Vector3f color);
    TGE::Vector3f rgbToHSV(TGE::Color color, bool normalize = false);
    void eraseDrawable(TGE::Drawable* drawable, std::vector<TGE::Drawable*>* drawableStack);
    void eraseSprites(std::vector<TGE::Sprite*>* sprites, std::vector<TGE::Drawable*>* drawableStack);
	int getGCD(int x, int y);
	float sqr(float val);
    void hashRotate(unsigned int& x, unsigned int& y);
    void hashMix(unsigned int& a, unsigned int& b, unsigned int& c);
    void hashFinalMix(unsigned int& a, unsigned int& b, unsigned int& c);
    unsigned int hashFinalMixAlt(unsigned int a, unsigned int b, unsigned int c);
    unsigned int hashString(std::string str, unsigned int previousHashAsSeed = 0);
    float smoothStep(float front, float back, float value);
    float clamp(float front, float back, float value);
}

#endif
