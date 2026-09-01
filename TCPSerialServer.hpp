#pragma once

#ifdef _WIN32

#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

#include "SerialServer.hpp"
#include "SerialPort.hpp"
#include "IP.hpp"
#include "TCPSerialPort.hpp"

#if defined(pankey_Log) && (defined(TCPSerialServer_Log) || defined(pankey_Global_Log) || defined(pankey_TCPSerialServerSystem_Log))
	#include "Logger_status.hpp"
	#define TCPSerialServerLog(status,method,mns) pankey_Log(status,"TCPSerialServer",method,mns)
#else
	#define TCPSerialServerLog(status,method,mns)
#endif

namespace pankey{

	namespace Library{

		namespace Network{

            class TCPSerialServer : public SerialServer {
                public:
                    TCPSerialServer(const pankey::Utility::Test::CharPointer& a_name, int a_port) {
                        m_name = a_name;
                        m_port = a_port;
                    }

                    virtual ~TCPSerialServer() {
                        stop();
                    }
                
                    virtual pankey::Utility::Test::CharPointer getName(){return m_name;}

                    virtual SerialPort* connect(const pankey::Type::Array::IP& a_ip, int a_port){
                        TCPSerialPort* i_port = new TCPSerialPort();

                        if(!i_port->connect(a_ip, a_port)){
                            delete i_port;
                            return nullptr;
                        }

                        i_port->setIP(a_ip);
                        i_port->setPort(a_port);

                        return i_port;
                    }

                    virtual void begin() override {
                        if (m_begin) {
                            return;
                        }

                        m_begin = true;

                        WSADATA wsa;

                        if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
                            m_begin = false;
                            return;
                        }

                        m_server = socket(
                            AF_INET,
                            SOCK_STREAM,
                            IPPROTO_TCP
                        );

                        if (m_server == INVALID_SOCKET) {
                            WSACleanup();
                            m_begin = false;
                            return;
                        }

                        sockaddr_in addr{};
                        addr.sin_family = AF_INET;
                        addr.sin_port = htons(m_port);
                        addr.sin_addr.s_addr = INADDR_ANY;

                        if (bind(
                            m_server,
                            (sockaddr*)&addr,
                            sizeof(addr)
                        ) == SOCKET_ERROR) {

                            closesocket(m_server);
                            m_server = INVALID_SOCKET;

                            WSACleanup();
                            m_begin = false;
                            return;
                        }

                        if (listen(
                            m_server,
                            SOMAXCONN
                        ) == SOCKET_ERROR) {

                            closesocket(m_server);
                            m_server = INVALID_SOCKET;

                            WSACleanup();
                            m_begin = false;
                            return;
                        }

                        // NON-BLOCKING MODE
                        u_long mode = 1;

                        if (ioctlsocket(
                            m_server,
                            FIONBIO,
                            &mode
                        ) == SOCKET_ERROR) {

                            closesocket(m_server);
                            m_server = INVALID_SOCKET;

                            WSACleanup();
                            m_begin = false;
                            return;
                        }
                    }

                    virtual void stop() override {
                        if (!m_begin) {
                            return;
                        }

                        m_begin = false;

                        if (m_server != INVALID_SOCKET) {
                            closesocket(m_server);
                            m_server = INVALID_SOCKET;
                        }

                        WSACleanup();
                    }

                    virtual SerialPort* accept() override {

                        if (m_server == INVALID_SOCKET) {
                            return nullptr;
                        }
                        
                        sockaddr_in clientAddr{};
                        int size = sizeof(clientAddr);

                        SOCKET client = ::accept(
                            m_server,
                            (sockaddr*)&clientAddr,
                            &size
                        );

                        // SOCKET client = ::accept(
                        //     m_server,
                        //     nullptr,
                        //     nullptr
                        // );

                        if (client == INVALID_SOCKET) {

                            int error = WSAGetLastError();

                            if (error == WSAEWOULDBLOCK) {
                                // No client waiting.
                                return nullptr;
                            }

                            return nullptr;
                        }

                        u_long mode = 1;
                        ioctlsocket(client, FIONBIO, &mode);
                        
                        char ip[INET_ADDRSTRLEN];

                        inet_ntop(
                            AF_INET,
                            &(clientAddr.sin_addr),
                            ip,
                            INET_ADDRSTRLEN
                        );

                        SerialPort* i_port = new TCPSerialPort(client);
                        pankey::Type::Array::IP i_ip;
                        for(int x = 0; x < INET_ADDRSTRLEN; x++){
                            if(ip[x] == '\0'){
                                break;
                            }
                            if(ip[x] == '.'){
                                continue;
                            }
                            i_ip.add((pankey::Type::Array::Byte)ip[x]);


                        }
                        i_port->setIP(i_ip);
                        i_port->setPort(ntohs(clientAddr.sin_port));

                        return i_port;
                    }

                protected:
                    SOCKET m_server = INVALID_SOCKET;
                    pankey::Utility::Test::CharPointer m_name = "";
                    int m_port = 80;
                    bool m_begin = false;

            };

		}

	}

}

#endif