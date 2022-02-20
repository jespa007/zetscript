/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

#define ZS_ONE_OVER_RAND_MAX	(1.0f/(float)(RAND_MAX))

namespace zetscript{

	const zs_float MathModule_PI=M_PI;

	 zs_float MathModule_degToRad(ZetScript *_zs,zs_float *deg){
		return (*deg*M_PI)/180.0f;
	}

	 zs_float MathModule_sin(ZetScript *_zs,zs_float *rad){
		return sinf(*rad);
	}

	 zs_float MathModule_cos(ZetScript *_zs,zs_float *rad){
		return cosf(*rad);
	}

	 zs_float MathModule_abs(ZetScript *_zs,zs_float *n){
		return fabs(*n);
	}

	 zs_float MathModule_pow(ZetScript *_zs,zs_float *base, zs_float *exp){
		return powf(*base,*exp);
	}

	 zs_float MathModule_random(ZetScript *_zs){
		return rand()*ZS_ONE_OVER_RAND_MAX;
	}
}
