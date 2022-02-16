/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "../zetscript.h"

namespace zetscript{
	namespace zs_rtti{
		zs_string  demangle(const char * name) {

		#ifdef _MSC_VER // visual studio doesn't support this.
			return name;
		#else
			if(name == zs_string(typeid(zs_string *).name())){
				return "zetscript::zs_string*";
			}

			if(name == zs_string(typeid(zs_int *).name())){
					return "zetscript::zs_int*";
				}

			if(name == zs_string(typeid(bool *).name())){
					return "zetscript::bool*";
				}

			if(name == zs_string(typeid(zs_float *).name())){
					return ZS_STR(zs_float)"*";
				}


			if(name == zs_string(typeid(zs_string).name())){
				return "zetscript::zs_string";
			}

			if(name == zs_string("FunctionMember")){
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
