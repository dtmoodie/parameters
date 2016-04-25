#define BOOST_TEST_MAIN
#include "parameters/ParameteredObject.h"
#include "parameters/ParameteredObjectImpl.hpp"
#include "parameters/TypedInputParameter.hpp"
#include "parameters/Persistence/TextSerializer.hpp"

#include <boost/test/unit_test.hpp>
#include <boost/thread.hpp>
using namespace Parameters;


#define TYPED_TEST_CASE_VEC(type) \
{ \
	BOOST_TEST_CHECKPOINT("serialization and deserialization of " #type); \
	TypedParameter<std::vector<type>> test("Vec_" #type, std::vector<type>(__COUNTER__, __COUNTER__)); params.push_back(&test); \
	std::stringstream ss; \
	Persistence::Text::Serialize(&ss, &test); \
	std::string str = ss.str(); \
	auto deserialized_param = Persistence::Text::DeSerialize(&str); \
	auto typed = std::dynamic_pointer_cast<TypedParameter<std::vector<type>>>(deserialized_param); \
	BOOST_REQUIRE(typed); \
	BOOST_REQUIRE(typed->Data()); \
	BOOST_REQUIRE(*typed->Data() == *test.Data()); \
}

#define TYPED_TEST_CASE(type) TYPED_TEST_CASE_VEC(type);

BOOST_AUTO_TEST_SUITE(serialization_vectors)
BOOST_AUTO_TEST_CASE(serialization_vectors)
{
	std::vector<Parameter*> params;
	TYPED_TEST_CASE(uchar);
	TYPED_TEST_CASE(char);
	TYPED_TEST_CASE(ushort);
	TYPED_TEST_CASE(short);
	TYPED_TEST_CASE(uint);
	TYPED_TEST_CASE(int);
	TYPED_TEST_CASE(float);
	TYPED_TEST_CASE(double);
}
BOOST_AUTO_TEST_SUITE_END()