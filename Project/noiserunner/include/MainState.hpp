#ifndef MAIN_STATE_HPP
#define MAIN_STATE_HPP

#include <unordered_map>
#include "Noise.hpp"
#include "NoiseMessage.hpp"
#include "Parser.hpp"
#include "Utilities.hpp"
#include "Tyrant/Framework.hpp"

#define ENEMY_COLOR_DELAY 20
#define ENEMY_MAX_SIZE 128
#define DEFAULT_LIVES_COUNT 5
#define LETTER_FRAMES_PER_ROW 60
#define LETTER_FRAME_ROW_COUNT 3
#define LETTER_ROW_COUNT 7

// HIGH SCORE: PIERCE - 5048 - 1/28/2016

class MainState : public TGE::State
{
    public:
        DECLARE_STATE_CLASS(MainState)
        void enter();
        void exit();
        void update();
        void movePlayer(bool direction);
        void startGame(bool life);
        void endGame();
        void updateGame(float updateTime);
        void selectGame();
        void handlePlayerColor();
        void debugPrint(TGE::Vector3f& vector, unsigned int tabs = 0);
        void captureScreen();
        void fullscreen();
        void spawnCopies();
        float getLetterPosition(unsigned int index);
        struct Letter
        {
            public:
                Letter(TGE::Image* image);
                ~Letter();
                void update();
                int frame, width, height;
                TGE::Sprite* sprite;
                TGE::Texture* texture;
                TGE::Shader* shader;
        };
        struct Enemy
        {
            public:
                Enemy(unsigned int enemy, bool center = false);
                Enemy(Enemy* enemy, unsigned int index);
                ~Enemy();
                float getArea();
                void update(float deltaTime);
                unsigned int size, count, value;
                float colors[3];
                float angles[3];
                float distance, direction;
                bool spin, spinDirection, spinOrigin;
                std::vector<TGE::CircleShape*> vertices;
                TGE::VertexArray* form;
                TGE::VertexArray* outline;
                TGE::Vector2f position, origin;
        };
        static float soundCompensation;

    private:
        bool dead, life, play, menu, pause, check;
        char byte;
        unsigned long score, highscore, highscoreLife, deaths;
        unsigned int lagCoverup, enemy, playerColor, delayMax, letterRowCount, lives;
        int maxWidth;
        float deltaTime, deltaTimeMax, shaderFade;
        TGE::Clock clock;
        TGE::Vector2f mouse, windowSize;
        TGE::Vector3f colorBase, colorInvert, colorBasePrev, colorInvertPrev;
        Noise* noise;
        TGE::Shader* shader;
        TGE::Font* font;
        TGE::Text* text;
        TGE::CircleShape* player;
        std::vector<Enemy*> enemies;
        std::unordered_map<char,TGE::Image*> images;
        std::vector<std::vector<Letter*>> letters;
        std::string tempString;

};

#endif
