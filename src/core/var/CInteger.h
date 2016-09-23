#pragma once

class CInteger: public CScriptVariable{

public:
	int m_value;


	static CInteger * Parse(const string & );
	static int * ParsePrimitive(const string & );

	CInteger();


	

};
