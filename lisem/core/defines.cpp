#include "defines.h"

namespace arma
{
  thread_local arma_rng_cxx11 arma_rng_cxx11_instance;
}

QString splitterSheetH =			\
"QSplitter[orientation=\"1\"]::handle {	\
    border: 1px black;			\
    margin: 2px 2px;					\
    min-width: 10px;					\
    max-width: 10px;					\
}";

QString splitterSheetV =			\
"QSplitter[orientation=\"2\"]::handle {	\
    border: 1px black;			\
    margin: 2px 2px;					\
    min-height: 10px;					\
    max-height: 10px;					\
}";


QString splitterSheet = splitterSheetH + splitterSheetV;


