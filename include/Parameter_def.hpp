#pragma once

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

// These are the logging calls that are extensively used
#ifdef _MSC_VER
#define LOG_TRIVIAL(severity) BOOST_LOG_FUNCTION(); BOOST_LOG_TRIVIAL(severity)
#else
#define LOG_TRIVIAL(severity) BOOST_LOG_FUNCTION(); BOOST_LOG_TRIVIAL(severity)
#endif
#define LOG_TRACE  LOG_TRIVIAL(trace) << " "