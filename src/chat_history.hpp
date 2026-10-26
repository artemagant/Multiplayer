#pragma once

#include <string>
#include <vector>
#include <sstream>

#define TEXT_MAX_LINES 15

typedef struct {
    std::string name;
    std::string text;
} User;

class ChatHistory
{
private:
    User get_user(std::string line)
    {
        User user;
        std::istringstream input(line);
        std::string name;
        input >> name;
        //name.pop_back();
        user.name = name;

        std::string message = line.substr(name.size(), line.size()-name.size());
        user.text = message;
    
        return user;
    }
    std::vector<std::string> get_lines(std::string chat_history)
    {
        std::vector<std::string> lines;

        std::istringstream stream(chat_history);
        std::string line;

        while (std::getline(stream, line)) {
            lines.push_back(line);
        }

        while (lines.size() > 15) {
            lines.erase(lines.begin());
        }

        return lines;
    }
public:
    std::string history;
    std::vector<User> get_users()
    {
        std::vector<User> users;
        std::vector<std::string> lines = get_lines(history);

        for (size_t i = 0; i < lines.size(); i++) {
            users.push_back(get_user(lines[i]));
        }

        return users;
    }

};
