#ifndef SCRIPTFUNCTIONALS_H
#define SCRIPTFUNCTIONALS_H

#include "lsmscriptengine.h"
#include "functionmap.h"
#include "mathfunction.h"


inline void RegisterFunctionalsToScriptEngine(asIScriptEngine *engine)
{

        engine->RegisterObjectType("MathExpression", sizeof(MathFunction), asOBJ_VALUE | asOBJ_POD| asGetTypeTraits<MathFunction>());
        engine->RegisterObjectMethod("MathExpression", "double Evaluate(array<double> &in inputs)",asMETHODPR(MathFunction,AS_Evaluate,(std::vector<double>),double),asCALL_THISCALL);
        engine->RegisterObjectMethod("MathExpression", "string GetExpression()",asMETHODPR(MathFunction,AS_GetExpression,(),QString),asCALL_THISCALL);

        engine->RegisterObjectBehaviour("MathExpression", asBEHAVE_CONSTRUCT, "void CSF1(string)", asFUNCTIONPR(AS_MathFunctionC1,(QString, void*),void), asCALL_CDECL_OBJLAST);
        engine->RegisterObjectBehaviour("MathExpression", asBEHAVE_CONSTRUCT, "void CSF0()", asFUNCTIONPR(AS_MathFunctionC0,(void*),void), asCALL_CDECL_OBJLAST);

        engine->RegisterObjectMethod("MathExpression", "MathExpression opAssign(string)", asMETHODPR(MathFunction,AS_OpAssign,(QString),MathFunction), asCALL_THISCALL);

        engine->RegisterGlobalFunction("Map @MapFunction(Map &in val, MathExpression m)", asFUNCTIONPR(AS_MapFunction,(cTMap*,MathFunction),cTMap*),asCALL_CDECL);
        engine->RegisterGlobalFunction("Map @MapFunction(array<Map> &in vals, MathExpression m)", asFUNCTIONPR(AS_MapFunction,(std::vector<cTMap*>,MathFunction),cTMap*),asCALL_CDECL);


}


#endif // SCRIPTFUNCTIONALS_H
