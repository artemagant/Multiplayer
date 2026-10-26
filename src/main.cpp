#include "server.hpp"
#include "chat_history.hpp"

#include <stdio.h>
#include <raylib.h>

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
    srand(time(0));

    ChatHistory ch;

    Server server;
    server.PORT = 5000; 
    server.ip = "127.0.0.1";
    std::string name = std::to_string(rand() % 99999999);
    if (argc >= 3){
        server.PORT = std::stoi(argv[2]);
    }
    if (argc >= 4){
        server.ip = argv[3];
    }
    if (argc >= 2 && std::strcmp(argv[1], "server") == 0) {
        server.create_server();
    }
    if (argc == 2 && std::strcmp(argv[1], "ns") == 0){
        while (true){
            std::string message;
            std::getline(std::cin, message);
            printf("%s\n", server.send_message(message, name).c_str());
        }
    }
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "fuck u");
    SetTargetFPS(FPS);

    std::string chat_history;

    while(!WindowShouldClose()) {
        system("clear");
        ch.history = server.listen();
        std::vector<User> users = ch.get_users();
        
        BeginDrawing();
        
        ClearBackground(WHITE);

        DrawText(name.c_str(), 2, 2, 15, BLACK);

        for (int i = 0; i < (int)users.size(); i++){
            printf("%s: %s\n", users[i].name.c_str(), users[i].text.c_str());
            printf("%d", TEXT_BUBBLE_SIZE);
            draw_text_bubble((Vector2){10, static_cast<float>(30+TEXT_BUBBLE_SIZE*i)}, users[i]);
        }
        

        
        EndDrawing();
    }
}
