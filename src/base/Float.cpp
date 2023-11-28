/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "@base.h"

namespace zetscript{


	zs_float *  Float::parse(const String & s){

		char *end;
		char *data=(char *)s.toConstChar();
		float  l;
		errno = 0;
		l = strtof(data, &end);

		if (end == data){
			return NULL;
		}else	if ((errno == ERANGE && l == FLT_MAX) || l > FLT_MAX) {
			return NULL;
		}else if ((errno == ERANGE && l < FLT_MIN) /* || l < FLT_MIN*/) {
			return NULL;
		}
		if (*s.toConstChar() == '\0' || *end != '\0') {
			return NULL;
		}

		return new zs_float(l);
	}

	String Float::toString(zs_float _number){

		char float_str[100000];

		sprintf(float_str,"%f",_number);

	   return String(float_str);
	}
}
