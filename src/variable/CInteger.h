#pragma once

class CInteger: public CVariable{

public:
	int m_value;


	static CInteger * Parse(const string & );
	static int * ParsePrimitive(const string & );

	CInteger();


	

};
