#pragma once
#include "Parameter.hpp"
#include <opencv2/core.hpp>
namespace Parameters
{
	namespace Converters
	{
		namespace Double
		{
			class IConverter
			{
				virtual double GetValue(int row = 0,int col = 0,int channel = 0, int index= 0) = 0;
				virtual int NumRows() = 0;
				virtual int NumCols() = 0;
				virtual int NumChan() = 0;
				virtual int NumMats() = 0;
			};

			template<typename T> auto ToMat(const T& data, cv::cuda::Stream* stream, int) ->decltype(cv::InputArray(data), cv::Mat())
			{
				cv::Mat output;
				cv::InputArray arr(data);
				if (arr.kind() == cv::InputArray::CUDA_GPU_MAT)
				{
					if(stream)
						arr.getGpuMat().download(output, *stream);
					else
						arr.getGpuMat().download(output);
					return output;
				}
				arr.copyTo(output);
				return output;
			}
			// Input output array converter
			template<typename T> class Converter : public IConverter
			{
				cv::Mat data;
				Converter(Parameter* param, cv::cuda::Stream* stream)
				{
					data = ToMat(*(static_cast<ITypedParameter<T>*>(param))->Data(), stream, 0);
					data.convertTo(data, CV_64F);
				}
				virtual double Getvalue(int row, int col, int channel)
				{
					
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
					return 0;
				}
			};



			template<typename T> class ConvertPolicy
			{
				
			};
		}
	}
}