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


Keyword (K)
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

B <- [E;|K]* // A set of expressions ended with ; or Keyword

*/



#ifdef  __ZETSCRIPT_VERBOSE_MESSAGE__

#define print_eval_cr ZS_PRINT_DEBUG
#else
#define print_eval_cr(s,...)
#endif

#include "eval_data.cpp"
#include "eval_object.cpp"
#include "eval_expression.cpp"
#include "eval_keyword.cpp"

namespace zetscript{

	namespace eval{

		void eval(ZetScript *zs,const char * str, const char *  _filename, int _line){
			EvalData *eval_data=new EvalData(zs);
			int line =_line;
			eval_data->current_parsing_file=_filename;
			char *end_char = NULL;

			push_function(eval_data,MAIN_FUNCTION(eval_data));
			end_char=eval_recursive(eval_data,str,line,MAIN_SCOPE(eval_data));
			pop_function(eval_data);

			delete eval_data;

			if(*end_char != 0){
				THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line,"Unexpected \'%c\' ",*end_char);
			}

		}

		Scope * eval_new_scope(EvalData *eval_data, Scope *scope_parent){
			Scope *new_scope = NEW_SCOPE(eval_data,scope_parent);
			scope_parent->registered_scopes->push_back((intptr_t)new_scope);

			new_scope->tmp_idx_instruction_push_scope=(int)eval_data->current_function->instructions.size();

			return new_scope;
		}

		void inc_jmp_codes(EvalData *eval_data, int idx_start_instruction, int idx_end_instruction, unsigned inc_value){

			if(idx_end_instruction>eval_data->current_function->instructions.size()){
				THROW_RUNTIME_ERROR("idx_end_instruction out of bounds ");
			}
			EvalInstruction **it=&eval_data->current_function->instructions[idx_start_instruction];
			unsigned size = eval_data->current_function->instructions.size();

			for(unsigned i=idx_start_instruction; i < size; i++,it++){
				switch((*it)->vm_instruction.byte_code){
				case BYTE_CODE_JMP:
				case BYTE_CODE_JNT:
				case BYTE_CODE_JE:
				case BYTE_CODE_JT:
					(*it)->vm_instruction.value_op2+=inc_value;
					break;
				default: // ignore
					break;
				}
			}
		}

		void eval_check_scope(EvalData *eval_data, Scope *scope, unsigned idx_instruction_start){
			if(scope->registered_symbols->count > 0){ // if there's local symbols insert push/pop scope for there symbols
				if(scope->tmp_idx_instruction_push_scope!=ZS_IDX_UNDEFINED){
					eval_data->current_function->instructions.insert(
							eval_data->current_function->instructions.begin()+scope->tmp_idx_instruction_push_scope
							,new EvalInstruction(BYTE_CODE_PUSH_SCOPE,0,(intptr_t)scope)
					);

					inc_jmp_codes(eval_data,idx_instruction_start,eval_data->current_function->instructions.size(),1);

					// and finally insert pop scope
					eval_data->current_function->instructions.push_back(new EvalInstruction(BYTE_CODE_POP_SCOPE,0));
				}
			}
			else{ // remove scope
				scope->markAsUnusued();
			}
		}

		char * eval_block(EvalData *eval_data,const char *s,int & line,  Scope *scope_info){
			// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
			char *aux_p = (char *)s;

			Scope *new_scope_info=  NULL;
			IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

			// check for keyword ...
			if(*aux_p == '{'){

				unsigned idx_instruction_start_block=eval_data->current_function->instructions.size();

				aux_p++;

				new_scope_info = eval_new_scope(eval_data,scope_info); // special case... ast is created later ...


				if((aux_p = eval_recursive(
						eval_data
						,aux_p
						, line
						, new_scope_info
				)) != NULL){

					if(*aux_p != '}'){
						THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line,"Expected } ");
					}

					eval_check_scope(eval_data,new_scope_info,idx_instruction_start_block);
					return aux_p+1;
				}
			}
			return NULL;
		}

		char * eval_recursive(EvalData *eval_data,const char *s, int & line, Scope *scope_info, bool return_on_break_or_case){
			// PRE: *node_to_be_evaluated must be created (the pointer is only read mode)
			bool custom_quit = false;
			char *aux = (char *)s;
			char *end_expr=0;
			bool processed_directive=false;
			Keyword keyw;

			IGNORE_BLANKS(aux,eval_data,aux, line);

			while(*aux != 0 && !custom_quit){
				processed_directive=false;
				// ignore all ;
				while(*aux==';' && *aux != 0){
					IGNORE_BLANKS(aux,eval_data,aux+1, line);
				}

				if(*aux==0){ // custom case exit..
					return aux;
				}

				if(*aux == '}'){ // trivial cases...
					return aux;
				}else{

					// try directive ...
					Directive directive = is_directive(aux);
					char *start_var,* end_var;
					std::string str_symbol;
					if(directive != Directive::DIRECTIVE_UNKNOWN){
						switch(directive){
						case DIRECTIVE_INCLUDE:
							aux += strlen(eval_info_directives[directive].str);
							IGNORE_BLANKS(aux,eval_data,aux,line);
							if(*aux != '\"'){
								THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line,"expected starting \" directive");
							}
							aux++;
							start_var=aux;

							while(*aux != '\n' && *aux!=0 && !(*aux=='\"' && *(aux-1)!='\\')) aux++;

							if(*aux != '\"'){
								THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line,"expected end \" directive");
							}

							end_var=aux;

							zs_strutils::copy_from_ptr_diff(str_symbol,start_var,end_var);

							ZS_PRINT_DEBUG("include file: %s",str_symbol);

							// save current file info...
							eval_data->zs->evalFile(str_symbol);

							aux++;// advance ..
							break;
						default:
							THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line,"directive \"%s\" not supported",eval_info_directives[directive].str);
							break;
						}

						processed_directive = true;
						end_expr=aux;
					}
				}

				// 0st special case member class extension ...
				if(!processed_directive){ // not processed yet ...
					// 1st. check whether eval a keyword...
					if(((is_keyword(aux) == Keyword::KEYWORD_BREAK) || (is_keyword(aux) == Keyword::KEYWORD_CASE)) && return_on_break_or_case){ // it cuts current expression to link breaks...
						return end_expr;
					}

					if((end_expr = eval_keyword(
						eval_data
						,aux
						, line
						, scope_info
						, keyw
					)) == NULL){
						// If was unsuccessful then try to eval expression.
						// 2nd. check whether eval a block
						if((end_expr = eval_block(
								eval_data
								,aux
								,line
								, scope_info
								))==NULL){

							// 2nd. try expression
							//int starting_expression=line;

							//EvalInstruction *current_instruction=eval_data->current_function->instructions[eval_data->current_function->instructions.size()-1];

							if((end_expr = eval_expression(
									eval_data
									,aux
									,line
									, scope_info
									,&eval_data->current_function->instructions
							)) == NULL){ // something wrong was happen.
								THROW_RUNTIME_ERROR("eval_expression: unexpected NULL expression");
							}

							if(*end_expr != ';'){
								THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line,"expected end ;");
							}

							IGNORE_BLANKS(end_expr,eval_data,end_expr+1, line);
						}
					}
				}

				aux=end_expr;
				IGNORE_BLANKS(aux,eval_data,aux, line);
			}
			return aux;
		}

		void push_function(EvalData *eval_data,ScriptFunction *script_function){
			eval_data->functions.push_back(eval_data->current_function=new EvalFunction(script_function));
		}

		void pop_function(EvalData *eval_data){
			eval_data->current_function->script_function->instructions=NULL;

			// get total size op + 1 ends with NULL
			size_t size = (eval_data->current_function->instructions.size() + 1) * sizeof(Instruction);
			eval_data->current_function->script_function->instructions = (PtrInstruction)malloc(size);
			memset(eval_data->current_function->script_function->instructions, 0, size);

			for(unsigned i=0; i < eval_data->current_function->instructions.size(); i++){

				Symbol *vis=NULL;
				EvalInstruction *instruction = eval_data->current_function->instructions[i];
				LinkSymbolFirstAccess *ls=&instruction->link_symbol_first_access;

				if(ls->idx_script_function != ZS_IDX_UNDEFINED){ // solve first symbol first access...

					ScriptFunction *sf=GET_SCRIPT_FUNCTION(eval_data,ls->idx_script_function);
					ScriptClass *sc = GET_SCRIPT_CLASS(eval_data,sf->idx_class);

					if(instruction->vm_instruction.properties & MSK_INSTRUCTION_PROPERTY_SCOPE_TYPE_ACCESS){ // nothing to do...
						if(instruction->vm_instruction.byte_code==BYTE_CODE_CALL){
							instruction->vm_instruction.value_op2=ZS_IDX_INSTRUCTION_OP2_SOLVE_AT_RUNTIME;
						}
					}else if(instruction->vm_instruction.properties & MSK_INSTRUCTION_PROPERTY_SCOPE_TYPE_THIS){ // trivial this.

						// is automatically created on vm...
						if(ls->n_params==NO_PARAMS_IS_VARIABLE){ // it will search at runtime
							instruction->vm_instruction.value_op2=ZS_IDX_UNDEFINED;//vis->idx_symbol;
						}
						else{
							Symbol *symbol_function=sc->getSymbol(ls->value,ls->n_params);
							if(symbol_function==NULL){
								THROW_SCRIPT_ERROR(instruction->instruction_source_info.file,instruction->instruction_source_info.line,"Cannot find function %s::%s",sf->symbol.name.c_str(),ls->value.c_str());
								return;
							}

							// it stores the script function in the op...
							instruction->vm_instruction.value_op2=symbol_function->idx_position;
						}

					}else if(instruction->vm_instruction.properties & MSK_INSTRUCTION_PROPERTY_SCOPE_TYPE_SUPER){ // trivial super.

						Symbol *symbol_sf_foundf=NULL;
						std::string str_symbol_to_find = sf->symbol.name;

						for(int i = sf->symbol.idx_position-1; i >=0 && symbol_sf_foundf==NULL; i--){
							Symbol *symbol_member = (Symbol *)sc->symbol_members->items[i];
							if(symbol_member->symbol_properties & SYMBOL_PROPERTY_IS_SCRIPT_FUNCTION){
								ScriptFunction *sf=(ScriptFunction *)symbol_member->ref_ptr;
								if(
										(ls->n_params == sf->params->count)
									&& (str_symbol_to_find == symbol_member->name)
									){
									symbol_sf_foundf = symbol_member;
								}
							}
						}

						// ok get the super function...
						if(symbol_sf_foundf == NULL){
							THROW_SCRIPT_ERROR(instruction->instruction_source_info.file,instruction->instruction_source_info.line,"Cannot find super function %s::%s",sf->symbol.name.c_str(),ls->value.c_str());
							return;
						}
						instruction->vm_instruction.value_op2=symbol_sf_foundf->ref_ptr;

					}else if(sf->existArgumentName(ls->value)==ZS_IDX_UNDEFINED){ // not argument, try find local ...
						bool local_found=false;
						ScriptFunction *script_function_found=NULL;
						LoadType load_type=LoadType::LOAD_TYPE_UNDEFINED;
						int n_symbols_found=0;

						// try find local symbol  ...
						Scope *scope=ls->scope;
						Symbol * sc_var = scope->getSymbol(ls->value, ls->n_params);

						if(sc_var != NULL){ // local symbol found

							instruction->vm_instruction.properties |=MSK_INSTRUCTION_PROPERTY_SCOPE_TYPE_LOCAL;


							if(ls->n_params==NO_PARAMS_IS_VARIABLE){ // symbol is variable...
								if((vis=sf->getSymbol(sc_var->scope,ls->value))!=NULL){
									load_type=LoadType::LOAD_TYPE_VARIABLE;
									instruction->vm_instruction.value_op2=vis->idx_position;
									local_found=true;
								}
							}
							else{ // symbol is function...
								Symbol *function_symbol=sf->getSymbol(sc_var->scope,ls->value,ls->n_params,&n_symbols_found);
								if(function_symbol!=NULL){
									script_function_found=(ScriptFunction *)function_symbol->ref_ptr;
									instruction->vm_instruction.value_op2=function_symbol->idx_position; // store script function
									load_type=LoadType::LOAD_TYPE_FUNCTION;
									local_found =true;
								}
							}
							//}
						}

						if(!local_found){ // try global...
							// try symbol as var...
							Symbol * sc_var = MAIN_SCOPE(eval_data)->getSymbol(ls->value, NO_PARAMS_SYMBOL_ONLY);

							if(sc_var != NULL){

								if(sc_var->n_params==NO_PARAMS_IS_VARIABLE){
									load_type=LoadType::LOAD_TYPE_VARIABLE;
									instruction->vm_instruction.value_op2=sc_var->idx_position;
								}else{ // function if((sc_var = MAIN_FUNCTION(eval_data)->getSymbol(sc_var->scope,ls->value,ls->n_params))!=NULL){ // function
									// assign script function ...
									if(sc_var->symbol_properties & SYMBOL_PROPERTY_C_OBJECT_REF){ // get number symbols matching parameters
										sc_var=MAIN_FUNCTION(eval_data)->getSymbol(sc_var->scope,ls->value,ls->n_params,&n_symbols_found);
									}

									if(sc_var == NULL){
										THROW_RUNTIME_ERROR("internal error expected symbol");
									}

									script_function_found=(ScriptFunction *)sc_var->ref_ptr;
									instruction->vm_instruction.value_op2=sc_var->idx_position;
									load_type=LoadType::LOAD_TYPE_FUNCTION;
								}
							}else{
								THROW_SCRIPT_ERROR(instruction->instruction_source_info.file,instruction->instruction_source_info.line,"Cannot find symbol \"%s\"",ls->value.c_str());
							}
						}

						if(instruction->vm_instruction.byte_code == BYTE_CODE_LOAD){
							instruction->vm_instruction.value_op1=load_type;
						}


						// determine or not to solve at runtime for calling c functions with same symbol and different signatures
						if(instruction->vm_instruction.byte_code == BYTE_CODE_CALL){
							if(script_function_found != NULL){//instruction->vm_instruction.value_op2 != ZS_IDX_UNDEFINED){

								if((script_function_found->symbol.symbol_properties & SYMBOL_PROPERTY_C_OBJECT_REF) != 0 && n_symbols_found>1){ // function will be solved at run time because it has to check param type
									instruction->vm_instruction.value_op2=ZS_IDX_INSTRUCTION_OP2_SOLVE_AT_RUNTIME; // late binding, solve at runtime...
								}
							}
						}
					}
				}

				// save instruction ...
				eval_data->current_function->script_function->instructions[i]=instruction->vm_instruction;

				//------------------------------------
				// symbol value to save at runtime ...
				InstructionSourceInfo instruction_info=instruction->instruction_source_info;

				// Save str_symbol that was created on eval process, and is destroyed when eval finish.
				if(instruction->instruction_source_info.str_symbol != NULL){
					instruction_info.str_symbol=new std::string(*instruction->instruction_source_info.str_symbol);
				}

				eval_data->current_function->script_function->instruction_source_info[i]=instruction_info;

			}

			// delete and popback function information...
			delete eval_data->current_function;
			eval_data->functions.pop_back();

			eval_data->current_function = NULL;
			if(eval_data->functions.size() > 0){
				eval_data->current_function = eval_data->functions.at(eval_data->functions.size()-1);
			}
		}
	}
}
