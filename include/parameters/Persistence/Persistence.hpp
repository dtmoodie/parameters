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
#ifdef PARAMTERS_GENERATE_PERSISTENCE
#include "OpenCV.hpp"
#include "TextSerializer.hpp"
#include "ProtobufSerializer.hpp"
#include "cereal.hpp"

namespace Parameters
{
	namespace Persistence
	{
		template<typename T> class PersistencePolicy :
			public Parameters::Persistence::Text::PersistencePolicy<T>
			, public Parameters::Persistence::cv::PersistencePolicy<T>
			, public Parameters::Persistence::Protobuf::ProtoPolicy<T>
		{	
		};
	}
}
#else
namespace Parameters
{
	namespace Persistence
	{
		template<typename T> class PersistencePolicy
		{
		};
	}
}
#endif