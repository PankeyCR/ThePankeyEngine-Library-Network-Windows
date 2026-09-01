#pragma once

#ifdef _WIN32

#include "SerialPort.hpp"
#include <windows.h>
#include <conio.h>

#if defined(pankey_Log) && (defined(WindowsConsoleSerialPort_Log) || defined(pankey_Global_Log) || defined(pankey_WindowsConsoleSerialPortSystem_Log))
	#include "Logger_status.hpp"
	#define WindowsConsoleSerialPortLog(status,method,mns) pankey_Log(status,"WindowsConsoleSerialPort",method,mns)
#else
	#define WindowsConsoleSerialPortLog(status,method,mns)
#endif

namespace pankey{

	namespace Library{

		namespace Network{		

            namespace Windows{		

                class WindowsConsoleSerialPort : public SerialPort{
                    public:
                        WindowsConsoleSerialPort(){}
                        virtual ~WindowsConsoleSerialPort(){}

                        virtual int available() override{
						    // WindowsConsoleSerialPortLog(pankey_Log_StartMethod, "available", "");
                            auto i_available = _kbhit() ? 1 : 0;
						    // WindowsConsoleSerialPortLog(pankey_Log_EndMethod, "available", "");
                            return i_available;
                        }

                        virtual pankey::Type::Array::Byte read() override{
						    WindowsConsoleSerialPortLog(pankey_Log_StartMethod, "read", "");
                            if(!_kbhit()){
                                return -1;
                            }

                            WindowsConsoleSerialPortLog(pankey_Log_Statement, "read", "char:");
                            char i_read = _getch();
                            WindowsConsoleSerialPortLog(pankey_Log_Statement, "read", i_read);
                            WindowsConsoleSerialPortLog(pankey_Log_EndMethod, "read", "");
                            return static_cast<pankey::Type::Array::Byte>(i_read);
                        }

                        virtual int peek() override{
                            if(!_kbhit()){
                                return -1;
                            }

                            int c = _getch();
                            ungetc(c, stdin);
                            return c;
                        }

                        virtual pankey::Memory::Allocator::memory_size write(
                            pankey::Type::Array::Byte a_byte
                        ) override{
                            std::cout << static_cast<char>(a_byte);
                            return 1;
                        }

                        virtual pankey::Memory::Allocator::memory_size print(const pankey::Type::Array::ByteArray& a_message){
                            for(int x = 0; x < a_message.length(); x++){
                                std::cout << static_cast<char>(a_message.get(x));
                            }

                            return a_message.length();
                        }

                        virtual pankey::Memory::Allocator::memory_size println(const pankey::Type::Array::ByteArray& a_message){
                            auto size = print(a_message);
                            std::cout << std::endl;
                            return size + 1;
                        }

                        virtual bool connected() override{
                            return true;
                        }

                        virtual bool connect(
                            const pankey::Type::Array::ByteArray& a_ip,
                            int port
                        ) override{
                            return true;
                        }

                        virtual void flush() override{
                            std::cout << std::flush;
                        }

                        virtual operator bool() override{
                            return true;
                        }
                };

            }

		}

	}

}

#endif