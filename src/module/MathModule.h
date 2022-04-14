/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795f
#endif 



namespace zetscript{

	 extern const zs_float MathModule_PI;

	 zs_float MathModule_degToRad(ZetScript *_zs,zs_float *_deg);
	 zs_float MathModule_sin(ZetScript *_zs,zs_float *_rad);
	 zs_float MathModule_cos(ZetScript *_zs,zs_float *_rad);
	 zs_float MathModule_abs(ZetScript *_zs,zs_float *_n);
	 zs_float MathModule_pow(ZetScript *_zs,zs_float *_base, zs_float *_exp);
	 zs_float MathModule_random(ZetScript *_zs);
	 zs_float MathModule_max(ZetScript *_zs,zs_float *_n1,zs_float *_n2);
}

