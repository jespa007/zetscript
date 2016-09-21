#pragma once

class CNumber: public CVariable{

public:
	float m_value;

	static CNumber * Parse(const string & );
	static float * ParsePrimitive(const string & );

	CNumber();


	

};
