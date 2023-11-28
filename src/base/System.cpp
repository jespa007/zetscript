/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "@base.h"

#define ZS_ONE_OVER_MS_PER_SECONDS	(1.0/1000.0)

namespace zetscript{
	namespace System{
		static zs_int __begin_ms__=0;

		zs_float clock(){
			zs_int current_ms=std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
			zs_int elapsed_ms_from_begin=0;

			if(__begin_ms__==0){
				__begin_ms__=current_ms;
			}else{
				elapsed_ms_from_begin=current_ms-__begin_ms__;
			}

			return elapsed_ms_from_begin*ZS_ONE_OVER_MS_PER_SECONDS;
		}

		int getLastError(){
#ifdef _WIN32
			return GetLastError();
#else
			return errno;
#endif
		}

		const char * getIoErrorCodeDetails(int _error_code){


#ifdef _WIN32
			switch(_error_code){
			case ERROR_FILE_NOT_FOUND:
				return "The system cannot find the file specified";
			case ERROR_DIR_NOT_EMPTY:
				return " The directory is not empty";
			case ERROR_PATH_NOT_FOUND:
				return "The system cannot find the path specified";
			case ERROR_INVALID_NAME:
				return "Invalid name";
			case ERROR_ACCESS_DENIED:
				return "Access denied";
			}
#else
			switch (_error_code) {
			case EACCES:
				return "Search permission is denied on a component of the path prefix, or write permission is denied on the parent directory of the directory to be created.";
			case EEXIST:
				return  "The named file exists.";
			case ELOOP:
				return  "A loop exists in symbolic links encountered during resolution of the path argument.";
			case EMLINK:
				return  "The link count of the parent directory would exceed {LINK_MAX}.";
			case ENAMETOOLONG:
				return  "The length of the path argument exceeds {PATH_MAX} or a pathname component is longer than {NAME_MAX}.";
			case ENOENT:
				return  "A component of the path prefix specified by path does not name an existing directory or path is an empty std::string.";
			case ENOSPC:
				return  "The file system does not contain enough space to hold the contents of the new directory or to extend the parent directory of the new directory.";
			case ENOTDIR:
				return  "A component of the path prefix is not a directory.";
			case EROFS:
				return  "The parent directory resides on a read-only file system.";
			}
#endif

			return "";
		}


		const char * getErrorCodeDetails(int _error_code){
			switch(_error_code){
			case ERANGE:
				return "Result too large";
			}

			return "";
		}
	}
}
