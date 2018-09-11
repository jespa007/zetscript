namespace zetscript{

	class CEval{
	public:

		char * evalString(const char *s, int & m_line, CScope *scope_info);

	private:

		char * evalString_Recursive(const char *s, int & m_line, CScope *scope_info, bool & error);
	};

}
