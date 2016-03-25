#include "parameters/BufferProxy.hpp"
using namespace Parameters;
using namespace Parameters::Buffer;

ParameterProxyBufferFactory* ParameterProxyBufferFactory::Instance()
{
	static ParameterProxyBufferFactory inst;
	return &inst;
}
void ParameterProxyBufferFactory::RegisterFunction(Loki::TypeInfo, const create_buffer& func)
{

}
std::shared_ptr<Parameter>  ParameterProxyBufferFactory::CreateProxy(std::shared_ptr<Parameter> param)
{
	auto factory_func = _registered_buffer_factories.find(param->GetTypeInfo());
	if (factory_func != _registered_buffer_factories.end())
	{
		return factory_func->second(param);
	}
	return std::shared_ptr<Parameter>();
}