/*
 * CStringObjectFactory.cpp
 *
 *  Created on: Jun 29, 2014
 *      Author: jespada
 */

#include "script/zg_script.h"

REGISTER_FACTORY_FUNCTIONS(CString);

void CStringFactory::registerScriptFunctions(){
	// registers operator * to CNumber object.
	registerOperator("+",[](CString *n1,CNumber *n2){

		CString *result= NEW_STRING();
		result->m_value = n1->m_value + std::to_string(n2->m_value);
		return result;
	});

	registerOperator("+",[](CString *n1,CString *n2){

		CString *result= NEW_STRING();
		result->m_value = n1->m_value + n2->m_value;
		return result;
	});
}

