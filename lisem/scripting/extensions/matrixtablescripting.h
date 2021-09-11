#ifndef MATRIXTABLESCRIPTING_H
#define MATRIXTABLESCRIPTING_H


#include <angelscript.h>
#include "scriptmanager.h"
#include "matrixtable.h"
#include "table/tableanalysis.h"
#include "table/correlations.h"
#include "scriptarray.h"




/*r = engine->RegisterObjectMethod("Table", "Table& opAssign(const array<string> &m)", asMETHODPR(MatrixTable,AssignArrS,(CScriptArray *),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
r = engine->RegisterObjectMethod("Table", "Table& opAssign(const array<double> &m)", asMETHODPR(MatrixTable,AssignArrD,(CScriptArray *),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
r = engine->RegisterObjectMethod("Table", "Table& opAssign(const array<float> &m)", asMETHODPR(MatrixTable,AssignArrF,(CScriptArray *),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
r = engine->RegisterObjectMethod("Table", "Table& opAssign(const array<int> &m)", asMETHODPR(MatrixTable,AssignArrI,(CScriptArray *),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
r = engine->RegisterObjectMethod("Table", "Table& opAssign(const array<array<string>> &m)", asMETHODPR(MatrixTable,AssignArrSA,(CScriptArray *),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
r = engine->RegisterObjectMethod("Table", "Table& opAssign(const array<array<double>> &m)", asMETHODPR(MatrixTable,AssignArrDA,(CScriptArray *),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
r = engine->RegisterObjectMethod("Table", "Table& opAssign(const array<array<float>> &m)", asMETHODPR(MatrixTable,AssignArrFA,(CScriptArray *),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
r = engine->RegisterObjectMethod("Table", "Table& opAssign(const array<array<int>> &m)", asMETHODPR(MatrixTable,AssignArrIA,(CScriptArray *),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
*/


inline static MatrixTable * AssignArrS(CScriptArray* a)
{
    QList<QString> qa = a->ToQNPList<QString>();

    /*MatrixTable*target = this;
    if(writeonassign)
    {
        target = readfunc(FileName);
    }
    std::function<double(double,double)> powfuncd = [](double a, double b){return b;};
    std::function<int(int,int)> powfunci = [](int a, int b){return b;};
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return b;};
    ApplyOperatorToMatrixTables(target,s,powfunci,powfuncd,addfunc,true);

    target->UpdateParent();
    return target;*/

    return nullptr;
}
inline static MatrixTable * AssignArrD( CScriptArray* a)
{

    return nullptr;
}
inline static MatrixTable * AssignArrF(CScriptArray* a)
{

    return nullptr;
}
inline static MatrixTable * AssignArrI( CScriptArray* a)
{

    return nullptr;
}
inline static MatrixTable * AssignArrSA( CScriptArray* a)
{

    return nullptr;
}
inline static MatrixTable * AssignArrDA(CScriptArray* a)
{

    return nullptr;
}
inline static MatrixTable * AssignArrFA( CScriptArray* a)
{

    return nullptr;
}
inline static MatrixTable * AssignArrIA( CScriptArray* a)
{

    return nullptr;
}
inline void RegisterTableToScriptEngine(asIScriptEngine *engine)
{

    //register object type
    int r = engine->RegisterObjectType("Table",0,asOBJ_REF);//Shapes

    //register reference counting for garbage collecting
    r = engine->RegisterObjectBehaviour("Table",asBEHAVE_ADDREF,"void f()",asMETHOD(MatrixTable,AddRef),asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectBehaviour("Table",asBEHAVE_RELEASE,"void f()",asMETHOD(MatrixTable,ReleaseRef),asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opAssign(Table &in m)", asMETHODPR(MatrixTable,Assign,(MatrixTable*),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );

    //register constructors by using factory functions
    r = engine->RegisterObjectBehaviour("Table",asBEHAVE_FACTORY,"Table@ CSF0()",asFUNCTIONPR(TableFactory,(),MatrixTable *),asCALL_CDECL); assert( r >= 0 );

    //regiter member functions

    r = engine->RegisterObjectMethod("Table","Table &opIndex(int index)",asMETHODPR(MatrixTable,OpIndex,(int),MatrixTable *), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table","string &opIndex(int index, int index2)",asMETHODPR(MatrixTable,OpIndex,(int,int),QString *),asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table","void SetSize(int r, int c)",asMETHODPR(MatrixTable,SetSize,(int,int),void), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table","void SetSize(int r)",asMETHODPR(MatrixTable,SetSize,(int),void), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table","int Rows()",asMETHODPR(MatrixTable,GetSizeR,(void),int), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table","int Cols()",asMETHODPR(MatrixTable,GetSizeC,(void),int), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table","string &GetValue(int r,int c)",asMETHODPR(MatrixTable,GetValueString,(int,int),std::string*), asCALL_THISCALL); assert( r >= 0 );


    ////operators with tables
    r = engine->RegisterObjectMethod("Table", "Table& opNeg()", asMETHODPR(MatrixTable,OpNeg,(void),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opCom()", asMETHODPR(MatrixTable,OpCom,(void),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );


    r = engine->RegisterObjectMethod("Table", "Table@ opSub(Table &in s)", asMETHODPR(MatrixTable,OpSub,(MatrixTable *),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table@ opAdd(Table &in s)", asMETHODPR(MatrixTable,OpAdd,(MatrixTable *),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opMul(Table &in m)", asMETHODPR(MatrixTable,OpMul,(MatrixTable *),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opDiv(Table &in m)", asMETHODPR(MatrixTable,OpDiv,(MatrixTable *),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opPow(Table &in m)", asMETHODPR(MatrixTable,OpPow,(MatrixTable *),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opMod(Table &in m)", asMETHODPR(MatrixTable,OpMod,(MatrixTable *),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );

    r = engine->RegisterObjectMethod("Table", "Table& opAddAssign(Table &in m)", asMETHODPR(MatrixTable,OpAddAssign,(MatrixTable *),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opMulAssign(Table &in m)", asMETHODPR(MatrixTable,OpMulAssign,(MatrixTable *),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opSubAssign(Table &in m)", asMETHODPR(MatrixTable,OpSubAssign,(MatrixTable *),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opDivAssign(Table &in m)", asMETHODPR(MatrixTable,OpDivAssign,(MatrixTable *),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opPowAssign(Table &in m)", asMETHODPR(MatrixTable,OpPowAssign,(MatrixTable *),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opModAssign(Table &in m)", asMETHODPR(MatrixTable,OpModAssign,(MatrixTable *),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );

    r = engine->RegisterObjectMethod("Table", "Table& opAnd(Table &in m)", asMETHODPR(MatrixTable,And,(MatrixTable *),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opOr(Table &in m)", asMETHODPR(MatrixTable,Or,(MatrixTable *),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opXor(Table &in m)", asMETHODPR(MatrixTable,Xor,(MatrixTable *),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );

    r = engine->RegisterObjectMethod("Table", "Table& eq(Table &in m)", asMETHODPR(MatrixTable,EqualTo,(MatrixTable *),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& neq(Table &in m)", asMETHODPR(MatrixTable,NotEqualTo,(MatrixTable *),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opShl(Table &in m)", asMETHODPR(MatrixTable,SmallerThen,(MatrixTable *),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opShr(Table &in m)", asMETHODPR(MatrixTable,LargerThen,(MatrixTable *),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opShlAssign(Table &in m)", asMETHODPR(MatrixTable,SmallerEqualThen,(MatrixTable *),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opShrAssign(Table &in m)", asMETHODPR(MatrixTable,LargerEqualThen,(MatrixTable *),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );


    //operators with doubles
    r = engine->RegisterObjectMethod("Table", "Table@ opSub(double s)", asMETHODPR(MatrixTable,OpSub,(double),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table@ opAdd(double s)", asMETHODPR(MatrixTable,OpAdd,(double),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opMul(double m)", asMETHODPR(MatrixTable,OpMul,(double),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opDiv(double m)", asMETHODPR(MatrixTable,OpDiv,(double),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opPow(double m)", asMETHODPR(MatrixTable,OpPow,(double),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opMod(double m)", asMETHODPR(MatrixTable,OpMod,(double),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );

    r = engine->RegisterObjectMethod("Table", "Table& opAddAssign(double m)", asMETHODPR(MatrixTable,OpAddAssign,(double),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opMulAssign(double m)", asMETHODPR(MatrixTable,OpMulAssign,(double),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opSubAssign(double m)", asMETHODPR(MatrixTable,OpSubAssign,(double),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opDivAssign(double m)", asMETHODPR(MatrixTable,OpDivAssign,(double),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opPowAssign(double m)", asMETHODPR(MatrixTable,OpPowAssign,(double),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opModAssign(double m)", asMETHODPR(MatrixTable,OpModAssign,(double),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );

    r = engine->RegisterObjectMethod("Table", "Table& opAnd(double m)", asMETHODPR(MatrixTable,And,(double),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opOr(double m)", asMETHODPR(MatrixTable,Or,(double),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opXor(double m)", asMETHODPR(MatrixTable,Xor,(double),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );

    r = engine->RegisterObjectMethod("Table", "Table& eq(double m)", asMETHODPR(MatrixTable,EqualTo,(double),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& neq(double m)", asMETHODPR(MatrixTable,NotEqualTo,(double),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opShl(double m)", asMETHODPR(MatrixTable,SmallerThen,(double),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opShr(double m)", asMETHODPR(MatrixTable,LargerThen,(double),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opShlAssign(double m)", asMETHODPR(MatrixTable,SmallerEqualThen,(double),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opShrAssign(double m)", asMETHODPR(MatrixTable,LargerEqualThen,(double),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );

    r = engine->RegisterObjectMethod("Table", "Table@ opSub_r(double s)", asMETHODPR(MatrixTable,OpSub_r,(double),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table@ opAdd_r(double s)", asMETHODPR(MatrixTable,OpAdd_r,(double),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opMul_r(double m)", asMETHODPR(MatrixTable,OpMul_r,(double),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opDiv_r(double m)", asMETHODPR(MatrixTable,OpDiv_r,(double),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opPow_r(double m)", asMETHODPR(MatrixTable,OpPow_r,(double),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opMod_r(double m)", asMETHODPR(MatrixTable,OpMod_r,(double),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );

    r = engine->RegisterObjectMethod("Table", "Table& opAnd_r(double m)", asMETHODPR(MatrixTable,And_r,(double),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opOr_r(double m)", asMETHODPR(MatrixTable,Or_r,(double),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opXor_r(double m)", asMETHODPR(MatrixTable,Xor_r,(double),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );

    r = engine->RegisterObjectMethod("Table", "Table& eq_r(double m)", asMETHODPR(MatrixTable,EqualTo_r,(double),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& neq_r(double m)", asMETHODPR(MatrixTable,NotEqualTo_r,(double),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opShl_r(double m)", asMETHODPR(MatrixTable,SmallerThen_r,(double),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opShr_r(double m)", asMETHODPR(MatrixTable,LargerThen_r,(double),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );

    //operators with int
    r = engine->RegisterObjectMethod("Table", "Table@ opSub(int s)", asMETHODPR(MatrixTable,OpSub,(int),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table@ opAdd(int s)", asMETHODPR(MatrixTable,OpAdd,(int),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opMul(int m)", asMETHODPR(MatrixTable,OpMul,(int),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opDiv(int m)", asMETHODPR(MatrixTable,OpDiv,(int),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opPow(int m)", asMETHODPR(MatrixTable,OpPow,(int),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opMod(int m)", asMETHODPR(MatrixTable,OpMod,(int),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );

    r = engine->RegisterObjectMethod("Table", "Table& opAddAssign(int m)", asMETHODPR(MatrixTable,OpAddAssign,(int),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opMulAssign(int m)", asMETHODPR(MatrixTable,OpMulAssign,(int),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opSubAssign(int m)", asMETHODPR(MatrixTable,OpSubAssign,(int),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opDivAssign(int m)", asMETHODPR(MatrixTable,OpDivAssign,(int),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opPowAssign(int m)", asMETHODPR(MatrixTable,OpPowAssign,(int),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opModAssign(int m)", asMETHODPR(MatrixTable,OpModAssign,(int),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );

    r = engine->RegisterObjectMethod("Table", "Table& opAnd(int m)", asMETHODPR(MatrixTable,And,(int),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opOr(int m)", asMETHODPR(MatrixTable,Or,(int),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opXor(int m)", asMETHODPR(MatrixTable,Xor,(int),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );

    r = engine->RegisterObjectMethod("Table", "Table& eq(int m)", asMETHODPR(MatrixTable,EqualTo,(int),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& neq(int m)", asMETHODPR(MatrixTable,NotEqualTo,(int),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opShl(int m)", asMETHODPR(MatrixTable,SmallerThen,(int),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opShr(int m)", asMETHODPR(MatrixTable,LargerThen,(int),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opShlAssign(int m)", asMETHODPR(MatrixTable,SmallerEqualThen,(int),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opShrAssign(int m)", asMETHODPR(MatrixTable,LargerEqualThen,(int),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );

    //operators with int
    r = engine->RegisterObjectMethod("Table", "Table@ opSub_r(int s)", asMETHODPR(MatrixTable,OpSub_r,(int),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table@ opAdd_r(int s)", asMETHODPR(MatrixTable,OpAdd_r,(int),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opMul_r(int m)", asMETHODPR(MatrixTable,OpMul_r,(int),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opDiv_r(int m)", asMETHODPR(MatrixTable,OpDiv_r,(int),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opPow_r(int m)", asMETHODPR(MatrixTable,OpPow_r,(int),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opMod_r(int m)", asMETHODPR(MatrixTable,OpMod_r,(int),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );

    r = engine->RegisterObjectMethod("Table", "Table& opAnd_r(int m)", asMETHODPR(MatrixTable,And_r,(int),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opOr_r(int m)", asMETHODPR(MatrixTable,Or_r,(int),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opXor_r(int m)", asMETHODPR(MatrixTable,Xor_r,(int),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );

    r = engine->RegisterObjectMethod("Table", "Table& eq_r(int m)", asMETHODPR(MatrixTable,EqualTo_r,(int),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& neq_r(int m)", asMETHODPR(MatrixTable,NotEqualTo_r,(int),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opShl_r(int m)", asMETHODPR(MatrixTable,SmallerThen_r,(int),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opShr_r(int m)", asMETHODPR(MatrixTable,LargerThen_r,(int),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );

    //operators with string

    r = engine->RegisterObjectMethod("Table", "Table& opAssign(string m)", asMETHODPR(MatrixTable,Assign,(QString),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opAssign(double m)", asMETHODPR(MatrixTable,Assign,(double),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opAssign(int m)", asMETHODPR(MatrixTable,Assign,(int),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opAssign(const array<string> &m)", asFUNCTIONPR(AssignArrS,(CScriptArray *),MatrixTable*), asCALL_CDECL_OBJFIRST); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opAssign(const array<double> &m)", asFUNCTIONPR(AssignArrD,(CScriptArray *),MatrixTable*), asCALL_CDECL_OBJFIRST); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opAssign(const array<float> &m)", asFUNCTIONPR(AssignArrF,(CScriptArray *),MatrixTable*), asCALL_CDECL_OBJFIRST); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opAssign(const array<int> &m)", asFUNCTIONPR(AssignArrI,(CScriptArray *),MatrixTable*), asCALL_CDECL_OBJFIRST); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opAssign(const array<array<string>> &m)", asFUNCTIONPR(AssignArrSA,(CScriptArray *),MatrixTable*), asCALL_CDECL_OBJFIRST); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opAssign(const array<array<double>> &m)", asFUNCTIONPR(AssignArrDA,(CScriptArray *),MatrixTable*), asCALL_CDECL_OBJFIRST); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opAssign(const array<array<float>> &m)", asFUNCTIONPR(AssignArrFA,(CScriptArray *),MatrixTable*), asCALL_CDECL_OBJFIRST); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opAssign(const array<array<int>> &m)", asFUNCTIONPR(AssignArrIA,(CScriptArray *),MatrixTable*), asCALL_CDECL_OBJFIRST); assert( r >= 0 );

    r = engine->RegisterObjectMethod("Table", "Table@ opAdd(string s)", asMETHODPR(MatrixTable,OpAdd,(QString),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opAddAssign(string m)", asMETHODPR(MatrixTable,OpAddAssign,(QString),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );

    r = engine->RegisterObjectMethod("Table", "Table@ opAdd_r(string s)", asMETHODPR(MatrixTable,OpAdd_r,(QString),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );

    r = engine->RegisterObjectMethod("Table", "Table& eq(string m)", asMETHODPR(MatrixTable,EqualTo,(QString),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& neq(string m)", asMETHODPR(MatrixTable,NotEqualTo,(QString),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& eq_r(string m)", asMETHODPR(MatrixTable,EqualTo,(QString),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& neq_r(string m)", asMETHODPR(MatrixTable,NotEqualTo,(QString),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );



    //adding and removing columns and rows

    r = engine->RegisterObjectMethod("Table", "void AppendColumns(Table &in s)", asMETHODPR(MatrixTable,AppendColumns,(MatrixTable *),void), asCALL_THISCALL); assert( r >= 0 );

    //conversions
    r = engine->RegisterGlobalFunction("Table @ TToInt(Table& in table)",asFUNCTIONPR(TToInt,(MatrixTable*),MatrixTable*),asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Table @ TToDouble(Table& in table)",asFUNCTIONPR(TToDouble,(MatrixTable*),MatrixTable*),asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Table @ TToString(Table& in table)",asFUNCTIONPR(TToString,(MatrixTable*),MatrixTable*),asCALL_CDECL); assert( r >= 0 );

    //sorting
    r = engine->RegisterGlobalFunction("Table @ SortByColumn(Table& in table, int column = 0, bool descending = false)",asFUNCTIONPR(SortByColumn,(MatrixTable*,int,bool),MatrixTable*),asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Table @ SortColumn(Table& in table, int column = 0, bool descending = false)",asFUNCTIONPR(SortColumn,(MatrixTable*,int,bool),MatrixTable*),asCALL_CDECL); assert( r >= 0 );

    //selecting
    r = engine->RegisterGlobalFunction("Table @ GetColumn(Table& in table, int column = 0)",asFUNCTIONPR(TableGetColumn,(MatrixTable*, int column),MatrixTable*),asCALL_CDECL); assert( r >= 0 );


    //correlations
    r = engine->RegisterGlobalFunction("float PearsonCorrelation(Table& in table)",asFUNCTIONPR(TableCorrelationPearson,(MatrixTable*),float),asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("float SpearmanCorrelation(Table& in table)",asFUNCTIONPR(TableCorrelationSpearman,(MatrixTable*),float),asCALL_CDECL); assert( r >= 0 );

    //deleting rows and column
    r = engine->RegisterGlobalFunction("Table @ DropColumn(Table& in table, int index)",asFUNCTIONPR(DropColumn,(MatrixTable*,int),MatrixTable*),asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Table @ DropRow(Table& in table, int index)",asFUNCTIONPR(DropRow,(MatrixTable*,int),MatrixTable*),asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Table @ DropColumn(Table& in table, string index)",asFUNCTIONPR(DropColumn,(MatrixTable*,QString),MatrixTable*),asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Table @ DropRow(Table& in table, string index)",asFUNCTIONPR(DropRow,(MatrixTable*,QString),MatrixTable*),asCALL_CDECL); assert( r >= 0 );

    //get title name
    r = engine->RegisterObjectMethod("Table", "string ColumnTitle(int index)", asMETHODPR(MatrixTable,ColumnTitle,(int),QString), asCALL_THISCALL); assert( r >= 0 );

}


#endif // MATRIXTABLESCRIPTING_H
