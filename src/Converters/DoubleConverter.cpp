#include "Converters/DoubleConverter.hpp"


using namespace Parameters;
using namespace Parameters::Converters;
using namespace Parameters::Converters::Double;
Factory& Factory::Instance()
{
	static Factory inst;
	return inst;
}
IConverter* Factory::Create(Parameter* param, cv::cuda::Stream* stream)
{
	auto itr = Factory::Instance().Registry.find(param->GetTypeInfo());
	if (itr != Factory::Instance().Registry.end())
	{
		return itr->second(param, stream);
	}
	return nullptr;
}
void Factory::Register(ConverterCreationFunctor func, Loki::TypeInfo type)
{
	Factory::Instance().Registry[type] = func;
}
//std::map<Loki::TypeInfo, Factory::ConverterCreationFunctor> Factory::Registry;