#ifndef PYLISEM_OBJECTS_H
#define PYLISEM_OBJECTS_H

#include "pybind11/pybind11.h"
namespace py = pybind11;

#include "matrixtable.h"
#include "table/correlations.h"
#include "table/tableanalysis.h"
inline void init_pylisem_objects(py::module &m)
{
    py::class_<MatrixTable>(m,"Table")
      .def(py::init(py::overload_cast<>(&TableFactory)))
      .def("__copy__",[](const MatrixTable * A){return new MatrixTable(*A);})
      .def("__getitem__",[](MatrixTable * A, int r){return A->GetValueStringQRef(r,0);},py::is_operator())
      .def("__getitem__",[](MatrixTable * A, int r, int c){return A->GetValueStringQRef(r,c);},py::is_operator())
      .def("__setitem__",[](MatrixTable * A, int r, QString value){return A->SetValue(r,value);},py::is_operator())
      .def("__setitem__",[](MatrixTable * A, int r, int c, QString value){return A->SetValue(r,c,value);},py::is_operator())
    .def("__add__",[](MatrixTable*A,MatrixTable *B){return A->OpAdd(B);},py::is_operator())
    .def("__sub__",[](MatrixTable*A,MatrixTable *B){return A->OpSub(B);},py::is_operator())
    .def("__mul__",[](MatrixTable*A,MatrixTable *B){return A->OpMul(B);},py::is_operator())
    .def("__pow__",[](MatrixTable*A,MatrixTable *B){return A->OpPow(B);},py::is_operator())
    .def("__truediv__",[](MatrixTable*A,MatrixTable *B){return A->OpDiv(B);},py::is_operator())
    //.def("__floordiv__",[](MatrixTable*A,MatrixTable *B){return A->OpAdd(B);},py::is_operator())
    .def("__mod__",[](MatrixTable*A,MatrixTable *B){return A->OpMod(B);},py::is_operator())
    .def("__and__",[](MatrixTable*A,MatrixTable *B){return A->And(B);},py::is_operator())
    .def("__or__",[](MatrixTable*A,MatrixTable *B){return A->Or(B);},py::is_operator())
    .def("__xor__",[](MatrixTable*A,MatrixTable *B){return A->Xor(B);},py::is_operator())
    .def("__invert__",[](MatrixTable*A){return A->OpNeg();},py::is_operator())
    .def("__lt__",[](MatrixTable*A,MatrixTable *B){return A->SmallerThen(B);},py::is_operator())
    .def("__le__",[](MatrixTable*A,MatrixTable *B){return A->SmallerEqualThen(B);},py::is_operator())
    .def("__eq__",[](MatrixTable*A,MatrixTable *B){return A->EqualTo(B);},py::is_operator())
    .def("__ne__",[](MatrixTable*A,MatrixTable *B){return A->NotEqualTo(B);},py::is_operator())
    .def("__gt__",[](MatrixTable*A,MatrixTable *B){return A->LargerThen(B);},py::is_operator())
    .def("__ge__",[](MatrixTable*A,MatrixTable *B){return A->LargerEqualThen(B);},py::is_operator())

    .def("__add__",[](MatrixTable*A,double B){return A->OpAdd(B);},py::is_operator())
    .def("__sub__",[](MatrixTable*A,double B){return A->OpSub(B);},py::is_operator())
    .def("__mul__",[](MatrixTable*A,double B){return A->OpMul(B);},py::is_operator())
    .def("__pow__",[](MatrixTable*A,double B){return A->OpPow(B);},py::is_operator())
    .def("__truediv__",[](MatrixTable*A,double B){return A->OpDiv(B);},py::is_operator())
    //.def("__floordiv__",[](MatrixTable*A,double B){return A->OpAdd(B);},py::is_operator())
    .def("__mod__",[](MatrixTable*A,double B){return A->OpMod(B);},py::is_operator())
    .def("__and__",[](MatrixTable*A,double B){return A->And(B);},py::is_operator())
    .def("__or__",[](MatrixTable*A,double B){return A->Or(B);},py::is_operator())
    .def("__xor__",[](MatrixTable*A,double B){return A->Xor(B);},py::is_operator())
    .def("__lt__",[](MatrixTable*A,double B){return A->SmallerThen(B);},py::is_operator())
    .def("__le__",[](MatrixTable*A,double B){return A->SmallerEqualThen(B);},py::is_operator())
    .def("__eq__",[](MatrixTable*A,double B){return A->EqualTo(B);},py::is_operator())
    .def("__ne__",[](MatrixTable*A,double B){return A->NotEqualTo(B);},py::is_operator())
    .def("__gt__",[](MatrixTable*A,double B){return A->LargerThen(B);},py::is_operator())
    .def("__ge__",[](MatrixTable*A,double B){return A->LargerEqualThen(B);},py::is_operator())

    .def("__add__",[](double B,MatrixTable* A){return A->OpAdd_r(B);},py::is_operator())
    .def("__sub__",[](double B,MatrixTable* A){return A->OpSub_r(B);},py::is_operator())
    .def("__mul__",[](double B,MatrixTable* A){return A->OpMul_r(B);},py::is_operator())
    .def("__pow__",[](double B,MatrixTable* A){return A->OpPow_r(B);},py::is_operator())
    .def("__truediv__",[](double B,MatrixTable* A){return A->OpDiv_r(B);},py::is_operator())
    //.def("__floordiv__",[](double B,MatrixTable* A){return A->OpAdd(B);},py::is_operator())
    .def("__mod__",[](double B,MatrixTable* A){return A->OpMod_r(B);},py::is_operator())
    .def("__and__",[](double B,MatrixTable* A){return A->And_r(B);},py::is_operator())
    .def("__or__",[](double B,MatrixTable* A){return A->Or_r(B);},py::is_operator())
    .def("__xor__",[](double B,MatrixTable* A){return A->Xor_r(B);},py::is_operator())
    .def("__lt__",[](double B,MatrixTable* A){return A->SmallerThen_r(B);},py::is_operator())
    .def("__le__",[](double B,MatrixTable* A){return A->SmallerEqualThen_r(B);},py::is_operator())
    .def("__eq__",[](double B,MatrixTable* A){return A->EqualTo_r(B);},py::is_operator())
    .def("__ne__",[](double B,MatrixTable* A){return A->NotEqualTo_r(B);},py::is_operator())
    .def("__gt__",[](double B,MatrixTable* A){return A->LargerThen_r(B);},py::is_operator())
    .def("__ge__",[](double B,MatrixTable* A){return A->LargerEqualThen_r(B);},py::is_operator())


    .def("__add__",[](MatrixTable*A,int B){return A->OpAdd(B);},py::is_operator())
    .def("__sub__",[](MatrixTable*A,int B){return A->OpSub(B);},py::is_operator())
    .def("__mul__",[](MatrixTable*A,int B){return A->OpMul(B);},py::is_operator())
    .def("__pow__",[](MatrixTable*A,int B){return A->OpPow(B);},py::is_operator())
    .def("__truediv__",[](MatrixTable*A,int B){return A->OpDiv(B);},py::is_operator())
    //.def("__floordiv__",[](MatrixTable*A,int B){return A->OpAdd(B);},py::is_operator())
    .def("__mod__",[](MatrixTable*A,int B){return A->OpMod(B);},py::is_operator())
    .def("__and__",[](MatrixTable*A,int B){return A->And(B);},py::is_operator())
    .def("__or__",[](MatrixTable*A,int B){return A->Or(B);},py::is_operator())
    .def("__xor__",[](MatrixTable*A,int B){return A->Xor(B);},py::is_operator())
    .def("__lt__",[](MatrixTable*A,int B){return A->SmallerThen(B);},py::is_operator())
    .def("__le__",[](MatrixTable*A,int B){return A->SmallerEqualThen(B);},py::is_operator())
    .def("__eq__",[](MatrixTable*A,int B){return A->EqualTo(B);},py::is_operator())
    .def("__ne__",[](MatrixTable*A,int B){return A->NotEqualTo(B);},py::is_operator())
    .def("__gt__",[](MatrixTable*A,int B){return A->LargerThen(B);},py::is_operator())
    .def("__ge__",[](MatrixTable*A,int B){return A->LargerEqualThen(B);},py::is_operator())

    .def("__add__",[](int B,MatrixTable* A){return A->OpAdd_r(B);},py::is_operator())
    .def("__sub__",[](int B,MatrixTable* A){return A->OpSub_r(B);},py::is_operator())
    .def("__mul__",[](int B,MatrixTable* A){return A->OpMul_r(B);},py::is_operator())
    .def("__pow__",[](int B,MatrixTable* A){return A->OpPow_r(B);},py::is_operator())
    .def("__truediv__",[](int B,MatrixTable* A){return A->OpDiv_r(B);},py::is_operator())
    //.def("__floordiv__",[](int B,MatrixTable* A){return A->OpAdd(B);},py::is_operator())
    .def("__mod__",[](int B,MatrixTable* A){return A->OpMod_r(B);},py::is_operator())
    .def("__and__",[](int B,MatrixTable* A){return A->And_r(B);},py::is_operator())
    .def("__or__",[](int B,MatrixTable* A){return A->Or_r(B);},py::is_operator())
    .def("__xor__",[](int B,MatrixTable* A){return A->Xor_r(B);},py::is_operator())
    .def("__lt__",[](int B,MatrixTable* A){return A->SmallerThen_r(B);},py::is_operator())
    .def("__le__",[](int B,MatrixTable* A){return A->SmallerEqualThen_r(B);},py::is_operator())
    .def("__eq__",[](int B,MatrixTable* A){return A->EqualTo_r(B);},py::is_operator())
    .def("__ne__",[](int B,MatrixTable* A){return A->NotEqualTo_r(B);},py::is_operator())
    .def("__gt__",[](int B,MatrixTable* A){return A->LargerThen_r(B);},py::is_operator())
    .def("__ge__",[](int B,MatrixTable* A){return A->LargerEqualThen_r(B);},py::is_operator())

    .def("__eq__",[](QString B,MatrixTable* A){return A->EqualTo_r(B);},py::is_operator())
    .def("__ne__",[](QString B,MatrixTable* A){return A->NotEqualTo_r(B);},py::is_operator())
    .def("__eq__",[](MatrixTable* A,QString B){return A->EqualTo_r(B);},py::is_operator())
    .def("__ne__",[](MatrixTable* A, QString B){return A->NotEqualTo_r(B);},py::is_operator())
    .def("__add__",[](QString B,MatrixTable* A){return A->EqualTo_r(B);},py::is_operator())
    .def("__add__",[](MatrixTable* A, QString B){return A->NotEqualTo_r(B);},py::is_operator())

    .def("SetSize",py::overload_cast<int,int>(&MatrixTable::SetSize),py::arg("Rows"),py::arg("Cols"))
    .def("SetSize",py::overload_cast<int>(&MatrixTable::SetSize),py::arg("Rows"))
    .def("GetRows",&MatrixTable::GetSizeR)
    .def("GetCols",&MatrixTable::GetSizeC)
    .def("GetColumnTitle",&MatrixTable::GetColumnTitle)
    .def("SetColumnTitle",py::overload_cast<int,QString>(&MatrixTable::SetColumnTitle))
    .def("AppendColumns",&MatrixTable::AppendColumns)
    .def("InsertColumn",py::overload_cast<int>(&MatrixTable::AddColumn))
    .def("GetColumn",&MatrixTable::GetColumn)
    .def("ReplaceColumn",py::overload_cast<int,MatrixTable*>(&MatrixTable::ReplaceColumn))

    .def("Value",&MatrixTable::GetValueStringQ)
    .def("__getitem__",py::overload_cast<int,int>(&MatrixTable::OpIndex))
    .def("__getitem__",py::overload_cast<int>(&MatrixTable::OpIndex))
    .def("__setitem__",py::overload_cast<int,int, QString>(&MatrixTable::OpIndexSet))
    .def("__setitem__",py::overload_cast<int,MatrixTable*>(&MatrixTable::OpIndexSet));

    m.def("GetColumn",&TableGetColumn, py::arg("Table"),py::arg("Column"));

    m.def("DropColumn",py::overload_cast<MatrixTable*,int>(&DropColumn),py::arg("Table"),py::arg("Column"));
    m.def("DropRow",py::overload_cast<MatrixTable*,int>(&DropRow),py::arg("Table"),py::arg("Row"));
    m.def("DropColumn",py::overload_cast<MatrixTable*,QString>(&DropColumn),py::arg("Table"),py::arg("Column"));
    m.def("DropRow",py::overload_cast<MatrixTable*,QString>(&DropRow),py::arg("Table"),py::arg("Row"));

    m.def("PearsonCorrelation",&TableCorrelationPearson);
    m.def("SpearmanCorrelation",&TableCorrelationSpearman);
    m.def("TableTypeToInt",&TToInt);
    m.def("TableTypeToDouble",&TToDouble);
    m.def("TableTypeToString",&TToString);

    m.def("SortColumn",py::overload_cast<MatrixTable*,int,bool>(&SortColumn),py::arg("Table"),py::arg("Column"),py::arg("Descending") = false);
    m.def("SortByColumn",py::overload_cast<MatrixTable*,int,bool>(&SortByColumn),py::arg("Table"),py::arg("Column"),py::arg("Descending") = false);




}


#endif // PYLISEM_OBJECTS_H
