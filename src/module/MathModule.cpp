/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

#define ZS_ONE_OVER_RAND_MAX	(1.0f/(float)(RAND_MAX))



namespace zetscript{
	static bool srand_init=false;

	zs_float MathModule_PI(ScriptEngine *_script_engine){
		 ZS_UNUSUED_PARAM(_script_engine);
		return M_PI;
	}

	 zs_float MathModule_degToRad(ScriptEngine *_script_engine,zs_float *deg){
		 ZS_UNUSUED_PARAM(_script_engine);
		return (*deg*M_PI)/180.0f;
	}

	 zs_float MathModule_sin(ScriptEngine *_script_engine,zs_float *rad){
		 ZS_UNUSUED_PARAM(_script_engine);
		return sinf(*rad);
	}

	 zs_float MathModule_cos(ScriptEngine *_script_engine,zs_float *rad){
		 ZS_UNUSUED_PARAM(_script_engine);
		return cosf(*rad);
	}

	 zs_float MathModule_abs(ScriptEngine *_script_engine,zs_float *n){
		 ZS_UNUSUED_PARAM(_script_engine);
		return fabs(*n);
	}

	 zs_float MathModule_pow(ScriptEngine *_script_engine,zs_float *base, zs_float *exp){
		 ZS_UNUSUED_PARAM(_script_engine);
		return powf(*base,*exp);
	}

	 zs_float MathModule_random(ScriptEngine *_script_engine){
		 ZS_UNUSUED_PARAM(_script_engine);

		 if(srand_init==false){
			 // initialize random seed:
			 srand (time(NULL));

			 srand_init=true;
		 }

		return rand()*ZS_ONE_OVER_RAND_MAX;
	}

	 zs_float MathModule_max(ScriptEngine *_script_engine,zs_float *_n1,zs_float *_n2){
		 ZS_UNUSUED_PARAM(_script_engine);
		 return *_n1>*_n2?*_n1:*_n2;
	 }

	 zs_float MathModule_min(ScriptEngine *_script_engine,zs_float *_n1,zs_float *_n2){
		 ZS_UNUSUED_PARAM(_script_engine);
		 return *_n1<*_n2?*_n1:*_n2;
	 }

	 zs_float MathModule_sqrt(ScriptEngine *_script_engine,zs_float *_n){
		 ZS_UNUSUED_PARAM(_script_engine);
		 return sqrt(*_n);
	 }

	 zs_float MathModule_floor(ScriptEngine *_script_engine,zs_float *_n){
		 ZS_UNUSUED_PARAM(_script_engine);
		 return floor(*_n);
	 }

	 zs_float MathModule_ceil(ScriptEngine *_script_engine,zs_float *_n){
		 ZS_UNUSUED_PARAM(_script_engine);
		 return ceil(*_n);
	 }

	 zs_float MathModule_round(ScriptEngine *_script_engine,zs_float *_n){
		 ZS_UNUSUED_PARAM(_script_engine);
		 return round(*_n);
	 }
}
