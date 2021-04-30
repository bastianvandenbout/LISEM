#ifndef GENERICNUMERICLISTABLE_H
#define GENERICNUMERICLISTABLE_H



class GenericNumericListable
{

    //these virtual functions are overloaded by cTMap (for a generic 2-d array) and MatrixTable (for 1-D array)
    inline virtual bool GenNumList_IsValid();
    inline virtual void GenNumList_GetValueCount();
    inline virtual bool GenNumList_GetMask(int ind);
    inline virtual double GenNumList_GetValue(int ind);
    inline virtual void GenNumList_SetValue(int index, double value);
};




#endif // GENERICNUMERICLISTABLE_H
