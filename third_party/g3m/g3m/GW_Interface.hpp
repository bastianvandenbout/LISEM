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

#ifndef GLOBAL_FLOW_GW_INTERFACE_H
#define GLOBAL_FLOW_GW_INTERFACE_H

#include <unordered_map>
#include "Simulation/Options.hpp"

namespace GlobalFlow {

/**
 * @class Container
 * @class Field
 * @brief Container for transferring data from GW-modell to WaterGAP
 * Where as get(Field) always returns a map from type <ArcID, value>
 *
 * Rivers:
 *
 * Lakes:
 *
 * Wetlands:
 */
    enum class Field {
        RIVER,
        LAKE,
        WETLANDS,
    };

    using namespace std;

    class Container {
    public:
        Container(unordered_map<int, double> rivers) : rivers(rivers) {}

        const unordered_map<int, double>
        get(Field field) {
            switch (field) {
                case Field::RIVER:
                    return rivers;
                default :
                    throw out_of_range("No such field");
            }
        }

    private:
        const unordered_map<int, double> rivers;
    };

    /**
     * @class Callback for container setting
     */
    class Callback {
    public:
        //default implementation does nothing
        void call(Container container) {}

        void operator()(Container container) { call(container); };
    };

    /**
     * @interface GW_Interface
     * @class GW_Interface
     * @brief Main interface to the groundwater model
     *
     * Interface to the groundwater simulation
     * Implement me!
     */
    class GW_Interface {
    public:
        virtual ~GW_Interface() {}

        /**
         * Read general simulation settings
         * e.g. Options
         */
        virtual void loadSettings() = 0;

        /**
         * Do additional work required for a running simulation
         */
        virtual void setupSimulation() = 0;

        /**
         * How should the data be written out
         * @deprecated
         */
        virtual void writeData() = 0;

        /**
         * Set up the calback for model coupling
         * Could be inefficient
         * @param callback a callback function
         */
        void setupCallBack(Callback callback) {
            this->callback = [&](Container container) { callback.call(container); };
        }

        /**
         * Simulate/Run the model
         */
        virtual void simulate() = 0;

    protected:
        std::function<void(Container container)> callback;
    };
}//ns
#endif //GW_INTERFACE_H
