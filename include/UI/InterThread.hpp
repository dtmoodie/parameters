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

#include <boost/asio/io_service.hpp>
#include "Parameter_def.hpp"
#include <boost/function.hpp>
#include <queue>
#include <list>
#include <boost/thread/condition_variable.hpp>
#include <boost/thread/mutex.hpp>
#include "LokiTypeInfo.h"
namespace Parameters
{
	namespace UI
	{
        template<typename Data>
        class concurrent_queue
        {
        private:
            boost::condition_variable the_condition_variable;
            std::queue<Data> the_queue;
            mutable boost::mutex the_mutex;
        public:
            void wait_for_data()
            {
                boost::mutex::scoped_lock lock(the_mutex);
                while (the_queue.empty())
                {
                    the_condition_variable.wait(lock);
                }
            }
            void wait_push(Data const& data)
            {
                boost::mutex::scoped_lock lock(the_mutex);
                while (!the_queue.empty()) // Wait till the consumer pulls data from the queue
                {
                    the_condition_variable.wait(lock);
                }
                the_queue.push_back(data);
            }
            void push(Data const& data)
            {
                boost::mutex::scoped_lock lock(the_mutex);
                bool const was_empty = the_queue.empty();
                the_queue.push(data);

                lock.unlock(); // unlock the mutex

                if (was_empty)
                {
                    the_condition_variable.notify_one();
                }
            }
            void wait_and_pop(Data& popped_value)
            {
                boost::mutex::scoped_lock lock(the_mutex);
                while (the_queue.empty())
                {
                    the_condition_variable.wait(lock);
                }

                popped_value = the_queue.front();
                the_queue.pop();
            }
            bool try_pop(Data& popped_value)
            {
                boost::mutex::scoped_lock lock(the_mutex);
                if (the_queue.empty())
                    return false;
                popped_value = the_queue.front();
                the_queue.pop();
                return true;
            }

            size_t size()
            {
                boost::mutex::scoped_lock lock(the_mutex);
                return the_queue.size();
            }
            void clear()
            {
                boost::mutex::scoped_lock lock(the_mutex);
                the_queue = std::queue<Data>();
            }
        };
        class Parameter_EXPORTS InvalidCallbacks
        {
        public:
            static void invalidate(void* sender);
            static bool check_valid(void* sender);
            static void clear();
        private:
            static std::list<void*> invalid_senders;
            static boost::mutex mtx;
        };
		class Parameter_EXPORTS UiCallbackService
		{
            boost::function<void(boost::function<void(void)>, std::pair<void*, Loki::TypeInfo>)> user_thread_callback_service;
			boost::function<void(void)> user_thread_callback_notifier;
            concurrent_queue < std::pair<std::pair<void*, Loki::TypeInfo>, boost::function<void(void)>>> io_queue;
		public:
            static UiCallbackService* Instance();

            void post(boost::function<void(void)> f, std::pair<void*, Loki::TypeInfo> sender = std::make_pair((void*)nullptr, Loki::TypeInfo(typeid(void*))));
            size_t queue_size();
            static void setCallback(boost::function<void(boost::function<void(void)>, std::pair<void*, Loki::TypeInfo>)> f);
			static void setCallback(boost::function<void(void)>& f);
            // To be called from the UI thread
            static void run();


		};
		class Parameter_EXPORTS ProcessingThreadCallbackService
		{
            concurrent_queue < std::pair<std::pair<void*, Loki::TypeInfo>, boost::function<void(void)>>> io_queue;
			boost::function<void(boost::function<void(void)>, std::pair<void*, Loki::TypeInfo>)> user_processing_thread_callback_function;
		public:
            static ProcessingThreadCallbackService* Instance();
			static void setCallback(boost::function<void(boost::function<void(void)>, std::pair<void*, Loki::TypeInfo>)> f);
			static void run();
			static void post(boost::function<void(void)> f, std::pair<void*, Loki::TypeInfo> sender = std::make_pair((void*)nullptr, Loki::TypeInfo(typeid(void*))));
		};
	}
}
