#define BOOST_TEST_MAIN
#include "parameters/ParameteredObject.h"
#include "parameters/ParameteredObjectImpl.hpp"
#include "parameters/TypedInputParameter.hpp"
#include "parameters/Persistence/TextSerializer.hpp"

#include <boost/test/unit_test.hpp>
#include <boost/thread.hpp>
using namespace Parameters;

class test_paramed_object : public ParameteredObject
{
public:
	~test_paramed_object()
	{

	}
	BEGIN_PARAMS
		PARAM(int, test, 1)
		PARAM(int, test2, 0, 5)
		PARAM(double, test3, 0, 5, 3)
		PARAM(std::vector<float>, test4, 0, 10)
	END_PARAMS;
};


BOOST_AUTO_TEST_SUITE(parametered_object)
BOOST_AUTO_TEST_CASE(explicit_parameters)
{
	test_paramed_object test_obj;
	test_obj.setup_signals(Signals::signal_manager::get_instance());
	test_obj.RegisterAllParams();
	
	// ----------------------------------------------------
	// Test basic access update and ranging
	BOOST_TEST_CHECKPOINT("raw access");
	BOOST_TEST_MESSAGE("testing basic parameter access");
	BOOST_CHECK(test_obj.test == 1);
	BOOST_CHECK(test_obj.test3 == 3);
	BOOST_CHECK(test_obj.test4.empty());
	test_obj.test2_param.UpdateData(3);
	BOOST_CHECK(test_obj.test2 == 3);
	test_obj.test2_param.UpdateData(100);
	BOOST_CHECK(test_obj.test2 < 5);
	BOOST_CHECK(test_obj.test2 > 0);

	// ----------------------------------------------------
	// Test dynamic access, etc
	BOOST_TEST_CHECKPOINT("parameter access");
	BOOST_TEST_MESSAGE("testing dynamic parameter access");
	auto display_params = test_obj.getDisplayParameters();
	BOOST_CHECK(display_params.size() == 4);
	auto int_params = test_obj.getParameters<int>();
	BOOST_CHECK(int_params.size() == 2);

// ----------------------------------------------------
// Test parameter linking
	{
		BOOST_TEST_CHECKPOINT("static parameter linking");
		TypedInputParameter<int> test_input_int("test_input_int");
		test_input_int.SetInput(&test_obj.test2_param);
		BOOST_CHECK(test_input_int.Data());
		test_obj.test2 = 3;
		BOOST_CHECK(*test_input_int.Data() == 3);
		*test_input_int.Data() = 5;
		BOOST_CHECK(test_obj.test2 == 5);
		BOOST_CHECK(test_obj.test2_param.subscribers == 1);
	}
	BOOST_CHECK(test_obj.test2_param.subscribers == 0);
	{
		BOOST_TEST_CHECKPOINT("dynamic parameter linking");
		auto param = test_obj.getParameter("test2");
		TypedInputParameter<int> test_input("test_input");
		test_input.SetInput(param);
		BOOST_CHECK(test_input.Data());
		test_obj.test2 = 2;
		BOOST_CHECK(*test_input.Data() == 2);
		BOOST_CHECK(test_input.Data() == &test_obj.test2);
		BOOST_CHECK(test_obj.test2_param.subscribers == 1);
	}
	BOOST_CHECK(test_obj.test2_param.subscribers == 0);
	{
		BOOST_TEST_CHECKPOINT("dynamic buffer creation");
		auto int_proxy = Parameters::Buffer::ParameterProxyBufferFactory::Instance()->CreateProxy(&test_obj.test2_param);
		BOOST_CHECK(int_proxy);


		TypedInputParameter<int> test_input("test_input");
		test_input.SetInput(int_proxy);
		test_obj.test2_param.SetRange(-1, 1000);
		test_obj.test2_param.UpdateData(0, 0);
		for (int i = 1; i < 100; ++i)
		{
			test_obj.test2_param.UpdateData(i, i);
			if (test_input.Data(i - 1))
				BOOST_CHECK(*test_input.Data(i - 1) == i - 1);
		}
		BOOST_TEST_CHECKPOINT("multi threaded buffer access");
		BOOST_REQUIRE(dynamic_cast<Buffer::IParameterBuffer*>(int_proxy));
		
		dynamic_cast<Buffer::IParameterBuffer*>(int_proxy)->SetSize(0);
		dynamic_cast<Buffer::IParameterBuffer*>(int_proxy)->SetSize(200);
		test_obj.test2_param.SetRange(-1, 100000);
		boost::thread thread = boost::thread(
			[&test_input]()
		{
			for (int i = 0; i < 1000; ++i)
			{
				for (int j = 0; j < 100; ++j)
				{
					int value;
					if (test_input.GetData(value, i))
					{
						BOOST_CHECK(value == i);
						break;
					}
				}
			}
		}
		);
		for (int i = 0; i < 1000; ++i)
		{
			test_obj.test2_param.UpdateData(i, i);
		}
		thread.join();
		delete int_proxy;
		BOOST_CHECK(test_obj.test2_param.subscribers == 0);		
	}
	{
		BOOST_TEST_CHECKPOINT("parameter update signals");
		bool update_signal_sent = false;
		auto connection = test_obj.test2_param.RegisterNotifier([&update_signal_sent](cv::cuda::Stream*) {
			update_signal_sent = true;
		});
		test_obj.test2_param.UpdateData(10);
		BOOST_CHECK(update_signal_sent);
	}
	{
		BOOST_TEST_CHECKPOINT("parameter delete signals");

		bool delete_signal_sent = false;
		Signals::connection::Ptr connection;
		{
			TypedParameterPtr<int> test_delete_obj("test_delete_obj");
			connection = test_delete_obj.RegisterDeleteNotifier([&delete_signal_sent](Parameter*)
			{
				delete_signal_sent = true;
			});
		}
		BOOST_CHECK(delete_signal_sent);
	}
}
BOOST_AUTO_TEST_SUITE_END()