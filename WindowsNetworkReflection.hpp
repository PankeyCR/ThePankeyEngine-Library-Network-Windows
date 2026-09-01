#pragma once

#include "WindowsNetwork.hpp"
#include "WindowsNetworkReflectionModel.hpp"

#include "Network.hpp"
#include "ByteArray.hpp"
#include "IP.hpp"
#include "CharPointer.hpp"

#include "Class.hpp"
#include "Method.hpp"
#include "TClass.hpp"


using namespace pankey::Utility::Base;
using namespace pankey::Meta::Reflection;
using namespace pankey::Support::Reflection;
using namespace pankey::Type::Array;
using namespace pankey::Library::Network;
using namespace pankey::Library::Network::Windows;

template<> Class* ReflectionClass<Network>::instance = new TClass<Network>();
template<> CharPointer* ReflectionClass<Network>::name = new CharPointer("Network");

InvokeClassMethod<Network,const ByteArray&> r_instantSend_2 = &Network::instantSend;
template<> Method* ReflectionMethod<Network,WindowsTCPInstantSend>::instance = new Method(createClassMethod<Network,const ByteArray&>(&r_instantSend_2));

template<> CharPointer* ReflectionMethod<Network,WindowsTCPInstantSend>::name = new CharPointer("send");

InvokeMethod<Network&,const IP&,int> r_connectTCP = WindowsConnectTCP;
template<> Method* ReflectionMethod<Network,WindowsTCPConnect>::instance = new Method(createExtendedClassMethod<Network,const IP&,int>(&r_connectTCP));

template<> CharPointer* ReflectionMethod<Network,WindowsTCPConnect>::name = new CharPointer("connect");
