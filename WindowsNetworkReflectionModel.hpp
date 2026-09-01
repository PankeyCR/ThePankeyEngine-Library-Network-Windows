#pragma once

#include "AnnotationPack.hpp"
#include "MethodPack.hpp"
#include "Network.hpp"

namespace pankey{

	namespace Library{

		namespace Network{

            namespace Windows{

                struct WindowsTCPInstantSend{};
                struct WindowsTCPConnect{};
                
                struct WindowsNetworkReflectionModel{
                    
                    using Instance = Network;
                    
                    using Annotations = pankey::Meta::Reflection::AnnotationPack<>;
                    
                    using Methods = pankey::Meta::Reflection::MethodPack<
                                                                 WindowsTCPInstantSend
                                                                ,WindowsTCPConnect
                                                                >;
                    
                };

            }

		}

	}

}