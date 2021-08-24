#include "zetscript.h"



#ifdef _WIN32
// MSDN recommends against using getcwd & chdir names
#ifndef __getcwd__
#define __getcwd__ _getcwd
#endif
#ifndef __chdir__
#define __chdir__ _chdir
#endif
#else
#ifndef __chdir__
#define __chdir__ chdir
#endif
#endif

namespace zetscript{
	namespace zs_dir{
		bool change_dir(const std::string & _path){
			if(__chdir__(_path.c_str())==-1) {
				return false;
			}
			return true;
		}

		std::string get_current_directory(){
			char cwd[MAX_PATH]={0};
			std::string ret="";
			if (getcwd(cwd,MAX_PATH) ==NULL){
				THROW_RUNTIME_ERROR("getcwd() error");
			}

			ret = cwd;

			return ret;
		}
	}

}
