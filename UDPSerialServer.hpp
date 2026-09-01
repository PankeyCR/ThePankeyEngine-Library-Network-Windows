#pragma once

#ifdef _WIN32

#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

#include "SerialServer.hpp"
#include "MessageSerialPort.hpp"
#include "CharPointer.hpp"
#include "IP.hpp"

#if defined(pankey_Log) && (defined(UDPSerialServer_Log) || defined(pankey_Global_Log))
    #include "Logger_status.hpp"
    #define UDPSerialServerLog(status,method,mns) pankey_Log(status,"UDPSerialServer",method,mns)
#else
    #define UDPSerialServerLog(status,method,mns)
#endif

namespace pankey{

    namespace Library{

        namespace Network{

            class UDPSerialServer : public SerialServer{
                public:
                    UDPSerialServer(const pankey::Utility::Test::CharPointer& a_name, int a_port){
                        m_name = a_name;
                        m_port = a_port;
                    }

                    virtual ~UDPSerialServer(){
                        stop();
                    }
                
                    virtual pankey::Utility::Test::CharPointer getName(){return m_name;}

                    virtual void begin() override{

                        if(m_begin){
                            return;
                        }

                        m_begin = true;

                        WSADATA wsa;

                        if(WSAStartup(MAKEWORD(2,2), &wsa) != 0){
                            m_begin = false;
                            return;
                        }

                        m_socket = socket(
                            AF_INET,
                            SOCK_DGRAM,
                            IPPROTO_UDP
                        );

                        if(m_socket == INVALID_SOCKET){
                            WSACleanup();
                            m_begin = false;
                            return;
                        }

                        sockaddr_in addr{};
                        addr.sin_family = AF_INET;
                        addr.sin_port = htons(m_port);
                        addr.sin_addr.s_addr = INADDR_ANY;

                        if(bind(
                            m_socket,
                            (sockaddr*)&addr,
                            sizeof(addr)
                        ) == SOCKET_ERROR){

                            closesocket(m_socket);
                            m_socket = INVALID_SOCKET;

                            WSACleanup();

                            m_begin = false;

                            return;
                        }

                        u_long mode = 1;

                        ioctlsocket(
                            m_socket,
                            FIONBIO,
                            &mode
                        );
                    }

                    virtual void stop() override{

                        if(!m_begin){
                            return;
                        }

                        m_begin = false;

                        if(m_socket != INVALID_SOCKET){
                            closesocket(m_socket);
                            m_socket = INVALID_SOCKET;
                        }

                        WSACleanup();
                    }

                    virtual void send(
                        const pankey::Type::Array::IP& a_ip,
                        int a_port,
                        const pankey::Type::Array::ByteArray& a_message
                    ){

                        if(m_socket == INVALID_SOCKET){
                            return;
                        }

                        sockaddr_in remote{};
                        remote.sin_family = AF_INET;
                        remote.sin_port = htons(a_port);

                        char ipString[32];

                        sprintf(
                            ipString,
                            "%d.%d.%d.%d",
                            a_ip.get(0),
                            a_ip.get(1),
                            a_ip.get(2),
                            a_ip.get(3)
                        );

                        inet_pton(
                            AF_INET,
                            ipString,
                            &remote.sin_addr
                        );

                        sendto(
                            m_socket,
                            (const char*)a_message.getArrayPointer(),
                            a_message.length(),
                            0,
                            (sockaddr*)&remote,
                            sizeof(remote)
                        );
                    }

                    virtual void update() override{

                        if(m_socket == INVALID_SOCKET){
                            return;
                        }

                        while(true){

                            sockaddr_in remote{};
                            int remoteSize = sizeof(remote);

                            char buffer[65535];

                            int received = recvfrom(
                                m_socket,
                                buffer,
                                sizeof(buffer),
                                0,
                                (sockaddr*)&remote,
                                &remoteSize
                            );

                            if(received <= 0){
                                break;
                            }

                            char ip[INET_ADDRSTRLEN];

                            inet_ntop(
                                AF_INET,
                                &remote.sin_addr,
                                ip,
                                sizeof(ip)
                            );

                            pankey::Type::Array::IP i_ip;

                            int value = 0;

                            for(int x = 0;;x++){

                                char c = ip[x];

                                if(c == '.' || c == '\0'){

                                    i_ip.add((pankey::Type::Array::Byte)value);

                                    value = 0;

                                    if(c == '\0'){
                                        break;
                                    }

                                    continue;
                                }

                                value *= 10;
                                value += c - '0';
                            }

                            pankey::Type::Array::ByteArray message;

                            for(int x=0;x<received;x++){
                                message.add(
                                    (pankey::Type::Array::Byte)buffer[x]
                                );
                            }

                            SerialPort* port =
                                new MessageSerialPort(message);

                            port->setIP(i_ip);
                            port->setPort(ntohs(remote.sin_port));

                            m_ports.addPointer(port);
                        }
                    }

                    virtual SerialPort* available(){

                        return m_ports.removeFirst();
                    }

                protected:

                    SOCKET m_socket = INVALID_SOCKET;

                    pankey::DataStructure::List::ArrayRawPointerList<SerialPort> m_ports;

                    pankey::Utility::Test::CharPointer m_name = "";
                    int m_port = 102;
                    bool m_begin = false;
            };

        }

    }

}

#endif