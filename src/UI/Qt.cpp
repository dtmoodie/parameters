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
#ifdef Qt5_FOUND
#include "parameters/UI/Qt.hpp"
#include "parameters/Parameter.hpp"
#include "parameters/UI/Qt/DefaultProxy.hpp"

#include <signals/logging.hpp>


using namespace Parameters::UI::qt;
WidgetFactory::WidgetFactory()
{

}
WidgetFactory* WidgetFactory::Instance()
{
    static WidgetFactory inst;
    return &inst;
}

void WidgetFactory::RegisterCreator(Loki::TypeInfo type, HandlerCreator f)
{
    LOG(trace) << "Registering type " << type.name();
    registry[type] = f;
}
std::string print_types(std::map<Loki::TypeInfo, WidgetFactory::HandlerCreator>& registry)
{
    std::stringstream ss;
    for(auto& item : registry)
    {
        ss << item.first.name() << ", ";
    }
    return ss.str();
}
std::shared_ptr<IParameterProxy> WidgetFactory::Createhandler(Parameters::Parameter* param)
{
    std::string typeName = param->GetTypeInfo().name();
    std::string treeName = param->GetTreeName();
    auto itr = registry.find(param->GetTypeInfo());
    if (itr == registry.end())
    {
        LOG(debug) << "No Widget Factory registered for type " << typeName 
            << " unable to make widget for parameter: " << treeName 
            << ".  Known types: " << print_types(registry);
            
        return std::shared_ptr<IParameterProxy>(new DefaultProxy(param));
    }
    LOG(trace) << "Creating handler for " << typeName << " " << treeName;
    return itr->second(param);
}



#endif // Qt5_FOUND