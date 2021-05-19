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

#ifndef GLOBAL_FLOW_PHYSICALPROPERTIES_HPP
#define GLOBAL_FLOW_PHYSICALPROPERTIES_HPP

#include "Units.hpp"

namespace GlobalFlow {
namespace Model {

/**
 * This container holds physical properties of the groundwater nodes
 * It is intended to be a strogly typed container without flooding the node class with fields
 *
 * Inpired by https://jguegant.github.io/blogs/tech/thread-safe-multi-type-map.html
 */


struct DefaultProperty;

/**
 * @class PhysicalProperty
 * Actual container holding the physical propertie
 */
template<class Type, class Key = DefaultProperty>
class PhysicalProperty {
    protected:
        Type &
        __get() const {
            if (not initialized) {
                throw std::invalid_argument("Variable is not initialized");
            }
            return value_;
        }

        Type
        __get() {
            if (not initialized) {
                throw std::invalid_argument("Variable is not initialized");
            }
            return value_;
        }

        void
        __set(const Type &value) {
            value_ = value;
            initialized = true;
        }

        template<class... Args>
        void
        __emplace(Args &&... args) {
            value_ = Type(std::forward<Args>(args)...);
            initialized = true;
        }

    private:
        bool initialized{false};
        Type value_;
};

/**
 * @class PropertiyRepository
 * Type repository holding the properties
 */
template<class... PhysicalProperties>
class PropertyRepository : private PhysicalProperties ... {
    public:
        template<class Type, class Key = DefaultProperty>
        Type &
        get() const {
            static_assert(
                    std::is_base_of<PhysicalProperty<Type, Key>, PropertyRepository<PhysicalProperties...>>::value,
                    "Please ensure that this type or this key exists in this repository");
            return PhysicalProperty<Type, Key>::__get();
        }

        template<class Type, class Key = DefaultProperty>
        Type
        get() {
            static_assert(
                    std::is_base_of<PhysicalProperty<Type, Key>, PropertyRepository<PhysicalProperties...>>::value,
                    "Please ensure that this type or this key exists in this repository");
            return PhysicalProperty<Type, Key>::__get();
        }

        template<class Type, class Key = DefaultProperty>
        void
        set(const Type &value) {
            static_assert(
                    std::is_base_of<PhysicalProperty<Type, Key>, PropertyRepository<PhysicalProperties...>>::value,
                    "Please ensure that this type or this key exists in this repository");
            PhysicalProperty<Type, Key>::__set(value);
        }

        template<class Type, class Key = DefaultProperty, class... Args>
        void
        emplace(Args &&... args) {
            static_assert(
                    std::is_base_of<PhysicalProperty<Type, Key>, PropertyRepository<PhysicalProperties...>>::value,
                    "Please ensure that this type or this key exists in this repository");
            PhysicalProperty<Type, Key>::__emplace(std::forward<Args>(args)...);
        }

        template<class Type, class Key = DefaultProperty>
        void
        addTo(const Type &value) {
            static_assert(
                    std::is_base_of<PhysicalProperty<Type, Key>, PropertyRepository<PhysicalProperties...>>::value,
                    "Please ensure that this type or this key exists in this repository");
            PhysicalProperty<Type, Key>::__set(PhysicalProperty<Type, Key>::__get() + value);
        }
};

/**
 * Dummmy structs - only needed during compile time to determine type
 * Each represents a physical property
 */
struct ID;
struct ArcID;
struct Lat;
struct Lon;
struct Layer;
struct StepModifier;
struct Area;
struct VerticalSize;
struct Elevation; //Always elevation of upper celll boundary
struct TopElevation; //Store elevation of TOP layer
struct Slope;
struct EFolding;
struct Confinement;
struct K;
struct Anisotropy;
struct StepSize;
struct OUT;
struct IN;
struct Head;
struct EQHead;
struct HeadChange;
struct Head_TZero;
struct HeadChange_TZero;
struct SpecificYield;
struct SpecificStorage;
struct EdgeLenght;
struct SideSurface;
struct VolumeOfCell;

/**
 * Defintion of type and unit for each field
 */
using PhysicalProperties = PropertyRepository<
        PhysicalProperty<large_num, ID>,
        PhysicalProperty<large_num, ArcID>,
        PhysicalProperty<double, Lat>, //TODO use boost unit
        PhysicalProperty<double, Lon>,
        PhysicalProperty<int, Layer>,
        PhysicalProperty<t_dim, StepModifier>,
        PhysicalProperty<t_s_meter, Area>,
        PhysicalProperty<t_meter, VerticalSize>,
        PhysicalProperty<t_meter, Elevation>,
        PhysicalProperty<t_meter, TopElevation>,
        PhysicalProperty<t_dim, Slope>,
        PhysicalProperty<t_meter, EFolding>,
PhysicalProperty<bool, Confinement>,
        PhysicalProperty<t_vel, K>,
        PhysicalProperty<t_dim, Anisotropy>,
PhysicalProperty<quantity < d_time>, StepSize>,
PhysicalProperty<t_c_meter, OUT>,
PhysicalProperty<t_c_meter, IN>,
PhysicalProperty<t_meter, Head>,
PhysicalProperty<t_meter, EQHead>,
PhysicalProperty<t_meter, HeadChange>,
PhysicalProperty<t_meter, Head_TZero>,
PhysicalProperty<t_meter, HeadChange_TZero>,
PhysicalProperty<t_dim, SpecificYield>,
PhysicalProperty<quantity < perUnit>, SpecificStorage>,
PhysicalProperty<t_meter, EdgeLenght>,
PhysicalProperty<t_s_meter, SideSurface>,
PhysicalProperty<t_c_meter, VolumeOfCell>
>;

}
}//ns
#endif
