#pragma once

#ifdef PE_WINDOWS
    #ifdef PE_DLL
        #define PE_API __declspec(dllexport)
    #else
        #define PE_API __declspec(dllimport)
    #endif
#else
    #error "Planetoid Engine only supports Windows!"
    #define PE_API
#endif