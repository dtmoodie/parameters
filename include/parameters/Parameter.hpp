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

#include "Parameter_def.hpp"
#include <string>
#include <parameters/LokiTypeInfo.h>
#include <mutex>
#include <signals/signal.h>
namespace cv
{
	namespace cuda
	{
		class Stream;
	}
}
namespace Parameters
{
	class PARAMETER_EXPORTS Parameter
	{
		
	public:
		enum ParameterType
		{
			None = 0,
			Input = 1,
			Output = 2,
			State = 4,
			Control = 8
		};
		Parameter(const ::std::string& name, const ParameterType& type = ParameterType::Control, const ::std::string& tooltip = "");
		virtual ~Parameter();
		typedef ::std::shared_ptr<Parameter> Ptr;
		virtual Loki::TypeInfo GetTypeInfo() = 0;

		virtual Parameter* SetName(const std::string& name_);
		virtual Parameter* SetTooltip(const std::string& tooltip_);
		virtual Parameter* SetTreeName(const std::string& treeName_);
		virtual Parameter* SetTreeRoot(const std::string& treeRoot_);
		virtual const ::std::string& GetName() const;
		virtual const ::std::string& GetTooltip() const;
		virtual const ::std::string GetTreeName() const;
		virtual const ::std::string& GetTreeRoot() const;
		virtual long long GetTimeIndex() const;
		virtual void SetTimeIndex(long long index = -1);
		// Update with the values from another parameter
		virtual bool Update(Parameter* other);
        virtual Ptr DeepCopy() const = 0;

		virtual std::shared_ptr<Signals::connection> RegisterNotifier(std::function<void(cv::cuda::Stream*)> f);
		virtual std::shared_ptr<Signals::connection> RegisterDeleteNotifier(std::function<void(Parameter*)> f);

		virtual std::recursive_mutex& mtx();
		
		ParameterType type;
		bool changed;
		unsigned short subscribers;
		// Sets changed to true and emits update signal
		void OnUpdate(cv::cuda::Stream* stream = nullptr);
		Signals::typed_signal_base<void(cv::cuda::Stream*)> update_signal;
		Signals::typed_signal_base<void(Parameter*)> delete_signal;
	protected:
		std::string name;
		std::string tooltip;
		std::string treeRoot;
		long long _current_time_index;
		std::recursive_mutex _mtx;
	};
}