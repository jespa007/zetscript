/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#include "zetscript.h"

//#include "zetscript.h"

/*

Constants (C)
-------------

C <= Integer,Number,Boolean,String


Pre operators (Pe)
-------------

Pe <= [ --, ++ ]

Post operators (Po)
-------------

Po <= [ --, ++ ]


Symbol (S)
-------------

S <=  [A-Za-Z ][A-Za-z0-9 ]*  --> token variable
S <=  [0-9]*.[0-9]* | [e|E][-|+][0-9]*  --> token integer or number
S <=  0x[0-9a-fA-F]* --> token hexadecimal
S <=  [0-1]*b --> token binary
S <= "[all characters]" --> token std::string


Punctuator (P)
-------------

Arithmetic

P <= [  /, *, %, -, + ]

Bit

P <= [  &, |, ^, <<, >> ]

Logical

P <= [  ==, >=, <=, >, <, !=, &&, || ]

Assignments

P <= [  =, +=, -=, /=, %=, *=, <<=, >>=, ^=, |=, &=]

Object

P <= [  . ]

Ternary

P <= [  ?,: ]


Anonymous Objects (O)
-------------

O <= [E1,E2,E3,...,En] 
O <= function(a1,a2,...,an){ B }
O <= {S1:E1,S2:E2,...,Sn:Em}

Expression (E)
-------------

E <= (E) // parentesis E
E <= E P E // punctuator
E <= !E // negate result of expression
E <= -E // Prerest (negate result of expression)
E <= +E // Presum (it does nothing)
E <= [Pe]S[Po] // variable with/out pre/post operator.
E <= (S|O)[E1][E2]...[En] // array access
E <= (S|O)(E1,E2,..,En) // function call
E(t)<= E; | E,


KeywordType (K)
-------------

- var S=E;
- for(var S1=Ei1,S2=Ei2,...,Sn=Eim;Ec;Ep1,Ep2,Ep3,...,Epk){ B } //
- for(var S1 in S2) { B } //
- while(Ec) { B }
- do{ B } while(Ec);
- switch(E){ // switch should have constants on case
	default:
	case C0:
		� break;
	case C1:
		� break;
		� ..
	case CN:
		� break;
	}

-if(Ec){ B1 } else if (Ec2) { B2 } else { B3 }


Body (B)
----------

Starts with '{' and ends with '}'

B <- [E;|K]* // A set of expressions ended with ; or KeywordType

*/



#ifdef  __ZETSCRIPT_VERBOSE_MESSAGE__

#define print_eval_cr ZS_PRINT_DEBUG
#else
#define print_eval_cr(s,...)
#endif

#include "eval_parser.cpp"
#include "eval_data.cpp"
#include "eval_object.cpp"
#include "eval_expression.cpp"
#include "eval_keyword.cpp"






namespace zetscript{

	void  writeError(const char *filename, int line, const  char  *string_text, ...);

	namespace eval{



		char * evalRecursive(EvalData *eval_data,const char *s, int & line, Scope *scope_info,  bool & error);


		bool eval(ZetScript *zs,const char * str, const char *  _filename, int _line){
			EvalData *eval_data=new EvalData(zs);
			bool error=false;
			int line =_line;
			eval_data->current_parsing_file=_filename;
			char *end_char = NULL;

			pushFunction(eval_data,MAIN_FUNCTION(eval_data));
			end_char=evalRecursive(eval_data,str,line,MAIN_SCOPE(eval_data),error);
			popFunction(eval_data);

			delete eval_data;

			if(*end_char != 0){
				writeError(eval_data->current_parsing_file,line,"Unexpected \'%c\' ",*end_char);
				THROW_SCRIPT_ERROR();
				return false;
			}

			return !error;
		}

		char * evalBlock(EvalData *eval_data,const char *s,int & line,  Scope *scope_info, bool & error, bool entry_function){
			// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
			char *aux_p = (char *)s;

			//Scope *_localScope =  scope_info != NULL ? scope_info->symbol_info.ast->scope_info_ptr: NULL;
			Scope *current_scope=  NULL;
			aux_p=ignoreBlanks(aux_p,line);

			// check for keyword ...
			if(*aux_p == '{'){
				aux_p++;

				current_scope =scope_info->getScopePtrCurrent();
				current_scope = scope_info->pushScope(); // special case... ast is created later ...

				if(entry_function==false){
					eval_data->current_evaluated_function->instructions.push_back(new EvaluatedInstruction(BYTE_CODE_PUSH_SCOPE));
				}

				if((aux_p = evalRecursive(
						eval_data
						,aux_p
						, line
						, current_scope,error
				)) != NULL){
					if(error){
						return NULL;
					}

					if(*aux_p != '}'){
						error = true;
						writeError(eval_data->current_parsing_file,line,"Expected } ");
						return NULL;
					}


						scope_info->popScope();

					if(entry_function==false){
						eval_data->current_evaluated_function->instructions.push_back(new EvaluatedInstruction(BYTE_CODE_POP_SCOPE));
					}
					return aux_p+1;
				}
			}
			return NULL;
		}

		char * evalRecursive(EvalData *eval_data,const char *s, int & line, Scope *scope_info,  bool & error){
			// PRE: *node_to_be_evaluated must be created (the pointer is only read mode)
			bool custom_quit = false;
			char *aux = (char *)s;
			char *end_expr=0;
			bool processed_directive=false;

			aux=ignoreBlanks(aux, line);

			while(*aux != 0 && !custom_quit){

				processed_directive=false;
				//children = NULL;
				// ignore all ;
				while(*aux==';' && *aux != 0){
					aux =ignoreBlanks(aux+1, line);
				}

				if(*aux==0){ // custom case exit..
					return aux;
				}

				if(*aux == '}'){ // trivial cases...
					return aux;
				}else{

					// try directive ...
					DirectiveType directive = isDirectiveType(aux);
					char *start_var,* end_var;
					std::string str_symbol;
					if(directive != DirectiveType::DIRECTIVE_TYPE_UNKNOWN){
						switch(directive){
						case DIRECTIVE_TYPE_INCLUDE:
							aux += strlen(eval_info_directives[directive].str);
							aux = ignoreBlanks(aux,line);
							if(*aux != '\"'){
								writeError(eval_data->current_parsing_file,line,"expected starting \" directive");
								THROW_SCRIPT_ERROR();
								return NULL;
							}
							aux++;
							start_var=aux;

							while(*aux != '\n' && *aux!=0 && !(*aux=='\"' && *(aux-1)!='\\')) aux++;

							if(*aux != '\"'){
								writeError(eval_data->current_parsing_file,line,"expected end \" directive");
								THROW_SCRIPT_ERROR();
								return NULL;
							}

							end_var=aux;

							if(!zs_strutils::copyFromPointerDiff(str_symbol,start_var,end_var)){
								THROW_SCRIPT_ERROR();
								return NULL;
							}

							ZS_PRINT_DEBUG("include file: %s",str_symbol);

							{
								// save current file info...
								try{
									eval_data->zs->evalFile(str_symbol);
								}catch(exception::ScriptExceptionError & error){
									THROW_EXCEPTION(error);
									return NULL;
								}
							}

							aux++;// advance ..
							break;
						default:
							writeError(eval_data->current_parsing_file,line,"directive \"%s\" not supported",eval_info_directives[directive].str);
							break;
						}

						processed_directive = true;
						end_expr=aux;
					}
				}

				// 0st special case member class extension ...
				if(!processed_directive){ // not processed yet ...
					// 1st. check whether eval a keyword...
					if((end_expr = evalKeywordType(
						eval_data
						,aux
						, line
						, scope_info
						, error
					)) == NULL){

						// If was unsuccessful then try to eval expression.
						if(error){

							THROW_SCRIPT_ERROR();
							return NULL;
						}
						// 2nd. check whether eval a block
						if((end_expr = evalBlock(
								eval_data
								,aux
								,line
								, scope_info
								, error
								))==NULL){

							// If was unsuccessful then try to eval expression.
							if(error){
								THROW_SCRIPT_ERROR();
								return NULL;
							}
							// 2nd. try expression
							//int starting_expression=line;

							if((end_expr = evalExpression(
									eval_data
									,aux
									,line
									, scope_info
									,&eval_data->current_evaluated_function->instructions
							)) == NULL){ // something wrong was happen.
								THROW_SCRIPT_ERROR();
								return NULL;
							}

							if(*end_expr != ';'){

								writeError(eval_data->current_parsing_file,line,"expected end ;");
								THROW_SCRIPT_ERROR();
								return NULL;
							}

							end_expr=ignoreBlanks(end_expr+1, line);
						}
					}
				}

				aux=end_expr;
				aux=ignoreBlanks(aux, line);
			}
			return aux;
		}

		void pushFunction(EvalData *eval_data,ScriptFunction *script_function){
			eval_data->evaluated_functions.push_back(eval_data->current_evaluated_function=new EvaluatedFunction(script_function));
		}

		void popFunction(EvalData *eval_data){
			eval_data->current_evaluated_function->script_function->instruction=NULL;

			// get total size op + 1 ends with NULL
			size_t size = (eval_data->current_evaluated_function->instructions.size() + 1) * sizeof(Instruction);
			eval_data->current_evaluated_function->script_function->instruction = (PtrInstruction)malloc(size);
			memset(eval_data->current_evaluated_function->script_function->instruction, 0, size);

			for(unsigned i=0; i < eval_data->current_evaluated_function->instructions.size(); i++){

				SymbolInfo *vis=NULL;
				EvaluatedInstruction *evaluated_instruction = eval_data->current_evaluated_function->instructions[i];
				LinkSymbolFirstAccess *ls=&evaluated_instruction->link_symbol_first_access;

				if(ls->idx_script_function != ZS_IDX_UNDEFINED){ // solve first symbol first access...

					ScriptFunction *sf=GET_SCRIPT_FUNCTION(eval_data,ls->idx_script_function);
					ScriptClass *sc = GET_SCRIPT_CLASS(eval_data,sf->idx_class);


					if(evaluated_instruction->vm_instruction.properties & MSK_INSTRUCTION_PROPERTY_SCOPE_TYPE_THIS){ // trivial this.

						// is automatically created on vm...
						if(ls->n_params==NO_PARAMS_IS_VARIABLE){
							/*if((vis=sc->getVariable(ls->value,sc->symbol_info.symbol->idx_scope))==0){
								THROW_RUNTIME_ERROR(zs_strutils::format("Cannot find variable %s::%s",sf->symbol_info.symbol->name.c_str(),ls->value.c_str()));
								return;
							}*/
							evaluated_instruction->vm_instruction.value_op2=ZS_IDX_UNDEFINED;//vis->idx_symbol;
						}
						else{
							if((evaluated_instruction->vm_instruction.value_op2=(intptr_t)sc->getFunction(ls->value,sc->symbol_info.symbol->idx_scope,ls->n_params))==0){
								THROW_RUNTIME_ERROR(zs_strutils::format("Cannot find function %s::%s",sf->symbol_info.symbol->name.c_str(),ls->value.c_str()));
								return;
							}
						}

					}else if(evaluated_instruction->vm_instruction.properties & MSK_INSTRUCTION_PROPERTY_SCOPE_TYPE_SUPER){ // trivial super.

						ScriptFunction *sf_found=NULL;
						std::string str_symbol_to_find = sf->symbol_info.symbol->name;

						for(int i = sf->symbol_info.idx_symbol-1; i >=0 && sf_found==NULL; i--){

							if(
									(ls->n_params == (int)sc->local_function[i]->arg_info.size())
								&& (str_symbol_to_find == sc->local_function[i]->symbol_info.symbol->name)
								){
								sf_found = sc->local_function[i];
							}
						}

						// ok get the super function...
						if(sf_found == NULL){
							THROW_RUNTIME_ERROR(zs_strutils::format("Cannot find super function %s::%s",sf->symbol_info.symbol->name.c_str(),ls->value.c_str()));
							return;
						}
						evaluated_instruction->vm_instruction.value_op2=(intptr_t)sf_found;

					}else if(sf->existArgumentName(ls->value)==ZS_IDX_UNDEFINED){ // not argument ...
						// trivial super.else{ // find local/global/argument var/function ...
						bool local_found=false;
						LoadType load_type=LoadType::LOAD_TYPE_UNDEFINED;

						// try find local symbol  ...
						Scope *scope=GET_SCOPE(eval_data,ls->idx_scope);
						Symbol * sc_var = scope->getSymbol(ls->value, ls->n_params);

						// search for local
						if(sc_var != NULL){
							if(ls->n_params==NO_PARAMS_IS_VARIABLE){
								if((vis=sf->getVariable(ls->value,sc_var->idx_scope))!=NULL){
									load_type=LoadType::LOAD_TYPE_VARIABLE;
									evaluated_instruction->vm_instruction.value_op2=vis->idx_symbol;
									local_found=true;
								}
							}
							else{

								if((evaluated_instruction->vm_instruction.value_op2=(intptr_t)sf->getFunction(ls->value,sc_var->idx_scope,ls->n_params))!=0){
									load_type=LoadType::LOAD_TYPE_FUNCTION;
									local_found =true;
								}
							}
						}

						if(!local_found){ // try global...
							Symbol * sc_var = MAIN_SCOPE(eval_data)->getSymbol(ls->value, ls->n_params);

							if(sc_var != NULL){
								if(ls->n_params==NO_PARAMS_IS_VARIABLE){

									if((vis=MAIN_FUNCTION(eval_data)->getVariable(ls->value,sc_var->idx_scope))==NULL){
										THROW_RUNTIME_ERROR(zs_strutils::format("Cannot find variable \"%s\"",ls->value.c_str()));
										return;
									}

									load_type=LoadType::LOAD_TYPE_VARIABLE;
									evaluated_instruction->vm_instruction.value_op2=vis->idx_symbol;
								}
								else{

									if((evaluated_instruction->vm_instruction.value_op2=(intptr_t)MAIN_FUNCTION(eval_data)->getFunction(ls->value,sc_var->idx_scope,ls->n_params))==0){
										THROW_RUNTIME_ERROR(zs_strutils::format("Cannot find function \"%s\"",ls->value.c_str()));
										return;
									}

									load_type=LoadType::LOAD_TYPE_FUNCTION;
								}
							}
						}

						evaluated_instruction->vm_instruction.value_op1=load_type;

						if(load_type==LoadType::LOAD_TYPE_FUNCTION){
							ScriptFunction *sf = ((ScriptFunction *)evaluated_instruction->vm_instruction.value_op2);
							if((sf->symbol_info.symbol_info_properties & SYMBOL_INFO_PROPERTY_C_OBJECT_REF) != 0){ // function will be solved at run time because it has to check param type
								evaluated_instruction->vm_instruction.value_op2=ZS_IDX_SYMBOL_SOLVE_AT_RUNTIME; // late binding, solve at runtime...
							}
						}
					}
				}

				// save instruction ...

				eval_data->current_evaluated_function->script_function->instruction[i]=evaluated_instruction->vm_instruction;


				// symbol value to save at runtime ...
				if(evaluated_instruction->instruction_source_info.str_symbol != NULL){
					eval_data->current_evaluated_function->script_function->instruction_source_info[i]=evaluated_instruction->instruction_source_info;
				}
			}

			eval_data->current_evaluated_function->script_function->linkScopeBlockVars();

			// delete and popback function information...
			delete eval_data->current_evaluated_function;
			eval_data->evaluated_functions.pop_back();

			eval_data->current_evaluated_function = NULL;
			if(eval_data->evaluated_functions.size() > 0){
				eval_data->current_evaluated_function = eval_data->evaluated_functions.at(eval_data->evaluated_functions.size()-1);
			}
		}
	}

}


