#pragma once

#include "Network.hpp"
#include "ByteArray.hpp"
#include "CharPointer.hpp"
#include "WindowsTCPSocketSerialPort.hpp"
#include "NewLinePortProtocol.hpp"

using namespace pankey::Utility::Base;
using namespace pankey::Meta::Reflection;
// using namespace pankey::Support::Reflection;
using namespace pankey::Type::Array;
using namespace pankey::Library::Network;
using namespace pankey::Library::Network::Windows;


void WindowsConnectTCP(Network& a_network, const IP& a_ip, int a_port){
    auto i_port = new WindowsTCPSocketSerialPort();
    IP i_ip = createIP(192,168,5,180);
    i_port->setIP(i_ip);
    i_port->connect(i_ip, a_port);
    a_network.addSerialPort(i_port, new NewLinePortProtocol());
}