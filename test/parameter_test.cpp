
#include "parameters/Parameters.hpp"
#include "parameters/UI/Qt.hpp"
#include <string>
#include <iostream>
/*
int main()
{
	{
		// In this use case, there are two userspace variables, and two Parameter objects.  The parameter objects
		// are constructed with pointers to the user space variables, and to automatically update the user space input
		// based on the Output parameter object.  
		int* userInput = nullptr;
		int userOutput = 50;
		int userOutput2 = 500;
		Parameters::TypedInputParameterPtr<int> typedInputParam("Test input", &userInput);
		Parameters::TypedParameterPtr<int>::Ptr typedParameter(new Parameters::TypedParameterPtr<int>("Output", &userOutput));
		if (typedInputParam.SetInput(typedParameter))
		{
			userOutput = 60;
		}
		typedParameter->UpdateData(&userOutput2);
		*userInput == userOutput2;
	}

	{
		// In this example, param is the holder of the data.  Useful if data is a large object and we want to avoid large copies with serialization
		int* userInput = nullptr;
		auto param = Parameters::TypedParameter<int>::create(20, "TestParam");
		auto inputParam = Parameters::TypedInputParameterPtr<int>("TestInput", &userInput);
		inputParam.SetInput(param);
		param->UpdateData(500);
		*userInput == *param->Data();
}
	{
		// In this case everything is stored in the parameters
		auto param = Parameters::TypedParameter<double>::create(20.0, "TestParam");
		auto inputParameter = Parameters::TypedInputParameter<double>("Test Input Parameter");
		inputParameter.SetInput(param);
		bool eqVal = *inputParameter.Data() == *param->Data();
		bool eqPtr = inputParameter.Data() == param->Data();
	}
	{

		cv::FileStorage fs("test.yml", cv::FileStorage::WRITE);
		auto s_param = Parameters::TypedParameter<cv::Scalar>("TestParam", cv::Scalar(255, 0, 0));
		auto s_matParam = Parameters::TypedParameter<cv::Mat>("MatParam", cv::Mat::eye(4, 4, CV_32F));
		auto d_param = Parameters::TypedParameter<cv::Scalar>("TestParam", cv::Scalar(0, 0, 0));
		auto d_matParam = Parameters::TypedParameter<cv::Mat>("MatParam", cv::Mat::eye(1, 1, CV_32F));
		fs << "Parameters" << "{";
		Parameters::Persistence::cv::Serialize(&fs, &s_param);
		Parameters::Persistence::cv::Serialize(&fs, &s_matParam);
		fs << "}";
		fs.release();
		fs.open("test.yml", cv::FileStorage::READ);
		cv::FileNode& node = fs["Parameters"];
		Parameters::Persistence::cv::DeSerialize(&node, &d_param);
		Parameters::Persistence::cv::DeSerialize(&node, &d_matParam);
		auto scalarResult = *s_param.Data() == *d_param.Data();
		auto matResult = *s_matParam.Data() == *d_matParam.Data();
		//Parameters::Persistence::cv::InterpreterRegistry::GetInterpretingFunction(param.GetTypeInfo())(&fs, &param);
	}

	//Parameters::Persistence::cv::InterpreterRegistry::GetInterpretingFunction()


    return 0;
}
*/
int main()
{
    Parameters::TypedParameter<int> param("test");
	Parameters::TypedParameter<std::vector<int>> vecParam("test");
	vecParam.Data()->push_back(10);
	vecParam.Data()->push_back(15);
    std::stringstream test;
    Parameters::Persistence::Text::Serialize(&test, &param);
	Parameters::Persistence::Text::Serialize(&test, &vecParam);
    std::cout << test.str() << std::endl;
	auto param1 = Parameters::Persistence::Text::DeSerialize(&test);
	auto param2 = Parameters::Persistence::Text::DeSerialize(&test);
    return 0;
}
