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

	 zs_float MathModule_degToRad(zs_float *deg);
	 zs_float MathModule_sin(zs_float *rad);
	 zs_float MathModule_cos(zs_float *rad);
	 zs_float MathModule_abs(zs_float *n);
	 zs_float MathModule_pow(zs_float *base, zs_float *exp);
	 zs_float MathModule_random();
}

