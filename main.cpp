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

    
    float maxBarWidth = 400.0f;
    float barHeight = 25.0f;
    float maxHP = 12.0f; 

    // 1P: 小骑士血条
    sf::RectangleShape p1BarBack(sf::Vector2f(maxBarWidth, barHeight));
    p1BarBack.setFillColor(sf::Color(80, 80, 80, 200));
    p1BarBack.setPosition(50.0f, 40.0f);

    sf::RectangleShape p1BarFront(sf::Vector2f(maxBarWidth, barHeight));
    p1BarFront.setFillColor(sf::Color::White);
    p1BarFront.setPosition(50.0f, 40.0f);

    // 2P: 大黄蜂血条
    sf::RectangleShape p2BarBack(sf::Vector2f(maxBarWidth, barHeight));
    p2BarBack.setFillColor(sf::Color(80, 80, 80, 200));
    p2BarBack.setPosition(1280.0f - 50.0f - maxBarWidth, 40.0f);

    sf::RectangleShape p2BarFront(sf::Vector2f(maxBarWidth, barHeight));
    p2BarFront.setFillColor(sf::Color(255, 50, 50));
    p2BarFront.setPosition(1280.0f - 50.0f - maxBarWidth, 40.0f);

    sf::View mainCamera(sf::FloatRect(0, 0, 1280, 720));

    // 主循环
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            // 跳跃事件与攻击事件
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::W) player1.jump();
                if (event.key.code == sf::Keyboard::Up) player2.jump();
                if (event.key.code == sf::Keyboard::Numpad0) player2.dash();

                
                if (event.key.code == sf::Keyboard::J) {
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
                        player1.downAttack();
                    }
                    else {
                        player1.attack();
                    }
                }
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

        // 小骑士碰触大黄蜂本体扣血
        if (player1.shape.getGlobalBounds().intersects(player2.shape.getGlobalBounds())) {
            if (player1.invinTimer <= 0) {
                player1.hp -= 1;
                player1.invinTimer = 60.0f;
                player1.velocityY = -15.0f;
                player1.isGrounded = false;
                std::cout << "Hit! Knight HP left: " << player1.hp << std::endl;
            }
        }

        // 大黄蜂受击闪烁
        if (player2.invinTimer > 0) {
            player2.invinTimer -= 1.0f;
            player2.sprite.setColor(sf::Color(255, 150, 150, 200));
        }
        else {
            player2.sprite.setColor(sf::Color(255, 255, 255, 255));
        }

        //骑士的攻击判定框
        if (player1.isAttacking && player1.effectFrame >= 0 && player1.effectFrame <= 2) {

            sf::FloatRect attackRect;
            float attackWidth = 120.0f;
            float attackHeight = 100.0f;

            if (player1.currentState == Knight::DOWN_ATTACK) {
                attackRect.left = player1.shape.getPosition().x - 20.0f;
                attackRect.top = player1.shape.getPosition().y + player1.shape.getSize().y;
                attackRect.width = attackWidth;
                attackRect.height = attackHeight;
            }
            else {
                attackRect.top = player1.shape.getPosition().y;
                attackRect.width = attackWidth;
                attackRect.height = attackHeight;
                if (player1.isFacingRight) {
                    attackRect.left = player1.shape.getPosition().x + player1.shape.getSize().x;
                }
                else {
                    attackRect.left = player1.shape.getPosition().x - attackWidth;
                }
            }

            if (attackRect.intersects(player2.shape.getGlobalBounds())) {
                if (player2.invinTimer <= 0) {
                    player2.hp -= 1;
                    player2.invinTimer = 30.0f;

                    player2.velocityY = -8.0f;
                    float knockback = player1.isFacingRight ? 15.0f : -15.0f;
                    player2.shape.move(knockback, 0);

                    if (player1.currentState == Knight::DOWN_ATTACK) {
                        player1.velocityY = -35.0f;
                    }

                    std::cout << "Hornet Hit! HP left: " << player2.hp << std::endl;
                }
            }
        } 

        // 小骑士受击闪烁
        if (player1.invinTimer > 0) {
            player1.sprite.setColor(sf::Color(255, 150, 150, 200));
        }
        else {
            player1.sprite.setColor(sf::Color(255, 255, 255, 255));
        }

        // 镜头跟随计算
        float p1CenterX = player1.shape.getPosition().x + player1.shape.getSize().x / 2.0f;
        float p1CenterY = player1.shape.getPosition().y + player1.shape.getSize().y / 2.0f;
        float p2CenterX = player2.shape.getPosition().x + player2.shape.getSize().x / 2.0f;
        float p2CenterY = player2.shape.getPosition().y + player2.shape.getSize().y / 2.0f;

        float targetCenterX = (p1CenterX + p2CenterX) / 2.0f;
        float targetCenterY = (p1CenterY + p2CenterY) / 2.0f;

        float distanceX = std::abs(p1CenterX - p2CenterX);

        float currentViewWidth = std::max(2000.0f, distanceX + 600.0f);
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

        // 渲染画面
        window.setView(mainCamera);
        window.clear(sf::Color::Black);
        window.draw(bgSprite);

        window.draw(player1.sprite);
        if (player1.isAttacking && player1.effectFrame < 4) {
            window.draw(player1.effectSprite);
        }
        window.draw(player2.sprite);

        // 渲染 UI 血条
        window.setView(window.getDefaultView()); 

        float p1CurrentWidth = (player1.hp / maxHP) * maxBarWidth;
        if (p1CurrentWidth < 0) p1CurrentWidth = 0;
        p1BarFront.setSize(sf::Vector2f(p1CurrentWidth, barHeight));

        window.draw(p1BarBack);
        window.draw(p1BarFront);

        float p2CurrentWidth = (player2.hp / maxHP) * maxBarWidth;
        if (p2CurrentWidth < 0) p2CurrentWidth = 0;
        p2BarFront.setSize(sf::Vector2f(p2CurrentWidth, barHeight));

        window.draw(p2BarBack);
        window.draw(p2BarFront);

        window.display();
    }
    return 0;
}