#pragma once
#include <boost/signals2.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <mutex>
#include "LokiTypeInfo.h"
#include "UI/UI.hpp"
#include "Persistence/Persistence.hpp"
#ifdef Parameter_EXPORTS
#undef Parameter_EXPORTS
#endif
#if (defined WIN32 || defined _WIN32 || defined WINCE || defined __CYGWIN__) && defined libParameter_EXPORTS
#  define Parameter_EXPORTS __declspec(dllexport)
#elif defined __GNUC__ && __GNUC__ >= 4
#  define Parameter_EXPORTS __attribute__ ((visibility ("default")))
#else
#  define Parameter_EXPORTS
#endif

#define ENUM(value) value, #value

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
		Parameter(const std::string& name, const ParameterType& type = ParameterType::Control, const std::string& tooltip = "");
		typedef std::shared_ptr<Parameter> Ptr;
		virtual Loki::TypeInfo GetTypeInfo() = 0;

		virtual void SetName(const std::string& name_);
		virtual void SetTooltip(const std::string& tooltip_);
		virtual void SetTreeName(const std::string& treeName_);
		virtual void SetTreeRoot(const std::string& treeRoot_)
		{
			SetTreeName(treeRoot_ + ":" + GetName());
		}
		virtual std::string& GetName();		
		virtual const std::string& GetTooltip();		
		virtual const std::string& GetTreeName();
		
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

	class Parameter_EXPORTS InputParameter
	{
	protected:
		boost::function<bool(Parameter*)> qualifier;
	public:
		typedef std::shared_ptr<InputParameter> Ptr;
		virtual bool SetInput(const std::string& name_) = 0;
		virtual bool SetInput(const Parameter::Ptr param) = 0;
		virtual bool AcceptsInput(const Parameter::Ptr param) = 0;
		virtual bool AcceptsType(const Loki::TypeInfo& type) = 0;
		virtual void SetQualifier(const boost::function<bool(Parameter*)>& f)
		{
			qualifier = f;
		}
	};

    template<typename T> class ITypedParameter: 
			public Parameter
    {
    public:
		typedef std::shared_ptr<ITypedParameter<T>> Ptr;
        virtual T* Data() = 0;
		virtual void UpdateData(T& data_) = 0;
		virtual void UpdateData(const T& data_) = 0;
		virtual void UpdateData(T* data_) = 0;
		ITypedParameter(const std::string& name, const ParameterType& type = Parameter::Control, const std::string& tooltip = "") :
			Parameter(name, type, tooltip){}
		virtual Loki::TypeInfo GetTypeInfo()
		{
			return Loki::TypeInfo(typeid(T));
		}
		/*bool operator ==(const ITypedParameter& lhs, const Parameter& rhs)
		{
			if (lhs.GetType() == rhs.GetType())
			{
				auto typedRhs = dynamic_cast<ITypedParameter<T>>(rhs);
				return *Data() == *typedRhs.Data();
			}
			return false;
		}*/
    };

	template<typename T, template<typename> class Policy1 = Persistence::PersistencePolicy, template<typename> class Policy2 = UI::UiPolicy> 
	class MetaTypedParameter : 
		public ITypedParameter<T>, public Policy1<T>, public Policy2<T>
	{
	public:
		MetaTypedParameter(const std::string& name, const ParameterType& type = Parameter::Control, const std::string& tooltip = "") :
			ITypedParameter<T>(name, type, tooltip){}
	};


    template<typename T> class TypedParameter: public MetaTypedParameter<T>
    {
        T data;
    public:
		typedef std::shared_ptr<TypedParameter<T>> Ptr;
		static Ptr create(const T& init, const std::string& name, const ParameterType& type = ParameterType::Control, const std::string& tooltip = "")
		{
			return Ptr(new TypedParameter<T>(name, init, type, tooltip));
		}
		TypedParameter(const std::string& name, const T& init = T(), const ParameterType& type = ParameterType::Control, const std::string& tooltip = ""):
			MetaTypedParameter<T>(name, type, tooltip), data(init) {}

        virtual T* Data()
        {
            return &data;
        }
        virtual void UpdateData(T& data_)
        {
            data = data_;
			changed = true;
			UpdateSignal();
        }
		virtual void UpdateData(const T& data_)
		{
			data = data_;
			changed = true;
			UpdateSignal();
		}
		virtual void UpdateData(T* data_)
		{
			data = *data_;
			changed = true;
			UpdateSignal();
		}
    };
	
	template<typename T> class TypedParameterRef : public MetaTypedParameter<T>
	{
		T& data;
	public:
		typedef std::shared_ptr<TypedParameterRef<T>> Ptr;
		TypedParameterRef(const std::string& name, const ParameterType& type = ParameterType::Control, const std::string& tooltip = ""):
			MetaTypedParameter<T>(name, type, tooltip){}
		virtual T* Data()
		{
			return &data;
		}
		virtual void UpdateData(const T& data_)
		{
			data = data_;
			changed = true;
			UpdateSignal();
		}
		virtual void UpdateData(const T* data_)
		{
			data = *data_;
			changed = true;
			UpdateSignal();
		}

	};

    template<typename T> class TypedParameterPtr: public MetaTypedParameter<T>
    {
        T* ptr;
		bool ownsData;
    public:
		typedef std::shared_ptr<TypedParameterPtr<T>> Ptr;
		static Ptr create(const std::string& name, T* ptr_ = nullptr,
			const ParameterType& type = ParameterType::Control, const std::string& tooltip = "")
		{
			return Ptr(new TypedInputParameterPtr(name, ptr_, type, tooltip));
		}
		TypedParameterPtr(const std::string& name, T* ptr_ = nullptr, 
			const ParameterType& type = ParameterType::Control, const std::string& tooltip = "", bool ownsData_ = false) : 
			ptr(ptr_),
			MetaTypedParameter<T>(name, type, tooltip), ownsData(ownsData_){}

        virtual T* Data()
        {
            return ptr;   
        }
        virtual void UpdateData(T& data)
        {
            ptr = &data;
			changed = true;
			UpdateSignal();
        }
		virtual void UpdateData(const T& data)
		{
			if (ptr)
				*ptr = data;
			changed = true;
			UpdateSignal();
		}
		virtual void UpdateData(T* data_)
		{
			ptr = data_;
			changed = true;
			UpdateSignal();
		}
    };
	// Meant for internal use and access, ie access through TypedInputParameter::Data()
	template<typename T> class TypedInputParameter : public MetaTypedParameter<T>, public InputParameter
	{
		ITypedParameter<T>::Ptr input;
		boost::signals2::connection inputConnection;
		virtual void onInputUpdate()
		{

		}
	public:
		typedef std::shared_ptr<TypedInputParameter<T>> Ptr;
		TypedInputParameter(const std::string& name, const std::string& tooltip = "", const boost::function<bool(Parameter*)>& qualifier_ = boost::function<bool(Parameter*)>()) :
			MetaTypedParameter<T>(name, ParameterType::Input, tooltip){ qualifier = qualifier_; }
		virtual bool SetInput(const std::string& name_)
		{
			return false;
		}

		virtual bool SetInput(const Parameter::Ptr param)
		{
			ITypedParameter<T>::Ptr castedParam = std::dynamic_pointer_cast<ITypedParameter<T>>(param);
			if (castedParam)
			{
				input = castedParam;
				inputConnection.disconnect();
				inputConnection = castedParam->RegisterNotifier(boost::bind(&TypedInputParameter<T>::onInputUpdate, this));
				return true;
			}
			return false;
		}

		virtual bool AcceptsInput(const Parameter::Ptr param)
		{
			return Loki::TypeInfo(typeid(T)) == param->GetTypeInfo();
		}

		virtual bool AcceptsType(const Loki::TypeInfo& type)
		{
			return Loki::TypeInfo(typeid(T)) == type;
		}
		virtual T* Data()
		{
			if (input)
				return input->Data();
			return nullptr;
		}
		virtual void UpdateData(T& data_)
		{

		}
		virtual void UpdateData(const T& data_)
		{

		}
		virtual void UpdateData(T* data_)
		{

		}

	};

	// Meant to reference a pointer variable in user space, and to update that variable whenever 
	// IE int* myVar; 
	// auto typedParam = TypedInputParameterPtr(&myVar); // TypedInputParameter now updates myvar to point to whatever the
	// input variable is for typedParam.
	template<typename T> class TypedInputParameterPtr : public MetaTypedParameter<T>, public InputParameter
	{
		T** userVar; // Pointer to the user space pointer variable of type T
		ITypedParameter<T>::Ptr input;
		boost::signals2::connection inputConnection;
		virtual void onInputUpdate()
		{
			// The input variable has been updated, update user var
			*userVar = input->Data();
		}
	public:
		typedef std::shared_ptr<TypedInputParameterPtr<T>> Ptr;
		static Ptr create(T** userVar_)
		{
			return Ptr(new TypedInputParameterPtr(userVar_));
		}
		TypedInputParameterPtr(const std::string& name, T** userVar_, 
			const ParameterType& type = ParameterType::Control, const std::string& tooltip = ""):
			MetaTypedParameter<T>(name, type, tooltip), userVar(userVar_){}

		virtual bool SetInput(const std::string& name_)
		{
			return false;
		}

		virtual bool SetInput(const Parameter::Ptr param)
		{
			ITypedParameter<T>::Ptr castedParam = std::dynamic_pointer_cast<ITypedParameter<T>>(param);
			if (castedParam)
			{
				input = castedParam;
				inputConnection.disconnect();
				inputConnection = castedParam->RegisterNotifier(boost::bind(&TypedInputParameterPtr<T>::onInputUpdate, this));
				*userVar = input->Data();
				return true;
			}
			return false;
		}

		virtual bool AcceptsInput(const Parameter::Ptr param)
		{
			return Loki::TypeInfo(typeid(T)) == param->GetTypeInfo();
		}

		virtual bool AcceptsType(const Loki::TypeInfo& type)
		{
			return Loki::TypeInfo(typeid(T)) == type;
		}
		virtual T* Data()
		{
			if (input)
				return input->Data();
			return nullptr;
		}
		virtual void UpdateData(T& data_)
		{

		}
		virtual void UpdateData(const T& data_)
		{

		}
		virtual void UpdateData(T* data_)
		{

		}
	};
}
