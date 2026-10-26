#pragma once

#include <stdio.h>

#include <iostream>
#include <cstring>
#include <string>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <thread>

#include <enet/enet.h>

std::vector<std::string> chat_history;


class Server
{
private:
    std::string get_chat_history_string()
    {
        std::string history;
        for (std::string mes : chat_history)
            history += mes + "\n";
        return history;
    }
public:
    int PORT;
    const char* ip;

    int create_server()
    { 
        ENetAddress address{};
        address.host = ENET_HOST_ANY;
        address.port = PORT;

        ENetHost* host = enet_host_create(&address, 8, 2, 0, 0);
        if (!host)
        {
            printf("Couldnt create server host\n");
            return 1;
        }

        printf("Listening...\n");

        ENetEvent event{};
        while (enet_host_service(host, &event, 1000) >= 0)
        {
            if (event.type == ENET_EVENT_TYPE_CONNECT)
            {
                ENetPacket* packet = enet_packet_create(
                    get_chat_history_string().c_str(),
                    get_chat_history_string().size(),
                    ENET_PACKET_FLAG_RELIABLE
                );
                if (enet_peer_send(event.peer, 0, packet) < 0) {
                    enet_packet_destroy(packet);
                    printf("Couldnt send packet to new peer\n");
                }
                printf("Client connected\n");
            }else if(event.type == ENET_EVENT_TYPE_RECEIVE)
            {
                std::string message(
                    reinterpret_cast<char*>(event.packet->data), 
                    event.packet->dataLength
                );
                printf("Got message from client\n");
                chat_history.push_back(message);

                ENetPacket* packet = enet_packet_create(
                    get_chat_history_string().c_str(),
                    get_chat_history_string().size(),
                    ENET_PACKET_FLAG_RELIABLE
                );
                if (packet) {
                    enet_host_broadcast(host, 0, packet); // channel 0
                    enet_host_flush(host);                // send queued packets now
                }

                enet_packet_destroy(event.packet);
            }else if (event.type == ENET_EVENT_TYPE_DISCONNECT)
            {
                printf("Client disconnected\n");
            }
        }

        enet_host_destroy(host);
        return 0;
    }
};


class Client
{
    ENetHost* host = nullptr;
    ENetPeer* peer = nullptr;
public:
    std::string history;
    bool connected = false;

    std::string ip = "127.0.0.1";
    enet_uint16 port = 5000;

    bool connect()
    {
        host = enet_host_create(nullptr, 1, 2, 0, 0);
        ENetAddress addr{};
        enet_address_set_host(&addr, ip.c_str());
        addr.port = port;
        peer = enet_host_connect(host, &addr, 2, 0);
        return host && peer;
    }

    void update()
    {
        ENetEvent event;

        while (enet_host_service(host, &event, 0) > 0) {
            if (event.type == ENET_EVENT_TYPE_CONNECT) connected = true;
            else if (event.type == ENET_EVENT_TYPE_RECEIVE) {
                history.assign((char*)event.packet->data, event.packet->dataLength);
                enet_packet_destroy(event.packet);
            } else if (event.type == ENET_EVENT_TYPE_DISCONNECT) connected = false;
        }
    }

    void send(const std::string& message)
    {
        if (!connected) return;
        enet_peer_send(
                peer, 
                0, 
                enet_packet_create(message.data(), 
                message.size(), 
                ENET_PACKET_FLAG_RELIABLE)
        );
    }
};
