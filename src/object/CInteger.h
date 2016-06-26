#pragma once

class CInteger: public CVariable<int>{

public:



	static CInteger * Parse(const string & );
	static int * ParsePrimitive(const string & );

	CInteger();
	CInteger(float v);

	virtual string toString();
	
};
