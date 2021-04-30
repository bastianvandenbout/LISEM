#include "matrixtable.h"
#include "tablealgebra.h"
#include "math.h"

#define LISEM_ASMAP_BOOLFROMDOUBLE(x) (x>0.5?true:false)
#define LISEM_ASMAP_BOOLFROMINT(x) (x > 0?true:false)
#define LISEM_ASMAP_BOOLFROMSTRING(x) (x.compare("True",Qt::CaseInsensitive) == 0? true: (x.compare("False",Qt::CaseInsensitive) == 0? false: (x.toInt() > 0)))


MatrixTable*        MatrixTable::OpNeg             ()
{

    std::function<QString(QString,QString)> addfunc = [](QString a, QString /*b*/){return a;};
    return ApplyOperatorToMatrixTables(this,-1,std::multiplies<int>(),std::multiplies<double>(),addfunc);
}


MatrixTable*        MatrixTable::OpCom            ()
{

    std::function<double(double,double)> powfuncd = [](double a, double b){return (!(LISEM_ASMAP_BOOLFROMDOUBLE(a)))? 1.0f:0.0f;};
    std::function<int(int,int)> powfunci = [](int a, int b){return ((!LISEM_ASMAP_BOOLFROMINT(a)))? 1.0f:0.0f;};
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return ((!LISEM_ASMAP_BOOLFROMSTRING(a)))? "True":"False";};
    return ApplyOperatorToMatrixTables(this,"1",powfunci,powfuncd,addfunc);
}

MatrixTable*        MatrixTable::OpAdd             (MatrixTable*other)
{

    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return a+b;};
    return ApplyOperatorToMatrixTables(this,other,std::plus<int>(),std::plus<double>(),addfunc);
}

MatrixTable*        MatrixTable::OpMul             (MatrixTable*other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString /*b*/){return a;};
    return ApplyOperatorToMatrixTables(this,other,std::multiplies<int>(),std::multiplies<double>(),addfunc);
}


MatrixTable*        MatrixTable::OpSub             (MatrixTable*other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString /*b*/){return a;};
    return ApplyOperatorToMatrixTables(this,other,std::minus<int>(),std::minus<double>(),addfunc);
}

MatrixTable*        MatrixTable::OpDiv             (MatrixTable*other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString /*b*/){return a;};
    return ApplyOperatorToMatrixTables(this,other,std::divides<int>(),std::divides<double>(),addfunc);
}

MatrixTable*        MatrixTable::OpPow             (MatrixTable*other)
{

    std::function<QString(QString,QString)> addfunc = [](QString a, QString /*b*/){return a;};
    std::function<double(double,double)> powfuncd = [](double a, double b){return std::pow(a,b);};
    std::function<int(int,int)> powfunci = [](int a, int b){return std::pow(a,b);};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);
}
MatrixTable*        MatrixTable::OpMod             (MatrixTable*other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString /*b*/){return a;};
    std::function<double(double,double)> powfuncd = [](double a, double b){return std::fmod(a,b);};
    std::function<int(int,int)> powfunci = [](int a, int b){return a%b;};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);

}


MatrixTable*        MatrixTable::EqualTo           (MatrixTable*other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return a == b? "1":"0";};
    std::function<double(double,double)> powfuncd = [](double a, double b){return a == b;};
    std::function<int(int,int)> powfunci = [](int a, int b){return a == b;};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);
}
MatrixTable*        MatrixTable::LargerThen        (MatrixTable*other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return a > b? "1":"0";};
    std::function<double(double,double)> powfuncd = [](double a, double b){return a > b;};
    std::function<int(int,int)> powfunci = [](int a, int b){return a > b;};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);
}
MatrixTable*        MatrixTable::SmallerThen       (MatrixTable*other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return a < b? "1":"0";};
    std::function<double(double,double)> powfuncd = [](double a, double b){return a < b;};
    std::function<int(int,int)> powfunci = [](int a, int b){return a < b;};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);

}
MatrixTable*        MatrixTable::NotEqualTo        (MatrixTable*other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return a != b? "1":"0";};
    std::function<double(double,double)> powfuncd = [](double a, double b){return a != b;};
    std::function<int(int,int)> powfunci = [](int a, int b){return a != b;};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);

}
MatrixTable*        MatrixTable::LargerEqualThen   (MatrixTable*other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return a >= b? "1":"0";};
    std::function<double(double,double)> powfuncd = [](double a, double b){return a >= b;};
    std::function<int(int,int)> powfunci = [](int a, int b){return a >= b;};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);

}
MatrixTable*        MatrixTable::SmallerEqualThen  (MatrixTable*other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return a <= b? "1":"0";};
    std::function<double(double,double)> powfuncd = [](double a, double b){return a <= b;};
    std::function<int(int,int)> powfunci = [](int a, int b){return a <= b;};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);

}

MatrixTable*        MatrixTable::And               (MatrixTable*other)
{
    std::function<double(double,double)> powfuncd = [](double a, double b){return (LISEM_ASMAP_BOOLFROMDOUBLE(a) && LISEM_ASMAP_BOOLFROMDOUBLE(b))? 1.0f:0.0f;};
    std::function<int(int,int)> powfunci = [](int a, int b){return (LISEM_ASMAP_BOOLFROMINT(a) && LISEM_ASMAP_BOOLFROMINT(b))? 1.0f:0.0f;};
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return (LISEM_ASMAP_BOOLFROMSTRING(a) && LISEM_ASMAP_BOOLFROMSTRING(b))? "True":"False";};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);
}
MatrixTable*        MatrixTable::Or                (MatrixTable*other)
{
    std::function<double(double,double)> powfuncd = [](double a, double b){return (LISEM_ASMAP_BOOLFROMDOUBLE(a) || LISEM_ASMAP_BOOLFROMDOUBLE(b))? 1.0f:0.0f;};
    std::function<int(int,int)> powfunci = [](int a, int b){return (LISEM_ASMAP_BOOLFROMINT(a) || LISEM_ASMAP_BOOLFROMINT(b))? 1.0f:0.0f;};
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return (LISEM_ASMAP_BOOLFROMSTRING(a) || LISEM_ASMAP_BOOLFROMSTRING(b))? "True":"False";};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);

}
MatrixTable*        MatrixTable::Xor               (MatrixTable*other)
{
    std::function<double(double,double)> powfuncd = [](double a, double b){return (LISEM_ASMAP_BOOLFROMDOUBLE(a) ^ LISEM_ASMAP_BOOLFROMDOUBLE(b))? 1.0f:0.0f;};
    std::function<int(int,int)> powfunci = [](int a, int b){return (LISEM_ASMAP_BOOLFROMINT(a) ^ LISEM_ASMAP_BOOLFROMINT(b))? 1.0f:0.0f;};
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return (LISEM_ASMAP_BOOLFROMSTRING(a) ^ LISEM_ASMAP_BOOLFROMSTRING(b))? "True":"False";};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);
}

MatrixTable*        MatrixTable::OpAddAssign       (MatrixTable*other)
{
    MatrixTable*target = this;
    if(writeonassign)
    {
        target = readfunc(FileName);
    }

    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return a+b;};
    ApplyOperatorToMatrixTables(target,other,std::plus<int>(),std::plus<double>(),addfunc,true);

    target->UpdateParent();
    return target;
}

MatrixTable*        MatrixTable::OpSubAssign       (MatrixTable*other)
{
    MatrixTable*target = this;
    if(writeonassign)
    {
        target = readfunc(FileName);
    }

    std::function<QString(QString,QString)> addfunc = [](QString a, QString /*b*/){return a;};
    ApplyOperatorToMatrixTables(target,other,std::minus<int>(),std::minus<double>(),addfunc,true);

    target->UpdateParent();
    return target;

}
MatrixTable*        MatrixTable::OpMulAssign       (MatrixTable*other)
{
    MatrixTable*target = this;
    if(writeonassign)
    {
        target = readfunc(FileName);
    }

    std::function<QString(QString,QString)> addfunc = [](QString a, QString /*b*/){return a;};
    ApplyOperatorToMatrixTables(target,other,std::multiplies<int>(),std::multiplies<double>(),addfunc,true);

    target->UpdateParent();
    return target;

}
MatrixTable*        MatrixTable::OpDivAssign       (MatrixTable*other)
{
    MatrixTable*target = this;
    if(writeonassign)
    {
        target = readfunc(FileName);
    }

    std::function<QString(QString,QString)> addfunc = [](QString a, QString /*b*/){return a;};
    ApplyOperatorToMatrixTables(target,other,std::divides<int>(),std::divides<double>(),addfunc,true);

    target->UpdateParent();
    return target;

}
MatrixTable*        MatrixTable::OpPowAssign       (MatrixTable*other)
{
    MatrixTable*target = this;
    if(writeonassign)
    {
        target = readfunc(FileName);
    }
    std::function<double(double,double)> powfuncd = [](double a, double b){return std::pow(a,b);};
    std::function<int(int,int)> powfunci = [](int a, int b){return std::pow(a,b);};
    std::function<QString(QString,QString)> addfunc = [](QString a, QString /*b*/){return a;};
    ApplyOperatorToMatrixTables(target,other,powfunci,powfuncd,addfunc,true);

    target->UpdateParent();
    return target;

}
MatrixTable*        MatrixTable::OpModAssign       (MatrixTable*other)
{
    MatrixTable*target = this;
    if(writeonassign)
    {
        target = readfunc(FileName);
    }

    std::function<QString(QString,QString)> addfunc = [](QString a, QString /*b*/){return a;};
    std::function<double(double,double)> powfuncd = [](double a, double b){return std::fmod(a,b);};
    std::function<int(int,int)> powfunci = [](int a, int b){return a%b;};
    ApplyOperatorToMatrixTables(target,other,powfunci,powfuncd,addfunc,true);

    target->UpdateParent();
    return target;
}


MatrixTable*        MatrixTable::AndAssign         (MatrixTable*other)
{
    MatrixTable*target = this;
    if(writeonassign)
    {
        target = readfunc(FileName);
    }
    std::function<double(double,double)> powfuncd = [](double a, double b){return (LISEM_ASMAP_BOOLFROMDOUBLE(a) && LISEM_ASMAP_BOOLFROMDOUBLE(b))? 1.0f:0.0f;};
    std::function<int(int,int)> powfunci = [](int a, int b){return (LISEM_ASMAP_BOOLFROMINT(a) && LISEM_ASMAP_BOOLFROMINT(b))? 1.0f:0.0f;};
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return (LISEM_ASMAP_BOOLFROMSTRING(a) && LISEM_ASMAP_BOOLFROMSTRING(b))? "True":"False";};
    ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);

    target->UpdateParent();
    return target;

}
MatrixTable*        MatrixTable::OrAssign          (MatrixTable*other)
{
    MatrixTable*target = this;
    if(writeonassign)
    {
        target = readfunc(FileName);
    }
    std::function<double(double,double)> powfuncd = [](double a, double b){return (LISEM_ASMAP_BOOLFROMDOUBLE(a) || LISEM_ASMAP_BOOLFROMDOUBLE(b))? 1.0f:0.0f;};
    std::function<int(int,int)> powfunci = [](int a, int b){return (LISEM_ASMAP_BOOLFROMINT(a) || LISEM_ASMAP_BOOLFROMINT(b))? 1.0f:0.0f;};
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return (LISEM_ASMAP_BOOLFROMSTRING(a) || LISEM_ASMAP_BOOLFROMSTRING(b))? "True":"False";};
    ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);

    target->UpdateParent();
    return target;
}
MatrixTable*        MatrixTable::XorAssign         (MatrixTable*other)
{
    MatrixTable*target = this;
    if(writeonassign)
    {
        target = readfunc(FileName);
    }
    std::function<double(double,double)> powfuncd = [](double a, double b){return (LISEM_ASMAP_BOOLFROMDOUBLE(a) ^ LISEM_ASMAP_BOOLFROMDOUBLE(b))? 1.0f:0.0f;};
    std::function<int(int,int)> powfunci = [](int a, int b){return (LISEM_ASMAP_BOOLFROMINT(a) ^ LISEM_ASMAP_BOOLFROMINT(b))? 1.0f:0.0f;};
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return (LISEM_ASMAP_BOOLFROMSTRING(a) ^ LISEM_ASMAP_BOOLFROMSTRING(b))? "True":"False";};
    ApplyOperatorToMatrixTables(target,other,powfunci,powfuncd,addfunc,true);

    target->UpdateParent();
    return target;
}




MatrixTable*        MatrixTable::OpAdd             (double other)
{

    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return a+b;};
    return ApplyOperatorToMatrixTables(this,other,std::plus<int>(),std::plus<double>(),addfunc);
}

MatrixTable*        MatrixTable::OpMul             (double other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString /*b*/){return a;};
    return ApplyOperatorToMatrixTables(this,other,std::multiplies<int>(),std::multiplies<double>(),addfunc);
}


MatrixTable*        MatrixTable::OpSub             (double other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString /*b*/){return a;};
    return ApplyOperatorToMatrixTables(this,other,std::minus<int>(),std::minus<double>(),addfunc);
}

MatrixTable*        MatrixTable::OpDiv             (double other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString /*b*/){return a;};
    return ApplyOperatorToMatrixTables(this,other,std::divides<int>(),std::divides<double>(),addfunc);
}

MatrixTable*        MatrixTable::OpPow             (double other)
{

    std::function<QString(QString,QString)> addfunc = [](QString a, QString /*b*/){return a;};
    std::function<double(double,double)> powfuncd = [](double a, double b){return std::pow(a,b);};
    std::function<int(int,int)> powfunci = [](int a, int b){return std::pow(a,b);};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);
}
MatrixTable*        MatrixTable::OpMod             (double other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString /*b*/){return a;};
    std::function<double(double,double)> powfuncd = [](double a, double b){return std::fmod(a,b);};
    std::function<int(int,int)> powfunci = [](int a, int b){return a%b;};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);

}


MatrixTable*        MatrixTable::EqualTo           (double other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return a == b? "1":"0";};
    std::function<double(double,double)> powfuncd = [](double a, double b){return a == b;};
    std::function<int(int,int)> powfunci = [](int a, int b){return a == b;};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);
}
MatrixTable*        MatrixTable::LargerThen        (double other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return a > b? "1":"0";};
    std::function<double(double,double)> powfuncd = [](double a, double b){return a > b;};
    std::function<int(int,int)> powfunci = [](int a, int b){return a > b;};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);
}
MatrixTable*        MatrixTable::SmallerThen       (double other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return a < b? "1":"0";};
    std::function<double(double,double)> powfuncd = [](double a, double b){return a < b;};
    std::function<int(int,int)> powfunci = [](int a, int b){return a < b;};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);

}
MatrixTable*        MatrixTable::NotEqualTo        (double other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return a != b? "1":"0";};
    std::function<double(double,double)> powfuncd = [](double a, double b){return a != b;};
    std::function<int(int,int)> powfunci = [](int a, int b){return a != b;};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);

}
MatrixTable*        MatrixTable::LargerEqualThen   (double other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return a >= b? "1":"0";};
    std::function<double(double,double)> powfuncd = [](double a, double b){return a >= b;};
    std::function<int(int,int)> powfunci = [](int a, int b){return a >= b;};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);

}
MatrixTable*        MatrixTable::SmallerEqualThen  (double other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return a <= b? "1":"0";};
    std::function<double(double,double)> powfuncd = [](double a, double b){return a <= b;};
    std::function<int(int,int)> powfunci = [](int a, int b){return a <= b;};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);

}

MatrixTable*        MatrixTable::And               (double other)
{
    std::function<double(double,double)> powfuncd = [](double a, double b){return (LISEM_ASMAP_BOOLFROMDOUBLE(a) && LISEM_ASMAP_BOOLFROMDOUBLE(b))? 1.0f:0.0f;};
    std::function<int(int,int)> powfunci = [](int a, int b){return (LISEM_ASMAP_BOOLFROMINT(a) && LISEM_ASMAP_BOOLFROMINT(b))? 1.0f:0.0f;};
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return (LISEM_ASMAP_BOOLFROMSTRING(a) && LISEM_ASMAP_BOOLFROMSTRING(b))? "True":"False";};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);
}
MatrixTable*        MatrixTable::Or                (double other)
{
    std::function<double(double,double)> powfuncd = [](double a, double b){return (LISEM_ASMAP_BOOLFROMDOUBLE(a) || LISEM_ASMAP_BOOLFROMDOUBLE(b))? 1.0f:0.0f;};
    std::function<int(int,int)> powfunci = [](int a, int b){return (LISEM_ASMAP_BOOLFROMINT(a) || LISEM_ASMAP_BOOLFROMINT(b))? 1.0f:0.0f;};
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return (LISEM_ASMAP_BOOLFROMSTRING(a) || LISEM_ASMAP_BOOLFROMSTRING(b))? "True":"False";};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);

}
MatrixTable*        MatrixTable::Xor               (double other)
{
    std::function<double(double,double)> powfuncd = [](double a, double b){return (LISEM_ASMAP_BOOLFROMDOUBLE(a) ^ LISEM_ASMAP_BOOLFROMDOUBLE(b))? 1.0f:0.0f;};
    std::function<int(int,int)> powfunci = [](int a, int b){return (LISEM_ASMAP_BOOLFROMINT(a) ^ LISEM_ASMAP_BOOLFROMINT(b))? 1.0f:0.0f;};
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return (LISEM_ASMAP_BOOLFROMSTRING(a) ^ LISEM_ASMAP_BOOLFROMSTRING(b))? "True":"False";};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);
}

MatrixTable*        MatrixTable::OpAddAssign       (double other)
{
    MatrixTable*target = this;
    if(writeonassign)
    {
        target = readfunc(FileName);
    }

    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return a+b;};
    ApplyOperatorToMatrixTables(target,other,std::plus<int>(),std::plus<double>(),addfunc,true);

    target->UpdateParent();
    return target;
}

MatrixTable*        MatrixTable::OpSubAssign       (double other)
{
    MatrixTable*target = this;
    if(writeonassign)
    {
        target = readfunc(FileName);
    }

    std::function<QString(QString,QString)> addfunc = [](QString a, QString /*b*/){return a;};
    ApplyOperatorToMatrixTables(target,other,std::minus<int>(),std::minus<double>(),addfunc,true);

    target->UpdateParent();
    return target;

}
MatrixTable*        MatrixTable::OpMulAssign       (double other)
{
    MatrixTable*target = this;
    if(writeonassign)
    {
        target = readfunc(FileName);
    }

    std::function<QString(QString,QString)> addfunc = [](QString a, QString /*b*/){return a;};
    ApplyOperatorToMatrixTables(target,other,std::multiplies<int>(),std::multiplies<double>(),addfunc,true);

    target->UpdateParent();
    return target;

}
MatrixTable*        MatrixTable::OpDivAssign       (double other)
{
    MatrixTable*target = this;
    if(writeonassign)
    {
        target = readfunc(FileName);
    }

    std::function<QString(QString,QString)> addfunc = [](QString a, QString /*b*/){return a;};
    ApplyOperatorToMatrixTables(target,other,std::divides<int>(),std::divides<double>(),addfunc,true);

    target->UpdateParent();
    return target;

}
MatrixTable*        MatrixTable::OpPowAssign       (double other)
{
    MatrixTable*target = this;
    if(writeonassign)
    {
        target = readfunc(FileName);
    }
    std::function<double(double,double)> powfuncd = [](double a, double b){return std::pow(a,b);};
    std::function<int(int,int)> powfunci = [](int a, int b){return std::pow(a,b);};
    std::function<QString(QString,QString)> addfunc = [](QString a, QString /*b*/){return a;};
    ApplyOperatorToMatrixTables(target,other,powfunci,powfuncd,addfunc,true);

    target->UpdateParent();
    return target;

}
MatrixTable*        MatrixTable::OpModAssign       (double  other)
{
    MatrixTable*target = this;
    if(writeonassign)
    {
        target = readfunc(FileName);
    }

    std::function<QString(QString,QString)> addfunc = [](QString a, QString /*b*/){return a;};
    std::function<double(double,double)> powfuncd = [](double a, double b){return std::fmod(a,b);};
    std::function<int(int,int)> powfunci = [](int a, int b){return a%b;};
    ApplyOperatorToMatrixTables(target,other,powfunci,powfuncd,addfunc,true);

    target->UpdateParent();
    return target;
}


MatrixTable*        MatrixTable::AndAssign         (double  other)
{
    MatrixTable*target = this;
    if(writeonassign)
    {
        target = readfunc(FileName);
    }
    std::function<double(double,double)> powfuncd = [](double a, double b){return (LISEM_ASMAP_BOOLFROMDOUBLE(a) && LISEM_ASMAP_BOOLFROMDOUBLE(b))? 1.0f:0.0f;};
    std::function<int(int,int)> powfunci = [](int a, int b){return (LISEM_ASMAP_BOOLFROMINT(a) && LISEM_ASMAP_BOOLFROMINT(b))? 1.0f:0.0f;};
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return (LISEM_ASMAP_BOOLFROMSTRING(a) && LISEM_ASMAP_BOOLFROMSTRING(b))? "True":"False";};
    ApplyOperatorToMatrixTables(target,other,powfunci,powfuncd,addfunc);

    target->UpdateParent();
    return target;

}
MatrixTable*        MatrixTable::OrAssign          (double other)
{
    MatrixTable*target = this;
    if(writeonassign)
    {
        target = readfunc(FileName);
    }
    std::function<double(double,double)> powfuncd = [](double a, double b){return (LISEM_ASMAP_BOOLFROMDOUBLE(a) || LISEM_ASMAP_BOOLFROMDOUBLE(b))? 1.0f:0.0f;};
    std::function<int(int,int)> powfunci = [](int a, int b){return (LISEM_ASMAP_BOOLFROMINT(a) || LISEM_ASMAP_BOOLFROMINT(b))? 1.0f:0.0f;};
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return (LISEM_ASMAP_BOOLFROMSTRING(a) || LISEM_ASMAP_BOOLFROMSTRING(b))? "True":"False";};
    ApplyOperatorToMatrixTables(target,other,powfunci,powfuncd,addfunc);

    target->UpdateParent();
    return target;
}
MatrixTable*        MatrixTable::XorAssign         (double other)
{
    MatrixTable*target = this;
    if(writeonassign)
    {
        target = readfunc(FileName);
    }
    std::function<double(double,double)> powfuncd = [](double a, double b){return (LISEM_ASMAP_BOOLFROMDOUBLE(a) ^ LISEM_ASMAP_BOOLFROMDOUBLE(b))? 1.0f:0.0f;};
    std::function<int(int,int)> powfunci = [](int a, int b){return (LISEM_ASMAP_BOOLFROMINT(a) ^ LISEM_ASMAP_BOOLFROMINT(b))? 1.0f:0.0f;};
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return (LISEM_ASMAP_BOOLFROMSTRING(a) ^ LISEM_ASMAP_BOOLFROMSTRING(b))? "True":"False";};
    ApplyOperatorToMatrixTables(target,other,powfunci,powfuncd,addfunc,true);

    target->UpdateParent();
    return target;
}




MatrixTable*        MatrixTable::OpAdd             (int other)
{

    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return a+b;};
    return ApplyOperatorToMatrixTables(this,other,std::plus<int>(),std::plus<double>(),addfunc);
}

MatrixTable*        MatrixTable::OpMul             (int other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString /*b*/){return a;};
    return ApplyOperatorToMatrixTables(this,other,std::multiplies<int>(),std::multiplies<double>(),addfunc);
}


MatrixTable*        MatrixTable::OpSub             (int other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString /*b*/){return a;};
    return ApplyOperatorToMatrixTables(this,other,std::minus<int>(),std::minus<double>(),addfunc);
}

MatrixTable*        MatrixTable::OpDiv             (int other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString /*b*/){return a;};
    return ApplyOperatorToMatrixTables(this,other,std::divides<int>(),std::divides<double>(),addfunc);
}

MatrixTable*        MatrixTable::OpPow             (int other)
{

    std::function<QString(QString,QString)> addfunc = [](QString a, QString /*b*/){return a;};
    std::function<double(double,double)> powfuncd = [](double a, double b){return std::pow(a,b);};
    std::function<int(int,int)> powfunci = [](int a, int b){return std::pow(a,b);};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);
}
MatrixTable*        MatrixTable::OpMod             (int other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString /*b*/){return a;};
    std::function<double(double,double)> powfuncd = [](double a, double b){return std::fmod(a,b);};
    std::function<int(int,int)> powfunci = [](int a, int b){return a%b;};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);

}


MatrixTable*        MatrixTable::EqualTo           (int other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return a == b? "1":"0";};
    std::function<double(double,double)> powfuncd = [](double a, double b){return a == b;};
    std::function<int(int,int)> powfunci = [](int a, int b){return a == b;};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);
}
MatrixTable*        MatrixTable::LargerThen        (int other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return a > b? "1":"0";};
    std::function<double(double,double)> powfuncd = [](double a, double b){return a > b;};
    std::function<int(int,int)> powfunci = [](int a, int b){return a > b;};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);
}
MatrixTable*        MatrixTable::SmallerThen       (int other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return a < b? "1":"0";};
    std::function<double(double,double)> powfuncd = [](double a, double b){return a < b;};
    std::function<int(int,int)> powfunci = [](int a, int b){return a < b;};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);

}
MatrixTable*        MatrixTable::NotEqualTo        (int other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return a != b? "1":"0";};
    std::function<double(double,double)> powfuncd = [](double a, double b){return a != b;};
    std::function<int(int,int)> powfunci = [](int a, int b){return a != b;};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);

}
MatrixTable*        MatrixTable::LargerEqualThen   (int other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return a >= b? "1":"0";};
    std::function<double(double,double)> powfuncd = [](double a, double b){return a >= b;};
    std::function<int(int,int)> powfunci = [](int a, int b){return a >= b;};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);

}
MatrixTable*        MatrixTable::SmallerEqualThen  (int other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return a <= b? "1":"0";};
    std::function<double(double,double)> powfuncd = [](double a, double b){return a <= b;};
    std::function<int(int,int)> powfunci = [](int a, int b){return a <= b;};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);

}

MatrixTable*        MatrixTable::And               (int other)
{
    std::function<double(double,double)> powfuncd = [](double a, double b){return (LISEM_ASMAP_BOOLFROMDOUBLE(a) && LISEM_ASMAP_BOOLFROMDOUBLE(b))? 1.0f:0.0f;};
    std::function<int(int,int)> powfunci = [](int a, int b){return (LISEM_ASMAP_BOOLFROMINT(a) && LISEM_ASMAP_BOOLFROMINT(b))? 1.0f:0.0f;};
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return (LISEM_ASMAP_BOOLFROMSTRING(a) && LISEM_ASMAP_BOOLFROMSTRING(b))? "True":"False";};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);
}
MatrixTable*        MatrixTable::Or                (int other)
{
    std::function<double(double,double)> powfuncd = [](double a, double b){return (LISEM_ASMAP_BOOLFROMDOUBLE(a) || LISEM_ASMAP_BOOLFROMDOUBLE(b))? 1.0f:0.0f;};
    std::function<int(int,int)> powfunci = [](int a, int b){return (LISEM_ASMAP_BOOLFROMINT(a) || LISEM_ASMAP_BOOLFROMINT(b))? 1.0f:0.0f;};
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return (LISEM_ASMAP_BOOLFROMSTRING(a) || LISEM_ASMAP_BOOLFROMSTRING(b))? "True":"False";};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);

}
MatrixTable*        MatrixTable::Xor               (int other)
{
    std::function<double(double,double)> powfuncd = [](double a, double b){return (LISEM_ASMAP_BOOLFROMDOUBLE(a) ^ LISEM_ASMAP_BOOLFROMDOUBLE(b))? 1.0f:0.0f;};
    std::function<int(int,int)> powfunci = [](int a, int b){return (LISEM_ASMAP_BOOLFROMINT(a) ^ LISEM_ASMAP_BOOLFROMINT(b))? 1.0f:0.0f;};
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return (LISEM_ASMAP_BOOLFROMSTRING(a) ^ LISEM_ASMAP_BOOLFROMSTRING(b))? "True":"False";};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);
}

MatrixTable*        MatrixTable::OpAddAssign       (int other)
{
    MatrixTable*target = this;
    if(writeonassign)
    {
        target = readfunc(FileName);
    }

    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return a+b;};
    ApplyOperatorToMatrixTables(target,other,std::plus<int>(),std::plus<double>(),addfunc,true);

    target->UpdateParent();
    return target;
}

MatrixTable*        MatrixTable::OpSubAssign       (int other)
{
    MatrixTable*target = this;
    if(writeonassign)
    {
        target = readfunc(FileName);
    }

    std::function<QString(QString,QString)> addfunc = [](QString a, QString /*b*/){return a;};
    ApplyOperatorToMatrixTables(target,other,std::minus<int>(),std::minus<double>(),addfunc,true);

    target->UpdateParent();
    return target;

}
MatrixTable*        MatrixTable::OpMulAssign       (int other)
{
    MatrixTable*target = this;
    if(writeonassign)
    {
        target = readfunc(FileName);
    }

    std::function<QString(QString,QString)> addfunc = [](QString a, QString /*b*/){return a;};
    ApplyOperatorToMatrixTables(target,other,std::multiplies<int>(),std::multiplies<double>(),addfunc,true);

    target->UpdateParent();
    return target;

}
MatrixTable*        MatrixTable::OpDivAssign       (int other)
{
    MatrixTable*target = this;
    if(writeonassign)
    {
        target = readfunc(FileName);
    }

    std::function<QString(QString,QString)> addfunc = [](QString a, QString /*b*/){return a;};
    ApplyOperatorToMatrixTables(target,other,std::divides<int>(),std::divides<double>(),addfunc,true);

    target->UpdateParent();
    return target;

}
MatrixTable*        MatrixTable::OpPowAssign       (int other)
{
    MatrixTable*target = this;
    if(writeonassign)
    {
        target = readfunc(FileName);
    }
    std::function<double(double,double)> powfuncd = [](double a, double b){return std::pow(a,b);};
    std::function<int(int,int)> powfunci = [](int a, int b){return std::pow(a,b);};
    std::function<QString(QString,QString)> addfunc = [](QString a, QString /*b*/){return a;};
    ApplyOperatorToMatrixTables(target,other,powfunci,powfuncd,addfunc,true);

    target->UpdateParent();
    return target;

}
MatrixTable*        MatrixTable::OpModAssign       (int other)
{
    MatrixTable*target = this;
    if(writeonassign)
    {
        target = readfunc(FileName);
    }

    std::function<QString(QString,QString)> addfunc = [](QString a, QString /*b*/){return a;};
    std::function<double(double,double)> powfuncd = [](double a, double b){return std::fmod(a,b);};
    std::function<int(int,int)> powfunci = [](int a, int b){return a%b;};
    ApplyOperatorToMatrixTables(target,other,powfunci,powfuncd,addfunc,true);

    target->UpdateParent();
    return target;
}


MatrixTable*        MatrixTable::AndAssign         (int other)
{
    MatrixTable*target = this;
    if(writeonassign)
    {
        target = readfunc(FileName);
    }
    std::function<double(double,double)> powfuncd = [](double a, double b){return (LISEM_ASMAP_BOOLFROMDOUBLE(a) && LISEM_ASMAP_BOOLFROMDOUBLE(b))? 1.0f:0.0f;};
    std::function<int(int,int)> powfunci = [](int a, int b){return (LISEM_ASMAP_BOOLFROMINT(a) && LISEM_ASMAP_BOOLFROMINT(b))? 1.0f:0.0f;};
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return (LISEM_ASMAP_BOOLFROMSTRING(a) && LISEM_ASMAP_BOOLFROMSTRING(b))? "True":"False";};
    ApplyOperatorToMatrixTables(target,other,powfunci,powfuncd,addfunc);

    target->UpdateParent();
    return target;

}
MatrixTable*        MatrixTable::OrAssign          (int other)
{
    MatrixTable*target = this;
    if(writeonassign)
    {
        target = readfunc(FileName);
    }
    std::function<double(double,double)> powfuncd = [](double a, double b){return (LISEM_ASMAP_BOOLFROMDOUBLE(a) || LISEM_ASMAP_BOOLFROMDOUBLE(b))? 1.0f:0.0f;};
    std::function<int(int,int)> powfunci = [](int a, int b){return (LISEM_ASMAP_BOOLFROMINT(a) || LISEM_ASMAP_BOOLFROMINT(b))? 1.0f:0.0f;};
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return (LISEM_ASMAP_BOOLFROMSTRING(a) || LISEM_ASMAP_BOOLFROMSTRING(b))? "True":"False";};
    ApplyOperatorToMatrixTables(target,other,powfunci,powfuncd,addfunc);

    target->UpdateParent();
    return target;
}
MatrixTable*        MatrixTable::XorAssign         (int other)
{
    MatrixTable*target = this;
    if(writeonassign)
    {
        target = readfunc(FileName);
    }
    std::function<double(double,double)> powfuncd = [](double a, double b){return (LISEM_ASMAP_BOOLFROMDOUBLE(a) ^ LISEM_ASMAP_BOOLFROMDOUBLE(b))? 1.0f:0.0f;};
    std::function<int(int,int)> powfunci = [](int a, int b){return (LISEM_ASMAP_BOOLFROMINT(a) ^ LISEM_ASMAP_BOOLFROMINT(b))? 1.0f:0.0f;};
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return (LISEM_ASMAP_BOOLFROMSTRING(a) ^ LISEM_ASMAP_BOOLFROMSTRING(b))? "True":"False";};
    ApplyOperatorToMatrixTables(target,other,powfunci,powfuncd,addfunc,true);

    target->UpdateParent();
    return target;
}



MatrixTable*        MatrixTable::Assign            (double other)
{
    MatrixTable*target = this;
    if(writeonassign)
    {
        target = readfunc(FileName);
    }
    std::function<double(double,double)> powfuncd = [](double a, double b){return b;};
    std::function<int(int,int)> powfunci = [](int a, int b){return b;};
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return b;};
    ApplyOperatorToMatrixTables(target,other,powfunci,powfuncd,addfunc,true);

    target->UpdateParent();
    return target;

}
MatrixTable*        MatrixTable::Assign            (int other)
{
    MatrixTable*target = this;
    if(writeonassign)
    {
        target = readfunc(FileName);
    }
    std::function<double(double,double)> powfuncd = [](double a, double b){return b;};
    std::function<int(int,int)> powfunci = [](int a, int b){return b;};
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return b;};
    ApplyOperatorToMatrixTables(target,other,powfunci,powfuncd,addfunc,true);

    target->UpdateParent();
    return target;

}
MatrixTable*        MatrixTable::Assign            (QString s)
{
    MatrixTable*target = this;
    if(writeonassign)
    {
        target = readfunc(FileName);
    }
    std::function<double(double,double)> powfuncd = [](double a, double b){return b;};
    std::function<int(int,int)> powfunci = [](int a, int b){return b;};
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return b;};
    ApplyOperatorToMatrixTables(target,s,powfunci,powfuncd,addfunc,true);

    target->UpdateParent();
    return target;

}

MatrixTable*        MatrixTable::OpAdd             (QString s)
{
    return ApplyOperatorToMatrixTables(this,s,std::plus<int>(),std::plus<double>(),std::plus<QString>());
}
MatrixTable*        MatrixTable::OpAddAssign       (QString s)
{
    MatrixTable*target = this;
    if(writeonassign)
    {
        target = readfunc(FileName);
    }
    ApplyOperatorToMatrixTables(this,s,std::plus<int>(),std::plus<double>(),std::plus<QString>(),true);

    target->UpdateParent();
    return target;
}
MatrixTable*        MatrixTable::EqualTo           (QString s)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return a == b? "1":"0";};
    std::function<double(double,double)> powfuncd = [](double a, double b){return a == b;};
    std::function<int(int,int)> powfunci = [](int a, int b){return a == b;};
    return ApplyOperatorToMatrixTables(this,s,powfunci,powfuncd,addfunc);
}



MatrixTable*        MatrixTable::OpAdd_r             (QString s)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return b+a;};
    std::function<double(double,double)> powfuncd = [](double a, double b){return b+a;};
    std::function<int(int,int)> powfunci = [](int a, int b){return b+a;};
    return ApplyOperatorToMatrixTables(this,s,powfunci,powfuncd,addfunc);
}

MatrixTable*        MatrixTable::EqualTo_r           (QString s)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return a == b? "1":"0";};
    std::function<double(double,double)> powfuncd = [](double a, double b){return a == b;};
    std::function<int(int,int)> powfunci = [](int a, int b){return a == b;};
    return ApplyOperatorToMatrixTables(this,s,powfunci,powfuncd,addfunc);

}

MatrixTable*        MatrixTable::NotEqualTo_r           (QString s)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return a != b? "1":"0";};
    std::function<double(double,double)> powfuncd = [](double a, double b){return a != b;};
    std::function<int(int,int)> powfunci = [](int a, int b){return a != b;};
    return ApplyOperatorToMatrixTables(this,s,powfunci,powfuncd,addfunc);

}

MatrixTable*        MatrixTable::NotEqualTo           (QString s)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return a != b? "1":"0";};
    std::function<double(double,double)> powfuncd = [](double a, double b){return a != b;};
    std::function<int(int,int)> powfunci = [](int a, int b){return a != b;};
    return ApplyOperatorToMatrixTables(this,s,powfunci,powfuncd,addfunc);

}



MatrixTable *        MatrixTable::OpMod_r           (double other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString /*b*/){return a;};
    std::function<double(double,double)> powfuncd = [](double a, double b){return std::fmod(b,a);};
    std::function<int(int,int)> powfunci = [](int a, int b){return b%a;};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);


}


MatrixTable *        MatrixTable::OpAdd_r           (double other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return b+a;};
    std::function<double(double,double)> powfuncd = [](double a, double b){return b+a;};
    std::function<int(int,int)> powfunci = [](int a, int b){return b+a;};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);

}


MatrixTable *        MatrixTable::OpMul_r           (double other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString /*b*/){return a;};
    std::function<double(double,double)> powfuncd = [](double a, double b){return b*a;};
    std::function<int(int,int)> powfunci = [](int a, int b){return b*a;};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);

}


MatrixTable *        MatrixTable::OpSub_r           (double other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString /*b*/){return a;};
    std::function<double(double,double)> powfuncd = [](double a, double b){return b-a;};
    std::function<int(int,int)> powfunci = [](int a, int b){return b-a;};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);


}


MatrixTable *        MatrixTable::OpDiv_r           (double other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString /*b*/){return a;};
    std::function<double(double,double)> powfuncd = [](double a, double b){return b/a;};
    std::function<int(int,int)> powfunci = [](int a, int b){return b/a;};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);


}


MatrixTable *        MatrixTable::OpPow_r           (double other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString /*b*/){return a;};
    std::function<double(double,double)> powfuncd = [](double a, double b){return std::pow(b,a);};
    std::function<int(int,int)> powfunci = [](int a, int b){return std::pow(b,a);};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);


}


MatrixTable *        MatrixTable::LargerThen_r      (double other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return a > b? "1":"0";};
    std::function<double(double,double)> powfuncd = [](double a, double b){return a > b;};
    std::function<int(int,int)> powfunci = [](int a, int b){return a > b;};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);
}

MatrixTable *        MatrixTable::SmallerThen_r     (double other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return a < b? "1":"0";};
    std::function<double(double,double)> powfuncd = [](double a, double b){return a < b;};
    std::function<int(int,int)> powfunci = [](int a, int b){return a < b;};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);
}


MatrixTable *        MatrixTable::NotEqualTo_r      (double other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return a != b? "1":"0";};
    std::function<double(double,double)> powfuncd = [](double a, double b){return a != b;};
    std::function<int(int,int)> powfunci = [](int a, int b){return a != b;};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);
}

MatrixTable *        MatrixTable::LargerEqualThen_r (double other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return a <= b? "1":"0";};
    std::function<double(double,double)> powfuncd = [](double a, double b){return a <= b;};
    std::function<int(int,int)> powfunci = [](int a, int b){return a <= b;};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);
}

MatrixTable *        MatrixTable::SmallerEqualThen_r(double other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return a <= b? "1":"0";};
    std::function<double(double,double)> powfuncd = [](double a, double b){return a <= b;};
    std::function<int(int,int)> powfunci = [](int a, int b){return a <= b;};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);
}

MatrixTable *        MatrixTable::EqualTo_r         (double other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return a == b? "1":"0";};
    std::function<double(double,double)> powfuncd = [](double a, double b){return a == b;};
    std::function<int(int,int)> powfunci = [](int a, int b){return a == b;};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);
}

MatrixTable *        MatrixTable::And_r             (double other)
{
    std::function<double(double,double)> powfuncd = [](double a, double b){return (LISEM_ASMAP_BOOLFROMDOUBLE(b) && LISEM_ASMAP_BOOLFROMDOUBLE(a))? 1.0f:0.0f;};
    std::function<int(int,int)> powfunci = [](int a, int b){return (LISEM_ASMAP_BOOLFROMINT(b) && LISEM_ASMAP_BOOLFROMINT(a))? 1.0f:0.0f;};
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return (LISEM_ASMAP_BOOLFROMSTRING(b) && LISEM_ASMAP_BOOLFROMSTRING(a))? "True":"False";};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);

}

MatrixTable *        MatrixTable::Or_r              (double other)
{
    std::function<double(double,double)> powfuncd = [](double a, double b){return (LISEM_ASMAP_BOOLFROMDOUBLE(b) || LISEM_ASMAP_BOOLFROMDOUBLE(a))? 1.0f:0.0f;};
    std::function<int(int,int)> powfunci = [](int a, int b){return (LISEM_ASMAP_BOOLFROMINT(b) || LISEM_ASMAP_BOOLFROMINT(a))? 1.0f:0.0f;};
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return (LISEM_ASMAP_BOOLFROMSTRING(b) || LISEM_ASMAP_BOOLFROMSTRING(a))? "True":"False";};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);
}

MatrixTable *        MatrixTable::Xor_r             (double other)
{
    std::function<double(double,double)> powfuncd = [](double a, double b){return (LISEM_ASMAP_BOOLFROMDOUBLE(b) ^ LISEM_ASMAP_BOOLFROMDOUBLE(a))? 1.0f:0.0f;};
    std::function<int(int,int)> powfunci = [](int a, int b){return (LISEM_ASMAP_BOOLFROMINT(b) ^ LISEM_ASMAP_BOOLFROMINT(a))? 1.0f:0.0f;};
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return (LISEM_ASMAP_BOOLFROMSTRING(b) ^ LISEM_ASMAP_BOOLFROMSTRING(a))? "True":"False";};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);

}





MatrixTable *        MatrixTable::OpMod_r           (int other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString /*b*/){return a;};
    std::function<double(double,double)> powfuncd = [](double a, double b){return std::fmod(b,a);};
    std::function<int(int,int)> powfunci = [](int a, int b){return b%a;};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);


}


MatrixTable *        MatrixTable::OpAdd_r           (int other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return b+a;};
    std::function<double(double,double)> powfuncd = [](double a, double b){return b+a;};
    std::function<int(int,int)> powfunci = [](int a, int b){return b+a;};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);

}


MatrixTable *        MatrixTable::OpMul_r           (int other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString /*b*/){return a;};
    std::function<double(double,double)> powfuncd = [](double a, double b){return b*a;};
    std::function<int(int,int)> powfunci = [](int a, int b){return b*a;};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);

}


MatrixTable *        MatrixTable::OpSub_r           (int other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString /*b*/){return a;};
    std::function<double(double,double)> powfuncd = [](double a, double b){return b-a;};
    std::function<int(int,int)> powfunci = [](int a, int b){return b-a;};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);


}


MatrixTable *        MatrixTable::OpDiv_r           (int other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString /*b*/){return a;};
    std::function<double(double,double)> powfuncd = [](double a, double b){return b/a;};
    std::function<int(int,int)> powfunci = [](int a, int b){return b/a;};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);


}


MatrixTable *        MatrixTable::OpPow_r           (int other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString /*b*/){return a;};
    std::function<double(double,double)> powfuncd = [](double a, double b){return std::pow(b,a);};
    std::function<int(int,int)> powfunci = [](int a, int b){return std::pow(b,a);};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);


}


MatrixTable *        MatrixTable::LargerThen_r      (int other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return a > b? "1":"0";};
    std::function<double(double,double)> powfuncd = [](double a, double b){return a > b;};
    std::function<int(int,int)> powfunci = [](int a, int b){return a > b;};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);
}

MatrixTable *        MatrixTable::SmallerThen_r     (int other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return a < b? "1":"0";};
    std::function<double(double,double)> powfuncd = [](double a, double b){return a < b;};
    std::function<int(int,int)> powfunci = [](int a, int b){return a < b;};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);
}


MatrixTable *        MatrixTable::NotEqualTo_r      (int other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return a != b? "1":"0";};
    std::function<double(double,double)> powfuncd = [](double a, double b){return a != b;};
    std::function<int(int,int)> powfunci = [](int a, int b){return a != b;};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);
}

MatrixTable *        MatrixTable::LargerEqualThen_r (int other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return a <= b? "1":"0";};
    std::function<double(double,double)> powfuncd = [](double a, double b){return a <= b;};
    std::function<int(int,int)> powfunci = [](int a, int b){return a <= b;};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);
}

MatrixTable *        MatrixTable::SmallerEqualThen_r(int other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return a <= b? "1":"0";};
    std::function<double(double,double)> powfuncd = [](double a, double b){return a <= b;};
    std::function<int(int,int)> powfunci = [](int a, int b){return a <= b;};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);
}

MatrixTable *        MatrixTable::EqualTo_r         (int other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return a == b? "1":"0";};
    std::function<double(double,double)> powfuncd = [](double a, double b){return a == b;};
    std::function<int(int,int)> powfunci = [](int a, int b){return a == b;};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);
}

MatrixTable *        MatrixTable::And_r             (int other)
{
    std::function<double(double,double)> powfuncd = [](double a, double b){return (LISEM_ASMAP_BOOLFROMDOUBLE(b) && LISEM_ASMAP_BOOLFROMDOUBLE(a))? 1.0f:0.0f;};
    std::function<int(int,int)> powfunci = [](int a, int b){return (LISEM_ASMAP_BOOLFROMINT(b) && LISEM_ASMAP_BOOLFROMINT(a))? 1.0f:0.0f;};
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return (LISEM_ASMAP_BOOLFROMSTRING(b) && LISEM_ASMAP_BOOLFROMSTRING(a))? "True":"False";};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);

}

MatrixTable *        MatrixTable::Or_r              (int other)
{
    std::function<double(double,double)> powfuncd = [](double a, double b){return (LISEM_ASMAP_BOOLFROMDOUBLE(b) || LISEM_ASMAP_BOOLFROMDOUBLE(a))? 1.0f:0.0f;};
    std::function<int(int,int)> powfunci = [](int a, int b){return (LISEM_ASMAP_BOOLFROMINT(b) || LISEM_ASMAP_BOOLFROMINT(a))? 1.0f:0.0f;};
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return (LISEM_ASMAP_BOOLFROMSTRING(b) || LISEM_ASMAP_BOOLFROMSTRING(a))? "True":"False";};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);
}

MatrixTable *        MatrixTable::Xor_r             (int other)
{
    std::function<double(double,double)> powfuncd = [](double a, double b){return (LISEM_ASMAP_BOOLFROMDOUBLE(b) ^ LISEM_ASMAP_BOOLFROMDOUBLE(a))? 1.0f:0.0f;};
    std::function<int(int,int)> powfunci = [](int a, int b){return (LISEM_ASMAP_BOOLFROMINT(b) ^ LISEM_ASMAP_BOOLFROMINT(a))? 1.0f:0.0f;};
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return (LISEM_ASMAP_BOOLFROMSTRING(b) ^ LISEM_ASMAP_BOOLFROMSTRING(a))? "True":"False";};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);

}


