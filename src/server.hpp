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

enet_uint16 PORT = 5000;
std::string server_ip = "127.0.0.1";

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
                //printf("Client connected\n");
            }else if(event.type == ENET_EVENT_TYPE_RECEIVE)
            {
                std::string message(
                    reinterpret_cast<char*>(event.packet->data), 
                    event.packet->dataLength
                );
                if (std::strcmp(message.c_str(), "/ping") == 0){
                    //system("clear");
                    //printf("%s", get_chat_history_string().c_str());
                    //printf("Sending ping to client\n");
                }
                else{
                    printf("Got message from client\n");
                    //system("clear");
                    chat_history.push_back(message);
                    //printf("%s", get_chat_history_string().c_str());

                    ENetPacket* packet = enet_packet_create(
                        get_chat_history_string().c_str(),
                        get_chat_history_string().size(),
                        ENET_PACKET_FLAG_RELIABLE
                    );

                    if (packet) {
                        enet_host_broadcast(host, 0, packet); // channel 0
                        enet_host_flush(host);                // send queued packets now
                    }
                }
                

                ENetPacket* reply = enet_packet_create(
                    get_chat_history_string().c_str(), get_chat_history_string().size(), ENET_PACKET_FLAG_RELIABLE
                );

                if (enet_peer_send(event.peer, 0, reply) < 0)
                {
                    enet_packet_destroy(reply);
                    printf("Couldnt send reply\n");
                }

                enet_packet_destroy(event.packet);
            }else if (event.type == ENET_EVENT_TYPE_DISCONNECT)
            {
                //printf("Client disconnected\n");
            }
        }

        enet_host_destroy(host);
        return 0;
    }











    std::string send_message(std::string mes, std::string name)
    {
        ENetHost* client = enet_host_create(nullptr, 1, 2, 0, 0);
        if (!client)
        {
            printf("Couldnt create client host\n");
            return (std::string)"Error: Couldnt create client host";
        }

        ENetAddress address{};
        address.port = PORT;

        if (enet_address_set_host(&address, ip) < 0)
        {
            printf("Invalid server address\n");
            enet_host_destroy(client);
            return (std::string)"Error: Invalid server address";
        }

        ENetPeer* peer = enet_host_connect(client, &address, 2, 0);
        if (!peer)
        {
            printf("Couldnt start connection\n");
            enet_host_destroy(client);
            return (std::string)"Error: Couldnt start connection";
        }

        ENetEvent event{};
        bool connected = false;

        // Wait 5 seconds for the connection
        if (enet_host_service(client, &event, 5000) > 0 &&
            event.type == ENET_EVENT_TYPE_CONNECT)
        {
            connected = true;
            //printf("Connected to server\n");
        }

        if (!connected)
        {
            enet_peer_reset(peer);
            enet_host_destroy(client);
            return (std::string)"Error: Connection failed or timed out";
        }

        /*
        Sending message
         */
        std::string mess = mes;
        if (std::strcmp(mess.c_str(), "/ping") == 0){}
        else{
            mess = name + " " + mes;
        }
        const std::string message = mess;
        ENetPacket* packet = enet_packet_create(
            message.data(), message.size(), ENET_PACKET_FLAG_RELIABLE
        );

        if (enet_peer_send(peer, 0, packet) < 0)
        {
            enet_packet_destroy(packet);
            printf("Couldnt send message\n");
            enet_peer_reset(peer);
            enet_host_destroy(client);
            return (std::string)"Error: Couldnt send message";
        }

        enet_host_flush(client);
        std::string ret;
        // Wait 5 sec for response
        if (enet_host_service(client, &event, 5000) > 0 &&
            event.type == ENET_EVENT_TYPE_RECEIVE)
        {
            std::string reply(
                reinterpret_cast<char*>(event.packet->data),
                event.packet->dataLength
            );
            ret = reply;
            enet_packet_destroy(event.packet);
        }else 
        {
            printf("No reply recieved\n");
        }
        enet_peer_disconnect(peer, 0);
        enet_host_flush(client);
        enet_host_destroy(client);
        return ret;
    }

    std::string listen()
    {
        return send_message("/ping", "fuck u");
    }
};
