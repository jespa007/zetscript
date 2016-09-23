/*

 *
 *  Created on: Jun 27, 2016
 *      Author: jespada
 */

#pragma once


class 	CVariable{

public:



	enum VAR_TYPE{
		UNDEFINED=0,
		OBJECT,
		BOOLEAN,
		INTEGER,
		NUMBER, // real
		STRING,

		// function
		FUNCTION,
		VECTOR,
		MAX_VAR_TYPES
	};

	string m_strValue;


	CVariable();
	//CVariable(int idxClass, void *ptr_obj, const string & classNamePtr);

	void setPtr(void *ptr);

	/*
    void setClassStr(const string & _class) {
        m_classStr=_class;
    }

    void setPointerClassStr(const string & _class) {
        m_pointerClassStr=_class;
    }

    const string &  getClassStr() {
        return "YOU MUST ";
    }

    const string &  getPointerClassStr() {
        return m_pointerClassStr;
    }*/

    virtual string toString(){
    	return m_strValue;
    }

	VAR_TYPE getVariableType();


	virtual ~CVariable(){};


protected:


	void *m_ptr; // object pointer
	//int idxClass;
	VAR_TYPE m_varType;
    string m_classStr,m_pointerClassStr;

};



