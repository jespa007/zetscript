/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "@base.h"

namespace zetscript{

	String  Path::getDirectory(const String & _path) {

		if(Directory::exists(_path) == false){

			int pos = _path.findLastOf("\\/");
				return (String::npos == pos)
					? ""
					: _path.getSubstring(0, pos);
		}

		return _path;
	}

	String  Path::getFilename(const String & _path) {
		int found;
		String ss=_path;
		found=_path.findLastOf("/\\");
		if(found != String::npos){
			ss= _path.getSubstring(found+1);
		}
		return ss;
	}

	String  Path::getFilenameWithoutExtension(const String & _path) {

		String file = getFilename(_path);


		String fName(file);
			int pos = fName.findLastOf(".");
			if(pos == String::npos)  //No extension.
				return fName;

			if(pos == 0)    //. is at the front. Not an extension.
				return fName;

			return fName.getSubstring(0, pos);
	}
}
