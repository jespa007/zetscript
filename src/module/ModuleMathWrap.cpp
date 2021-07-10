/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#include "zetscript.h"

namespace zetscript{

	const zs_float ModuleMathWrap_PI=M_PI;

	 zs_float ModuleMathWrap_degToRad(zs_float *deg){
		return (*deg*M_PI)/180.0f;
	}

	 zs_float ModuleMathWrap_sin(zs_float *rad){
		return sinf(*rad);
	}

	 zs_float ModuleMathWrap_cos(zs_float *rad){
		return cosf(*rad);
	}

	 zs_float ModuleMathWrap_abs(zs_float *n){
		return fabs(*n);
	}

	 zs_float ModuleMathWrap_pow(zs_float *base, zs_float *exp){
		return powf(*base,*exp);
	}

	 zs_float ModuleMathWrap_random(){
		return rand()*ONE_OVER_RAND_MAX;
	}
}
