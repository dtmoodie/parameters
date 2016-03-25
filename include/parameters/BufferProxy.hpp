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
		class Parameter_EXPORTS ParameterProxyBufferFactory
		{
		public:
			typedef std::function<std::shared_ptr<Parameter>(std::shared_ptr<Parameter>)> create_buffer;
			static ParameterProxyBufferFactory* Instance();
			void RegisterFunction(Loki::TypeInfo, const create_buffer& func);
			std::shared_ptr<Parameter>  CreateProxy(std::shared_ptr<Parameter> param);
		private:
			std::map<Loki::TypeInfo, create_buffer> _registered_buffer_factories;
		};
				
		template<typename T> struct BufferFactory
		{
			BufferFactory()
			{
				ParameterProxyBufferFactory::Instance()->RegisterFunction(Loki::TypeInfo(typeid(T)), std::bind(&BufferFactory<T>::create, std::placeholders::_1));
			}
			static std::shared_ptr<Parameter> create(Parameters::Parameter::Ptr input)
			{
				if (auto typed_param = std::dynamic_pointer_cast<ITypedParameter<T>>(input))
				{
					return std::shared_ptr<ParameterBufferProxy<T>>(new ParameterBufferProxy<T>(typed_param));
				}
				return std::shared_ptr<Parameter>();
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