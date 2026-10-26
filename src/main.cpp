#include "server.hpp"
#include "chat_history.hpp"
#include "text_box.hpp"

#include <stdio.h>
#include <raylib.h>
#include <stdbool.h>

#include <iostream>
#include <string>
#include <cstring>
#include <ctime>
#include <cstdlib>
#include <sstream>
#include <vector>


#define WINDOW_WIDTH 100
#define WINDOW_HEIGHT 100
#define FPS 60

#define TEXT_NAME_SIZE 15
#define TEXT_TEXT_SIZE 20
#define TEXT_TEXT_INDENT 10
#define TEXT_TEXT_NAME_INDENT 5
#define TEXT_BUBBLE_MARGIN 10
const int TEXT_BUBBLE_SIZE = TEXT_NAME_SIZE + TEXT_TEXT_SIZE + TEXT_TEXT_INDENT + TEXT_TEXT_NAME_INDENT + TEXT_BUBBLE_MARGIN;

void draw_text_bubble(Vector2 position, User user)
{
    DrawText(user.name.c_str(), position.x, position.y, TEXT_NAME_SIZE, BLACK);
    DrawText(user.text.c_str(), position.x + TEXT_TEXT_INDENT, position.y + TEXT_NAME_SIZE + TEXT_TEXT_NAME_INDENT,TEXT_TEXT_SIZE, BLACK);
}

int main(int argc, char* argv[])
{
    printf("Usage: ./game [*mode] [*port] [*server ip] [*name]\n\n\t* - optional\n\nmode (optional)\n\tdefault - gui - client with gui\n\tserver - run as server without beeing able to see messages or write to chat\n\tns - no screen - run app without gui\n\n*port (optional)\n\tdefault - 5000\n\tDefines on what port to connect or create server\n\n*server ip\n\tdefault - 127.0.0.1 (localhost)\n\tDefines on through what ip to connect to server from client(you need to have server`s device ip)\n\n*name (optional)\n\tdefault - random number from 0 to 99999999\n\tFrom what name to write messages from client\n");
    srand(time(0));

    ChatHistory ch;
    TextBox text_box;
    text_box.box = {10, 950, 1900, 85};

    Server server;
    server.PORT = 5000; 
    server.ip = "127.0.0.1";
    std::string name = std::to_string(rand() % 99999999);

    if (argc >= 5) {
        name = std::string(argv[4]);
    }
    if (argc >= 3){
        server.PORT = std::stoi(argv[2]);
    }
    if (argc >= 4){
        server.ip = argv[3];
    }
    if (argc >= 2 && std::strcmp(argv[1], "server") == 0) {
        server.create_server();
    }
    if (argc >= 2 && std::strcmp(argv[1], "ns") == 0){
        while (true){
            std::string message;
            std::getline(std::cin, message);
            printf("%s\n", server.send_message(message, name).c_str());
        }
    }
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "fuck u");
    SetTargetFPS(FPS);

    text_box.name = name;
    text_box.server = server;

    std::string chat_history;

    while(!WindowShouldClose()) {
        ch.history = server.listen();
        std::vector<User> users = ch.get_users();

        text_box.Keyboard();
        
        BeginDrawing();
        
        ClearBackground(WHITE);

        DrawText(name.c_str(), 2, 2, 15, BLACK);

        for (int i = 0; i < (int)users.size(); i++){
            draw_text_bubble((Vector2){10, static_cast<float>(30+TEXT_BUBBLE_SIZE*i)}, users[i]);
        }

        text_box.Draw();
        text_box.DrawFlashCursor();
        

        
        EndDrawing();
    }
}
