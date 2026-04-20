#pragma once
#include "BaseCharacter.h"
#include <vector>
#include <string>
#include <iostream>

class Hornet : public BaseCharacter {
public:
    int status; // 0IDLE 1RUN 2DASH 3JUMP 4SILK

    sf::Sprite sprite;
    sf::Sprite effectSprite;

    std::vector<sf::Texture> idle_tex, run_tex, dash_tex, jump_tex;
    std::vector<sf::Texture> silk_tex;
    std::vector<sf::Texture> silk_eff_tex;

    float t_anim, t_eff;
    int cur_frame, eff_frame;

    bool is_dash, is_silk;
    float dash_s, dash_t, dash_dur;

    float dash_cd;
    float silk_cd;

    Hornet(float startX, float startY)
        : BaseCharacter(startX, startY, sf::Color::Transparent) {
        hp = 12;
        t_anim = 0.0f; t_eff = 0.0f;
        cur_frame = 0; eff_frame = 0;
        status = 0;
        is_dash = false; is_silk = false;
        dash_s = 35.0f; dash_dur = 0.3f;
        jump_p = -38.0f;

        dash_cd = 0.0f;
        silk_cd = 0.0f;

        float s = 1.6f;
        shape.setSize(sf::Vector2f(50.0f * s, 110.0f * s));

        for (int i = 1; i <= 6; ++i) {
            sf::Texture tex;
            if (tex.loadFromFile("Hornet/Idle/" + std::to_string(i) + ".png")) idle_tex.push_back(tex);
        }
        for (int i = 1; i <= 6; ++i) {
            sf::Texture tex;
            if (tex.loadFromFile("Hornet/Run/" + std::to_string(i) + ".png")) run_tex.push_back(tex);
        }
        for (int i = 1; i <= 2; ++i) {
            sf::Texture tex;
            if (tex.loadFromFile("Hornet/Dash/" + std::to_string(i) + ".png")) dash_tex.push_back(tex);
        }
        for (int i = 1; i <= 4; ++i) {
            sf::Texture tex;
            if (tex.loadFromFile("Hornet/Jump/" + std::to_string(i) + ".png")) jump_tex.push_back(tex);
        }
        for (int i = 1; i <= 17; ++i) {
            sf::Texture tex;
            if (tex.loadFromFile("Hornet/Silk/" + std::to_string(i) + ".png")) silk_tex.push_back(tex);
        }
        for (int i = 1; i <= 9; ++i) {
            sf::Texture tex;
            if (tex.loadFromFile("Hornet/SilkEffect/" + std::to_string(i) + ".png")) silk_eff_tex.push_back(tex);
        }

        if (!idle_tex.empty()) {
            sprite.setTexture(idle_tex[0], true);
            sprite.setOrigin(idle_tex[0].getSize().x / 2.0f, idle_tex[0].getSize().y);
        }
    }

    void dash() {
        if (!is_dash && !is_silk && on_ground && dash_cd <= 0.0f) {
            is_dash = true;
            dash_t = dash_dur;
            dash_cd = 3.0f;
            status = 2;
            cur_frame = 0;
        }
    }

    void silk() {
        if (!is_silk && !on_ground && silk_cd <= 0.0f) {
            is_silk = true;
            silk_cd = 3.0f;
            status = 4;
            cur_frame = 0;
            eff_frame = 0;
            v_y = 0;
        }
    }

    void updatePhysics(float floorY, float mapWidth) override {
        if (dash_cd > 0.0f) dash_cd -= 0.016f;
        if (silk_cd > 0.0f) silk_cd -= 0.016f;

        if (is_silk) {
            v_y += 0.2f;
            shape.move(0, v_y);
            if (shape.getPosition().y + shape.getSize().y >= floorY) {
                shape.setPosition(shape.getPosition().x, floorY - shape.getSize().y);
                is_silk = false; v_y = 0; on_ground = true;
            }
        }
        else if (is_dash) {
            dash_t -= 0.016f;
            if (face_right) shape.move(dash_s, 0);
            else shape.move(-dash_s, 0);

            if (shape.getPosition().x < 0) shape.setPosition(0, shape.getPosition().y);
            if (shape.getPosition().x > mapWidth - shape.getSize().x) shape.setPosition(mapWidth - shape.getSize().x, shape.getPosition().y);

            if (dash_t <= 0) {
                is_dash = false;
            }
        }
        else {
            BaseCharacter::updatePhysics(floorY, mapWidth);
        }

        if (is_silk) status = 4;
        else if (is_dash) status = 2;
        else if (!on_ground) status = 3;
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) status = 1;
        else status = 0;

        std::vector<sf::Texture>* cur_t = &idle_tex;
        if (status == 1) cur_t = &run_tex;
        else if (status == 3) cur_t = &jump_tex;
        else if (status == 2) cur_t = &dash_tex;
        else if (status == 4) cur_t = &silk_tex;

        if (cur_t && !cur_t->empty()) {
            t_anim += 0.016f;
            float spd = (status == 4) ? 0.04f : ((status == 0) ? 0.1f : 0.08f);
            if (t_anim > spd) {
                cur_frame++;

                if (is_silk && cur_frame >= cur_t->size()) {
                    is_silk = false;
                    cur_frame = 0;
                }
                else if ((status == 3 || status == 2) && cur_frame >= cur_t->size()) {
                    cur_frame = cur_t->size() - 1;
                }
                else if (status != 4 && status != 3 && status != 2) {
                    cur_frame %= cur_t->size();
                }

                sprite.setTexture(cur_t->at(cur_frame), true);
                sprite.setOrigin(cur_t->at(cur_frame).getSize().x / 2.0f, cur_t->at(cur_frame).getSize().y);
                t_anim = 0.0f;
            }
        }

        if (is_silk && !silk_eff_tex.empty()) {
            t_eff += 0.016f;
            if (t_eff > 0.05f) {
                eff_frame++;
                t_eff = 0.0f;
            }
            if (eff_frame < silk_eff_tex.size()) {
                effectSprite.setTexture(silk_eff_tex[eff_frame], true);
                effectSprite.setOrigin(silk_eff_tex[eff_frame].getSize().x / 2.0f, silk_eff_tex[eff_frame].getSize().y / 2.0f);
                effectSprite.setPosition(sprite.getPosition().x, sprite.getPosition().y - shape.getSize().y / 2.0f);
                effectSprite.setScale(2.0f, 2.0f);
            }
        }

        sprite.setPosition(shape.getPosition().x + shape.getSize().x / 2.0f, shape.getPosition().y + shape.getSize().y);
        float s = 1.6f;
        sprite.setScale(face_right ? -s : s, s);
    }
};