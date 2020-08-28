/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

class MathBuiltIn{
public:
	static const float PI;
	static float degToRad(float *deg){ return (*deg*M_PI)/180.0f;}
	static float sin(float *rad){ return sinf(*rad);}
	static float cos(float *rad){ return cosf(*rad);}
	static float abs(float *n){ return fabs(*n);}
	static float pow(float *base, float *exp){return powf(*base,*exp);}

};



