/*
 * CVectorObjectFactory.cpp
 *
 *  Created on: Jun 29, 2014
 *      Author: jespada
 */

#include "script/zg_script.h"

REGISTER_FACTORY_FUNCTIONS(CVector);

void CVectorFactory::registerScriptFunctions(){
	// registers operator * to CNumber object.
	/*registerOperator("+",[](CVector *n1,CNumber *n2){

		CVector *result= NEW_STRING();
		result->m_value = n1->m_value + std::to_string(n2->m_value);
		return result;
	});

	registerOperator("+",[](CVector *n1,CVector *n2){

		CVector *result= NEW_STRING();
		result->m_value = n1->m_value + n2->m_value;
		return result;
	});*/
}

