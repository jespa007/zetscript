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

	class ModuleMath{
	public:
		static const zs_float PI;

		static zs_float degToRad(zs_float *deg){
			return (*deg*M_PI)/180.0f;
		}

		static zs_float sin(zs_float *rad){
			return sinf(*rad);
		}

		static zs_float cos(zs_float *rad){
			return cosf(*rad);
		}

		static zs_float abs(zs_float *n){
			return fabs(*n);
		}

		static zs_float pow(zs_float *base, zs_float *exp){
			return powf(*base,*exp);
		}

		static zs_float random(){
			return rand()*ONE_OVER_RAND_MAX;
		}
	};


}

