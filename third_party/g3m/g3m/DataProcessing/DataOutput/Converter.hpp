/*
 * Copyright (c) <2016>, <Robert Reinecke>
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation and/or other
 * materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse
 * or promote products derived from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef GLOBAL_FLOW_CONVERTER_HPP
#define GLOBAL_FLOW_CONVERTER_HPP

/**
 * Converter Functions intended to be used internaly only
 * Currently only needed to allow compilation of different internal types
 * There should be a more beautiful solution to this
 * @bug Some conversions make no sense
 */

namespace GlobalFlow {
namespace DataProcessing {
namespace DataOutput {

template<typename T, typename Data>
T convert(Data d);

template<>
std::string convert(long d) { return std::to_string(d); }

template<>
std::string convert(unsigned long int d) { return std::to_string(d); }

template<>
std::string convert(std::string d) { return d; }

template<>
std::string convert(double d) { return std::to_string(d); }

template<>
std::string convert(bool d) { return std::to_string(d); }

template<>
std::string convert(std::pair<double, double> d) {
    return std::to_string(d.first) + "," + std::to_string(d.second);
}

template<>
double convert(double d) { return d; }

template<>
double convert(long d) { return d; }

template<>
double convert(unsigned long int d) { return d; }

template<>
double convert(bool d) { return d; }

template<>
std::pair<double, double> convert(std::pair<double, double> d) { return d; }

/**
 * They are obiously nonesense
 * FIXME
 */
template<>
double convert(std::pair<double, double> d) { return d.first; }

template<>
std::pair<double, double> convert(bool d) { return std::make_pair(d, d); }

template<>
std::pair<double, double> convert(double d) { return std::make_pair(d, d); }

template<>
std::pair<double, double> convert(long d) { return std::make_pair(d, d); }

template<>
std::pair<double, double> convert(unsigned long int d) { return std::make_pair(d, d); }

template<>
bool convert(long d) { return true; }

template<>
bool convert(unsigned long int d) { return true; }

template<>
bool convert(double d) { return true; }

template<>
bool convert(bool d) { return d; }

template<>
bool convert(std::pair<double, double> d) { return true; }
}
}
}
#endif