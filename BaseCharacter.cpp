#include "BaseCharacter.h"

BaseCharacter::BaseCharacter(float startX, float startY, sf::Color color) {
    shape.setSize(sf::Vector2f(80.0f, 120.0f));
    shape.setFillColor(color);
    shape.setPosition(startX, startY);

    v_y = 0.0f;
    on_ground = false;
    speed = 15.0f;
    jump_p = -25.0f;
    hp = 100;
    face_right = true;
    wudi_time = 0.0f;
}

BaseCharacter::~BaseCharacter() {}

void BaseCharacter::updatePhysics(float floorY, float mapWidth) {
    // 往下掉
    if (on_ground == false) {
        v_y += 1.2f;
    }
    shape.move(0, v_y);

    if (shape.getPosition().y + shape.getSize().y >= floorY) {
        shape.setPosition(shape.getPosition().x, floorY - shape.getSize().y);
        v_y = 0.0f;
        on_ground = true;
    }
    else {
        on_ground = false;
    }

    // 墙
    if (shape.getPosition().x < 0) {
        shape.setPosition(0, shape.getPosition().y);
    }
    if (shape.getPosition().x > mapWidth - shape.getSize().x) {
        shape.setPosition(mapWidth - shape.getSize().x, shape.getPosition().y);
    }
}

void BaseCharacter::moveLeft() {
    face_right = false;
    shape.move(-speed, 0);
}
void BaseCharacter::moveRight() {
    face_right = true;
    shape.move(speed, 0);
}

void BaseCharacter::jump() {
    if (on_ground) {
        v_y = jump_p;
        on_ground = false;
    }
}