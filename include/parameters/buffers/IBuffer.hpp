#pragma once
#include <parameters/Parameter_def.hpp>
namespace Parameters
{
	namespace Buffer
	{
		class PARAMETER_EXPORTS IBuffer
		{
		public:
			virtual ~IBuffer() {}
			virtual void SetSize(long long size = -1) = 0;
			virtual long long GetSize() const = 0;
			virtual void GetTimestampRange(long long& start, long long& end) const = 0;
		};
	}
}