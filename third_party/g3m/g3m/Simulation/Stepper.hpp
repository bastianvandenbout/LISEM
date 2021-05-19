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

#ifndef GLOBAL_FLOW_STEPPER_HPP
#define GLOBAL_FLOW_STEPPER_HPP

#include "../Solver/Equation.hpp"

namespace GlobalFlow {
namespace Simulation {

enum TimeFrame {
    DAY = 1,
    MONTH = 30,
    YEAR = 360
};

typedef std::pair<Solver::Equation *, size_t> step;

/**
 * @class AbstractStepper An iterator in order to iterate simply over simulation steps
 * Holds a pointer to the equation and the choosen stepsize
 */
class AbstractStepper {
    public:
        virtual Solver::Equation *
        get(int col) const = 0;
};

/**
 * @class Iterator The internal iterator holding the current simulation step
 */
class Iterator {
    public:
        Iterator(const AbstractStepper *stepper, TimeFrame time, int pos)
                : _pos(pos), _stepper(stepper), _time(time) {}

        bool
        operator!=(const Iterator &other) const {
            return _pos != other._pos;
        }

        step
        operator*() const {
            return step(_stepper->get(0), _pos);
        };

        const Iterator &
        operator++() {
            ++_pos;
            return *this;
        }

        int
        getPos() { return _pos; }

    private:
        int _pos;
        const AbstractStepper *_stepper;
        const TimeFrame _time;
};

/**
 * @class Stepper An instance holding the simulation iterator
 */
class Stepper : public AbstractStepper {
    public:

        Stepper(Solver::Equation *eq, const TimeFrame time, const size_t steps)
                : _equation(eq), _timeFrame(time), _steps(steps) {
            _equation->updateStepSize(_timeFrame);
        }

        virtual Solver::Equation *
        get(int col) const {
            return _equation.get();
        }

        Iterator
        begin() const {
            return Iterator(this, _timeFrame, 0);
        }

        Iterator
        end() const {
            return Iterator(this, _timeFrame, this->_steps);
        }

        const TimeFrame
        getStepSize() {
            return _timeFrame;
        };

    private:
        std::shared_ptr<Solver::Equation> _equation;
        const TimeFrame _timeFrame;
        const size_t _steps;
};

}
}//ns
#endif
