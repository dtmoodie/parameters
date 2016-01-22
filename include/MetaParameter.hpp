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
#pragma once
#include "Persistence/Persistence.hpp"
#include "ITypedParameter.hpp"
#include "UI/UI.hpp"
#include <Parameter.hpp>
#include "Converters/ConverterPolicy.hpp"

namespace Parameters
{
	template<typename T, template<typename> class Policy1 = Persistence::PersistencePolicy, template<typename> class Policy2 = UI::UiPolicy, template<typename> class Policy3 = Converters::ConverterPolicy>
	class MetaTypedParameter :
		public ITypedParameter<T>, public Policy1<T>, public Policy2 < T >, public Policy3<T>
	{
	public:
		MetaTypedParameter(const std::string& name, const Parameter::ParameterType& type = Parameter::Control, const std::string& tooltip = "") :
			ITypedParameter<T>(name, type, tooltip){}
	};
}
