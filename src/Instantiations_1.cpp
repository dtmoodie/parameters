#ifndef PARAMTERS_USE_UI
#define PARAMTERS_USE_UI
#endif

#include "Parameters.hpp"
#include "Types.hpp"
#include <vector>
#include <string>
#include <type_traits>

using namespace Parameters;

/*static TypedParameter<char> char_("instance");
static TypedParameter<unsigned char> uchar_("instance");
static TypedParameter<short> short_("instance");
static TypedParameter<unsigned short> ushort_("instance");
static TypedParameter<int> int_("instance");
static TypedParameter<unsigned int> uint_("instance");
static TypedParameter<long> long_("instance");
static TypedParameter<unsigned long> ulong_("instance");
static TypedParameter<long long> longlong_("instance");*/

static TypedParameter<unsigned long long> ulonglong_("instance");
static TypedParameter<float> float_("instance");
static TypedParameter<double> double_("instance");


static Parameters::TypedParameter<std::string> string_("instance");
static Parameters::TypedParameter<std::vector<std::string>> vecString_("instance");
static Parameters::TypedParameter<Parameters::ReadFile> readFile_("instance");
static Parameters::TypedParameter<Parameters::WriteFile> writeFile_("instance");
static Parameters::TypedParameter<Parameters::ReadDirectory> readDir_("instance");
static Parameters::TypedParameter<Parameters::WriteDirectory> writeDir_("instance");
static Parameters::TypedParameter<Parameters::EnumParameter> enum_("instance");

