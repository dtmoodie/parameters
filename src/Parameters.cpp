#include "Parameters.hpp"

using namespace Parameters;
Parameter::Parameter(const std::string& name_, const ParameterType& type_, const std::string& tooltip_):
name(name_), type(type_), tooltip(tooltip_), changed(false), subscribers(0)
{
}
const std::string& Parameter::GetName() const
{
	return name;
}
Parameter* Parameter::SetName(const std::string& name_)
{
	name = name_;
    return this;
}
const std::string& Parameter::GetTooltip() const
{
	return tooltip;
}
Parameter* Parameter::SetTooltip(const std::string& tooltip_)
{
	tooltip = tooltip_;
    return this;
}
Parameter* Parameter::SetTreeRoot(const std::string& treeRoot_)
{
	treeRoot = treeRoot_;
    return this;
}
const std::string& Parameter::GetTreeRoot() const
{
	return treeRoot;
}
const std::string Parameter::GetTreeName() const
{
	return treeRoot + ":" + name;
}
Parameter* Parameter::SetTreeName(const std::string& treeName_)
{
	treeRoot = treeName_;
    return this;
}
boost::signals2::connection Parameter::RegisterNotifier(const boost::function<void(cv::cuda::Stream*)>& f)
{
	return UpdateSignal.connect(f);
}
bool Parameter::Update(const Parameter::Ptr other)
{
	return false;
}
