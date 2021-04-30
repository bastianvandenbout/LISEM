#pragma once

#include "optimization/leastsquares.h"
#include "lsmscriptengine.h"
#include "stat/mlmodel.h"
#include "stat/logisticregression.h"

//a smapp api for this library

inline void RegisterStatisticsToScriptEngine(asIScriptEngine *engine)
{

    engine->RegisterGlobalFunction("MathExpression MapLogisticRegression(array<Map> &in covariates,Map &in labels)",asFUNCTIONPR(MapLogisticRegression,(std::vector<cTMap*>,cTMap*),MathFunction),asCALL_CDECL);
    engine->RegisterGlobalFunction("MathExpression MapLinearRegression(array<Map> &in covariates,Map &in labels)",asFUNCTIONPR(MapLinearRegression,(std::vector<cTMap*>,cTMap*),MathFunction),asCALL_CDECL);


    engine->RegisterGlobalFunction("Map @NormalizeMap(Map &in m)",asFUNCTIONPR(AS_NormalizeMap,(cTMap*),cTMap*),asCALL_CDECL);
}
