#pragma once
#include "parameters/Parameter_def.hpp"

#include <signals/signaler.h>
#include <signals/connection.h>
#include <memory>
#include <list>
#include <vector>
#include <mutex>

struct ISimpleSerializer;
namespace Parameters
{
	class Parameter;
	template<typename T> class ITypedParameter;
	template<typename T> class TypedInputParameter;
}
namespace Signals
{
    template<class Sig> class typed_signal_base;
	class signal_manager;
}
namespace cv
{
	class FileNode;
	namespace cuda
	{
		class Stream;
	}
}
namespace Parameters
{
	class IVariableManager;
	
    class PARAMETER_EXPORTS ParameteredObject: public Signals::signaler
    {
    public:
        ParameteredObject();
        ~ParameteredObject();
		//virtual void setup_signals(Signals::signal_manager* manager);
		virtual void SetupVariableManager(IVariableManager* manager);
        virtual IVariableManager* GetVariableManager();


		virtual Parameter* addParameter(Parameter* param);
		virtual Parameter* addParameter(std::shared_ptr<Parameter> param);
        
		virtual void RemoveParameter(std::string name);
        virtual void RemoveParameter(size_t index);

        virtual bool exists(const std::string& name);
        virtual bool exists(size_t index);

        // Thows exception on unable to get parameter
		virtual Parameter* getParameter(int idx);
		virtual Parameter* getParameter(const std::string& name);

        // Returns nullptr on unable to get parameter
		virtual Parameter* getParameterOptional(int idx);
		virtual Parameter* getParameterOptional(const std::string& name);

        virtual std::vector<Parameter*> getParameters();
		template<typename T> std::vector<ITypedParameter<T>*> getParameters();
		virtual std::vector<Parameter*> getDisplayParameters();

		// Registers a callback function to be called when a parameter is updated.  The callback will be called within one of the RunCallback.... private functions.
		// Mutexes will be locked according to the flags passed into the registration function.
		// lock_param will lock just the underlying parameter object during callback execution
		// lock_object will lock this whole object during the callback execution
        virtual void RegisterParameterCallback(int idx, const std::function<void(cv::cuda::Stream*)>& callback, bool lock_param = false, bool lock_object = false);
        virtual void RegisterParameterCallback(const std::string& name, const std::function<void(cv::cuda::Stream*)>& callback, bool lock_param = false, bool lock_object = false);
        virtual void RegisterParameterCallback(Parameters::Parameter* param, const std::function<void(cv::cuda::Stream*)>& callback, bool lock_param = false, bool lock_object = false);

		// Add a new implicit parameter with name that references user space data
        template<typename T> Parameters::ITypedParameter<T>* registerParameter(const std::string& name, T* data);

		// These functions are used for adding implicit parameters to an object
        template<typename T> ITypedParameter<T>* addParameter(const std::string& name, const T& data);
		template<typename T> ITypedParameter<T>* addParameter(const std::string& name, const T& data, const T& min_value_, const T& max_value_);
        template<typename T> ITypedParameter<T>* addIfNotExist(const std::string& name, const T& data);
        template<typename T> TypedInputParameter<T>* addInputParameter(const std::string& name);
		
        template<typename T> bool updateInputQualifier(const std::string& name, const std::function<bool(Parameters::Parameter*)>& qualifier);
        template<typename T> bool updateInputQualifier(int idx, const std::function<bool(Parameters::Parameter*)>& qualifier);

		// This functio is used for updating / adding a parameter of type T which is a pointer to a user member variable data
        template<typename T> ITypedParameter<T>* updateParameterPtr(const std::string& name, T* data, long long timestamp = -1, cv::cuda::Stream* stream = nullptr);

		// Update implicit parameters 
        template<typename T> ITypedParameter<T>* updateParameter(const std::string& name, const T& data, long long timestamp = -1, cv::cuda::Stream* stream = nullptr);
        template<typename T> ITypedParameter<T>* updateParameter(size_t idx, const T data, long long timestamp = -1, cv::cuda::Stream* stream = nullptr);
		template<typename T> ITypedParameter<T>* updateParameter(const std::string& name, const T& data, const T& min_value_, const T& max_value_, long long timestamp = -1, cv::cuda::Stream* stream = nullptr);
		template<typename T> ITypedParameter<T>* updateParameter(size_t idx, const T& data, const T& min_value_, const T& max_value_, long long timestamp = -1, cv::cuda::Stream* stream = nullptr);
		Parameter* updateParameter(Parameter* parameter);
		Parameter* updateParameter(std::shared_ptr<Parameter> parameter);

		// Typed parameter fetching functions
        template<typename T> ITypedParameter<T>* getParameter(std::string name);
        template<typename T> ITypedParameter<T>* getParameter(int idx);
        template<typename T> ITypedParameter<T>* getParameterOptional(std::string name);
        template<typename T> ITypedParameter<T>* getParameterOptional(int idx);

        
        // Mutex for blocking processing of a object during update
        std::recursive_mutex																mtx;
		SIGNALS_BEGIN(ParameteredObject)
			SIG_SEND(parameter_updated, ParameteredObject*);
			SIG_SEND(parameter_added, ParameteredObject*);
		SIGNALS_END
    protected:
		IVariableManager*			                            _variable_manager;
		
		//Signals::typed_signal_base<void(ParameteredObject*)>*   _sig_parameter_updated;
		//Signals::typed_signal_base<void(ParameteredObject*)>*	_sig_parameter_added;

		// callback connections for each parameter for the onUpdate signal
        std::list<std::shared_ptr<Signals::connection>>         _callback_connections;
        // Implicitly defined parameters are ones defined via addParameter or update parameter, there is no user space typed variable for access
		// They can only be accessed by getParameter calls.  Should be serialized accross object instances
		std::vector<std::shared_ptr<Parameter>>					_implicit_parameters;
		// Excplicitly defined parameters are usually defined via a macro, and they have an explicit user space variable
		// which can be accessed like a regular variable.  Not to be serialized across object instances since they will be 
		// re-registered during object initialization
		std::vector<Parameter*>									_explicit_parameters;
		// All implicit and explicitly defined parameters, implicit first.
		// Do not serialize, will be repopulated during initialization
		std::vector<Parameter*>									_parameters;
		// This function initializes any parameters created the PARAM_BEGIN macro to 
		virtual void RegisterAllParams();
		// This is called by each parameter when the parameter is updated
		virtual void onUpdate(Parameters::Parameter* param = nullptr, cv::cuda::Stream* stream = nullptr);
    private:
        void RunCallbackLockObject(cv::cuda::Stream* stream, const std::function<void(cv::cuda::Stream*)>& callback);
        void RunCallbackLockParameter(cv::cuda::Stream* stream, const std::function<void(cv::cuda::Stream*)>& callback, std::recursive_mutex* paramMtx);
        void RunCallbackLockBoth(cv::cuda::Stream* stream, const std::function<void(cv::cuda::Stream*)>& callback, std::recursive_mutex* paramMtx);
    };
}
