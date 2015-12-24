#pragma once
#include <opencv2/core.hpp>
#include <opencv2/core/cuda.hpp>
#include "Parameter.hpp"
#include "Parameter_def.hpp"
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
				std::mutex mtx;
				virtual void Update(Parameter* param, cv::cuda::Stream* stream) = 0;
				virtual double GetValue(int row = 0,int col = 0,int channel = 0, int index= 0) = 0;
				virtual int NumRows() = 0;
				virtual int NumCols() = 0;
				virtual int NumChan() = 0;
				virtual int NumMats() = 0;
				virtual Loki::TypeInfo GetTypeInfo() = 0;
			};
			
			template<typename T> cv::Mat ToMat(const T& data, cv::cuda::Stream* stream, unsigned int)
			{
				return cv::Mat();
			}
			// Input output array converter
			template<typename T> cv::Mat ToMat(const T& data, cv::cuda::Stream* stream, typename std::enable_if<std::is_constructible<cv::_InputArray, T>::value, int>::type)
			{
				cv::Mat output;
				cv::_InputArray arr(data);
				if (arr.kind() == cv::_InputArray::CUDA_GPU_MAT && !arr.empty())
				{
					if (stream)
						arr.getGpuMat().download(output, *stream);
					else
						arr.getGpuMat().download(output);
					return output;
				}
				else
				{
					arr.copyTo(output);
				}
				return output;
			}
			template<typename T> cv::Mat ToMat(const cv::Point_<T>& data, cv::cuda::Stream* stream, int)
			{
				cv::Mat output(1, 2, CV_64F);
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
                    data = ToMat(*(static_cast<ITypedParameter<T>*>(param))->Data(), stream, 0);
				}
				virtual double GetValue(int row, int col, int channel, int index)
				{
					std::lock_guard<std::mutex> lock(mtx);
					if(data.depth() != CV_64F)
						data.convertTo(data, CV_64F);

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
					return data.rows;
				}
				virtual int NumCols()
				{
					return data.cols;
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
