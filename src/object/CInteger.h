#pragma once

class CInteger: public CObject{

public:

	int m_value;

	static CInteger * Parse(const string & );
	static int * ParsePrimitive(const string & );

	CInteger();
	CInteger(float v);

	virtual string toString();
	
};
