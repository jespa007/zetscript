/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "@base.h"

namespace zetscript{
	bool * Boolean::parse(const String & s){

		if(StringUtils::toLower(s)=="true"){
			bool *b=new bool;
			*b=true;
			return b;

		}else if(StringUtils::toLower(s)=="false"){
			bool *b=new bool;
			*b=false;
			return b;
		}

		// TODO: develop exception handler.
		return NULL;
	}

}
