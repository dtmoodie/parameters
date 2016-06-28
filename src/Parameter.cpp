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
#include "parameters/Parameter.hpp"

using namespace Parameters;
Parameter::Parameter(const std::string& name_, const ParameterType& type_, const std::string& tooltip_):
name(name_), type(type_), tooltip(tooltip_), changed(false), subscribers(0), _current_time_index(-1)
{
}
Parameter::~Parameter()
{
    delete_signal(this);
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
std::shared_ptr<Signals::connection> Parameter::RegisterNotifier(std::function<void(cv::cuda::Stream*)> f)
{
    return update_signal.connect(f);
}
std::shared_ptr<Signals::connection> Parameter::RegisterDeleteNotifier(std::function<void(Parameter*)> f)
{
    return delete_signal.connect(f);
}
bool Parameter::Update(Parameter* other)
{
    return false;
}
long long Parameter::GetTimeIndex() const
{
    return _current_time_index;
}
void Parameter::SetTimeIndex(long long index)
{
    _current_time_index = index;
}
void Parameter::OnUpdate(cv::cuda::Stream* stream)
{
    changed = true;
    update_signal(stream);
}
std::recursive_mutex& Parameter::mtx()
{
    return _mtx;
}