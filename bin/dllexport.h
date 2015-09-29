/*--------------------------------------------------------------------------*/
/*  Ryuichi YAMAMOTO (zryuichi@gmail.com)                                   */
/*--------------------------------------------------------------------------*/

#ifndef SPTK_DLLEXPORT_H_
#define SPTK_DLLEXPORT_H_

#ifndef DLLEXPORT
#  ifdef _WIN32
#      define DLLEXPORT __declspec(dllexport)
#    else
#      define DLLEXPORT
#  endif
#endif

#endif  /* SPTK_DLLEXPORT_H_ */
