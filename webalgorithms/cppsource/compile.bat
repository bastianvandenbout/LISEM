cd C:/emscripten/emsdk-main/
emsdk_env.bat
cd C:/data/LISEM/src/webalgorithms/cppsource/
emcc fastflood.cpp -O3 -o fastflood.js -s ALLOW_MEMORY_GROWTH=1 -s EXPORTED_FUNCTIONS="['_do_algo_fastflood','_do_algo_hillshade','_do_rasterize_shapes','_do_algo_ldd','_do_algo_channeldim','_o_algo_accuflux','_do_algo_demfill','_SaxtonKSat','_malloc','_free']" -sEXPORTED_RUNTIME_METHODS=ccall,cwrap -pthread -sPTHREAD_POOL_SIZE=8 -sSHARED_MEMORY -sERROR_ON_UNDEFINED_SYMBOLS=0 -I ./ libopencv_core.a libopencv_imgproc.a