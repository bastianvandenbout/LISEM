#ifndef SCRIPTDATETIME_H
#define SCRIPTDATETIME_H

#ifndef ANGELSCRIPT_H 
// Avoid having to inform include path if header is already include before
#include <angelscript.h>
#endif

#ifdef AS_CAN_USE_CPP11
#include <chrono>
#else
#error Sorry, this requires C++11 which your compiler doesnt appear to support
#endif

BEGIN_AS_NAMESPACE

class CDateTime
{
public:
	// Constructors
	CDateTime();
	CDateTime(const CDateTime &other);
	CDateTime(asUINT year, asUINT month, asUINT day, asUINT hour, asUINT minute, asUINT second);

	// Copy the stored value from another any object
	CDateTime &operator=(const CDateTime &other);

	// Accessors
	asUINT getYear() const;
	asUINT getMonth() const;
	asUINT getDay() const;
	asUINT getHour() const;
	asUINT getMinute() const;
	asUINT getSecond() const;

	// Setters
	// Returns true if valid
	bool setDate(asUINT year, asUINT month, asUINT day);
	bool setTime(asUINT hour, asUINT minute, asUINT second);

	// Operators
	// Return difference in seconds
	asINT64          operator-(const CDateTime &other) const;
	CDateTime        operator+(asINT64 seconds) const;
	friend CDateTime operator+(asINT64 seconds, const CDateTime &other);
	CDateTime &      operator+=(asINT64 seconds);
	CDateTime        operator-(asINT64 seconds) const;
	friend CDateTime operator-(asINT64 seconds, const CDateTime &other);
	CDateTime &      operator-=(asINT64 seconds);
	bool             operator==(const CDateTime &other) const;
	bool             operator<(const CDateTime &other) const;

protected:
	std::chrono::system_clock::time_point tp;
};

extern std::chrono::time_point<std::chrono::high_resolution_clock> StartTime;

static inline double GetTimeInSeconds()
{
    return (static_cast<double>(std::chrono::duration_cast<std::chrono::nanoseconds>(
                (std::chrono::system_clock::now()-StartTime)).count()))*(0.000000001);

}
#include <thread>
static inline void AS_Sleep(double seconds)
{
    std::this_thread::sleep_for(std::chrono::microseconds((unsigned int)(seconds * 1000000.0)));

}


void RegisterScriptDateTime(asIScriptEngine *engine);

END_AS_NAMESPACE

#endif
