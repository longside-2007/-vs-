#pragma once
#include "BaseCharacter.h"
#include <vector>
#include <string>
#include <iostream>

class Knight : public BaseCharacter {
public:
    
    enum CharState { IDLE, RUN, JUMP, ATTACK, DOWN_ATTACK };
    CharState currentState;

    sf::Sprite sprite;
    std::vector<sf::Texture> idleTextures;
    std::vector<sf::Texture> runTextures;
    std::vector<sf::Texture> jumpTextures;

    
    std::vector<sf::Texture> slashTextures;       // 平砍动作
    std::vector<sf::Texture> slashEffectTextures; // 平砍刀光
    std::vector<sf::Texture> downSlashTextures;   // 下劈动作
    std::vector<sf::Texture> downEffectTextures;  // 下劈刀光

    // 3刀光独立精灵与控制变量
    sf::Sprite effectSprite;
    bool isAttacking;
    float animTimer;
    float effectAnimTimer;
    int currentFrame;
    int effectFrame;

    Knight(float startX, float startY)
        : BaseCharacter(startX, startY, sf::Color::Transparent) {
        hp = 12;
        animTimer = 0.0f;
        effectAnimTimer = 0.0f;
        currentFrame = 0;
        effectFrame = 0;
        currentState = IDLE;
        isAttacking = false;
        jumpPower = -35.0f;
        float scaleFactor = 1.5f;
        shape.setSize(sf::Vector2f(80.0f * scaleFactor, 120.0f * scaleFactor));

        
        auto pad = [](int num) {
            return (num < 10) ? "0" + std::to_string(num) : std::to_string(num);
            };

        // 基础动作加载
        for (int i = 0; i <= 8; ++i) {
            sf::Texture tex;
            if (tex.loadFromFile("Knight/Idle/001-" + pad(i) + ".png")) idleTextures.push_back(tex);
        }
        for (int i = 0; i <= 12; ++i) {
            sf::Texture tex;
            if (tex.loadFromFile("Knight/Run/005-" + pad(i) + ".png")) runTextures.push_back(tex);
        }
        for (int i = 0; i <= 11; ++i) { 
            sf::Texture tex;
            if (tex.loadFromFile("Knight/Jump/003-" + pad(i) + ".png")) jumpTextures.push_back(tex);
        }        
        for (int i = 0; i <= 14; ++i) {
            sf::Texture tex;
            if (tex.loadFromFile("Knight/Slash/006-" + pad(i) + ".png")) slashTextures.push_back(tex);
        }
        // 加载 平砍特效 SlashEffect
        for (int i = 0; i <= 3; ++i) {
            sf::Texture tex;
            if (tex.loadFromFile("Knight/SlashEffect/007-" + pad(i) + ".png")) slashEffectTextures.push_back(tex);
        }

        // 加载 下劈 DownSlash 
        for (int i = 0; i <= 14; ++i) {
            sf::Texture tex;
            std::string path;
            if (i <= 5) path = "Knight/DownSlash/010-" + pad(i) + ".png";
            else path = "Knight/DownSlash/010-" + pad(i) + "-147.png"; 

            if (tex.loadFromFile(path)) downSlashTextures.push_back(tex);
            else std::cout << "找不到下劈图片: " << path << std::endl;
        }
        // 加载 下劈特效 DownSlashEffect
        for (int i = 0; i <= 3; ++i) {
            sf::Texture tex;
            if (tex.loadFromFile("Knight/DownSlashEffect/016-" + pad(i) + ".png")) downEffectTextures.push_back(tex);
        }

        // 初始贴图
        if (!idleTextures.empty()) {
            sprite.setTexture(idleTextures[0], true);
            sprite.setOrigin(idleTextures[0].getSize().x / 2.0f, idleTextures[0].getSize().y);
        }
    }

    // 触发攻击
    void attack() {
        if (!isAttacking) {
            isAttacking = true;
            currentState = ATTACK;
            currentFrame = 0;
            effectFrame = 0;
        }
    }

    void downAttack() {
        if (!isAttacking && !isGrounded) { // 空洞骑士里通常跳起来才能下劈
            isAttacking = true;
            currentState = DOWN_ATTACK;
            currentFrame = 0;
            effectFrame = 0;
        }
    }

    void updatePhysics(float floorY, float mapWidth) override {
        BaseCharacter::updatePhysics(floorY, mapWidth);

        // 优先级判定
        if (isAttacking) {
            
        }
        else if (!isGrounded) {
            currentState = JUMP;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            currentState = RUN;
        }
        else {
            currentState = IDLE;
        }

        // 绑定当前动画数组
        std::vector<sf::Texture>* activeAnim = &idleTextures;
        if (currentState == RUN) activeAnim = &runTextures;
        if (currentState == JUMP) activeAnim = &jumpTextures;
        if (currentState == ATTACK) activeAnim = &slashTextures;
        if (currentState == DOWN_ATTACK) activeAnim = &downSlashTextures;

        if (activeAnim && !activeAnim->empty()) {
            animTimer += 0.016f;
   
            float frameSpeed = (isAttacking) ? 0.03f : ((currentState == IDLE) ? 0.1f : 0.08f);

            if (animTimer > frameSpeed) {
                currentFrame++;

            
                if (isAttacking && currentFrame >= activeAnim->size()) {
                    isAttacking = false;
                    currentFrame = 0;
                    currentState = isGrounded ? IDLE : JUMP;
                    activeAnim = isGrounded ? &idleTextures : &jumpTextures;
                }
                else if (!isAttacking && currentFrame >= activeAnim->size()) {
                    currentFrame = 0; // 正常循环
                }
                else if (currentState == JUMP && currentFrame >= activeAnim->size()) {
                    currentFrame = activeAnim->size() - 1; 
                }

                sprite.setTexture(activeAnim->at(currentFrame), true);
                sprite.setOrigin(activeAnim->at(currentFrame).getSize().x / 2.0f, activeAnim->at(currentFrame).getSize().y);
                animTimer = 0.0f;
            }

            // 同步人物坐标和翻转
            float bottomCenterX = shape.getPosition().x + shape.getSize().x / 2.0f;
            float bottomCenterY = shape.getPosition().y + shape.getSize().y;
            sprite.setPosition(bottomCenterX, bottomCenterY);

            float scaleFactor = 1.5f;
            if (isFacingRight) sprite.setScale(-scaleFactor, scaleFactor);
            else sprite.setScale(scaleFactor, scaleFactor);
        }

        // 
        if (isAttacking) {
            std::vector<sf::Texture>* activeEffect = (currentState == ATTACK) ? &slashEffectTextures : &downEffectTextures;

            if (activeEffect && !activeEffect->empty() && effectFrame < activeEffect->size()) {
                effectAnimTimer += 0.016f;
                if (effectAnimTimer > 0.05f) { // 特效播放速度
                    effectFrame++;
                    effectAnimTimer = 0.0f;
                }

                if (effectFrame < activeEffect->size()) {
                    effectSprite.setTexture(activeEffect->at(effectFrame), true);
                    // 刀光居中对齐
                    effectSprite.setOrigin(activeEffect->at(effectFrame).getSize().x / 2.0f, activeEffect->at(effectFrame).getSize().y / 2.0f);

                    // 核心：设置刀光位置！
                    float playerX = sprite.getPosition().x;
                    float playerY = sprite.getPosition().y - shape.getSize().y / 2.0f; // 骑士的中心腰部

                    float scaleFactor = 1.5f;
                    if (currentState == ATTACK) {
                        // 平砍：刀光放在人前面
                        float offsetX = isFacingRight ? 100.0f : -100.0f;
                        effectSprite.setPosition(playerX + offsetX, playerY);
                        effectSprite.setScale(isFacingRight ? -scaleFactor : scaleFactor, scaleFactor);
                        effectSprite.setRotation(0);
                    }
                    else if (currentState == DOWN_ATTACK) {
                        // 下劈：刀光放在人脚下
                        effectSprite.setPosition(playerX, sprite.getPosition().y + 50.0f);
                        effectSprite.setScale(scaleFactor, scaleFactor);
                    }
                }
            }
        }
    }
};