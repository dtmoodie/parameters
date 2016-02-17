/*
Copyright (c) 2015 Daniel Moodie.
All rights reserved.

Redistribution and use in source and binary forms are permitted
provided that the above copyright notice and this paragraph are
duplicated in all such forms and that any documentation,
advertising materials, and other materials related to such
distribution and use acknowledge that the software was developed
by the Daniel Moodie. The name of
Daniel Moodie may not be used to endorse or promote products derived
from this software without specific prior written permission.
THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.

https://github.com/dtmoodie/parameters
*/
#include "parameters/Converters/DoubleConverter.hpp"


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