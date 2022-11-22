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
	#ifndef __getcwd__
		#define __getcwd__ getcwd
	#endif
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
				ZS_THROW_RUNTIME_ERRORF("getcwd() error");
			}

			ret = cwd;

			return ret;
		}

		bool exists(const zs_string & _dir){

			if(zs_file::exists(_dir)){
				return false;
			}

			zs_string dir=_dir;

			// Native check directory it doesn't work with relative path so, relative levels (i.e ..)
			// has to be translated as absolute path
			if(zs_strutils::starts_with(dir,"..")){
				bool end=false;
				// get current path
				zs_string path=get_current_directory();

				// ignore relative paths
				do{
					end=zs_strutils::starts_with(dir,"..")==false;
					if(!end){
						int pos=dir.find("\\/");
						if(pos==zs_string::npos){
							end=true;
						}

						if(!end){

							dir=dir.substr(pos+1,zs_string::npos);

							// update directory
							pos=path.find_last_of("\\/");
							if(pos==zs_string::npos){
								end=true;
							}

							if(!end){
								path=path.substr(0,pos);
							}
						}
					}

				}while(!end);

				// concatenate normalized relative paths
				dir=path+"/"+dir;
			}


#if _WIN32
			DWORD ftyp = GetFileAttributesA(dir.c_str());
			  if (ftyp == INVALID_FILE_ATTRIBUTES){
			    return false;  //something is wrong with your path!
			  }

			  if (ftyp & FILE_ATTRIBUTE_DIRECTORY){
			    return true;   // this is a directory!
			  }

			  return false;    // this is not a directory!
#else
			  struct stat buffer;
			  return (stat (dir. c_str(), &buffer) == 0);
#endif
		}


	}

}
