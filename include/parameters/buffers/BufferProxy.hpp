#pragma once
#include "parameters/Parameter_def.hpp"
#include "parameters/LokiTypeInfo.h"
#include "parameters/buffers/CircularBuffer.hpp"
#include "parameters/buffers/ConstMap.hpp"
#include "parameters/buffers/proxy.hpp"
#include <functional>
#include <map>
#include <memory>

namespace Parameters
{
	class Parameter;
	namespace Buffer
	{
		template<typename T> class Proxy;
		template<typename T> class CircularBuffer;
		template<typename T> class ConstMap;
		template<typename T> class Map;

		class PARAMETER_EXPORTS ParameterProxyBufferFactory
		{
		public:
			enum buffer_type
			{
				cbuffer = 0,
				cmap = 1,
				map = 2
			};
			typedef std::function<Parameter*(Parameter*)> create_buffer_f;

			static ParameterProxyBufferFactory* Instance();
			void RegisterFunction(Loki::TypeInfo type, const create_buffer_f& func, buffer_type buffer_type_);
			std::shared_ptr<Parameter>  CreateProxy(std::shared_ptr<Parameter> param, buffer_type buffer_type_);
			Parameter*  CreateProxy(Parameter* param, buffer_type buffer_type_);

		private:
			std::map<Loki::TypeInfo, std::map<buffer_type, create_buffer_f>> _registered_buffer_factories;
		};
				
		template<typename T> struct BufferFactory
		{
			BufferFactory()
			{
				ParameterProxyBufferFactory::Instance()->RegisterFunction(Loki::TypeInfo(typeid(T)), std::bind(&BufferFactory<T>::create_cbuffer, std::placeholders::_1), ParameterProxyBufferFactory::cbuffer);
				ParameterProxyBufferFactory::Instance()->RegisterFunction(Loki::TypeInfo(typeid(T)), std::bind(&BufferFactory<T>::create_cmap, std::placeholders::_1), ParameterProxyBufferFactory::cmap);
				ParameterProxyBufferFactory::Instance()->RegisterFunction(Loki::TypeInfo(typeid(T)), std::bind(&BufferFactory<T>::create_map, std::placeholders::_1), ParameterProxyBufferFactory::map);
			}
			static Parameter* create_cbuffer(Parameter* input)
			{
				if (auto typed_param = dynamic_cast<ITypedParameter<T>*>(input))
				{
					return new Proxy<T>(typed_param, new CircularBuffer<T>("buffer for " + input->GetTreeName()));
				}
				return nullptr;
			}
			static Parameter* create_cmap(Parameter* input)
			{
				if (auto typed_param = dynamic_cast<ITypedParameter<T>*>(input))
				{
					return new Proxy<T>(typed_param, new ConstMap<T>("cmap for " + input->GetTreeName()));
				}
				return nullptr;
			}
			static Parameter* create_map(Parameter* input)
			{
				if (auto typed_param = dynamic_cast<ITypedParameter<T>*>(input))
				{
					return new Proxy<T>(typed_param, new Map<T>("map for " + input->GetTreeName()));
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