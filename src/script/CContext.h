#pragma once

class CContext{


	vector<CScriptClass *> m_function; // 0 is the main scope or main function.
public:

	CContext();

	bool eval(const string & s);
	bool executeFunction(string & function, CObject * argv[]);

	~CContext();

};
