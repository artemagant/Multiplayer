#include "server.hpp"

#include <stdio.h>
#include <raylib.h>

#include <iostream>
#include <string>
#include <cstring>
#include <ctime>
#include <cstdlib>
#include <sstream>


#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 500
#define FPS 10

typedef struct{
    std::string name;
    std::string text;
} User;

User get_user(std::string line)
{
    User user;
    std::istringstream input(line);
    std::string name;
    input >> name;
    name.pop_back();
    user.name = name;

    std::string message = line.substr(name.size(), line.size()-name.size());
    user.text = message;
    
    return user;
}


int main(int argc, char* argv[])
{
    srand(time(0));

    Server server;
    server.PORT = 5000; 
    server.ip = "127.0.0.1";
    std::string name = std::to_string(rand() % 99999999);
    if (argc == 2 && std::strcmp(argv[1], "server") == 0) {
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
        chat_history = server.listen();
        printf("%s\n", chat_history.c_str());

        BeginDrawing();
        
        ClearBackground(WHITE);
        DrawText("fuck u", 100, 100, 10, BLACK);
        
        EndDrawing();
    }
}
