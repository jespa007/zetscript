/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
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
		bool change_dir(const zs_string & _path){
			if(__chdir__(_path.c_str())==-1) {
				return false;
			}
			return true;
		}

		zs_string get_current_directory(){
			char cwd[MAX_PATH]={0};
			zs_string ret="";
			if (__getcwd__(cwd,MAX_PATH) ==NULL){
				THROW_RUNTIME_ERRORF("getcwd() error");
			}

			ret = cwd;

			return ret;
		}
	}

}
