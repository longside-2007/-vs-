#pragma once
#include "BaseCharacter.h"

class Knight : public BaseCharacter {
public:
    Knight(float startX, float startY)
        : BaseCharacter(startX, startY, sf::Color::White) {
        hp = 5; 
    }

    
    void attack() {
        
    }
};