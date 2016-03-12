class CContext{

	CLocalScope *m_localScope;

public:

	CContext();

	bool eval(const string & s);
	bool execute();

	~CContext();

};
