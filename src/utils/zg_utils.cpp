

//#include "CColorTerm.cpp"
//#include "CZip.cpp"
//#include "CLog.cpp"
//#include "CStringUtils.cpp"
//#include "CUtil.cpp"
//#include "CFile.cpp"



#include "CColorTerm.cpp"
#include "CLog.cpp"
#include "CStringUtils.cpp"
#include "CIO_Utils.cpp"


std::string demangle(const string & name) {

    int status = -4; // some arbitrary value to eliminate the compiler warning

    // enable c++11 by passing the flag -std=c++11 to g++
    std::unique_ptr<char, void(*)(void*)> res {
        abi::__cxa_demangle(name.c_str(), NULL, NULL, &status),
        std::free
    };

    return (status==0) ? res.get() : name.c_str() ;
}


