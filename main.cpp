#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <cmath>
#include <iostream>
#include "Knight.h"
#include "Hornet.h"

int main()
{
    sf::RenderWindow window(sf::VideoMode(1280, 720), "Hollow Knight VS Hornet - FINAL");
    window.setFramerateLimit(60);

    float MAP_WIDTH = 2728.0f;
    float MAP_HEIGHT = 1536.0f;
    float FLOOR_Y = 1454.0f;

    sf::Texture bg_tex;
    sf::Sprite bg_sp;
    bg_tex.loadFromFile("background.png");
    bg_sp.setTexture(bg_tex);

    sf::Font f;
    f.loadFromFile("arial.ttf");

    sf::Text win_t;
    win_t.setFont(f);
    win_t.setCharacterSize(100);
    win_t.setStyle(sf::Text::Bold);

    sf::Text res_t;
    res_t.setFont(f);
    res_t.setCharacterSize(30);
    res_t.setString("Press 'R' to Restart");
    res_t.setFillColor(sf::Color::White);

    sf::Music m_bgm;
    if (m_bgm.openFromFile("Hornet.mp3")) {
        m_bgm.setLoop(true);
        m_bgm.setVolume(40.0f);
        m_bgm.play();
    }

    sf::SoundBuffer sb1, sb2;
    sb1.loadFromFile("sword_up.wav");
    sb2.loadFromFile("player_damage.wav");
    sf::Sound snd_swing(sb1);
    sf::Sound snd_dmg(sb2);
    snd_swing.setVolume(70.0f);
    snd_dmg.setVolume(100.0f);

    sf::SoundBuffer hb1, hb2, hb3, hb4, hb5;
    hb1.loadFromFile("heigali.mp3");
    hb2.loadFromFile("aidito.mp3");
    hb3.loadFromFile("henhen.mp3");
    hb4.loadFromFile("death.mp3");
    hb5.loadFromFile("haha.mp3");

    sf::Sound s_dash(hb1);
    sf::Sound s_silk(hb2);
    sf::Sound s_hurt(hb3);
    sf::Sound s_die(hb4);
    sf::Sound s_xiao(hb5);

    Knight p1(800.0f, FLOOR_Y - 200.0f);
    Hornet p2(1800.0f, FLOOR_Y - 200.0f);

    // ui
    float bar_w = 400.0f;
    float bar_h = 25.0f;

    sf::RectangleShape bar1_bg(sf::Vector2f(bar_w, bar_h));
    bar1_bg.setFillColor(sf::Color(80, 80, 80, 200));
    bar1_bg.setPosition(50.0f, 40.0f);
    sf::RectangleShape bar1_in(sf::Vector2f(bar_w, bar_h));
    bar1_in.setFillColor(sf::Color::White);
    bar1_in.setPosition(50.0f, 40.0f);

    sf::RectangleShape bar2_bg(sf::Vector2f(bar_w, bar_h));
    bar2_bg.setFillColor(sf::Color(80, 80, 80, 200));
    bar2_bg.setPosition(1280.0f - 50.0f - bar_w, 40.0f);
    sf::RectangleShape bar2_in(sf::Vector2f(bar_w, bar_h));
    bar2_in.setFillColor(sf::Color(255, 50, 50));
    bar2_in.setPosition(1280.0f - 50.0f - bar_w, 40.0f);

    int game_state = 0; // 0玩 1结束
    sf::View cam(sf::FloatRect(0, 0, 1280, 720));

    while (window.isOpen())
    {
        sf::Event e;
        while (window.pollEvent(e))
        {
            if (e.type == sf::Event::Closed) window.close();

            if (e.type == sf::Event::KeyPressed) {
                if (game_state == 0) {
                    if (e.key.code == sf::Keyboard::W) p1.jump();
                    if (e.key.code == sf::Keyboard::J) {
                        bool was_a = p1.is_atk;
                        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
                            p1.downAttack();
                        }
                        else {
                            p1.attack();
                        }
                        if (was_a == false && p1.is_atk) snd_swing.play();
                    }

                    if (e.key.code == sf::Keyboard::Up) p2.jump();
                    if (e.key.code == sf::Keyboard::Numpad0) {
                        bool was_d = p2.is_dash;
                        p2.dash();
                        if (was_d == false && p2.is_dash) s_dash.play();
                    }
                    if (e.key.code == sf::Keyboard::Numpad1) {
                        bool was_s = p2.is_silk;
                        p2.silk();
                        if (was_s == false && p2.is_silk) s_silk.play();
                    }
                }
                else if (game_state == 1) {
                    if (e.key.code == sf::Keyboard::R) {
                        p1.hp = 12; p2.hp = 12;
                        p1.shape.setPosition(800.0f, FLOOR_Y - 200.0f);
                        p2.shape.setPosition(1800.0f, FLOOR_Y - 200.0f);
                        p1.v_y = 0; p2.v_y = 0;
                        game_state = 0;
                        m_bgm.play();
                    }
                }
            }
        }

        if (game_state == 0) {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) p1.moveLeft();
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) p1.moveRight();
            if (p2.is_silk == false && p2.is_dash == false) {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) p2.moveLeft();
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) p2.moveRight();
            }

            p1.updatePhysics(FLOOR_Y, MAP_WIDTH);
            p2.updatePhysics(FLOOR_Y, MAP_WIDTH);

            if (p1.wudi_time > 0) p1.wudi_time -= 1.0f;
            if (p2.wudi_time > 0) p2.wudi_time -= 1.0f;

            if (p1.wudi_time > 0) p1.sprite.setColor(sf::Color(255, 150, 150, 200));
            else p1.sprite.setColor(sf::Color::White);

            if (p2.wudi_time > 0) p2.sprite.setColor(sf::Color(255, 150, 150, 200));
            else p2.sprite.setColor(sf::Color::White);

            // 碰
            if (p1.shape.getGlobalBounds().intersects(p2.shape.getGlobalBounds()) && !p2.is_dash) {
                if (p1.wudi_time <= 0) {
                    p1.hp -= 1; p1.wudi_time = 60.0f;
                    p1.v_y = -15.0f; p1.on_ground = false;
                    snd_dmg.play();
                }
            }

            if (p2.is_dash && p2.shape.getGlobalBounds().intersects(p1.shape.getGlobalBounds())) {
                if (p1.wudi_time <= 0) {
                    p1.hp -= 1; p1.wudi_time = 60.0f;
                    p1.v_y = -10.0f;
                    if (p2.face_right) p1.shape.move(30.0f, 0);
                    else p1.shape.move(-30.0f, 0);
                    snd_dmg.play();
                }
            }

            if (p1.is_atk && p1.eff_frame >= 0 && p1.eff_frame <= 2) {
                sf::FloatRect r;
                float aw = 200.0f; float ah = 140.0f;
                if (p1.status == 4) {
                    r = sf::FloatRect(p1.shape.getPosition().x - 40.0f, p1.shape.getPosition().y + p1.shape.getSize().y, p1.shape.getSize().x + 80.0f, 150.0f);
                }
                else {
                    float sx = 0;
                    if (p1.face_right) sx = p1.shape.getPosition().x + p1.shape.getSize().x;
                    else sx = p1.shape.getPosition().x - aw;
                    r = sf::FloatRect(sx, p1.shape.getPosition().y - 20.0f, aw, ah);
                }

                if (r.intersects(p2.shape.getGlobalBounds()) && p2.wudi_time <= 0) {
                    p2.hp -= 1; p2.wudi_time = 30.0f;
                    p2.v_y = -8.0f;
                    if (p1.face_right) p2.shape.move(15.0f, 0);
                    else p2.shape.move(-15.0f, 0);

                    if (p1.status == 4) p1.v_y = -25.0f;

                    snd_dmg.play();
                    s_hurt.play();
                }
            }

            if (p2.is_silk && p2.eff_frame >= 1 && p2.eff_frame <= 7) {
                float dx = (p2.shape.getPosition().x + p2.shape.getSize().x / 2.0f) - (p1.shape.getPosition().x + p1.shape.getSize().x / 2.0f);
                float dy = (p2.shape.getPosition().y + p2.shape.getSize().y / 2.0f) - (p1.shape.getPosition().y + p1.shape.getSize().y / 2.0f);

                if (std::sqrt(dx * dx + dy * dy) < 350.0f && p1.wudi_time <= 0) {
                    p1.hp -= 2; p1.wudi_time = 60.0f;
                    snd_dmg.play();
                }
            }

            // 死
            if (p1.hp <= 0 || p2.hp <= 0) {
                if (game_state == 0) {
                    m_bgm.stop();

                    if (p2.hp <= 0) {
                        s_die.play();
                    }
                    else if (p1.hp <= 0) {
                        s_xiao.play();
                    }
                    game_state = 1;
                }

                if (p1.hp <= 0) {
                    win_t.setString("HORNET WINS!");
                    win_t.setFillColor(sf::Color(255, 50, 50));
                }
                else {
                    win_t.setString("KNIGHT WINS!");
                    win_t.setFillColor(sf::Color::White);
                }

                sf::FloatRect tr = win_t.getLocalBounds();
                win_t.setOrigin(tr.left + tr.width / 2.0f, tr.top + tr.height / 2.0f);
                win_t.setPosition(640.0f, 300.0f);

                sf::FloatRect rr = res_t.getLocalBounds();
                res_t.setOrigin(rr.left + rr.width / 2.0f, rr.top + rr.height / 2.0f);
                res_t.setPosition(640.0f, 420.0f);
            }

            // 镜头
            float tx = (p1.shape.getPosition().x + p2.shape.getPosition().x) / 2.0f;
            float ty = (p1.shape.getPosition().y + p2.shape.getPosition().y) / 2.0f;
            float vw = std::abs(p1.shape.getPosition().x - p2.shape.getPosition().x) + 600.0f;

            if (vw < 2000.0f) vw = 2000.0f;
            if (vw > MAP_WIDTH) vw = MAP_WIDTH;
            float vh = vw * (720.0f / 1280.0f);

            if (tx < vw / 2.0f) tx = vw / 2.0f;
            if (tx > MAP_WIDTH - vw / 2.0f) tx = MAP_WIDTH - vw / 2.0f;
            if (ty < vh / 2.0f) ty = vh / 2.0f;
            if (ty > MAP_HEIGHT - vh / 2.0f) ty = MAP_HEIGHT - vh / 2.0f;

            cam.setSize(vw, vh);
            cam.setCenter(tx, ty);
        }

        window.setView(cam);
        window.clear(sf::Color::Black);
        window.draw(bg_sp);
        window.draw(p1.sprite);
        if (p1.is_atk) window.draw(p1.effectSprite);
        window.draw(p2.sprite);
        if (p2.is_silk) window.draw(p2.effectSprite);

        window.setView(window.getDefaultView());

        float w1 = (p1.hp / 12.0f) * bar_w;
        if (w1 < 0) w1 = 0;
        bar1_in.setSize(sf::Vector2f(w1, bar_h));

        float w2 = (p2.hp / 12.0f) * bar_w;
        if (w2 < 0) w2 = 0;
        bar2_in.setSize(sf::Vector2f(w2, bar_h));

        window.draw(bar1_bg); window.draw(bar1_in);
        window.draw(bar2_bg); window.draw(bar2_in);

        if (game_state == 1) {
            window.draw(win_t);
            window.draw(res_t);
        }

        window.display();
    }
    return 0;
}