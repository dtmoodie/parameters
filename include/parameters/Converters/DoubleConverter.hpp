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

#include <opencv2/core.hpp>
#include <opencv2/core/cuda.hpp>
#include "parameters/Parameter.hpp"
#include "parameters/Parameter_def.hpp"
#include <type_traits>
#include <mutex>

namespace Parameters
{
    template<typename T> class ITypedParameter;
	namespace Converters
	{
		namespace Double
		{
			class Parameter_EXPORTS IConverter
			{
			public:
				cv::Rect roi;
				cv::Size fullSize;
				std::mutex mtx;
				virtual void Update(Parameter* param, cv::cuda::Stream* stream) = 0;
				virtual double GetValue(int row = 0,int col = 0,int channel = 0, int index= 0) = 0;
				virtual int NumRows() = 0;
				virtual int NumCols() = 0;
				virtual int NumChan() = 0;
				virtual int NumMats() = 0;
				virtual Loki::TypeInfo GetTypeInfo() = 0;
			};
			
			template<typename T> cv::Mat ToMat(const T& data, cv::cuda::Stream* stream, cv::Rect roi, cv::Size& full_size, unsigned int)
			{
				return cv::Mat();
			}
			// Input output array converter
			template<typename T> cv::Mat ToMat(const T& data, cv::cuda::Stream* stream, cv::Rect roi, cv::Size& full_size, typename std::enable_if<std::is_constructible<cv::_InputArray, T>::value, int>::type)
			{
				cv::Mat output;
				cv::_InputArray arr(data);
				full_size = arr.size();
				if (!arr.empty())
				{
					if (arr.kind() == cv::_InputArray::CUDA_GPU_MAT)
					{
                        auto tmp_roi = roi;
                        tmp_roi.width = std::min(tmp_roi.x + tmp_roi.width, arr.size().width);
                        tmp_roi.height = std::min(tmp_roi.y + tmp_roi.height, arr.size().height);
						if (stream)
							arr.getGpuMat()(tmp_roi).download(output, *stream);
						else
							arr.getGpuMat()(tmp_roi).download(output);
						return output;
					}
					else
					{
						arr.getMat()(roi).copyTo(output);
					}
				}				
				return output;
			}
			template<typename T> cv::Mat ToMat(const cv::Point_<T>& data, cv::cuda::Stream* stream, cv::Rect roi, cv::Size& full_size, int)
			{
				cv::Mat output(1, 2, CV_64F);
				full_size = output.size();
				output.at<double>(0) = data.x;
				output.at<double>(1) = data.y;
				return output;
			}
				
			
			
			template<typename T> class Converter : public IConverter
			{
				cv::Mat data;
			public:
				Converter(Parameter* param, cv::cuda::Stream* stream)
				{
					Update(param, stream);
				}
				virtual void Update(Parameter* param, cv::cuda::Stream* stream)
				{
					std::lock_guard<std::mutex> lock(mtx);
                    data = ToMat(*(static_cast<ITypedParameter<T>*>(param))->Data(), stream, roi, fullSize, 0);
					
				}
				virtual double GetValue(int row, int col, int channel, int index)
				{
					std::lock_guard<std::mutex> lock(mtx);
					if(data.depth() != CV_64F)
						data.convertTo(data, CV_64F);
					row -= roi.y;
					col -= roi.x;
					if (row < data.rows && col < data.cols && channel < data.channels())
					{
						int channels = data.channels();
						double* ptr = data.ptr<double>();
						return ptr[row * data.cols*channels + col*channels + channel];
					}
					return 0.0;
				}
				virtual int NumRows()
				{
					return fullSize.height;
				}
				virtual int NumCols()
				{
					return fullSize.width;
				}
				virtual int NumChan()
				{
					return data.channels();
				}
				virtual int NumMats()
				{
					return 1;
				}
				virtual Loki::TypeInfo GetTypeInfo()
				{
					return Loki::TypeInfo(typeid(T));
				}
			};

			class Parameter_EXPORTS Factory
			{
				
			public:
				typedef std::function < IConverter*(Parameter* param, cv::cuda::Stream* stream) > ConverterCreationFunctor;
				static IConverter* Create(Parameter* param, cv::cuda::Stream* stream);
				static void Register(ConverterCreationFunctor func, Loki::TypeInfo type);
			private:
				static Factory& Instance();
				std::map<Loki::TypeInfo, ConverterCreationFunctor> Registry;
			};
			template<typename T> class Constructor
			{
			public:
				Constructor()
				{
					// Register constructor functions for conversion engines
					Factory::Register(std::bind(&Constructor<T>::Construct, std::placeholders::_1, std::placeholders::_2), Loki::TypeInfo(typeid(T)));
				}
				static IConverter* Construct(Parameter* param, cv::cuda::Stream* stream)
				{
					return new Converter<T>(param, stream);
				}
			};

			template<typename T> class ConverterPolicy
			{
				static Constructor<T> constructor;
			public:
				ConverterPolicy()
				{
					(void)&constructor;
				}
			};
			template<typename T> Constructor<T> ConverterPolicy<T>::constructor;
		}
	}
}
