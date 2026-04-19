#include "BaseCharacter.h"

BaseCharacter::BaseCharacter(float startX, float startY, sf::Color color) {
    shape.setSize(sf::Vector2f(80.0f, 120.0f));
    shape.setFillColor(color);
    shape.setPosition(startX, startY);

    velocityY = 0.0f;
    isGrounded = false;
    moveSpeed = 15.0f;
    jumpPower = -25.0f;
    hp = 100; 
    isFacingRight = true; 
    invinTimer = 0.0f;
}

BaseCharacter::~BaseCharacter() {}

void BaseCharacter::updatePhysics(float floorY, float mapWidth) {
    // 1. 重力模拟
    if (!isGrounded) {
        velocityY += 1.2f;
    }
    shape.move(0, velocityY);

    // 2. 地板碰撞检测
    if (shape.getPosition().y + shape.getSize().y >= floorY) {
        shape.setPosition(shape.getPosition().x, floorY - shape.getSize().y);
        velocityY = 0.0f;
        isGrounded = true;
    }
    else {
        isGrounded = false;
    }

    // 3. 左右空气墙限制
    if (shape.getPosition().x < 0) {
        shape.setPosition(0, shape.getPosition().y);
    }
    if (shape.getPosition().x > mapWidth - shape.getSize().x) {
        shape.setPosition(mapWidth - shape.getSize().x, shape.getPosition().y);
    }
}

// 基础动作实现
void BaseCharacter::moveLeft() { isFacingRight = false; 
    shape.move(-moveSpeed, 0); 
}
void BaseCharacter::moveRight() { isFacingRight = true; 
    shape.move(moveSpeed, 0); 
}

void BaseCharacter::jump() {
    if (isGrounded) {
        velocityY = jumpPower;
        isGrounded = false;
    }
}