#ifndef __SPHAZARD_VERSION_H__
#define __SPHAZARD_VERSION_H__


#define LISEM_VERSION_MAJOR 0
#define LISEM_VERSION_MINOR 2
#define LISEM_VERSION_PATCH 2

// Preprocessor to string conversion
#define LISEM_TO_STRING_HELPER(x) #x
#define LISEM_TO_STRING(x) LISEM_TO_STRING_HELPER(x)

// LISEM version as a string; for example "1.0.0".
#define LISEM_VERSION_STRING LISEM_TO_STRING(LISEM_VERSION_MAJOR) "." \
  LISEM_TO_STRING(LISEM_VERSION_MINOR) "." \
  LISEM_TO_STRING(LISEM_VERSION_PATCH)
  
#endif
