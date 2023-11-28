/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "@base.h"

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
	namespace Directory{
		bool changeDir(const String & _path){
			if(__chdir__(_path.toConstChar())==-1) {
				return false;
			}
			return true;
		}

		String getCurrentDirectory(){
			char cwd[MAX_PATH]={0};
			String ret="";
			if (__getcwd__(cwd,MAX_PATH) ==NULL){
				ZS_THROW_RUNTIME_ERRORF("getcwd() error");
			}

			ret = cwd;

			return ret;
		}

		bool exists(const String & _dir){

			if(File::exists(_dir)){
				return false;
			}

			String dir=_dir;

			// Native check directory it doesn't work with relative path so, relative levels (i.e ..)
			// has to be translated as absolute path
			if(String::startsWith(dir,"..")){
				bool end=false;
				// get current path
				String path=getCurrentDirectory();

				// ignore relative paths
				do{
					end=String::startsWith(dir,"..")==false;
					if(!end){
						int pos=dir.find("\\/");
						if(pos==String::npos){
							end=true;
						}

						if(!end){

							dir=dir.substr(pos+1,String::npos);

							// update directory
							pos=path.findLastOf("\\/");
							if(pos==String::npos){
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
			DWORD ftyp = GetFileAttributesA(dir.toConstChar());
			  if (ftyp == INVALID_FILE_ATTRIBUTES){
			    return false;  //something is wrong with your path!
			  }

			  if (ftyp & FILE_ATTRIBUTE_DIRECTORY){
			    return true;   // this is a directory!
			  }

			  return false;    // this is not a directory!
#else
			  struct stat buffer;
			  return (stat (dir. toConstChar(), &buffer) == 0);
#endif
		}


	}

}
