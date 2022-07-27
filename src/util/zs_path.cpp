/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "../zetscript.h"

namespace zetscript{
	namespace zs_path{
		zs_string  get_directory(const zs_string & _path) {

			if(zs_dir::exists(_path) == false){

				size_t pos = _path.find_last_of("\\/");
				 return (zs_string::npos == pos)
					 ? ""
					 : _path.substr(0, pos);
			}

			return _path;
		}

		zs_string  get_filename(const zs_string & _path) {
		  unsigned found;
		  zs_string ss=_path;
		  found=_path.find_last_of("/\\");
		  if(found != zs_string::npos){
			  ss= _path.substr(found+1);
		  }
		  return ss;
		}

		zs_string  get_filename_without_extension(const zs_string & _path) {

			zs_string file = get_filename(_path);


			zs_string fName(file);
				unsigned pos = fName.find_last_of(".");
				if(pos == zs_string::npos)  //No extension.
					return fName;

				if(pos == 0)    //. is at the front. Not an extension.
					return fName;

				return fName.substr(0, pos);
		}


	}
}
