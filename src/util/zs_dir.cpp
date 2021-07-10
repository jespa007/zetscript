#include "zetscript.h"



#ifdef _WIN32
// MSDN recommends against using getcwd & chdir names
#define __getcwd__ _getcwd
#define __chdir__ _chdir
#else
#define __getcwd__ getcwd
#define __chdir__ chdir
#endif

namespace zetscript{
	namespace zs_dir{
		bool set_current_directory(const std::string & _path){
			if(__chdir__(_path.c_str())==-1) {
				return false;
			}
			return true;
		}
	}

}
