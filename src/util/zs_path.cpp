#include "../zetscript.h"

namespace zetscript{
	namespace zs_path{
		zs_string  get_directory(const zs_string & _path) {
			size_t pos = _path.find_last_of("\\/");
			 return (zs_string::npos == pos)
				 ? ""
				 : _path.substr(0, pos);
		}

		zs_string  get_filename(const zs_string & _path) {
		  size_t found;
		  zs_string ss=_path;
		  found=_path.find_last_of("/\\");
		  if((int)found != -1)
			  ss= _path.substr(found+1);
		  return ss;
		}

		zs_string  get_filename_without_extension(const zs_string & _path) {

			zs_string file = get_filename(_path);


			zs_string fName(file);
				size_t pos = fName.find_last_of(".");
				if(pos == zs_string::npos)  //No extension.
					return fName;

				if(pos == 0)    //. is at the front. Not an extension.
					return fName;

				return fName.substr(0, pos);
		}


	}
}
