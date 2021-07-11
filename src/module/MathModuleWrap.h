/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795f
#endif 

#define ONE_OVER_RAND_MAX	(1.0f/(float)(RAND_MAX))

namespace zetscript{

	 extern const zs_float MathModuleWrap_PI;

	 zs_float MathModuleWrap_degToRad(zs_float *deg);
	 zs_float MathModuleWrap_sin(zs_float *rad);
	 zs_float MathModuleWrap_cos(zs_float *rad);
	 zs_float MathModuleWrap_abs(zs_float *n);
	 zs_float MathModuleWrap_pow(zs_float *base, zs_float *exp);
	 zs_float MathModuleWrap_random();
}

