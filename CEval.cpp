
++,-- PostFix decrement
() Function call
[] 	Array subscripting
. 	member access
++,-- 	Prefix increment and decrement 	Right-to-left
+,- 	Unary plus and minus
!,~ 	Logical NOT and bitwise NOT
(type) 	Type cast
*,/,% 	Multiplication, division, and remainder 	Left-to-right
+,- 	Addition and subtraction
,<<,>> 	Bitwise left shift and right shift
<,<= 	For relational operators < and ≤ respectively
>, >= 	For relational operators > and ≥ respectively
==,!= 	For relational = and ≠ respectively
& 	Bitwise AND
^ 	Bitwise XOR (exclusive or)
| 	Bitwise OR (inclusive or)
&& 	Logical AND
|| 	Logical OR
?: 	Ternary conditional[note 3] 	Right-to-Left
= 	Simple assignment
+=,-= 	Assignment by sum and difference
*=,/=,%= 	Assignment by product, quotient, and remainder
<<=,>>= 	Assignment by bitwise left shift and right shift
&=,^=,|= 	Assignment by bitwise AND, XOR, and OR
,	Comma

	char * CEval::evalString_Recursive(const char *s, int & m_line, CScope *scope_info, bool & error){

		// PRE: *node_to_be_evaluated must be created (the pointer is only read mode)

		KEYWORD_TYPE keyw=KEYWORD_TYPE::UNKNOWN_KEYWORD;
		bool custom_quit = false;
		char *aux = (char *)s;
		char *end_expr=0;
		PASTNode children=NULL;
		bool is_main_node = false;
		bool processed_directive=false;

		if(node_to_be_evaluated!= NULL){

			is_main_node=MAIN_AST_NODE==node_to_be_evaluated;

			if(is_main_node){
				char *test = IGNORE_BLANKS(s,DUMMY_LINE);

				// empty script ? return true anyways
				if(test == 0) return NULL;
			}

		}
		aux=IGNORE_BLANKS(aux, m_line);

		while(*aux != 0 && !custom_quit){

			processed_directive=false;
			children = NULL;
			// ignore all ;
			while(*aux==';' && *aux != 0){
				aux =IGNORE_BLANKS(aux+1, m_line);
			}

			if(*aux==0){ // custom case exit..
				return aux;
			}

			if(*aux == '}'){ // trivial cases...
				return aux;
			}else{

				// try directive ...
				DIRECTIVE_TYPE directive = isDirective(aux);
				char *start_var,* end_var,*symbol_name;
				if(directive != DIRECTIVE_TYPE::UNKNOWN_DIRECTIVE){
					switch(directive){
					case INCLUDE_DIRECTIVE:
						aux += strlen(defined_directive[directive].str);
						aux = IGNORE_BLANKS(aux,m_line);
						if(*aux != '\"'){
							writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,"expected starting \" directive");
							THROW_SCRIPT_ERROR();
							return NULL;
						}
						aux++;
						start_var=aux;

						while(*aux != '\n' && *aux!=0 && !(*aux=='\"' && *(aux-1)!='\\')) aux++;

						if(*aux != '\"'){
							writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,"expected end \" directive");
							THROW_SCRIPT_ERROR();
							return NULL;
						}

						end_var=aux;

						if((symbol_name=CZetScriptUtils::copyStringFromInterval(start_var,end_var)) == NULL){
							THROW_SCRIPT_ERROR();
							return NULL;
						}

						zs_print_debug_cr("include file: %s",symbol_name);

						{

							// save current file info...
							string current_file_str=CASTNode::current_parsing_filename;
							int current_file_idx=CASTNode::current_idx_parsing_filename;
							string file_to_parse=symbol_name;

							if(CZetScript::getInstance()->isFilenameAlreadyParsed(file_to_parse.c_str())){
								writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,"\"%s\" already parsed",file_to_parse.c_str());
								THROW_SCRIPT_ERROR();
								return NULL;
							}

							try{
								CZetScript::getInstance()->parse_file(file_to_parse.c_str());
							}catch(script_error & error){
								THROW_SCRIPT_ERROR();
								return NULL;
							}

							//restore current file info...
							CASTNode::current_parsing_filename=current_file_str.c_str();
							CASTNode::current_idx_parsing_filename=current_file_idx;
						}

						aux++;// advance ..
						break;
					default:
						writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,"directive \"%s\" not supported",defined_directive[directive].str);
						break;
					}

					processed_directive = true;
					end_expr=aux;
				}
			}

			// 0st special case member class extension ...
			if(children==NULL && !processed_directive){ // not processed yet ...
				// 1st. check whether parse a keyword...
				if((end_expr = parseKeyWord(aux, m_line, scope_info, error, node_to_be_evaluated != NULL ? &children : NULL)) == NULL){

					// If was unsuccessful then try to parse expression.
					if(error){
						manageOnErrorParse(node_to_be_evaluated);
						THROW_SCRIPT_ERROR();
						return NULL;
					}
					// 2nd. check whether parse a block
					if((end_expr = parseBlock(aux
							,m_line
							, scope_info
							, error
							,node_to_be_evaluated != NULL ? &children:NULL
							,node_to_be_evaluated))==NULL){

						// If was unsuccessful then try to parse expression.
						if(error){
							manageOnErrorParse(node_to_be_evaluated);
							THROW_SCRIPT_ERROR();
							return NULL;
						}
						// 2nd. try expression
						int starting_expression=m_line;

						if((end_expr = parseExpression(aux,m_line, scope_info,node_to_be_evaluated != NULL ? &children:NULL)) == NULL){ // something wrong was happen.
							manageOnErrorParse(node_to_be_evaluated);
							THROW_SCRIPT_ERROR();
							return NULL;
						}

						if(*end_expr == ')'){ // unexpected close parenthesis.
							error = true;
							writeErrorMsg(CURRENT_PARSING_FILENAME,starting_expression,"missing '('");
							manageOnErrorParse(node_to_be_evaluated);
							THROW_SCRIPT_ERROR();
							return NULL;
						}

						if(*end_expr != ';'){
							error = true;
							writeErrorMsg(CURRENT_PARSING_FILENAME,starting_expression,"Expected ';'");
							manageOnErrorParse(node_to_be_evaluated);
							THROW_SCRIPT_ERROR();
							return NULL;
						}
						end_expr++;
					}
				}

				// new expression ready to be evaluated...

				if(node_to_be_evaluated != NULL && children != NULL){
					(node_to_be_evaluated)->children.push_back(children->idxAstNode);
					children->idxAstParent = (node_to_be_evaluated)->idxAstNode;


					if(is_main_node){
						astNodeToCompile->push_back({(node_to_be_evaluated)->idxAstNode,children->idxAstNode});
					}
				}
			}

			aux=end_expr;
			aux=IGNORE_BLANKS(aux, m_line);
		}
		return aux;
	}
