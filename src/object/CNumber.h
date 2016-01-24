#pragma once

class CNumber: public CObject{

public:

	double m_value;

	static CNumber * Parse(const string & );

	CNumber();
	CNumber(double v);

	virtual string toString();
	
};
