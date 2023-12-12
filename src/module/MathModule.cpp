/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

#define ZS_ONE_OVER_RAND_MAX	(1.0f/(float)(RAND_MAX))



namespace zetscript{
	static bool srand_init=false;

	zs_float MathModule_PI(ScriptEngine *_se){
		 ZS_UNUSUED_PARAM(_se);
		return M_PI;
	}

	 zs_float MathModule_degToRad(ScriptEngine *_se,zs_float *deg){
		 ZS_UNUSUED_PARAM(_se);
		return (*deg*M_PI)/180.0f;
	}

	 zs_float MathModule_sin(ScriptEngine *_se,zs_float *rad){
		 ZS_UNUSUED_PARAM(_se);
		return sinf(*rad);
	}

	 zs_float MathModule_cos(ScriptEngine *_se,zs_float *rad){
		 ZS_UNUSUED_PARAM(_se);
		return cosf(*rad);
	}

	 zs_float MathModule_abs(ScriptEngine *_se,zs_float *n){
		 ZS_UNUSUED_PARAM(_se);
		return fabs(*n);
	}

	 zs_float MathModule_pow(ScriptEngine *_se,zs_float *base, zs_float *exp){
		 ZS_UNUSUED_PARAM(_se);
		return powf(*base,*exp);
	}

	 zs_float MathModule_random(ScriptEngine *_se){
		 ZS_UNUSUED_PARAM(_se);

		 if(srand_init==false){
			 // initialize random seed:
			 srand (time(NULL));

			 srand_init=true;
		 }

		return rand()*ZS_ONE_OVER_RAND_MAX;
	}

	 zs_float MathModule_max(ScriptEngine *_se,zs_float *_n1,zs_float *_n2){
		 ZS_UNUSUED_PARAM(_se);
		 return *_n1>*_n2?*_n1:*_n2;
	 }

	 zs_float MathModule_min(ScriptEngine *_se,zs_float *_n1,zs_float *_n2){
		 ZS_UNUSUED_PARAM(_se);
		 return *_n1<*_n2?*_n1:*_n2;
	 }

	 zs_float MathModule_sqrt(ScriptEngine *_se,zs_float *_n){
		 ZS_UNUSUED_PARAM(_se);
		 return sqrt(*_n);
	 }

	 zs_float MathModule_floor(ScriptEngine *_se,zs_float *_n){
		 ZS_UNUSUED_PARAM(_se);
		 return floor(*_n);
	 }

	 zs_float MathModule_ceil(ScriptEngine *_se,zs_float *_n){
		 ZS_UNUSUED_PARAM(_se);
		 return ceil(*_n);
	 }

	 zs_float MathModule_round(ScriptEngine *_se,zs_float *_n){
		 ZS_UNUSUED_PARAM(_se);
		 return round(*_n);
	 }
}
