#pragma once

#include "server.hpp"

#include <raylib.h>
#include <stdbool.h>

#include <string>

class TextBox
{
    public:
        Rectangle box;
        std::string text;
        size_t max_length = 100;
        int text_size = 30;
        int FPS = 60;
        
        int frame = 0;
        int frame_need = FPS / 1.5;
        bool is_active = false;

        Client* client = nullptr;
        std::string name;

        void Draw()
        {
            DrawRectangleLinesEx(box, 3, GRAY);
            DrawTextEx(GetFontDefault(), text.c_str(), Vector2{box.x + 10, box.y + 5}, text_size, 2, BLACK);
        }
        
        void DrawFlashCursor()
        {
            frame++;
            if (!is_active){
                if (frame >= frame_need/2) {
                    is_active = true;
                    frame = 0;
                }
            }else {
                if (frame >= frame_need) {
                    is_active = false;
                    frame = 0;
                }
            }
            if (is_active) {
                Vector2 textsize = MeasureTextEx(GetFontDefault(), text.c_str(), text_size, 2); 
                DrawLineEx(Vector2{textsize.x + text_size-4, box.y + 5}, Vector2{textsize.x - 4 + text_size, static_cast<float>(box.y+text_size)}, 2, BLACK);
            }
        }

        void Keyboard()
        {
            char key = GetCharPressed();
            if (IsKeyPressed(KEY_BACKSPACE)) {
                if (text.size()){
                    text.pop_back();
                    frame = 0;
                    is_active = true;
                }
            }else if (IsKeyPressed(KEY_ENTER)) {
                client->send(name+ " " + text);
                text.clear();
            }else{
                if (key){
                    text += key;
                    frame = 0;
                    is_active = true;
                }
            }
        }

};

