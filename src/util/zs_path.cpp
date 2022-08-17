/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "../zetscript.h"

namespace zetscript{
	namespace zs_path{
		std::string  get_directory(const std::string & _path) {

			if(zs_dir::exists(_path) == false){

				size_t pos = _path.find_last_of("\\/");
				 return (std::string::npos == pos)
					 ? ""
					 : _path.substr(0, pos);
			}

			return _path;
		}

		std::string  get_filename(const std::string & _path) {
		  size_t found;
		  std::string ss=_path;
		  found=_path.find_last_of("/\\");
		  if(found != std::string::npos){
			  ss= _path.substr(found+1);
		  }
		  return ss;
		}

		std::string  get_filename_without_extension(const std::string & _path) {

			std::string file = get_filename(_path);


			std::string fName(file);
				size_t pos = fName.find_last_of(".");
				if(pos == std::string::npos)  //No extension.
					return fName;

				if(pos == 0)    //. is at the front. Not an extension.
					return fName;

				return fName.substr(0, pos);
		}


	}
}
