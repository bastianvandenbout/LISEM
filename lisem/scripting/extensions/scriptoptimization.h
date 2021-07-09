#ifndef SCRIPTOPTIMIZATION_H
#define SCRIPTOPTIMIZATION_H

#include "Eigen/Core"
#include "optimization/leastsquares.h"
#include "angelscript.h"
#include "QList"
#include "lsmscriptengine.h"
#include "error.h"
#include "stat/logisticregression.h"
#include "functional"
#include "vector"

inline static CScriptArray * OptimizeGumbel(CScriptArray * data_x, CScriptArray * data_y, double initial_c1, double initial_c2)
{
    if(!(data_x->IsNumberType() && data_y->IsNumberType()))
    {
        LISEMS_ERROR("array<> does not contain numbers");
        throw 1;
    }

    QList<double> *ListX = data_x->ToQListDouble();
    QList<double> *ListY = data_y->ToQListDouble();


    if((ListX == nullptr || ListY == nullptr))
    {
        LISEMS_ERROR("array<> could not be converted to numbers");
        throw 1;
    }

    if((ListX->length() == 0 || ListY->length() == 0))
    {
        LISEMS_ERROR("array<> can not contain zero elements");
        throw 1;
    }
    if(!(ListX->length() == ListY->length() ))
    {
        LISEMS_ERROR("array<> arguments are not of identical size");
        throw 1;
    }

    int n = 0;
    double x_avg = 0.0;
    double x_min = 1e30;
    double x_max = -1e30;

    for(int i = 0; i < ListX->length(); i++)
    {
        n++;
        x_avg += ListX->at(i);
        x_min = std::min(x_min,ListX->at(i));
        x_max = std::max(x_max,ListX->at(i));
    }
    n = std::max(1,n);
    x_avg = x_avg/n;

    lsq::GaussNewton<double, GumbelError, lsq::ArmijoBacktracking<double>> optimizer;

    // Set number of iterations as stop criterion.
    // Set it to 0 or negative for infinite iterations (default is 0).
    optimizer.setMaxIterations(200);

    // Set the minimum length of the gradient.
    // The optimizer stops minimizing if the gradient length falls below this
    // value.
    // Set it to 0 or negative to disable this stop criterion (default is 1e-9).
    optimizer.setMinGradientLength(0);

    // Set the minimum length of the step.
    // The optimizer stops minimizing if the step length falls below this
    // value.
    // Set it to 0 or negative to disable this stop criterion (default is 1e-9).
    optimizer.setMinStepLength(0);

    // Set the minimum least squares error.
    // The optimizer stops minimizing if the error falls below this
    // value.
    // Set it to 0 or negative to disable this stop criterion (default is 0).
    optimizer.setMinError(0);

    // Set the the parametrized StepSize functor used for the step calculation.
    optimizer.setStepSize(lsq::ArmijoBacktracking<double>(0.8, 0.1, 1e-10, 1.0, 0));


    optimizer.setVerbosity(2);

    GumbelError err;
    err.data_x = ListX;
    err.data_y = ListY;
    optimizer.setErrorFunction(err);

    // Set initial guess for parameters
    Eigen::VectorXd initialGuess(2);

    //just a guess about what would be some reasonable values for the constants
    double guess_c1 = x_min + 0.25*(x_avg - x_min);
    double guess_c2 = (x_max-x_min)*0.5;

    std::cout << initial_c1 << " " << initial_c2 << std::endl;
    if(!(initial_c1 <-1e30 ))
    {
        guess_c1 = initial_c1;
    }
    if(!(initial_c2 <-1e30 ))
    {
        guess_c2 = initial_c2;
    }

    initialGuess << guess_c1,guess_c2;

    try
    {

        // Start the optimization.
        auto result = optimizer.minimize(initialGuess);

        //delete fitting data
        delete ListX;
        delete ListY;

        std::cout << "xavg  " << x_avg << " y min, max " << x_min  << "  " << x_max << std::endl;
        std::cout << "Done! Converged: " << (result.converged ? "true" : "false")
            << " Iterations: " << result.iterations << std::endl;

        // do something with final function value
        std::cout << "Final fval: " << result.fval.transpose() << std::endl;

        // do something with final x-value
        std::cout << "Final xval: " << result.xval.transpose() << std::endl;


        // Obtain a pointer to the engine
        asIScriptContext *ctx = asGetActiveContext();
        if(ctx != nullptr)
        {
            asIScriptEngine *engine = ctx->GetEngine();
            asITypeInfo *arrayType = engine->GetTypeInfoByDecl("array<double>");

            // Create the array object
            CScriptArray *array = CScriptArray::Create(arrayType);

            array->Resize(result.xval.size());

            for(int i = 0; i < result.xval.size(); ++i)
            {
                array->SetValue(i,(void*)(new double(result.xval(i))),false);
            }

            return array;

        }
        LISEMS_ERROR("Could not get virtual machine");
        throw 1;


    }catch(...)
    {
        delete ListX;
        delete ListY;
        LISEMS_ERROR("Error in minimalization of objective function for data fitting");
        throw 1;
    }


}


inline static CScriptArray * OptimizeGumbel(CScriptArray * data_x, CScriptArray * data_y)
{
    return OptimizeGumbel(data_x,data_y,-1e31,-1e31);
}

/*inline static CScriptArray * OptimizePolynomial()
{


}*/


// Implement an objective functor.
struct ScriptFunctionError
{
     asIScriptFunction * Function;
     QList<double> *Positive;

    void operator()(const Eigen::VectorXd &xval,
        Eigen::VectorXd &fval,
        Eigen::MatrixXd &) const
    {
        // omit calculation of jacobian, so finite differences will be used
        // to estimate jacobian numerically
        fval.resize(1);

        //double errorr = std::abs(xval(0) - 0.2) * std::abs(xval(0) - 0.2) + std::abs(xval(1) - 0.8) * std::abs(xval(1) - 0.8);
        //fval(0) = errorr;


        asIScriptContext *ctx = asGetActiveContext();

        int ps =  ctx->PushState();

        // Store the current context state so we can restore it later
          if( ctx && ps > -1 )
          {
            // Use the context normally, e.g.
            //  ctx->Prepare(...);
            //  ctx->Execute(...);



            asIScriptEngine *engine = ctx->GetEngine();
            asITypeInfo *arrayType = engine->GetTypeInfoByDecl("array<double>");

            //call angelscript function callback
            CScriptArray *array = CScriptArray::Create(arrayType);


            array->Resize(xval.size());

            for(int i = 0; i < xval.size(); ++i)
            {
                double val =xval(i);
                double val_convert = val;
                if(Positive->at(i) > 0.5)
                {
                    if(val >= 0.0)
                    {
                        val = val + 1.0;
                    }else {
                        val = -1.0/(-1.0 + val);
                    }
                }
                array->SetValue(i,(void*)(new double(val)),false);
            }

            ctx->Prepare(Function);

            // Set the function arguments
            ctx->SetArgObject(0,array);

            int r = ctx->Execute();
            if( r == asEXECUTION_FINISHED )
            {
              // The return value is only valid if the execution finished successfully
              double ret = ctx->GetReturnDouble();

              std::cout.precision(17);
              std::cout << xval(0) << " " << xval(1) << "  error : " << ret <<std::endl;

              fval(0) = ret;


            }else {

                ctx->PopState();
                LISEMS_ERROR("Error when calling optimizing callback function!")
                throw 1;
            }



            // Once done, restore the previous state
            ctx->PopState();
          }else {

              LISEMS_ERROR("Error when calling optimizing callback function, could not push state!")
              throw 1;
          }


    }
};

// Implement an objective functor.
struct FunctionalFunctionError
{
     std::function<double(std::vector<double>)> Function;
     std::vector<bool> Positive;

    void operator()(const Eigen::VectorXd &xval,
        Eigen::VectorXd &fval,
        Eigen::MatrixXd &) const
    {
        // omit calculation of jacobian, so finite differences will be used
        // to estimate jacobian numerically
        fval.resize(1);

        //double errorr = std::abs(xval(0) - 0.2) * std::abs(xval(0) - 0.2) + std::abs(xval(1) - 0.8) * std::abs(xval(1) - 0.8);
        //fval(0) = errorr;

            //call angelscript function callback
            std::vector<double> array;


            for(int i = 0; i < xval.size(); ++i)
            {
                double val =xval(i);
                double val_convert = val;
                if(Positive.at(i))
                {
                    if(val >= 0.0)
                    {
                        val = val + 1.0;
                    }else {
                        val = -1.0/(-1.0 + val);
                    }
                }
                array.push_back(val);
            }

            double ret = Function(array);
            fval(0) = ret;
    }

};



//this is much slower as it has to call a function within the scripting environment many times
inline static CScriptArray * OptimizeCustom(CScriptArray * data_parameters,CScriptArray * data_positive, asIScriptFunction *cb, double step, double max_step, int max_nsteps = 200)
{


    //just a guess about what would be some reasonable values for the constants

    QList<double> *params = data_parameters->ToQListDouble();


    QList<double> *positive;
    if(data_positive != nullptr)
    {
        positive = data_positive->ToQListDouble();

    }else {

        positive = new QList<double>();
        for(int i = 0; i < params->length(); i++)
        {
            positive->append(0.0);

        }
    }

    if(positive->size() != params->size())
    {
        LISEMS_ERROR("Input parameters and positive-ness of parameters are not of equal size.");
        throw 1;
    }
    Eigen::VectorXd initialGuess(params->length());

    for(int i = 0; i < params->length(); i++)
    {
        if(positive->at(i) > 0.5)
        {
            if(params->at(i) >= 0.0)
            {
                if(params->at(i) >= 1.0)
                {
                    initialGuess(i) = -1.0 + params->at(i);
                }else {
                    params->replace(i,std::max(1e-20,params->at(i)));
                    initialGuess(i) = 1.0/initialGuess(i) - 1.0;
                }
            }else {
                LISEMS_ERROR("Parameter "+ QString::number(i) +" must be positive, but initial value is negative!");
                throw 1;
            }

        }else {

            initialGuess(i) = params->at(i);
        }
    }

    delete params;


    //optimize this custom function
    lsq::GaussNewton<double, ScriptFunctionError, lsq::ArmijoBacktracking<double>> optimizer(step);

    // Set number of iterations as stop criterion.
    // Set it to 0 or negative for infinite iterations (default is 0).
    optimizer.setMaxIterations(std::max(1,max_nsteps));

    // Set the minimum length of the gradient.
    // The optimizer stops minimizing if the gradient length falls below this
    // value.
    // Set it to 0 or negative to disable this stop criterion (default is 1e-9).
    optimizer.setMinGradientLength(0);

    // Set the minimum length of the step.
    // The optimizer stops minimizing if the step length falls below this
    // value.
    // Set it to 0 or negative to disable this stop criterion (default is 1e-9).
    optimizer.setMinStepLength(0);

    // Set the minimum least squares error.
    // The optimizer stops minimizing if the error falls below this
    // value.
    // Set it to 0 or negative to disable this stop criterion (default is 0).
    optimizer.setMinError(0);

    // Set the the parametrized StepSize functor used for the step calculation.
    optimizer.setStepSize(lsq::ArmijoBacktracking<double>(0.8, 0.1, 1e-10, max_step, 0));


    optimizer.setVerbosity(2);


    ScriptFunctionError err;
    err.Function = cb;
    err.Positive = positive;
    optimizer.setErrorFunction(err);

    try
    {

        // Start the optimization.
        auto result = optimizer.minimize(initialGuess);


        // Obtain a pointer to the engine
        asIScriptContext *ctx = asGetActiveContext();
        if(ctx != nullptr)
        {
            asIScriptEngine *engine = ctx->GetEngine();
            asITypeInfo *arrayType = engine->GetTypeInfoByDecl("array<double>");

            // Create the array object
            CScriptArray *array = CScriptArray::Create(arrayType);

            array->Resize(result.xval.size());

            for(int i = 0; i < result.xval.size(); ++i)
            {
                array->SetValue(i,(void*)(new double(result.xval(i))),false);
            }

            return array;

        }
        LISEMS_ERROR("Could not get virtual machine");
        throw 1;


    }catch(...)
    {
        LISEMS_ERROR("Error in minimalization of objective function for data fitting");
        throw 1;
    }

    return nullptr;

}


inline static CScriptArray * OptimizeCustom(CScriptArray * data_parameters,asIScriptFunction *cb, double step, double max_step, int max_nsteps = 200)
{
    return OptimizeCustom(data_parameters,nullptr,cb,step, max_step, max_nsteps);

}


inline static std::vector<double> OptimizeCustom2(std::vector<double> data_parameters, std::vector<bool> positive,std::function<double(std::vector<double>)> func, double step, double max_step, int max_nsteps = 200)
{


    std::vector<double> errorvals = data_parameters;

    //just a guess about what would be some reasonable values for the constants

    std::vector<double> params = data_parameters;


    if(positive.size() == params.size())
    {

    }else if(positive.size() == 0)
    {
        for(int i = 0; i < params.size(); i++)
        {
            positive.push_back(0.0);

        }
    }
    if(positive.size() != params.size())
    {
        LISEMS_ERROR("Input parameters and positive-ness of parameters are not of equal size.");
        throw 1;
    }


    Eigen::VectorXd initialGuess(params.size());

    for(int i = 0; i < params.size(); i++)
    {
        if(positive.at(i) > 0.5)
        {
            if(params.at(i) >= 0.0)
            {
                if(params.at(i) >= 1.0)
                {
                    initialGuess(i) = -1.0 + params.at(i);
                }else {
                    params[i] = std::max(1e-20,params.at(i));
                    initialGuess(i) = 1.0/initialGuess(i) - 1.0;
                }
            }else {
                LISEMS_ERROR("Parameter "+ QString::number(i) +" must be positive, but initial value is negative!");
                throw 1;
            }

        }else {

            initialGuess(i) = params.at(i);
        }
    }



    //optimize this custom function
    lsq::GaussNewton<double, FunctionalFunctionError, lsq::ArmijoBacktracking<double>> optimizer(step);

    // Set number of iterations as stop criterion.
    // Set it to 0 or negative for infinite iterations (default is 0).
    optimizer.setMaxIterations(std::max(1,max_nsteps));

    // Set the minimum length of the gradient.
    // The optimizer stops minimizing if the gradient length falls below this
    // value.
    // Set it to 0 or negative to disable this stop criterion (default is 1e-9).
    optimizer.setMinGradientLength(0);

    // Set the minimum length of the step.
    // The optimizer stops minimizing if the step length falls below this
    // value.
    // Set it to 0 or negative to disable this stop criterion (default is 1e-9).
    optimizer.setMinStepLength(0);

    // Set the minimum least squares error.
    // The optimizer stops minimizing if the error falls below this
    // value.
    // Set it to 0 or negative to disable this stop criterion (default is 0).
    optimizer.setMinError(0);

    // Set the the parametrized StepSize functor used for the step calculation.
    optimizer.setStepSize(lsq::ArmijoBacktracking<double>(0.8, 0.1, 1e-10, max_step, 0));


    optimizer.setVerbosity(2);


    FunctionalFunctionError err;
    err.Function = func;
    err.Positive = positive;
    optimizer.setErrorFunction(err);

    try
    {

        // Start the optimization.
        auto result = optimizer.minimize(initialGuess);


       std::vector<double>array;
            for(int i = 0; i < result.xval.size(); ++i)
            {
                array.push_back(double(result.xval(i)));
            }

            return array;


        LISEMS_ERROR("Could not get virtual machine");
        throw 1;


    }catch(...)
    {
        LISEMS_ERROR("Error in minimalization of objective function for data fitting");
        throw 1;
    }

    return errorvals;


}

inline static std::vector<double> OptimizeCustom2(std::vector<double> data_parameters, std::function<double(std::vector<double>)> func, double step, double max_step, int max_nsteps = 200)
{

    return OptimizeCustom2(data_parameters,std::vector<bool>(),func,step,max_step, max_nsteps);
}

inline static void RegisterLeastSquaresToScriptEngine(LSMScriptEngine * sm)
{

    sm->RegisterGlobalFunction("array<double> @FitGumbel(array<double> & in x, array<double> & in y, double guess_c1 , double guess_c2)",asFUNCTIONPR(OptimizeGumbel,(CScriptArray*,CScriptArray*,double,double),CScriptArray*),asCALL_CDECL);

    sm->RegisterGlobalFunction("array<double> @FitGumbel(array<double> & in x, array<double> & in y)",asFUNCTIONPR(OptimizeGumbel,(CScriptArray*,CScriptArray*),CScriptArray*),asCALL_CDECL);

    sm->RegisterGlobalFunction("array<double> @OptimizeCustom(array<double> & in params,CALLBACKDFDL @callbfunc, double finitestep = 0.001, double max_step = 1.0, int maxnumsteps = 200)",asFUNCTIONPR(OptimizeCustom,(CScriptArray*, asIScriptFunction *,double,double,int),CScriptArray*),asCALL_CDECL);
    sm->RegisterGlobalFunction("array<double> @OptimizeCustom(array<double> & in params,array<bool> & in positive , CALLBACKDFDL @callbfunc, double finitestep = 0.001, double max_step = 1.0, int maxnumsteps = 200)",asFUNCTIONPR(OptimizeCustom,(CScriptArray*,CScriptArray*, asIScriptFunction *, double,double,int),CScriptArray*),asCALL_CDECL);


}



#endif // SCRIPTOPTIMIZATION_H
