#pragma once
#include "Persistence/Persistence.hpp"
#include "ITypedParameter.hpp"
#include "UI/UI.hpp"
#include <Parameter.hpp>
namespace Parameters
{
	template<typename T, template<typename> class Policy1 = Persistence::PersistencePolicy, template<typename> class Policy2 = UI::UiPolicy>
	class MetaTypedParameter :
		public ITypedParameter<T>, public Policy1<T>, public Policy2 < T >
	{
	public:
		MetaTypedParameter(const std::string& name, const Parameter::ParameterType& type = Parameter::Control, const std::string& tooltip = "") :
			ITypedParameter<T>(name, type, tooltip){}
	};
}
