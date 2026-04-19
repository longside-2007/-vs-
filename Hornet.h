#pragma once
#include "BaseCharacter.h"
#include <vector>
#include <string>
#include <iostream>

class Hornet : public BaseCharacter {
public:
    // 1. 新增 JUMP 状态
    enum CharState { IDLE, RUN, DASH, JUMP };
    CharState currentState;

    sf::Sprite sprite;
    std::vector<sf::Texture> idleTextures;
    std::vector<sf::Texture> runTextures;
    std::vector<sf::Texture> dashTextures;
    std::vector<sf::Texture> jumpTextures; // 2. 新增跳跃动画数组
    float animTimer;
    int currentFrame;

    // ================= 冲刺技能专属变量 =================
    bool isDashing;
    float dashSpeed;
    float dashDuration;
    float dashTimer;

    Hornet(float startX, float startY)
        : BaseCharacter(startX, startY, sf::Color::Transparent) {
        hp = 12;
        animTimer = 0.0f;
        currentFrame = 0;
        currentState = IDLE;

        isDashing = false;
        dashSpeed = 35.0f;
        dashDuration = 0.3f;
        dashTimer = 0.0f;
        jumpPower = -38.0f;
        float scaleFactor = 1.6f;
        shape.setSize(sf::Vector2f(80.0f * scaleFactor, 130.0f * scaleFactor));

        // 加载 Idle
        for (int i = 1; i <= 6; ++i) {
            sf::Texture tex;
            if (tex.loadFromFile("Hornet/Idle/" + std::to_string(i) + ".png")) idleTextures.push_back(tex);
        }

        // 加载 Run 
        for (int i = 1; i <= 6; ++i) {
            sf::Texture tex;
            if (tex.loadFromFile("Hornet/Run/" + std::to_string(i) + ".png")) runTextures.push_back(tex);
        }

        // 加载 Dash 
        for (int i = 1; i <= 2; ++i) {
            sf::Texture tex;
            if (tex.loadFromFile("Hornet/Dash/" + std::to_string(i) + ".png")) dashTextures.push_back(tex);
        }

       
        for (int i = 1; i <= 8; ++i) { 
            sf::Texture tex;
            if (tex.loadFromFile("Hornet/Jump/" + std::to_string(i) + ".png"))
                jumpTextures.push_back(tex);
            else
                std::cout << "找不到大黄蜂跳跃图片: Hornet/Jump/" << i << ".png" << std::endl;
        }

        if (!idleTextures.empty()) {
            sprite.setTexture(idleTextures[0], true);
            sprite.setOrigin(idleTextures[0].getSize().x / 2.0f, idleTextures[0].getSize().y);
        }
    }

    void dash() {
        if (!isDashing && isGrounded) {
            isDashing = true;
            dashTimer = dashDuration;
            currentState = DASH;
            currentFrame = 0;
        }
    }

    void updatePhysics(float floorY, float mapWidth) override {
        BaseCharacter::updatePhysics(floorY, mapWidth);

      
        if (isDashing) {
            dashTimer -= 0.016f;
            if (isFacingRight) shape.move(dashSpeed, 0);
            else shape.move(-dashSpeed, 0);

            if (dashTimer <= 0) isDashing = false;
        }
        else if (!isGrounded) {
            
            currentState = JUMP;
        }
        else {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
                currentState = RUN;
            }
            else {
                currentState = IDLE;
            }
        }

        std::vector<sf::Texture>* activeAnim = &idleTextures;
        if (currentState == RUN && !runTextures.empty()) activeAnim = &runTextures;
        if (currentState == DASH && !dashTextures.empty()) activeAnim = &dashTextures;
        if (currentState == JUMP && !jumpTextures.empty()) activeAnim = &jumpTextures;

        // 播放动画
        if (!activeAnim->empty()) {
            animTimer += 0.016f;
            float frameSpeed = (currentState == IDLE) ? 0.1f : 0.06f;

            if (animTimer > frameSpeed) {
                currentFrame = (currentFrame + 1) % activeAnim->size();

                // 冲刺和跳跃在空中卡在最后一帧
                if ((currentState == DASH || currentState == JUMP) && currentFrame == 0) {
                    currentFrame = activeAnim->size() - 1;
                }

                // 加上 true 防止裁切
                sprite.setTexture(activeAnim->at(currentFrame), true);
                sprite.setOrigin(activeAnim->at(currentFrame).getSize().x / 2.0f, activeAnim->at(currentFrame).getSize().y);
                animTimer = 0.0f;
            }

            float bottomCenterX = shape.getPosition().x + shape.getSize().x / 2.0f;
            float bottomCenterY = shape.getPosition().y + shape.getSize().y;
            sprite.setPosition(bottomCenterX, bottomCenterY);

            float scaleFactor = 1.6f;
            if (isFacingRight) sprite.setScale(-scaleFactor, scaleFactor);
            else sprite.setScale(scaleFactor, scaleFactor);
        }
    }
};