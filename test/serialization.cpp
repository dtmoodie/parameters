#define BOOST_TEST_MAIN
#include "parameters/ParameteredObject.h"
#include "parameters/ParameteredObjectImpl.hpp"
#include "parameters/TypedInputParameter.hpp"
#include "parameters/Persistence/TextSerializer.hpp"

#include <boost/test/unit_test.hpp>
#include <boost/thread.hpp>

#include <fstream>

using namespace Parameters;

#define TYPED_TEST_CASE_(type) \
{ \
    BOOST_TEST_CHECKPOINT("serialization and deserialization of " #type); \
    TypedParameter<type> test(#type, __COUNTER__); params.push_back(&test); \
    std::stringstream ss; \
    Persistence::Text::Serialize(&ss, &test); \
    std::string str = ss.str(); \
    BOOST_LOG_TRIVIAL(debug) << "Serialized value: " << str; \
    auto deserialized_param = Persistence::Text::DeSerialize(&str); \
    auto typed = std::dynamic_pointer_cast<TypedParameter<type>>(deserialized_param); \
    BOOST_REQUIRE(typed); \
    BOOST_REQUIRE(typed->Data()); \
    BOOST_CHECK_EQUAL(*typed->Data() , *test.Data()); \
}
#define TYPED_TEST_CASE(type) TYPED_TEST_CASE_(type); 


BOOST_AUTO_TEST_SUITE(serialization)
BOOST_AUTO_TEST_CASE(serialization_pod)
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

BOOST_AUTO_TEST_CASE(CerealSerialization)
{
    TypedParameter<int> test;
    test.UpdateData(5);
    {
        std::ofstream ofs("test.xml");
        cereal::XMLOutputArchive ar(ofs);
        Parameters::Cereal::Serializer::instance()->Serialize(ar, &test);
    }
    
}

BOOST_AUTO_TEST_SUITE_END()