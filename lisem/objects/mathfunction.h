#pragma once


#include "muParser.h"
#include "QString"
#include "QList"
#include "error.h"

class MathFunction
{
    mu::Parser  p;
public:
    QString Expression;
    std::vector<double> vals;
    bool vars_set = false;


    inline MathFunction()
    {

    }

    inline MathFunction(QString s)
    {
        SetExpression(s);

    }

    inline MathFunction(const MathFunction &copy)
    {
        SetExpression(copy.Expression);
    }

    inline void SetExpression(QString s)
    {

        int pc = GetParameterCount();

        for(int i = 0; i < pc; i++)
        {
            QString name = varname(i);
            p.RemoveVar(name.toStdString());
        }

        Expression = s;
        p.SetExpr(Expression.toStdString());
        vars_set = false;
        vals.clear();


    }

    inline int GetParameterCount()
    {
        if(Expression.isEmpty())
        {
            return 0;
        }

        p.SetExpr(Expression.toStdString());
        return p.GetUsedVar().size();
    }

    inline bool IsValid()
    {

        return true;
    }

    inline QString varname(int index)
    {
        /*QString name;
        int tempindex = index;
        int nchars = 1;
        while(tempindex > 26)
        {
            tempindex = tempindex/26;
            nchars ++;
        }

        if(nchars == 1)
        {
            char c  = 'a' + index;
            name += c;
        }else if(nchars == 2)
        {
            int divide = index/26;
            int mode = index % 26;
            char c  = 'a' + divide;
            char c2  = 'a' + mode;
            name = name + c + c2;
        }else if(nchars == 3)
        {

            int divide1 = index/26*26;
            int mode1 = index % 26*26;
            int divide2 = mode1/26;
            int mode2 = mode1 % 26;
            char c  = 'a' + divide1;
            char c2  = 'a' + divide2;
            char c3  = 'a' + mode2;
            name = name + c + c2 + c3;

        }*/

        QString name;
        name = "x"+ QString::number(index);


        return name;
    }
    inline double Evaluate(std::vector<double> params)
    {
        try {

            if(!vars_set)
            {
                vals.clear();

                for(int i = 0; i < params.size(); i++)
                {
                    vals.push_back(0.0);
                }

                for(int i = 0; i < params.size(); i++)
                {
                    QString name = varname(i);
                    p.DefineVar(name.toStdString(),&(vals[i]));
                }
                vars_set = true;

            }
            for(int i = 0; i < params.size(); i++)
            {
                vals[i] = params[i];
            }

            double answer;
            p.Eval(&answer,1);
            return answer;
        }
        catch (mu::Parser::exception_type &e)
        {
            std::cout << e.GetMsg() << std::endl;
        }

        return 0.0;
    }


    inline double AS_Int_Evaluate(std::vector<double> params)
    {
        try {

            if(!vars_set)
            {
                vals.clear();

                for(int i = 0; i < params.size(); i++)
                {

                    vals.push_back(0.0);
                }
                for(int i = 0; i < params.size(); i++)
                {
                    QString name = varname(i);
                    p.DefineVar(name.toStdString(),&(vals[i]));
                }
                vars_set = true;

            }

            for(int i = 0; i < params.size(); i++)
            {
                vals[i] = params[i];
            }

            double answer;
            p.Eval(&answer,1);
            return answer;
        }
        catch (mu::Parser::exception_type &e)
        {
            LISEMS_ERROR("Could not use MathExpression to obtain value: " + QString(e.GetMsg().c_str()));
            throw 1;
        }

        return 0.0;
    }

    inline double AS_Evaluate(std::vector<double> p)
    {

        double val = AS_Int_Evaluate(p);

        return val;
    }
    inline QString AS_GetExpression()
    {
        return Expression;
    }

    inline MathFunction AS_OpAssign(QString string)
    {
        SetExpression(string);
        return MathFunction(string);
    }
};
static inline void AS_MathFunctionC1(QString string, void * mem)
{

    new(mem) MathFunction(string);
}

static inline void AS_MathFunctionC0(void * mem)
{

    new(mem)  MathFunction();
}
