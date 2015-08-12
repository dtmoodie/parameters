#pragma once

#include "Parameter_def.hpp"
#include <string>
#include <LokiTypeInfo.h>
#include <boost/function.hpp>
#include <boost/signals2/connection.hpp>
#include <boost/signals2/signal.hpp>
#include <boost/thread/recursive_mutex.hpp>





namespace Parameters
{
	class Parameter_EXPORTS Parameter
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
		typedef ::std::shared_ptr<Parameter> Ptr;
		virtual Loki::TypeInfo GetTypeInfo() = 0;

		virtual void SetName(const std::string& name_);
		virtual void SetTooltip(const std::string& tooltip_);
		virtual void SetTreeName(const std::string& treeName_);
		virtual void SetTreeRoot(const std::string& treeRoot_)
		{
			SetTreeName(treeRoot_ + ":" + GetName());
		}
		virtual ::std::string& GetName();
		virtual const ::std::string& GetTooltip();
		virtual const ::std::string& GetTreeName();

		virtual boost::signals2::connection RegisterNotifier(const boost::function<void(void)>& f);

		boost::recursive_mutex mtx;
		ParameterType type;
		bool changed;
		unsigned short subscribers;
		boost::signals2::signal<void(void)> UpdateSignal;

	private:
		std::string name;
		std::string tooltip;
		std::string treeName;

	};
}