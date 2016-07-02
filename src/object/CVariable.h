/*

 *
 *  Created on: Jun 27, 2016
 *      Author: jespada
 */

#pragma once


class CVariable: public CObject{

public:

	enum VAR_TYPE{
		OBJECT=0,
		BOOLEAN,
		INTEGER,
		NUMBER, // real
		STRING,

		// function
		FUNCTION,
		VECTOR,
		MAX_VAR_TYPES
	};


	CVariable();

	void setPtr(void *ptr);

	VAR_TYPE getVariableType();



protected:


	void *m_ptr;
	VAR_TYPE m_varType;

};



