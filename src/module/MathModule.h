/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795f
#endif 



namespace zetscript{

	 zs_float MathModule_PI(ScriptEngine *_script_engine);
	 zs_float MathModule_degToRad(ScriptEngine *_script_engine,zs_float *_deg);
	 zs_float MathModule_sin(ScriptEngine *_script_engine,zs_float *_rad);
	 zs_float MathModule_cos(ScriptEngine *_script_engine,zs_float *_rad);
	 zs_float MathModule_abs(ScriptEngine *_script_engine,zs_float *_n);
	 zs_float MathModule_pow(ScriptEngine *_script_engine,zs_float *_base, zs_float *_exp);
	 zs_float MathModule_random(ScriptEngine *_script_engine);
	 zs_float MathModule_max(ScriptEngine *_script_engine,zs_float *_n1,zs_float *_n2);
	 zs_float MathModule_min(ScriptEngine *_script_engine,zs_float *_n1,zs_float *_n2);
	 zs_float MathModule_sqrt(ScriptEngine *_script_engine,zs_float *_n);
	 zs_float MathModule_floor(ScriptEngine *_script_engine,zs_float *_n);
	 zs_float MathModule_ceil(ScriptEngine *_script_engine,zs_float *_n);
	 zs_float MathModule_round(ScriptEngine *_script_engine,zs_float *_n);
}

