#include "MainState.hpp"

float MainState::soundCompensation = 0.0f;

void MainState::movePlayer(bool direction)
{
    if (dead)
    {
        return;
    }
    for (unsigned int i = 0; i < lagCoverup; i++)
    {
        player->move(0,player->getRadius()*(deltaTime/lagCoverup)*(direction?20:-20));
        if (player->getPosition().y-player->getRadius() < 0)
        {
            player->setPosition(player->getPosition().x,player->getRadius());
        }
        if (player->getPosition().y+player->getRadius() > windowSize.y)
        {
            player->setPosition(player->getPosition().x,windowSize.y-player->getRadius());
        }
    }
}

void MainState::startGame(bool newLife)
{
    if (!dead)
    {
        return;
    }
    menu = false;
    play = false;
    dead = false;
    life = newLife;
    deaths = 0;
    if (life)
    {
        lives = DEFAULT_LIVES_COUNT;
    }
    else
    {
        lives = 0;
    }
    playerColor = 0;
    player = new TGE::CircleShape();
    player->setRadius(24);
    player->setOutlineColor(TGE::Color::White);
    player->setOutlineThickness(1);
    player->setPosition(player->getRadius()*10,windowSize.y/2);
    player->setOrigin(player->getRadius(),player->getRadius());
    player->setDepth(6);
    handlePlayerColor();
    drawableStack.push_back(player);
}

void MainState::endGame()
{
    if (menu)
    {
        TGE::Game::getInstance()->getWindow()->close();
        return;
    }
    dead = true;
    deaths = 0;
    lives = 0;
    util::eraseDrawable(player,&drawableStack);
    delete player;
    player = nullptr;
    for (unsigned int i = 0; i < enemies.size(); i++)
    {
        util::eraseDrawable(enemies[i]->form,&drawableStack);
        util::eraseDrawable(enemies[i]->outline,&drawableStack);
        for (unsigned int ii = 0; ii < 3; ii++)
        {
            util::eraseDrawable(enemies[i]->vertices[ii],&drawableStack);
        }
        delete enemies[i];
    }
    enemies.clear();
    if (score == 0)
    {
        play = false;
        menu = true;
        tempString = "- NOISE RUNNER -\n- BY PIERCE BROOKS -\nPRESS ENTER TO PLAY\nPRESS UP/DOWN TO MOVE\nPRESS ESCAPE TO QUIT\nPRESS F12 TO SAVE SCREENSHOT\nPRESS SPACE TO TOGGLE FULLSCREEN";
    }
    else
    {
        tempString = "- GAME OVER -\nSCORE: "+util::numToString(score)+"\nHIGH SCORE: ";
        if (life)
        {
            tempString += util::numToString(highscoreLife);
        }
        else
        {
            tempString += util::numToString(highscore);
        }
        tempString += "\nPRESS ESCAPE TO RETURN TO MENU";
        score = 0;
    }
    text->setString(tempString);
}

void MainState::updateGame(float updateTime)
{
    noise->update(updateTime);
    if (noise->getIsNewIndex())
    {
        tempString = "";
        byte = noise->getNewestByte();
        for (unsigned int i = 0; i < letters.size(); i++)
        {
            tempString.push_back(byte^i);
            if (images[tempString.back()] != nullptr)
            {
                letters[i].push_back(new Letter(images[tempString.back()]));
                letters[i].back()->sprite->setPosition(windowSize.x+maxWidth,getLetterPosition(i));
                drawableStack.push_back(letters[i].back()->sprite);
            }
        }
        if (shader != nullptr)
        {
            colorBasePrev = colorBase;
            colorInvertPrev = colorInvert;
            colorBase = util::rgbToHSV(noise->getBytesAsColor(false),true);
            colorInvert = util::rgbToHSV(noise->getBytesAsColor(true),true);
            shader->setParameter("colorBase",colorBase);
            shader->setParameter("colorBasePrev",colorBasePrev);
            shader->setParameter("colorInvert",colorInvert);
            shader->setParameter("colorInvertPrev",colorInvertPrev);
        }
    }
    if (shader != nullptr)
    {
        shader->setParameter("fadeFactor",shaderFade);
        shader->setParameter("timeBuffer",noise->getTimeBuffer());
        noise->getSprite()->setRenderStates(TGE::RenderStates(shader));
    }
    for (unsigned int i = 0; i < letters.size(); i++)
    {
        for (int ii = 0; ii < letters[i].size(); ii++)
        {
            letters[i][ii]->update();
            if (letters[i][ii]->shader != nullptr)
            {
                if (noise->getIsNewIndex())
                {
                    letters[i][ii]->shader->setParameter("colorBase",colorBase);
                    letters[i][ii]->shader->setParameter("colorBasePrev",colorBasePrev);
                    letters[i][ii]->shader->setParameter("colorInvert",colorInvert);
                    letters[i][ii]->shader->setParameter("colorInvertPrev",colorInvertPrev);
                }
                letters[i][ii]->shader->setParameter("fadeFactor",shaderFade);
                letters[i][ii]->shader->setParameter("timeBuffer",noise->getTimeBuffer());
            }
            letters[i][ii]->sprite->move((1.0f/MainState::soundCompensation)*(updateTime*noise->getWorldSpeedFactor()*noise->getWorldDirection())*noise->getWindowByteCount(),0);
            if (letters[i][ii]->sprite->getPosition().x < -letters[i][ii]->width)
            {
                util::eraseDrawable(letters[i][ii]->sprite,&drawableStack);
                delete letters[i][ii];
                letters[i].erase(letters[i].begin()+ii);
                ii--;
                continue;
            }
        }
    }
    if (shader != nullptr)
    {
        shaderFade += deltaTime*0.5f;
        if (shaderFade >= 1.0f)
        {
            shaderFade -= 1.0f;
            shaderFade = -1.0f+shaderFade;
        }
    }
}

void MainState::selectGame()
{
    menu = false;
    play = true;
    text->setString("- SELECT GAME MODE -\nPRESS ENTER TO PLAY STANDARD\nPRESS SPACE TO PLAY FREESTYLE\nPRESS ESCAPE TO RETURN TO MENU");
}

void MainState::handlePlayerColor()
{
    unsigned char first = playerColor&255;
    unsigned char second = (playerColor>>8)&255;
    unsigned char third = (playerColor>>16)&255;
    player->setFillColor(TGE::Color(first,second,third,160));
}

void MainState::debugPrint(TGE::Vector3f& vector, unsigned int tabs)
{
    for (unsigned int i = 0; i < tabs; i++)
    {
        std::cout << "\t";
    }
    std::cout << vector.x << "\n";
    for (unsigned int i = 0; i < tabs; i++)
    {
        std::cout << "\t";
    }
    std::cout << vector.y << "\n";
    for (unsigned int i = 0; i < tabs; i++)
    {
        std::cout << "\t";
    }
    std::cout << vector.z << "\n\n";
}

void MainState::captureScreen()
{
    pause = true;
    TGE::Texture* texture = new TGE::Texture();
    texture->create(windowSize.x,windowSize.y);
    texture->update(*TGE::Game::getInstance()->getWindow());
    tempString = "SCREENSHOT_";
    if (dead)
    {
        tempString += "MENU";
    }
    else
    {
        if (life)
        {
            tempString += "STANDARD_"+util::numToString(lives)+"_"+util::numToString(highscoreLife);
        }
        else
        {
            tempString += "FREESTYLE_"+util::numToString(deaths)+"_"+util::numToString(highscore);
        }
        tempString += "_"+util::numToString(score);
    }
    tempString += "_"+util::numToString(noise->getByteCount())+".png";
    texture->copyToImage().saveToFile(tempString);
    delete texture;
}

void MainState::fullscreen()
{
    pause = true;
}

void MainState::spawnCopies()
{
    if ((!check) && (noise->getIsNewIndex()))
    {
        unsigned int temp = enemies.size();
        for (int i = 0; i < temp; i++)
        {
            if (i%2 == 0)
            {
                continue;
            }
            enemies.push_back(new Enemy(enemies[i],i));
            drawableStack.push_back(enemies.back()->form);
            drawableStack.push_back(enemies.back()->outline);
            for (unsigned int i = 0; i < 3; i++)
            {
                drawableStack.push_back(enemies.back()->vertices[i]);
            }
        }
    }
}

float MainState::getLetterPosition(unsigned int index)
{
    return (windowSize.y/2)+((((windowSize.y/2)/LETTER_ROW_COUNT)*(index+((index%2==0)?0:1)))*((index%2==0)?1:-1));
}

void MainState::enter()
{
    unsigned int soundSampleRate = 0, soundChannelCount = 0;
    std::vector<unsigned int> worldSettings;
    std::string file = "NoiseRunner.exe";
    std::string fontFile = "Font/DATAT___.TTF";
    Parser parser("NoiseRunnerData");
    if (parser.stream.is_open())
    {
        if (parser.getLine())
        {
            file = parser.line;
            if (parser.getLine())
            {
                fontFile = parser.line;
                while (parser.getLine())
                {
                    worldSettings.push_back(abs(util::stringToNum(parser.line)));
                    if (worldSettings.size() == 6)
                    {
                        if (worldSettings.back() > 4)
                        {
                            worldSettings.back() = 3;
                        }
                    }
                    else if ((worldSettings.size() >= 7) && (worldSettings.size() < 11))
                    {
                        if (worldSettings.back() > 3)
                        {
                            worldSettings.back() = worldSettings.size()-7;
                        }
                    }
                }
            }
        }
    }
    parser.stream.close();
    windowSize = TGE::Vector2f(TGE::Game::getInstance()->getWindow()->getSize());
    deltaTime = 0;
    deltaTimeMax = 0.02f;
    lagCoverup = 0;
    delayMax = 1;
    MainState::soundCompensation = 2;
    if (worldSettings.size() >= 4)
    {
        delayMax = worldSettings[3];
        if (file.size() > 4)
        {
            if (file.substr(file.size()-4) == ".wav")
            {
                TGE::SoundBuffer* sound = new TGE::SoundBuffer();
                if (sound->loadFromFile(file))
                {
                    MainState::soundCompensation = 1;
                    soundSampleRate = sound->getSampleRate();
                    soundChannelCount = sound->getChannelCount();
                    worldSettings[1] = sqrtf(sound->getSampleRate()/worldSettings[3]);
                }
                delete sound;
            }
        }
    }
    else if (worldSettings.size() >= 2)
    {
        if (file.size() > 4)
        {
            if (file.substr(file.size()-4) == ".wav")
            {
                TGE::SoundBuffer* sound = new TGE::SoundBuffer();
                if (sound->loadFromFile(file))
                {
                    MainState::soundCompensation = 1;
                    soundSampleRate = sound->getSampleRate();
                    soundChannelCount = sound->getChannelCount();
                    worldSettings[1] = sqrtf(sound->getSampleRate()/worldSettings[3]);
                }
                delete sound;
            }
        }
    }
    switch (worldSettings.size())
    {
    case 0:
        noise = new Noise(file,&drawableStack);
        break;
    case 1:
        noise = new Noise(file,&drawableStack,worldSettings[0]);
        break;
    case 2:
        noise = new Noise(file,&drawableStack,worldSettings[0],worldSettings[1]);
        break;
    case 3:
        noise = new Noise(file,&drawableStack,worldSettings[0],worldSettings[1],worldSettings[2]);
        break;
    case 4:
        noise = new Noise(file,&drawableStack,worldSettings[0],worldSettings[1],worldSettings[2],worldSettings[3]);
        break;
    case 5:
        noise = new Noise(file,&drawableStack,worldSettings[0],worldSettings[1],worldSettings[2],worldSettings[3],worldSettings[4]);
        break;
    case 6:
        noise = new Noise(file,&drawableStack,worldSettings[0],worldSettings[1],worldSettings[2],worldSettings[3],worldSettings[4],worldSettings[5]);
        break;
    case 7:
        noise = new Noise(file,&drawableStack,worldSettings[0],worldSettings[1],worldSettings[2],worldSettings[3],worldSettings[4],worldSettings[5],worldSettings[6]);
        break;
    case 8:
        noise = new Noise(file,&drawableStack,worldSettings[0],worldSettings[1],worldSettings[2],worldSettings[3],worldSettings[4],worldSettings[5],worldSettings[6],worldSettings[7]);
        break;
    case 9:
        noise = new Noise(file,&drawableStack,worldSettings[0],worldSettings[1],worldSettings[2],worldSettings[3],worldSettings[4],worldSettings[5],worldSettings[6],worldSettings[7],worldSettings[8]);
        break;
    case 10:
        noise = new Noise(file,&drawableStack,worldSettings[0],worldSettings[1],worldSettings[2],worldSettings[3],worldSettings[4],worldSettings[5],worldSettings[6],worldSettings[7],worldSettings[8],worldSettings[9]);
    }
    if (soundSampleRate != 0)
    {
        noise->setSoundSampleRate(soundSampleRate);
    }
    if (soundChannelCount != 0)
    {
        noise->setSoundChannelCount(soundChannelCount);
    }
    noise->initialize();
    worldSettings.clear();
    TGE::Game::getInstance()->setView(TGE::View(TGE::FloatRect(0,0,windowSize.x,windowSize.y)));
    shader = new TGE::Shader();
    if (!shader->loadFromFile("NoiseRunnerShader.txt",TGE::Shader::Fragment))
    {
        delete shader;
        shader = nullptr;
    }
    else
    {
        shader->setParameter("textureSize",windowSize);
        shader->setParameter("textureRect",0,0,windowSize.x,windowSize.y);
    }
    font = new TGE::Font();
    font->loadFromFile(fontFile);
    text = new TGE::Text(TGE::String(""),*font,24);
    text->setColor(TGE::Color::White);
    drawableStackOverlay.push_back(text);
    dead = false;
    life = true;
    play = false;
    menu = false;
    pause = false;
    highscore = 0;
    score = 0;
    deaths = 0;
    lives = 0;
    maxWidth = 0;
    shaderFade = 0;
    player = nullptr;
    endGame();
    bindAction(TGE::Keyboard::Escape,[=](){},[=](){endGame();});
    bindAction(TGE::Keyboard::Space,[=](){},[=](){if(menu){fullscreen();}else{if(play){startGame(false);}}});
    bindAction(TGE::Keyboard::Return,[=](){},[=](){if((menu)||(play)){if(play){startGame(true);}else{selectGame();}}});
    bindAction(TGE::Keyboard::F12,[=](){},[=](){captureScreen();});
    for (int i = -128; i < 128; i++)
    {
        if (((char)i == '\t') || ((char)i == ' ') || ((char)i == '.') || ((char)i == ',') || ((char)i == '_'))
        {
            images[(char)i] = nullptr;
            continue;
        }
        tempString = "";
        tempString.push_back((char)i);
        tempString += util::numToString(i+128);
        NoiseMessage* noiseMessage = new NoiseMessage(tempString.substr(0,1),font,noise,delayMax,noise->getWorldSpeedFactor());
        if (noiseMessage->getIsRendered())
        {
            TGE::Image* image = new TGE::Image();
            if (image->loadFromFile("Letters/"+tempString.substr(1)+".png"))
            {
                images[(char)i] = image;
                if (image->getSize().x/LETTER_FRAMES_PER_ROW > maxWidth)
                {
                    maxWidth = image->getSize().x/LETTER_FRAMES_PER_ROW;
                }
            }
            else
            {
                TGE::RenderTexture* renderTexture = new TGE::RenderTexture();
                renderTexture->create(noiseMessage->getWidth()*LETTER_FRAMES_PER_ROW,noiseMessage->getHeight()*LETTER_FRAME_ROW_COUNT);
                renderTexture->clear(TGE::Color::Transparent);
                noiseMessage->getSprite()->setPosition(noiseMessage->getWidth()/2,noiseMessage->getHeight()/2);
                for (unsigned int ii = 0; ii < LETTER_FRAME_ROW_COUNT; ii++)
                {
                    for (unsigned int iii = 0; iii < LETTER_FRAMES_PER_ROW; iii++)
                    {
                        noiseMessage->update(false);
                        noiseMessage->update(true);
                        noise->update(DELTA_TIME_MAX/LETTER_FRAME_ROW_COUNT);
                        renderTexture->draw(*noiseMessage->getSprite());
                        noiseMessage->getSprite()->move(noiseMessage->getWidth(),0);
                    }
                    noiseMessage->getSprite()->move(-noiseMessage->getWidth()*LETTER_FRAMES_PER_ROW,noiseMessage->getHeight());
                }
                renderTexture->display();
                delete image;
                image = new TGE::Image(renderTexture->getTexture().copyToImage());
                images[(char)i] = image;
                image->saveToFile("Letters/"+tempString.substr(1)+".png");
                if (renderTexture->getSize().x/LETTER_FRAMES_PER_ROW > maxWidth)
                {
                    maxWidth = renderTexture->getSize().x/LETTER_FRAMES_PER_ROW;
                }
                delete renderTexture;
            }
        }
        else
        {
            images[(char)i] = nullptr;
        }
        delete noiseMessage;
    }
    noise->initialize(true);
    for (unsigned int i = 0; i < ((LETTER_ROW_COUNT%2==0)?(LETTER_ROW_COUNT+1):LETTER_ROW_COUNT); i++)
    {
        letters.push_back(std::vector<Letter*>());
    }
    clock.restart();
    //TGE::Game::getInstance()->setView(TGE::View(TGE::FloatRect(-windowSize.x,-windowSize.y,windowSize.x*3,windowSize.y*3)));
}

void MainState::exit()
{
    endGame();
    for (unsigned int i = 0; i < letters.size(); i++)
    {
        for (unsigned int ii = 0; ii < letters[i].size(); ii++)
        {
            util::eraseDrawable(letters[i][ii]->sprite,&drawableStack);
            delete letters[i][ii];
        }
        letters[i].clear();
    }
    letters.clear();
    for (std::unordered_map<char,TGE::Image*>::iterator itr = images.begin(); itr != images.end(); itr++)
    {
        delete itr->second;
    }
    images.clear();
    delete noise;
    util::eraseDrawable(text,&drawableStackOverlay);
    delete text;
    //TGE::Game::getInstance()->setRenderShader(nullptr);
    delete shader;
    drawableStack.clear();
    drawableStackOverlay.clear();
}

void MainState::update()
{
    lagCoverup = 1;
    if (pause)
    {
        clock.restart();
    }
    else
    {
        deltaTime = clock.restart().asSeconds()*0.8f;
    }
    if (deltaTime >= deltaTimeMax)
    {
        lagCoverup = 2;
        while (deltaTime/lagCoverup >= deltaTimeMax)
        {
            lagCoverup++;
        }
        if (pause)
        {
            deltaTime = -deltaTime;
        }
        for (unsigned int i = 0; i < lagCoverup; i++)
        {
            updateGame(deltaTime/lagCoverup);
        }
    }
    else
    {
        if (pause)
        {
            deltaTime = -deltaTime;
        }
        updateGame(deltaTime/lagCoverup);
    }
    if (!dead)
    {
        if (TGE::Keyboard::isKeyPressed(TGE::Keyboard::Up))
        {
            movePlayer(false);
        }
        if (TGE::Keyboard::isKeyPressed(TGE::Keyboard::Down))
        {
            movePlayer(true);
        }
        check = noise->getEnemy(&enemy,false);
        if (check)
        {
            enemies.push_back(new Enemy(enemy,(noise->getWorldIndex()%2==0)));
            if (enemies.back()->getArea() > 500)
            {
                drawableStack.push_back(enemies.back()->form);
                drawableStack.push_back(enemies.back()->outline);
                for (unsigned int i = 0; i < 3; i++)
                {
                    drawableStack.push_back(enemies.back()->vertices[i]);
                }
            }
            else
            {
                delete enemies.back();
                enemies.pop_back();
                check = false;
            }
        }
        if (noise->getEnemy(&enemy,true))
        {
            enemies.push_back(new Enemy(enemy,(noise->getWorldIndex()%2==0)));
            if (enemies.back()->getArea() > 500)
            {
                drawableStack.push_back(enemies.back()->form);
                drawableStack.push_back(enemies.back()->outline);
                for (unsigned int i = 0; i < 3; i++)
                {
                    drawableStack.push_back(enemies.back()->vertices[i]);
                }
            }
            else
            {
                delete enemies.back();
                enemies.pop_back();
                spawnCopies();
            }
        }
        else
        {
            spawnCopies();
        }
        for (unsigned int i = 0; i < lagCoverup; i++)
        {
            for (int ii = 0; ii < enemies.size(); ii++)
            {
                enemies[ii]->update(deltaTime/lagCoverup);
                if (enemies[ii]->position.x < -128)
                {
                    util::eraseDrawable(enemies[ii]->form,&drawableStack);
                    util::eraseDrawable(enemies[ii]->outline,&drawableStack);
                    for (unsigned int iii = 0; iii < 3; iii++)
                    {
                        util::eraseDrawable(enemies[ii]->vertices[iii],&drawableStack);
                    }
                    delete enemies[ii];
                    enemies.erase(enemies.begin()+ii);
                    ii--;
                    continue;
                }
                for (unsigned int iii = 0; iii < 3; iii++)
                {
                    if (util::getDistance(player->getPosition(),(*enemies[ii]->form)[iii].position) < player->getRadius())
                    {
                        util::eraseDrawable(enemies[ii]->form,&drawableStack);
                        util::eraseDrawable(enemies[ii]->outline,&drawableStack);
                        for (unsigned int iii = 0; iii < 3; iii++)
                        {
                            util::eraseDrawable(enemies[ii]->vertices[iii],&drawableStack);
                        }
                        delete enemies[ii];
                        enemies.erase(enemies.begin()+ii);
                        ii--;
                        if (life)
                        {
                            if (lives != 0)
                            {
                                lives--;
                            }
                        }
                        else
                        {
                            score = 0;
                            deaths++;
                        }
                        break;
                    }
                }
            }
        }
        if (life)
        {
            if (lives == 0)
            {
                endGame();
            }
        }
        if (!dead)
        {
            if (noise->getIsNewIndex())
            {
                playerColor = (windowSize.y/2)-fabsf(player->getPosition().y-(windowSize.y/2));
                playerColor /= (unsigned int)(windowSize.y/8);
                playerColor += 1;
                score += util::sqr(playerColor);
                if (life)
                {
                    if (score > highscoreLife)
                    {
                        highscoreLife = score;
                    }
                }
                else
                {
                    if (score > highscore)
                    {
                        highscore = score;
                    }
                }
                playerColor = noise->getPlayer((noise->getWorldIndex()%2==0));
                handlePlayerColor();
            }
            if (life)
            {
                text->setString("SCORE: "+util::numToString(score)+"\nHIGH SCORE: "+util::numToString(highscoreLife)+"\nLIVES: "+util::numToString(lives)+"\nBYTE COUNT: "+util::numToString(noise->getByteCount()));
            }
            else
            {
                text->setString("SCORE: "+util::numToString(score)+"\nHIGH SCORE: "+util::numToString(highscore)+"\nDEATHS: "+util::numToString(deaths)+"\nBYTE COUNT: "+util::numToString(noise->getByteCount()));
            }
        }
    }
    if (pause)
    {
        deltaTime = -deltaTime;
        pause = false;
    }
    //std::cout << drawableStack.size() << "\n";
    /*
    windowRatio.x = window.x/TGE::Game::getInstance()->getWindow()->getSize().x;
    windowRatio.y = window.y/TGE::Game::getInstance()->getWindow()->getSize().y;
    mouse.x = (TGE::Mouse::getPosition(*TGE::Game::getInstance()->getWindow()).x/scale)*windowRatio.x;
    mouse.y = (TGE::Mouse::getPosition(*TGE::Game::getInstance()->getWindow()).y/scale)*windowRatio.y;
    mousePrev = mouseReal;
    mouseReal.x = TGE::Mouse::getPosition(*TGE::Game::getInstance()->getWindow()).x*windowRatio.x;
    mouseReal.y = TGE::Mouse::getPosition(*TGE::Game::getInstance()->getWindow()).y*windowRatio.y;*/
}

void MainState::Letter::update()
{
    frame++;
    if (frame == LETTER_FRAMES_PER_ROW*LETTER_FRAME_ROW_COUNT)
    {
        frame = 0;
    }
    sprite->setTextureRect(TGE::IntRect((frame%LETTER_FRAMES_PER_ROW)*width,(frame/LETTER_FRAMES_PER_ROW)*height,width,height));
    shader->setParameter("textureRect",sprite->getTextureRect().left,sprite->getTextureRect().top,sprite->getTextureRect().width,sprite->getTextureRect().height);
    //sprite->setRenderStates(TGE::RenderStates(shader));
}

MainState::Letter::Letter(TGE::Image* image)
{
    frame = 0;
    texture = new TGE::Texture();
    texture->loadFromImage(*image);
    sprite = new TGE::Sprite(*texture);
    width = texture->getSize().x/LETTER_FRAMES_PER_ROW;
    height = texture->getSize().y/LETTER_FRAME_ROW_COUNT;
    sprite->setTextureRect(TGE::IntRect(0,0,width,height));
    sprite->setOrigin(width/2,height/2);
    sprite->setDepth(2);
    shader = new TGE::Shader();
    if (!shader->loadFromFile("NoiseRunnerShader.txt",TGE::Shader::Fragment))
    {
        delete shader;
        shader = nullptr;
    }
    else
    {
        sprite->setRenderStates(TGE::RenderStates(shader));
        shader->setParameter("textureSize",TGE::Vector2f(texture->getSize()));
    }
}

MainState::Letter::~Letter()
{
    delete texture;
    delete sprite;
    delete shader;
}

float MainState::Enemy::getArea()
{
    float area = (*form)[0].position.x*((*form)[1].position.y-(*form)[2].position.y);
    area += (*form)[1].position.x*((*form)[2].position.y-(*form)[0].position.y);
    area += (*form)[2].position.x*((*form)[0].position.y-(*form)[1].position.y);
    area /= 2;
    return fabsf(area);
}

void MainState::Enemy::update(float deltaTime)
{
    deltaTime *= size*10;
    position.x -= deltaTime;
    count++;
    if (count == 3*ENEMY_COLOR_DELAY)
    {
        count = 0;
    }
    if (spin)
    {
        if (spinOrigin)
        {
            origin.x = 0;
            origin.y = 0;
            for (unsigned int i = 0; i < 3; i++)
            {
                angles[i] += (0.5f*deltaTime*(spinDirection?1:-1))/size;
                while (angles[i] >= 360.0f)
                {
                    angles[i] -= 360.0f;
                }
                while (angles[i] <= -360.0f)
                {
                    angles[i] += 360.0f;
                }
                (*form)[i].position.x = cosf(angles[i])*size;
                (*form)[i].position.y = sinf(angles[i])*size;
                origin += (*form)[i].position;
                (*form)[i].position += position;
            }
            origin.x /= 3;
            origin.y /= 3;
            distance = util::getDistance(TGE::Vector2f(),origin);
            direction = util::getDirection(TGE::Vector2f(),origin);
            for (unsigned int i = 0; i < 3; i++)
            {
                (*form)[i].position.x -= distance*cosf(direction);
                (*form)[i].position.y -= distance*sinf(direction);
                (*outline)[i].position = (*form)[i].position;
                vertices[i]->setPosition((*form)[i].position);
            }
        }
        else
        {
            for (unsigned int i = 0; i < 3; i++)
            {
                angles[i] += (0.5f*deltaTime*(spinDirection?1:-1))/size;
                while (angles[i] >= 360.0f)
                {
                    angles[i] -= 360.0f;
                }
                while (angles[i] <= -360.0f)
                {
                    angles[i] += 360.0f;
                }
                (*form)[i].position.x = cosf(angles[i])*size;
                (*form)[i].position.y = sinf(angles[i])*size;
                origin += (*form)[i].position;
                (*form)[i].position += position;
                (*outline)[i].position = (*form)[i].position;
                vertices[i]->setPosition((*form)[i].position);
            }
        }
    }
    else
    {
        for (unsigned int i = 0; i < 3; i++)
        {
            vertices[i]->move(-deltaTime,0);
            (*form)[i].color = util::hsvToRGB(TGE::Vector3f(colors[count/ENEMY_COLOR_DELAY]*360,colors[((count/ENEMY_COLOR_DELAY)+1)%3],colors[((count/ENEMY_COLOR_DELAY)+2)%3]));
            (*form)[i].color.a = 160;
            (*form)[i].position.x -= deltaTime;
            (*outline)[i].position.x -= deltaTime;
        }
    }
    (*outline)[3].position = (*outline)[0].position;
}

MainState::Enemy::Enemy(unsigned int enemy, bool center)
{
    spin = false;
    spinDirection = true;
    spinOrigin = true;
    count = 0;
    value = enemy;
    unsigned int first = value&255;
    unsigned int second = (value>>8)&255;
    unsigned int third = (value>>16)&255;
    unsigned int average = (first+second+third)/3;
    float firstAngle = ((float)first)/255;
    float secondAngle = ((float)second)/255;
    float thirdAngle = ((float)third)/255;
    position.x = TGE::Game::getInstance()->getWindow()->getSize().x+ENEMY_MAX_SIZE;
    position.y = TGE::Game::getInstance()->getWindow()->getSize().y/2;
    //if ((first > second) == (third < average))
    if (!center)
    {
        position.y += (((float)value)/0xffffff)*position.y*((first^second>third^first)?1:-1);
    }
    if ((first%2 == 0) == ((second%2 == 0) == (third%2 == 0)))
    {
        spin = !spin;
        if ((average%2 == 0) == ((first%2 == 0) == (third%2 == 0))^(second%2 == 0))
        {
            spinDirection = !spinDirection;
        }
        else
        {
            spinOrigin = !spinOrigin;
        }
        /*if (((first%2 == 0) == (second%2 == 0)) == ((third%2 == 0) == (average%2 == 0)))
        {
            spinOrigin = !spinOrigin;
        }*/
        //std::cout << spinDirection << "  " << spinOrigin << "\n";
    }
    size = sqrtf(((float)average)/255)*ENEMY_MAX_SIZE;
    form = new TGE::VertexArray(TGE::Triangles,3);
    outline = new TGE::VertexArray(TGE::LinesStrip,4);
    colors[0] = firstAngle;
    colors[1] = secondAngle;
    colors[2] = thirdAngle;
    firstAngle *= TWO_PI;
    secondAngle *= TWO_PI;
    thirdAngle *= TWO_PI;
    secondAngle += firstAngle;
    thirdAngle += secondAngle;
    angles[0] = firstAngle;
    angles[1] = secondAngle;
    angles[2] = thirdAngle;
    for (unsigned int i = 0; i < 3; i++)
    {
        (*form)[i].color = util::hsvToRGB(TGE::Vector3f(colors[count/ENEMY_COLOR_DELAY]*360,colors[((count/ENEMY_COLOR_DELAY)+1)%3],colors[((count/ENEMY_COLOR_DELAY)+2)%3]));
        (*form)[i].color.a = 160;
        (*form)[i].position.x = cosf(angles[i])*size;
        (*form)[i].position.y = sinf(angles[i])*size;
        (*form)[i].position += position;
        (*outline)[i].position = (*form)[i].position;
        (*outline)[i].color = TGE::Color::White;
        vertices.push_back(new TGE::CircleShape());
        vertices.back()->setRadius(3);
        vertices.back()->setFillColor(TGE::Color::White);
        vertices.back()->setPosition((*form)[i].position);
        vertices.back()->setOrigin(vertices.back()->getRadius(),vertices.back()->getRadius());
        vertices.back()->setDepth(5);
    }
    (*outline)[3].position = (*outline)[0].position;
    (*outline)[3].color = (*outline)[0].color;
    form->setDepth(3);
    outline->setDepth(4);
}

MainState::Enemy::Enemy(Enemy* enemy, unsigned int index)
{
    size = enemy->size;
    count = enemy->count;
    value = enemy->value^index;
    spin = enemy->spin;
    spinDirection = enemy->spinDirection;
    spinOrigin = enemy->spinOrigin;
    unsigned int first = value&255;
    unsigned int second = (value>>8)&255;
    unsigned int third = (value>>16)&255;
    unsigned int average = (first+second+third)/3;
    position.x = TGE::Game::getInstance()->getWindow()->getSize().x+ENEMY_MAX_SIZE;
    position.y = TGE::Game::getInstance()->getWindow()->getSize().y/2;
    if (average^index == (first^second > third^first))
    {
        position.y += (((float)value)/0xffffff)*position.y*((first^second>third^first)?1:-1);
    }
    form = new TGE::VertexArray(TGE::Triangles,3);
    outline = new TGE::VertexArray(TGE::LinesStrip,4);
    for (unsigned int i = 0; i < 3; i++)
    {
        angles[i] = enemy->angles[i];
        colors[i] = enemy->colors[i];
    }
    for (unsigned int i = 0; i < 3; i++)
    {
        (*form)[i].color = util::hsvToRGB(TGE::Vector3f(colors[count/ENEMY_COLOR_DELAY]*360,colors[((count/ENEMY_COLOR_DELAY)+1)%3],colors[((count/ENEMY_COLOR_DELAY)+2)%3]));
        (*form)[i].color.a = 160;
        (*form)[i].position.x = cosf(angles[i])*size;
        (*form)[i].position.y = sinf(angles[i])*size;
        (*form)[i].position += position;
        (*outline)[i].position = (*form)[i].position;
        (*outline)[i].color = TGE::Color::White;
        vertices.push_back(new TGE::CircleShape());
        vertices.back()->setRadius(3);
        vertices.back()->setFillColor(TGE::Color::White);
        vertices.back()->setPosition((*form)[i].position);
        vertices.back()->setOrigin(vertices.back()->getRadius(),vertices.back()->getRadius());
        vertices.back()->setDepth(5);
    }
    (*outline)[3].position = (*outline)[0].position;
    (*outline)[3].color = (*outline)[0].color;
    form->setDepth(3);
    outline->setDepth(4);
}

MainState::Enemy::~Enemy()
{
    delete form;
    delete outline;
    for (unsigned int i = 0; i < 3; i++)
    {
        delete vertices[i];
    }
    vertices.clear();
}
