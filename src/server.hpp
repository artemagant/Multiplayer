#include <string>

class Server
{
public:
    int PORT;
    const char* server_ip;

    void create_server();
    void send_message(std::string);
    void listen();
private:
    std::string get_chat_history_string();
}

