#pragma once 
#include <SFML/Graphics.hpp>

class BaseCharacter {
public:
    sf::RectangleShape shape; 
    float velocityY;          
    bool isGrounded;          
    float moveSpeed;          
    float jumpPower;         

    BaseCharacter(float startX, float startY, sf::Color color);
    virtual ~BaseCharacter(); // 虚析构函数

    // 物理与动作方法声明
    virtual void updatePhysics(float floorY, float mapWidth);
    void moveLeft();
    void moveRight();
    void jump();

    int hp;               // 血量
    bool isFacingRight;   // 面朝右边
    float invinTimer;     // 无敌帧计时器 
};