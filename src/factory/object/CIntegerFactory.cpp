/*
 * CIntegerObjectFactory.cpp
 *
 *  Created on: Jun 29, 2014
 *      Author: jespada
 */

#include "script/zg_script.h"

REGISTER_FACTORY_FUNCTIONS(CInteger);




void CIntegerFactory::registerScriptFunctions(){

	// Define operations for CInteger.

	// registers operator + to CInteger object.
	/*registerOperator("+",[](CInteger * n1,CInteger *n2){

		CInteger *result= NEW_NUMBER();
		result->m_value = n1->m_value + n2->m_value;
		return result;
	});

	// registers operator - to CInteger object.
	registerOperator("+",[](CInteger *n1){
		CInteger *result=NEW_NUMBER();
		result->m_value = +n1->m_value;
		return result;
	});



	// registers operator - to CInteger object.
	registerOperator("-",[](CInteger *n1,CInteger *n2){
		CInteger *result=NEW_NUMBER();
		result->m_value = n1->m_value - n2->m_value;
		return result;
	});

	// registers operator - to CInteger object.
	registerOperator("-",[](CInteger *n1){
		CInteger *result=NEW_NUMBER();
		result->m_value = -n1->m_value;
		return result;
	});


	// registers operator / to CInteger object.
	registerOperator("/",[](CInteger *n1,CInteger *n2){
		CInteger *result= NEW_NUMBER();
		result->m_value = n1->m_value / n2->m_value;
		return result;
	});


	// registers operator * to CInteger object.
	registerOperator("*",[](CInteger *n1,CInteger *n2){

		CInteger *result= NEW_NUMBER();
		result->m_value = n1->m_value * n2->m_value;
		return result;
	});


	// registers operator * to CInteger object.
	registerOperator("==",[](CInteger *n1,CInteger *n2){

		CBoolean *result= NEW_BOOLEAN();
		result->m_value = n1->m_value == n2->m_value;
		return result;
	});

	// registers operator * to CInteger object.
	registerOperator(">=",[](CInteger *n1,CInteger *n2){

		CBoolean *result= NEW_BOOLEAN();
		result->m_value = n1->m_value >= n2->m_value;
		return result;
	});

	registerOperator("<=",[](CInteger *n1,CInteger *n2){

		CBoolean *result= NEW_BOOLEAN();
		result->m_value = n1->m_value <= n2->m_value;
		return result;
	});


	// registers operator * to CInteger object.
	registerOperator(">",[](CInteger *n1,CInteger *n2){

		CBoolean *result= NEW_BOOLEAN();
		result->m_value = n1->m_value > n2->m_value;
		return result;
	});

	registerOperator("<",[](CInteger *n1,CInteger *n2){

		CBoolean *result= NEW_BOOLEAN();
		result->m_value = n1->m_value < n2->m_value;
		return result;
	});

	// registers operator - to CInteger object.
	registerOperator("=",[](CInteger *dst,CInteger *src){

		dst->m_value = src->m_value;

		return dst;

	});
*/

	// registers operator % to CInteger object.
	/*registerOperator("%",[](CInteger *n1,CInteger *n2){
		CInteger *result= NEW_NUMBER();
		result->m_value = n1->m_value % (int)n2->m_value;
		return result;
	});*/


}
