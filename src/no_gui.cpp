// ns.cpp  (build: clang++ ns.cpp -o ns -lenet)
#include "server.hpp"

#include <poll.h>
#include <unistd.h>
#include <deque>

int main(int argc, char* argv[])
{
    if (enet_initialize() != 0) return 1;

    Client client;
    std::string name = argc >= 4 ? argv[3] : "phone";
    if (argc >= 2) client.ip = argv[1];
    if (argc >= 3) client.port = (enet_uint16)std::stoi(argv[2]);
    if (!client.connect()) { puts("Couldnt connect"); return 1; }

    std::deque<std::string> pending;   // typed, not yet sent
    std::string shown;

    while (true) {
        client.update();

        // Is there a typed line waiting? (wait at most 16 ms)
        pollfd fd{STDIN_FILENO, POLLIN, 0};
        if (poll(&fd, 1, 16) > 0 && (fd.revents & POLLIN)) {
            std::string line;
            if (!std::getline(std::cin, line)) break;   // Ctrl+D
            pending.push_back(line);
        }

        // Send queued messages once connected
        while (client.connected && !pending.empty()) {
            client.send(name + " " + pending.front());
            pending.pop_front();
        }

        // Redraw only when history changed
        if (client.history != shown) {
            shown = client.history;
            printf("\033[2J\033[H%s", shown.c_str());
            fflush(stdout);
        }
    }
    enet_deinitialize();
}
