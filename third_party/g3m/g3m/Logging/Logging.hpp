/*
 * Copyright (c) <2016>, <Robert Reinecke>
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef GLOBAL_FLOW_LOGGING_HPP
#define GLOBAL_FLOW_LOGGING_HPP

#include "iostream"
#include "Sinks.hpp"

#define LOG(level) BOOST_LOG_SEV(GlobalFlow::Logging::global_logger::get(), level)
namespace GlobalFlow {
/**
 * @enum The logging levels for the global logger
 */
enum severity_level {
    debug,
    userinfo,
    stateinfo,
    numerics,
    error,
    critical
};
namespace Logging {

// Initializing global boost::log logger
typedef boost::log::sources::severity_channel_logger_mt<
        severity_level, std::string> global_logger_type;

BOOST_LOG_INLINE_GLOBAL_LOGGER_INIT(global_logger, global_logger_type) {
    return
            global_logger_type(boost::log::keywords::channel = "global_logger");
}

BOOST_LOG_ATTRIBUTE_KEYWORD(severity, "Severity", severity_level);

template<typename C>
class Singleton {
    public:
        static C *instance() {
            if (!_instance) {
                _instance = new C();
            }
            return _instance;
        }

        virtual
        ~Singleton() {
            _instance = 0;
        }

    private:
        static C *_instance;
    protected:
        Singleton() {}
};

template<typename C> C *Singleton<C>::_instance = 0;

class LoggerInterface : public Singleton<LoggerInterface> {
        friend class Singleton<LoggerInterface>;

    private:

    public:
        virtual ~LoggerInterface() {};
};

/**
 * @class A simple logger to write logs to files
 */
class Logger : public LoggerInterface {
    private:
        void
        initSinks() {
            initLogFile();
            initCoutLog();
            //initInfoDump();
        }

        void
        initLogFile() {
            std::string logfilename = "test.log";
            // create sink to logfile
            boost::shared_ptr<text_sink> sink = boost::make_shared<text_sink>();
            sink->locked_backend()->add_stream(boost::make_shared<std::ofstream>(logfilename.c_str()));

            // flush
            sink->locked_backend()->auto_flush(true);

            // format sink
            sink->set_formatter
                    (
                            expr::format("%1%: <%2%> %3%")
                            % expr::attr < unsigned
            int > ("LineID")
                  % logboost::trivial::severity
                  % expr::smessage
            );

            // filter
            sink->set_filter(severity == debug);

            // register sink
            logboost::core::get()->add_sink(sink);
        }

        void
        initCoutLog() {
            // create sink to stdout
            boost::shared_ptr<text_sink> sink = boost::make_shared<text_sink>();
            sink->locked_backend()->add_stream(boost::shared_ptr<std::ostream>(&std::cout));

            // flush
            sink->locked_backend()->auto_flush(true);

            // format sink
            /*sink->set_formatter
                (
                    expr::format("%1%: <%2%> %3%")
                        % expr::attr < unsigned
            int > ("LineID")
                % logboost::trivial::severity
                % expr::smessage
                );*/

            // filter
            sink->set_filter(severity == userinfo | severity == numerics | severity == error | severity == critical);

            // register sink
            logboost::core::get()->add_sink(sink);
        }

        void
        initInfoDump() {

        }

    public:
        Logger() {
            initSinks();
        };
};

} //ns Logging
} //ns GlobalFlow
#endif //LOGGING_HPP
