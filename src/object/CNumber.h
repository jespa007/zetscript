#pragma once

class CNumber: public CVariable<float>{

public:


	static CNumber * Parse(const string & );
	static float * ParsePrimitive(const string & );

	CNumber();
	CNumber(float v);

	virtual string toString();
	
};
