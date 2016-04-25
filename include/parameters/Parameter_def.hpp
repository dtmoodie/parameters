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
#include <boost/log/trivial.hpp>
#include <boost/log/attributes/named_scope.hpp>
#ifdef PARAMETER_EXPORTS
#undef PARAMETER_EXPORTS
#endif
#if (defined WIN32 || defined _WIN32 || defined WINCE || defined __CYGWIN__) && defined parameters_EXPORTS
#  define PARAMETER_EXPORTS __declspec(dllexport)
#elif defined __GNUC__ && __GNUC__ >= 4
#  define PARAMETER_EXPORTS __attribute__ ((visibility ("default")))
#else
#  define PARAMETER_EXPORTS
#endif

// These are the logging calls that are extensively used
#ifdef _MSC_VER
#define LOG_TRIVIAL(severity) BOOST_LOG_FUNCTION(); BOOST_LOG_TRIVIAL(severity)
#else
#define LOG_TRIVIAL(severity) BOOST_LOG_FUNCTION(); BOOST_LOG_TRIVIAL(severity)
#endif

#ifdef _MSC_VER
  #ifndef parameters_EXPORTS
     #ifdef _DEBUG
        #pragma comment(lib, "parametersd.lib")
     #else
        #pragma comment(lib, "parameters.lib")
     #endif
  #endif
#endif
