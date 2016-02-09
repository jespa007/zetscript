/*
 * CNumberObjectFactory.cpp
 *
 *  Created on: Jun 29, 2014
 *      Author: jespada
 */

#include "script/zg_script.h"

REGISTER_FACTORY_FUNCTIONS(CNumber);




void CNumberFactory::registerScriptFunctions(){

	// Define operations for CNumber.

	// registers operator + to CNumber object.
	registerOperator("+",[](CNumber * n1,CNumber *n2){

		CNumber *result= NEW_NUMBER();
		result->m_value = n1->m_value + n2->m_value;
		return result;
	});

	// registers operator - to CNumber object.
	registerOperator("+",[](CNumber *n1){
		CNumber *result=NEW_NUMBER();
		result->m_value = +n1->m_value;
		return result;
	});



	// registers operator - to CNumber object.
	registerOperator("-",[](CNumber *n1,CNumber *n2){
		CNumber *result=NEW_NUMBER();
		result->m_value = n1->m_value - n2->m_value;
		return result;
	});

	// registers operator - to CNumber object.
	registerOperator("-",[](CNumber *n1){
		CNumber *result=NEW_NUMBER();
		result->m_value = -n1->m_value;
		return result;
	});


	// registers operator / to CNumber object.
	registerOperator("/",[](CNumber *n1,CNumber *n2){
		CNumber *result= NEW_NUMBER();
		result->m_value = n1->m_value / n2->m_value;
		return result;
	});


	// registers operator * to CNumber object.
	registerOperator("*",[](CNumber *n1,CNumber *n2){

		CNumber *result= NEW_NUMBER();
		result->m_value = n1->m_value * n2->m_value;
		return result;
	});


	// registers operator * to CNumber object.
	registerOperator("==",[](CNumber *n1,CNumber *n2){

		CBoolean *result= NEW_BOOLEAN();
		result->m_value = n1->m_value == n2->m_value;
		return result;
	});

	// registers operator * to CNumber object.
	registerOperator(">=",[](CNumber *n1,CNumber *n2){

		CBoolean *result= NEW_BOOLEAN();
		result->m_value = n1->m_value >= n2->m_value;
		return result;
	});

	registerOperator("<=",[](CNumber *n1,CNumber *n2){

		CBoolean *result= NEW_BOOLEAN();
		result->m_value = n1->m_value <= n2->m_value;
		return result;
	});


	// registers operator * to CNumber object.
	registerOperator(">",[](CNumber *n1,CNumber *n2){

		CBoolean *result= NEW_BOOLEAN();
		result->m_value = n1->m_value > n2->m_value;
		return result;
	});

	registerOperator("<",[](CNumber *n1,CNumber *n2){

		CBoolean *result= NEW_BOOLEAN();
		result->m_value = n1->m_value < n2->m_value;
		return result;
	});

	// registers operator - to CNumber object.
	registerOperator("=",[](CNumber *dst,CNumber *src){

		dst->m_value = src->m_value;

		return dst;

	});


	// registers operator % to CNumber object.
	/*registerOperator("%",[](CNumber *n1,CNumber *n2){
		CNumber *result= NEW_NUMBER();
		result->m_value = n1->m_value % (int)n2->m_value;
		return result;
	});*/


}
