/*
 * CBooleanObjectFactory.cpp
 *
 *  Created on: Jun 29, 2014
 *      Author: jespada
 */

#include "script/zg_script.h"

REGISTER_FACTORY_FUNCTIONS(CBoolean);

void CBooleanFactory::registerScriptFunctions(){
	registerOperator("&&",[](CBoolean * n1,CBoolean *n2){

		CBoolean *result= NEW_BOOLEAN();
		result->m_value = n1->m_value && n2->m_value;
		return result;
	});


	registerOperator("||",[](CBoolean * n1,CBoolean *n2){

		CBoolean *result= NEW_BOOLEAN();
		result->m_value = n1->m_value || n2->m_value;
		return result;
	});

}
