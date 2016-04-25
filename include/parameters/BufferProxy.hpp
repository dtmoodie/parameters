#pragma once
#include "Parameter_def.hpp"
#include "LokiTypeInfo.h"
#include "ParameterBuffer.hpp"
#include <functional>
#include <map>
#include <memory>

namespace Parameters
{
	class Parameter;
	template<typename T> class ParameterBufferProxy;
	namespace Buffer
	{
		class PARAMETER_EXPORTS ParameterProxyBufferFactory
		{
		public:
			typedef std::function<Parameter*(Parameter*)> create_buffer;
			static ParameterProxyBufferFactory* Instance();
			void RegisterFunction(Loki::TypeInfo, const create_buffer& func);
			std::shared_ptr<Parameter>  CreateProxy(std::shared_ptr<Parameter> param);
			Parameter*  CreateProxy(Parameter* param);
		private:
			std::map<Loki::TypeInfo, create_buffer> _registered_buffer_factories;
		};
				
		template<typename T> struct BufferFactory
		{
			BufferFactory()
			{
				ParameterProxyBufferFactory::Instance()->RegisterFunction(Loki::TypeInfo(typeid(T)), std::bind(&BufferFactory<T>::create, std::placeholders::_1));
			}
			static Parameter* create(Parameter* input)
			{
				if (auto typed_param = dynamic_cast<ITypedParameter<T>*>(input))
				{
					return new ParameterBufferProxy<T>(typed_param);
				}
				return nullptr;
			}
		};

		template<typename T> class ParameterBufferPolicy
		{
			static BufferFactory<T> factory;
		public:
			ParameterBufferPolicy()
			{
				(void)&factory;
			}
		};
		template<typename T> BufferFactory<T> ParameterBufferPolicy<T>::factory;
	}
}