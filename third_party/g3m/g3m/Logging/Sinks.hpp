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

#ifndef GLOBAL_FLOW_Sinks_HPP
#define GLOBAL_FLOW_Sinks_HPP

#include <boost/log/core.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/common.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <fstream>

namespace GlobalFlow {
namespace Logging {
namespace logboost = boost::log;
namespace sinks = boost::log::sinks;
namespace src = boost::log::sources;
namespace expr = boost::log::expressions;
namespace attrs = boost::log::attributes;
namespace keywords = boost::log::keywords;

typedef sinks::synchronous_sink<sinks::text_ostream_backend> text_sink;

/**
 * @deprecated Use methods from DataProcessing
 */
class InfoSinkInterface :
        public sinks::basic_sink_backend<
                sinks::combine_requirements<
                        sinks::synchronized_feeding,
                        sinks::flushing
                >::type
        > {
    private:
        // The file to write the collected information to
        std::ofstream m_file;
    public:
        InfoSinkInterface(std::string file_name) {};

        virtual ~InfoSinkInterface() {};

        // The function consumes the log records that come from the frontend
        virtual void consume(logboost::record_view const &rec) = 0;

        void flush() {};

        void write_data() {};
};

/*class CSVWriter : public InfoSinkInterface {

};

class NetCDFWriter : public InfoSinkInterface {

};

class DataBaseWrite : public InfoSinkInterface {

};*/

} //ns Logging
} //ns GlobalFlow
#endif //Sinks_HPP
