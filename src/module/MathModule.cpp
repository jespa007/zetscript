/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

#define ZS_ONE_OVER_RAND_MAX	(1.0f/(float)(RAND_MAX))



namespace zetscript{
	static bool srand_init=false;

	const zs_float MathModule_PI=M_PI;

	 zs_float MathModule_degToRad(ZetScript *_zs,zs_float *deg){
		 ZS_UNUSUED_PARAM(_zs);
		return (*deg*M_PI)/180.0f;
	}

	 zs_float MathModule_sin(ZetScript *_zs,zs_float *rad){
		 ZS_UNUSUED_PARAM(_zs);
		return sinf(*rad);
	}

	 zs_float MathModule_cos(ZetScript *_zs,zs_float *rad){
		 ZS_UNUSUED_PARAM(_zs);
		return cosf(*rad);
	}

	 zs_float MathModule_abs(ZetScript *_zs,zs_float *n){
		 ZS_UNUSUED_PARAM(_zs);
		return fabs(*n);
	}

	 zs_float MathModule_pow(ZetScript *_zs,zs_float *base, zs_float *exp){
		 ZS_UNUSUED_PARAM(_zs);
		return powf(*base,*exp);
	}

	 zs_float MathModule_random(ZetScript *_zs){
		 ZS_UNUSUED_PARAM(_zs);

		 if(srand_init==false){
			 // initialize random seed:
			 srand (time(NULL));

			 srand_init=true;
		 }

		return rand()*ZS_ONE_OVER_RAND_MAX;
	}

	 zs_float MathModule_max(ZetScript *_zs,zs_float *_n1,zs_float *_n2){
		 ZS_UNUSUED_PARAM(_zs);
		 return *_n1>*_n2?*_n1:*_n2;
	 }

	 zs_float MathModule_sqrt(ZetScript *_zs,zs_float *_n){
		 ZS_UNUSUED_PARAM(_zs);
		 return sqrt(*_n);
	 }
}
