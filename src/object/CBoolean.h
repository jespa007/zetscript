#pragma once

class CBoolean: public CVariable<bool>{
	

public:

	static CBoolean * Parse(const string & );
	static bool * ParsePrimitive(const string & );

	CBoolean();
	CBoolean(bool b);

	virtual string toString();
	
};
