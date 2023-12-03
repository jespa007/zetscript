/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "@base.h"

namespace zetscript{
	bool * Boolean::parse(const String & _str){
		String str=_str.toLower();

		if(str=="true"){
			bool *b=new bool;
			*b=true;
			return b;

		}else if(str=="false"){
			bool *b=new bool;
			*b=false;
			return b;
		}

		// TODO: develop exception handler.
		return NULL;
	}

}
