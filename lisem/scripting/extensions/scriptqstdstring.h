//
// Script QString
//
// This function registers the QString type with AngelScript to be used as the default string type.
//
// The string type is registered as a value type, thus may have performance issues if a lot of 
// string operations are performed in the script. However, for relatively few operations, this should
// not cause any problem for most applications.
//

#ifndef SCRIPTQSTDSTRING_H
#define SCRIPTQSTDSTRING_H

#include <angelscript.h>
#include "QString"


void RegisterQStdString(asIScriptEngine *engine);



#endif
