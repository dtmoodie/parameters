#ifndef PARAMTERS_USE_UI
#define PARAMTERS_USE_UI
#endif

#include "Parameters.hpp"
#include "Types.hpp"
#include <vector>
#include <string>
#include <type_traits>

using namespace Parameters;



static TypedParameter<std::vector<char>> vecchar_("instance");
static TypedParameter<std::vector<unsigned char>> vecuchar_("instance");
static TypedParameter<std::vector<short>> vecshort_("instance");
static TypedParameter<std::vector<unsigned short>> vecushort_("instance");
static TypedParameter<std::vector<int>> vecint_("instance");
static TypedParameter<std::vector<unsigned int>> vecuint_("instance");
static TypedParameter<std::vector<long>> veclong_("instance");
static TypedParameter<std::vector<unsigned long>> veculong_("instance");
static TypedParameter<std::vector<long long>> veclonglong_("instance");
static TypedParameter<std::vector<unsigned long long>> veculonglong_("instance");
static TypedParameter<std::vector<float>> vecfloat_("instance");
static TypedParameter<std::vector<double>> vecdouble_("instance");
