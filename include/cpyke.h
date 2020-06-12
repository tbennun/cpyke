#ifndef __CPYKE_H
#define __CPYKE_H

#include <string>
#include "types.h"

#ifndef CPYKE_INTERNAL
#ifdef _MSC_VER
#define CPYKE_EXPORTED extern __declspec(dllimport)
#else
#define CPYKE_EXPORTED extern
#endif
#else
#ifdef _MSC_VER
#define CPYKE_EXPORTED extern __declspec(dllexport)
#else
#define CPYKE_EXPORTED extern
#endif
#endif

CPYKE_EXPORTED cpy::result _cpyke(const char *script, cpy::TypePair *data, int nargs);
CPYKE_EXPORTED bool cpyke_pip_install(const char *package);

template <typename... Args>
cpy::result cpyke(const char *script, Args &&... args)
{
    cpy::TypePair arg_array[] = {cpy::TypePair(args)...};
    return _cpyke(script, arg_array, sizeof...(args));
}

cpy::result cpyke(const char *script)
{
    return _cpyke(script, nullptr, 0);
}

#endif // __CPYKE_H
