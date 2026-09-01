#pragma once

#ifdef _WIN32

#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

#include "SerialPort.hpp"
#include "CharPointer.hpp"
#include "to_CharPointer_ByteArray.hpp"

#if defined(pankey_Log) && (defined(TCPSerialPort_Log) || defined(pankey_Global_Log) || defined(pankey_TCPSerialPortSystem_Log))
	#include "Logger_status.hpp"
	#define TCPSerialPortLog(status,method,mns) pankey_Log(status,"TCPSerialPort",method,mns)
#else
	#define TCPSerialPortLog(status,method,mns)
#endif

namespace pankey{

	namespace Library{

		namespace Network{	

            class TCPSerialPort : public SerialPort{
                public:

                    TCPSerialPort(){

                    }
                    
                    TCPSerialPort(SOCKET a_socket){
                        m_socket = a_socket;
                    }

                    virtual ~TCPSerialPort(){
                        stop();
                    }

                    virtual int available() override{
                        u_long bytes = 0;

                        if(ioctlsocket(
                            m_socket,
                            FIONREAD,
                            &bytes
                        ) == SOCKET_ERROR){
                            return 0;
                        }

                        return (int)bytes;
                    }

                    virtual pankey::Type::Array::Byte read() override{
                        TCPSerialPortLog(pankey_Log_StartMethod, "read", "");
                        char c;

                        int result = recv(m_socket, &c, 1, 0);

                        if(result <= 0){
                            TCPSerialPortLog(pankey_Log_EndMethod, "read", "");
                            return -1;
                        }

                        TCPSerialPortLog(pankey_Log_EndMethod, "read", "");
                        return (pankey::Type::Array::Byte)c;
                    }

                    virtual pankey::Memory::Allocator::memory_size write(pankey::Type::Array::Byte a_byte) override{
                        TCPSerialPortLog(pankey_Log_StartMethod, "write", "");

                        char c = (char)a_byte;

                        TCPSerialPortLog(pankey_Log_EndMethod, "update", "");
                        return send(m_socket, &c, 1, 0);
                    }
                    
                    virtual pankey::Memory::Allocator::memory_size print(const pankey::Type::Array::ByteArray& a_message) override{

                        if(m_socket == INVALID_SOCKET){
                            return 0;
                        }

                        if(a_message.length() == 0){
                            return 0;
                        }

                        CharPointer i_chars = toCharPointer(a_message);
                        return send(
                            m_socket,
                            i_chars.get(),
                            a_message.length(),
                            0
                        );
                    }
                    
                    virtual pankey::Memory::Allocator::memory_size println(
                        const pankey::Type::Array::ByteArray& a_message
                    ) override{

                        auto sent = print(a_message);

                        const char* newline = "\r\n";

                        auto sent_newline = send(
                            m_socket,
                            newline,
                            2,
                            0
                        );

                        if(sent_newline < 0){
                            return sent;
                        }

                        return sent + sent_newline;
                    }

                    virtual bool connected() override{
                        TCPSerialPortLog(pankey_Log_StartMethod, "update", "");
                        bool result = m_socket != INVALID_SOCKET;
                        TCPSerialPortLog(pankey_Log_EndMethod, "update", "");
                        return result;
                    }
                
                    virtual bool connect(const pankey::Type::Array::IP& a_ip, int a_port){
                        TCPSerialPortLog(pankey_Log_StartMethod, "connect", "");
                        stop();

                        m_socket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

                        if(m_socket == INVALID_SOCKET){
                            TCPSerialPortLog(pankey_Log_EndMethod,"connect","socket creation failed");
                            return false;
                        }

                        sockaddr_in addr;
                        memset(&addr, 0, sizeof(addr));

                        addr.sin_family = AF_INET;
                        addr.sin_port = htons(a_port);

                        // Example if IP exposes 4 bytes
                        addr.sin_addr.S_un.S_un_b.s_b1 = a_ip.get(0);
                        addr.sin_addr.S_un.S_un_b.s_b2 = a_ip.get(1);
                        addr.sin_addr.S_un.S_un_b.s_b3 = a_ip.get(2);
                        addr.sin_addr.S_un.S_un_b.s_b4 = a_ip.get(3);

                        int result = ::connect( m_socket, reinterpret_cast<sockaddr*>(&addr), sizeof(addr) );

                        if(result == SOCKET_ERROR){
                            closesocket(m_socket);
                            m_socket = INVALID_SOCKET;

                            TCPSerialPortLog( pankey_Log_EndMethod, "connect", "connection failed" );

                            return false;
                        }

                        TCPSerialPortLog(pankey_Log_EndMethod, "connect", "");

                        return true;
                    }

                    virtual void stop() override{
                        TCPSerialPortLog(pankey_Log_StartMethod, "update", "");

                        if(m_socket != INVALID_SOCKET){
                            closesocket(m_socket);
                            m_socket = INVALID_SOCKET;
                        }
                    }

                protected:
                    SOCKET m_socket = INVALID_SOCKET;
            };

		}

	}

}

#endif