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

#ifndef GLOBAL_FLOW_UNITS_HPP
#define GLOBAL_FLOW_UNITS_HPP

#include <boost/units/unit.hpp>
#include <boost/units/quantity.hpp>
#include <boost/units/systems/si.hpp>
#include <boost/units/systems/si/base.hpp>
#include <boost/units/base_units/metric/day.hpp>
#include <boost/units/cmath.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/void_cast.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/unique_ptr.hpp>
#include <boost/serialization/shared_ptr.hpp>

namespace GlobalFlow {
namespace Model {

/**
 * Base unit for all calculations is DAY
 * For e.g. monthly it gets automatically modified by unitless modifier
 */
using namespace boost::units;
typedef make_system<metric::day_base_unit>::type dm_system;
typedef unit<time_dimension, dm_system> d_time;
BOOST_UNITS_STATIC_CONSTANT(day, d_time);

using Meter=si::length;
typedef derived_dimension<length_dimension, -1>::type perUnit_dimension;
typedef unit<perUnit_dimension, si::system> perUnit;
BOOST_UNITS_STATIC_CONSTANT(perMeter, perUnit);

using Velocity=divide_typeof_helper<Meter, d_time>::type;
using SquareMeter=si::area;
using CubicMeter=si::volume;
using MeterSquaredPerTime=divide_typeof_helper<SquareMeter, d_time>::type;
using VolumePerTime=divide_typeof_helper<CubicMeter, d_time>::type;
using Dimensionless=si::dimensionless;
using t_meter = quantity<Meter>;
using t_dim = quantity<Dimensionless>;
using t_vel = quantity<Velocity>;
using t_vol_t = quantity<VolumePerTime>;
using t_c_meter = quantity<CubicMeter>;
using t_s_meter_t = quantity<MeterSquaredPerTime>;
using t_s_meter = quantity<SquareMeter>;

using large_num = unsigned long int;

}
}
#endif //_HPP
