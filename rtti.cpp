namespace zs{
	namespace rttir{
		std::string demangle(const std::string & name) {

		#ifdef _MSC_VER // visual studio doesn't support this.
			return name;
		#else
			if(name == std::string(typeid(std::string *).name())){
				return "std::string *";
			}

			if(name == std::string(typeid(int *).name())){
					return "int *";
				}

			if(name == std::string(typeid(bool *).name())){
					return "bool *";
				}

			if(name == std::string(typeid(float *).name())){
					return "float *";
				}


			if(name == std::string(typeid(std::string).name())){
				return "std::string";
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

	}
}
