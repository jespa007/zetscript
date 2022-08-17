/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "../zetscript.h"

namespace zetscript{
	namespace zs_rtti{
		std::string  demangle(const char * name) {

		#ifdef _MSC_VER // visual studio doesn't support this.
			return name;
		#else
			if(name == std::string(typeid(std::string *).name())){
				return "std::string*";
			}

			if(name == std::string(typeid(zs_int *).name())){
					return "zetscript::zs_int*";
				}

			if(name == std::string(typeid(bool *).name())){
					return "zetscript::bool*";
				}

			if(name == std::string(typeid(zs_float *).name())){
					return ZS_STR(zs_float)"*";
				}


			if(name == std::string(typeid(std::string).name())){
				return "std::string";
			}

			if(name == std::string("MemberFunction")){
				return "zetscript::ScriptObjectMemberFunction *";
			}

			int status = -4; // some arbitrary value to eliminate the compiler warning

			// enable c++11 by passing the flag -std=c++11 to g++
			std::unique_ptr<char, void(*)(void*)> res {
				abi::__cxa_demangle(name, NULL, NULL, &status),
				std::free
			};

			return (status==0) ? res.get() : name ;
		#endif
		}

	}
}
