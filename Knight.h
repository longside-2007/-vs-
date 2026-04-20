#pragma once
#include "BaseCharacter.h"
#include <vector>
#include <string>
#include <iostream>

class Knight : public BaseCharacter {
public:
    int status; // 0=IDLE, 1=RUN, 2=JUMP, 3=ATTACK, 4=DOWN_ATTACK

    sf::Sprite sprite;
    std::vector<sf::Texture> idle_tex;
    std::vector<sf::Texture> run_tex;
    std::vector<sf::Texture> jump_tex;
    std::vector<sf::Texture> slash_tex;
    std::vector<sf::Texture> slash_eff_tex;
    std::vector<sf::Texture> down_slash_tex;
    std::vector<sf::Texture> down_eff_tex;

    sf::Sprite effectSprite;
    bool is_atk;
    float t_anim;
    float t_eff;
    int cur_frame;
    int eff_frame;

    Knight(float startX, float startY)
        : BaseCharacter(startX, startY, sf::Color::Transparent) {
        hp = 12;
        t_anim = 0.0f;
        t_eff = 0.0f;
        cur_frame = 0;
        eff_frame = 0;
        status = 0;
        is_atk = false;
        jump_p = -35.0f;
        float scale = 1.5f;
        shape.setSize(sf::Vector2f(80.0f * scale, 120.0f * scale));

        for (int i = 0; i <= 8; ++i) {
            sf::Texture tex;
            std::string num = std::to_string(i);
            if (i < 10) num = "0" + num;
            if (tex.loadFromFile("Knight/Idle/001-" + num + ".png")) idle_tex.push_back(tex);
        }
        for (int i = 0; i <= 12; ++i) {
            sf::Texture tex;
            std::string num = std::to_string(i);
            if (i < 10) num = "0" + num;
            if (tex.loadFromFile("Knight/Run/005-" + num + ".png")) run_tex.push_back(tex);
        }
        for (int i = 0; i <= 11; ++i) {
            sf::Texture tex;
            std::string num = std::to_string(i);
            if (i < 10) num = "0" + num;
            if (tex.loadFromFile("Knight/Jump/003-" + num + ".png")) jump_tex.push_back(tex);
        }
        for (int i = 0; i <= 14; ++i) {
            sf::Texture tex;
            std::string num = std::to_string(i);
            if (i < 10) num = "0" + num;
            if (tex.loadFromFile("Knight/Slash/006-" + num + ".png")) slash_tex.push_back(tex);
        }
        for (int i = 0; i <= 3; ++i) {
            sf::Texture tex;
            std::string num = std::to_string(i);
            if (i < 10) num = "0" + num;
            if (tex.loadFromFile("Knight/SlashEffect/007-" + num + ".png")) slash_eff_tex.push_back(tex);
        }
        for (int i = 0; i <= 14; ++i) {
            sf::Texture tex;
            std::string num = std::to_string(i);
            if (i < 10) num = "0" + num;
            std::string path;
            if (i <= 5) path = "Knight/DownSlash/010-" + num + ".png";
            else path = "Knight/DownSlash/010-" + num + "-147.png";

            if (tex.loadFromFile(path)) down_slash_tex.push_back(tex);
        }
        for (int i = 0; i <= 3; ++i) {
            sf::Texture tex;
            std::string num = std::to_string(i);
            if (i < 10) num = "0" + num;
            if (tex.loadFromFile("Knight/DownSlashEffect/016-" + num + ".png")) down_eff_tex.push_back(tex);
        }

        if (!idle_tex.empty()) {
            sprite.setTexture(idle_tex[0], true);
            sprite.setOrigin(idle_tex[0].getSize().x / 2.0f, idle_tex[0].getSize().y);
        }
    }

    void attack() {
        if (!is_atk) {
            is_atk = true;
            status = 3;
            cur_frame = 0;
            eff_frame = 0;
        }
    }

    void downAttack() {
        if (!is_atk && !on_ground) {
            is_atk = true;
            status = 4;
            cur_frame = 0;
            eff_frame = 0;
        }
    }

    void updatePhysics(float floorY, float mapWidth) override {
        BaseCharacter::updatePhysics(floorY, mapWidth);

        if (is_atk) {
            // do nothing
        }
        else if (!on_ground) {
            status = 2;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            status = 1;
        }
        else {
            status = 0;
        }

        std::vector<sf::Texture>* current_tex_arr = &idle_tex;
        if (status == 1) current_tex_arr = &run_tex;
        if (status == 2) current_tex_arr = &jump_tex;
        if (status == 3) current_tex_arr = &slash_tex;
        if (status == 4) current_tex_arr = &down_slash_tex;

        if (current_tex_arr && !current_tex_arr->empty()) {
            t_anim += 0.016f;
            float speed = (is_atk) ? 0.03f : ((status == 0) ? 0.1f : 0.08f);

            if (t_anim > speed) {
                cur_frame++;

                if (is_atk && cur_frame >= current_tex_arr->size()) {
                    is_atk = false;
                    cur_frame = 0;
                    status = on_ground ? 0 : 2;
                    current_tex_arr = on_ground ? &idle_tex : &jump_tex;
                }
                else if (!is_atk && cur_frame >= current_tex_arr->size()) {
                    cur_frame = 0;
                }
                else if (status == 2 && cur_frame >= current_tex_arr->size()) {
                    cur_frame = current_tex_arr->size() - 1;
                }

                sprite.setTexture(current_tex_arr->at(cur_frame), true);
                sprite.setOrigin(current_tex_arr->at(cur_frame).getSize().x / 2.0f, current_tex_arr->at(cur_frame).getSize().y);
                t_anim = 0.0f;
            }

            sprite.setPosition(shape.getPosition().x + shape.getSize().x / 2.0f, shape.getPosition().y + shape.getSize().y);

            float s = 1.5f;
            if (face_right) sprite.setScale(-s, s);
            else sprite.setScale(s, s);
        }

        if (is_atk) {
            std::vector<sf::Texture>* eff = (status == 3) ? &slash_eff_tex : &down_eff_tex;

            if (eff && !eff->empty() && eff_frame < eff->size()) {
                t_eff += 0.016f;
                if (t_eff > 0.05f) {
                    eff_frame++;
                    t_eff = 0.0f;
                }

                if (eff_frame < eff->size()) {
                    effectSprite.setTexture(eff->at(eff_frame), true);
                    effectSprite.setOrigin(eff->at(eff_frame).getSize().x / 2.0f, eff->at(eff_frame).getSize().y / 2.0f);

                    float px = sprite.getPosition().x;
                    float py = sprite.getPosition().y - shape.getSize().y / 2.0f;

                    float s = 1.5f;
                    if (status == 3) {
                        float ox = face_right ? 100.0f : -100.0f;
                        effectSprite.setPosition(px + ox, py);
                        effectSprite.setScale(face_right ? -s : s, s);
                        effectSprite.setRotation(0);
                    }
                    else if (status == 4) {
                        effectSprite.setPosition(px, sprite.getPosition().y + 50.0f);
                        effectSprite.setScale(s, s);
                    }
                }
            }
        }
    }
};