#pragma once

class CNumber: public CObject{

public:

	float m_value;

	static CNumber * Parse(const string & );
	static float * ParsePrimitive(const string & );

	CNumber();
	CNumber(float v);

	virtual string toString();
	
};
