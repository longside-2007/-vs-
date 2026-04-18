#pragma once
#include "BaseCharacter.h"

class Hornet : public BaseCharacter {
public:
    Hornet(float startX, float startY)
        : BaseCharacter(startX, startY, sf::Color::Red) {
        hp = 10; 
        moveSpeed = 18.0f; 
    }

    
    void dash() {
       
    }
};
