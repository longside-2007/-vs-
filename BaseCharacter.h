#pragma once 
#include <SFML/Graphics.hpp>
using namespace std;

class BaseCharacter {
public:
    sf::RectangleShape shape;
    float v_y;
    bool on_ground;
    float speed;
    float jump_p;

    int hp;
    bool face_right;
    float wudi_time;     //无敌 

    BaseCharacter(float startX, float startY, sf::Color color);
    virtual ~BaseCharacter();

    virtual void updatePhysics(float floorY, float mapWidth);
    void moveLeft();
    void moveRight();
    void jump();
};