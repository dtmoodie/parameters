#pragma once
#include <parameters/Parameter_def.hpp>
#ifdef HAVE_PROTOBUF
#include "parameter.pb.h"
#endif
#include <parameters/LokiTypeInfo.h>

#include <map>
#include <functional>
#include <istream>
namespace google
{
    namespace protobuf
    {
        namespace io
        {
            class ZeroCopyInputStream;
            class CodedInputStream;
            class ZeroCopyOutputStream;
            class CodedOutputStream;
        }
    }
}
namespace Parameters
{
	class Parameter;
	namespace Persistence
	{
		namespace Protobuf
		{
			class PARAMETER_EXPORTS SerializationFactory
			{
				typedef std::function<bool(Parameter*, void*, int)>											deserialize_array_f;
				typedef std::function<bool(Parameter*, ::google::protobuf::io::ZeroCopyInputStream*, int)>	deserialize_zero_copy_stream_f;
				typedef std::function<bool(Parameter*, ::google::protobuf::io::CodedInputStream*)>			deserialize_coded_stream_f;
				typedef std::function<bool(Parameter*, int)>													deserialize_file_descriptor_f;
				typedef std::function<bool(Parameter*, std::istream*)>										deserialize_istream_f;
				typedef std::function<bool(Parameter*, const std::string&)>									deserialize_string_f;

				typedef std::function<bool(Parameter*, void*, int)>											serialize_array_f;
				typedef std::function<bool(Parameter*, ::google::protobuf::io::ZeroCopyOutputStream*, int)> serialize_zero_copy_stream_f;
				typedef std::function<bool(Parameter*, ::google::protobuf::io::CodedOutputStream*)>			serialize_coded_stream_f;
				typedef std::function<bool(Parameter*, int)>												serialize_file_descriptor_f;
				typedef std::function<bool(Parameter*, std::ostream*)>										serialize_ostream_f;
				typedef std::function<bool(Parameter*, std::string*)>										serialize_string_f;
			public:
				Parameter* MessageToParameter(void* byte_array, int size);
				Parameter* MessageToParameter(::google::protobuf::io::ZeroCopyInputStream* stream, int size);
				Parameter* MessageToParameter(::google::protobuf::io::CodedInputStream* stream);
				Parameter* MessageToParameter(int file_descriptor);
				Parameter* MessageToParameter(std::istream* stream);
				Parameter* MessageToParameter(const std::string& string);
				
				bool ParameterToMessage(Parameter* param, void* byte_array, int size);
				bool ParameterToMessage(Parameter* param, ::google::protobuf::io::ZeroCopyOutputStream* stream, int size);
				bool ParameterToMessage(Parameter* param, ::google::protobuf::io::CodedOutputStream* stream);
				bool ParameterToMessage(Parameter* param, int file_descriptor);
				bool ParameterToMessage(Parameter* param, std::ostream* stream);
				bool ParameterToMessage(Parameter* param, std::string* string);

				struct serializer
				{
					serialize_array_f serialize_array;
					serialize_zero_copy_stream_f serialize_zero_copy_stream;
					serialize_coded_stream_f serialize_coded_stream;
					serialize_file_descriptor_f serialize_file_descriptor;
					serialize_ostream_f serialize_ostream;
					serialize_string_f serialize_string;
				};

				struct deserializer
				{
					deserialize_array_f deserialize_array;
					deserialize_zero_copy_stream_f deserialize_zero_copy_stream;
					deserialize_coded_stream_f deserialize_coded_stream;
					deserialize_file_descriptor_f deserialize_file_descriptor;
					deserialize_istream_f deserialize_istream;
					deserialize_string_f deserialize_string;
				};

				void register_serializer(Loki::TypeInfo type, serializer serializer_);
				void register_deserializer(Loki::TypeInfo type, deserializer deserializer_);
				void register_both(Loki::TypeInfo type, serializer serializer_, deserializer deserializer);
				static SerializationFactory* instance();
			protected:
				std::map<Loki::TypeInfo, serializer> _registered_serializers;
				std::map<Loki::TypeInfo, deserializer> _registered_deserializers;
			private:
				SerializationFactory();
				SerializationFactory(const SerializationFactory& other);
			};
#ifdef HAVE_PROTOBUF
			template<class T> struct PARAMETER_EXPORTS interpreter
			{
				static const bool IS_DEFAULT = true;
				typedef T value_type;

				static bool deserialize(Parameter* param, void* byte_array, int size)
				{
					return false;
				}
				static bool deserialize(Parameter* param, ::google::protobuf::io::ZeroCopyInputStream* stream, int size)
				{
					return false;
				}
				static bool deserialize(Parameter* param, ::google::protobuf::io::CodedInputStream* stream)
				{
					return false;
				}
				static bool deserialize(Parameter* param, int file_descriptor)
				{
					return false;
				}
				static bool deserialize(Parameter* param, std::istream* stream)
				{
					return false;
				}
				static bool deserialize(Parameter* param, const std::string& string)
				{
					return false;
				}

				static bool serialize(Parameter* param, void* byte_array, int size)
				{
					return false;
				}
				static bool serialize(Parameter* param, ::google::protobuf::io::ZeroCopyOutputStream* stream, int size)
				{
					return false;
				}
				static bool serialize(Parameter* param, ::google::protobuf::io::CodedOutputStream* stream)
				{
					return false;
				}
				static bool serialize(Parameter* param, int file_descriptor)
				{
					return false;
				}
				static bool serialize(Parameter* param, std::ostream* stream)
				{
					return false;
				}
				static bool serialize(Parameter* param, std::string* string)
				{
					return false;
				}
			};

			template<class T> struct PARAMETER_EXPORTS constructor
			{
				constructor()
				{
					if(!interpreter<T>::IS_DEFAULT)
					{
						SerializationFactory::instance()->register_serializer(Loki::TypeInfo(typeid(T)),
						{
							std::bind(static_cast<bool(*)(Parameter*, void*, int)>                                        (&interpreter<T>::serialize), std::placeholders::_1, std::placeholders::_2, std::placeholders::_3),
							std::bind(static_cast<bool(*)(Parameter*, ::google::protobuf::io::ZeroCopyOutputStream*, int)>(&interpreter<T>::serialize), std::placeholders::_1, std::placeholders::_2, std::placeholders::_3),
							std::bind(static_cast<bool(*)(Parameter*, ::google::protobuf::io::CodedOutputStream*)>        (&interpreter<T>::serialize), std::placeholders::_1, std::placeholders::_2),
							std::bind(static_cast<bool(*)(Parameter*, int)>                                               (&interpreter<T>::serialize), std::placeholders::_1, std::placeholders::_2),
							std::bind(static_cast<bool(*)(Parameter*, std::ostream*)>                                     (&interpreter<T>::serialize), std::placeholders::_1, std::placeholders::_2),
							std::bind(static_cast<bool(*)(Parameter*, std::string*)>                                      (&interpreter<T>::serialize), std::placeholders::_1, std::placeholders::_2)
						});

						SerializationFactory::instance()->register_deserializer(Loki::TypeInfo(typeid(T)),
						{
							std::bind(static_cast<bool(*)(Parameter*, void*, int)>                                       (&interpreter<T>::deserialize), std::placeholders::_1, std::placeholders::_2, std::placeholders::_3),
							std::bind(static_cast<bool(*)(Parameter*, ::google::protobuf::io::ZeroCopyInputStream*, int)>(&interpreter<T>::deserialize), std::placeholders::_1, std::placeholders::_2, std::placeholders::_3),
							std::bind(static_cast<bool(*)(Parameter*, ::google::protobuf::io::CodedInputStream*)>        (&interpreter<T>::deserialize), std::placeholders::_1, std::placeholders::_2),
							std::bind(static_cast<bool(*)(Parameter*, int)>                                              (&interpreter<T>::deserialize), std::placeholders::_1, std::placeholders::_2),
							std::bind(static_cast<bool(*)(Parameter*, std::istream*)>                                    (&interpreter<T>::deserialize), std::placeholders::_1, std::placeholders::_2),
							std::bind(static_cast<bool(*)(Parameter*, const std::string&)>                               (&interpreter<T>::deserialize), std::placeholders::_1, std::placeholders::_2)
						});
					}
				}
			};

			template<class T> class PARAMETER_EXPORTS ProtoPolicy
			{
			public:
				ProtoPolicy()
				{
					(void*)&_constructor;
				}
			private:
				static constructor<T> _constructor;
			};
			template<class T> constructor<T> ProtoPolicy<T>::_constructor;
#else
            template<class T> class PARAMETER_EXPORTS ProtoPolicy
            {
            };
#endif
		}
	}
}
