

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

#ifdef _MSC_VER // visual studio doesn't support this.
	return name;
#else
	if(name == string(typeid(string *).name())){
		return "string *";
	}

	if(name == string(typeid(int *).name())){
			return "int *";
		}

	if(name == string(typeid(bool *).name())){
			return "bool *";
		}

	if(name == string(typeid(float *).name())){
			return "float *";
		}


	if(name == string(typeid(string).name())){
		return "string";
	}

    int status = -4; // some arbitrary value to eliminate the compiler warning

    // enable c++11 by passing the flag -std=c++11 to g++
    std::unique_ptr<char, void(*)(void*)> res {
        abi::__cxa_demangle(name.c_str(), NULL, NULL, &status),
        std::free
    };

    return (status==0) ? res.get() : name.c_str() ;
#endif
}


