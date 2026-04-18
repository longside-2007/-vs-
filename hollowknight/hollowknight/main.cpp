#include <SFML/Graphics.hpp>
#include <cmath>
#include <algorithm>
#include <iostream>


#include "Knight.h"
#include "Hornet.h"

int main()
{
    // 1. 创建游戏窗口
    sf::RenderWindow window(sf::VideoMode(1280, 720), "Hollow Knight VS Hornet - V3.0 Hitbox Test");
    window.setFramerateLimit(60);

    // 2. 地图规格和地板线
    const float MAP_WIDTH = 2728.0f;
    const float MAP_HEIGHT = 1536.0f;
    const float FLOOR_Y = 1454.0f;

    // 3. 加载背景图
    sf::Texture bgTexture;
    sf::Sprite bgSprite;
    if (!bgTexture.loadFromFile("background.png")) {
        std::cout << "Error: Cannot load background.png!" << std::endl;
    }
    bgSprite.setTexture(bgTexture);

    Knight player1(800.0f, FLOOR_Y - 120.0f);  
    Hornet player2(1800.0f, FLOOR_Y - 120.0f); 

    sf::View mainCamera(sf::FloatRect(0, 0, 1280, 720));

    // 主循环
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            // 跳跃事件
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::W) player1.jump();
                if (event.key.code == sf::Keyboard::Up) player2.jump();
            }
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) player1.moveLeft();
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) player1.moveRight();

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) player2.moveLeft();
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) player2.moveRight();

        player1.updatePhysics(FLOOR_Y, MAP_WIDTH);
        player2.updatePhysics(FLOOR_Y, MAP_WIDTH);

        // 递减无敌时间计时器
        if (player1.invinTimer > 0) {
            player1.invinTimer -= 1.0f;
        }

        if (player1.shape.getGlobalBounds().intersects(player2.shape.getGlobalBounds())) {
            if (player1.invinTimer <= 0) {
                player1.hp -= 1; 
                player1.invinTimer = 60.0f; 
                player1.velocityY = -15.0f;
                player1.isGrounded = false;

                std::cout << "Hit! Knight HP left: " << player1.hp << std::endl;
            }
        }

        if (player1.invinTimer > 0) {
            player1.shape.setFillColor(sf::Color(150, 150, 150));
        }
        else {
            player1.shape.setFillColor(sf::Color::White); 
        }

        float p1CenterX = player1.shape.getPosition().x + player1.shape.getSize().x / 2.0f;
        float p1CenterY = player1.shape.getPosition().y + player1.shape.getSize().y / 2.0f;
        float p2CenterX = player2.shape.getPosition().x + player2.shape.getSize().x / 2.0f;
        float p2CenterY = player2.shape.getPosition().y + player2.shape.getSize().y / 2.0f;

        float targetCenterX = (p1CenterX + p2CenterX) / 2.0f;
        float targetCenterY = (p1CenterY + p2CenterY) / 2.0f;

        float distanceX = std::abs(p1CenterX - p2CenterX);

        // 限制镜头大小与越界
        float currentViewWidth = std::max(1280.0f, distanceX + 600.0f);
        currentViewWidth = std::min(currentViewWidth, MAP_WIDTH);
        float currentViewHeight = currentViewWidth * (720.0f / 1280.0f);

        float viewLeft = targetCenterX - currentViewWidth / 2.0f;
        float viewRight = targetCenterX + currentViewWidth / 2.0f;
        float viewTop = targetCenterY - currentViewHeight / 2.0f;
        float viewBottom = targetCenterY + currentViewHeight / 2.0f;

        if (viewLeft < 0) targetCenterX += (0 - viewLeft);
        if (viewRight > MAP_WIDTH) targetCenterX -= (viewRight - MAP_WIDTH);
        if (viewTop < 0) targetCenterY += (0 - viewTop);
        if (viewBottom > MAP_HEIGHT) targetCenterY -= (viewBottom - MAP_HEIGHT);

        mainCamera.setSize(currentViewWidth, currentViewHeight);
        mainCamera.setCenter(targetCenterX, targetCenterY);
        window.setView(mainCamera);

        window.clear(sf::Color::Black);
        window.draw(bgSprite);
        window.draw(player1.shape);
        window.draw(player2.shape);
        window.display();
    }

    return 0;
}