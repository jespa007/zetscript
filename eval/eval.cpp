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
			bool error;
			std::string error_str;
			eval_data->current_parsing_file=_filename;
			push_function(eval_data,MAIN_FUNCTION(eval_data));
			eval_recursive(eval_data,str,line,MAIN_SCOPE(eval_data));
			pop_function(eval_data);

			error=eval_data->error;
			error_str=eval_data->error_str;

			delete eval_data;

			if(error){
				THROW_EXCEPTION(error_str);
			}

		}

		Scope * eval_new_scope(EvalData *eval_data, Scope *scope_parent, bool is_function){
			Scope *new_scope = NEW_SCOPE(eval_data,scope_parent);
			scope_parent->registered_scopes->push_back((zs_int)new_scope);
			new_scope->is_scope_function=is_function;
			if(is_function){
				new_scope->tmp_idx_instruction_push_scope=0;
			}
			else{
				new_scope->tmp_idx_instruction_push_scope=(int)eval_data->current_function->instructions.size();
			}

			return new_scope;
		}

		void inc_jmp_codes(EvalData *eval_data, int idx_start_instruction, int idx_end_instruction, unsigned inc_value){

			if(idx_end_instruction>(int)eval_data->current_function->instructions.size()){
				THROW_RUNTIME_ERROR("idx_end_instruction out of bounds ");
			}
			EvalInstruction **it=&eval_data->current_function->instructions[idx_start_instruction];
			size_t size = eval_data->current_function->instructions.size();

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
			if(scope->n_registered_symbols_as_variables > 0){ // if there's local symbols insert push/pop scope for there symbols
				if(scope->tmp_idx_instruction_push_scope!=ZS_IDX_UNDEFINED){
					eval_data->current_function->instructions.insert(
							eval_data->current_function->instructions.begin()+scope->tmp_idx_instruction_push_scope
							,new EvalInstruction(BYTE_CODE_PUSH_SCOPE,0,(zs_int)scope)
					);

					inc_jmp_codes(eval_data,idx_instruction_start,(int)(eval_data->current_function->instructions.size()),1);

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

				int idx_instruction_start_block=(int)(eval_data->current_function->instructions.size());

				aux_p++;

				new_scope_info = eval_new_scope(eval_data,scope_info); // special case... ast is created later ...

				if((aux_p = eval_recursive(
						eval_data
						,aux_p
						, line
						, new_scope_info
				)) != NULL){

					if(*aux_p != '}'){
						EVAL_ERROR(eval_data->current_parsing_file,line,"Expected '}' ");
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

			Keyword keyw;
			Directive directive;

			IGNORE_BLANKS(aux,eval_data,aux, line);

			while(*aux != 0 && !custom_quit){

				// ignore all ;
				while(*aux==';' && *aux != 0){
					IGNORE_BLANKS(aux,eval_data,aux+1, line);
				}

				if(*aux==0){ // custom case exit..
					return aux;
				}

				keyw=is_keyword(aux);

				if(*aux == '}'){ // ending block trivial cases...
					return aux;
				}else if(keyw != Keyword::KEYWORD_UNKNOWN && keyw < KEYWORDS_WITHIN_EXPRESSIONS){ // it cuts current expression to link breaks...

					if(((keyw == Keyword::KEYWORD_BREAK) || (keyw == Keyword::KEYWORD_CASE)) && return_on_break_or_case){
						return aux;
					}

					end_expr = eval_keyword(
						eval_data
						,aux
						, line
						, scope_info
						, keyw
					);

				}else if((directive = is_directive(aux)) != Directive::DIRECTIVE_UNKNOWN){ // eval directive
					// try directive ...
					char *start_var,* end_var;
					std::string str_symbol;
					if(directive != Directive::DIRECTIVE_UNKNOWN){
						switch(directive){
						case DIRECTIVE_INCLUDE:
							aux += strlen(eval_data_directives[directive].str);
							IGNORE_BLANKS(aux,eval_data,aux,line);
							if(*aux != '\"'){
								EVAL_ERROR(eval_data->current_parsing_file,line,"expected starting \" directive");
							}
							aux++;
							start_var=aux;

							while(*aux != '\n' && *aux!=0 && !(*aux=='\"' && *(aux-1)!='\\')) aux++;

							if(*aux != '\"'){
								EVAL_ERROR(eval_data->current_parsing_file,line,"expected end \" directive");
							}

							end_var=aux;

							zs_strutils::copy_from_ptr_diff(str_symbol,start_var,end_var);

							ZS_PRINT_DEBUG("include file: %s",str_symbol);

							// save current file info...
							eval_data->zs->evalFile(str_symbol);

							aux++;// advance ..
							break;
						default:
							EVAL_ERROR(eval_data->current_parsing_file,line,"directive \"%s\" not supported",eval_data_directives[directive].str);
							break;
						}

						end_expr=aux;
					}
				}else if(*aux == '{') { // eval block

					// 2nd. check whether eval a block
					end_expr = eval_block(
						eval_data
						,aux
						,line
						, scope_info
					);

				}else{ // eval expression

					end_expr = eval_expression(
						eval_data
						,aux
						,line
						, scope_info
						,&eval_data->current_function->instructions
						,{}
						,EVAL_EXPRESSION_PROPERTY_ALLOW_EXPRESSION_SEQUENCE
					);
				}

				if(end_expr == NULL){
					return NULL;
				}

				aux=end_expr;
				IGNORE_BLANKS(aux,eval_data,aux, line);
			}
			return aux;
		}

		void push_function(EvalData *eval_data,ScriptFunction *script_function){
			eval_data->functions.push_back(eval_data->current_function=new EvalFunction(script_function));
		}

		int pop_function(EvalData *eval_data){

			ScriptFunction *sf = eval_data->current_function->script_function;
			ScriptClass *sf_class = GET_SCRIPT_CLASS(eval_data,sf->idx_class);

			if(sf->instructions != NULL){
				free(sf->instructions);
				sf->instructions=NULL;
			}

			// get total size op + 1 ends with 0 (INVALID BYTE_CODE)
			size_t size = (eval_data->current_function->instructions.size() + 1) * sizeof(Instruction);
			sf->instructions = (PtrInstruction)malloc(size);
			memset(sf->instructions, 0, size);
			//bool is_static = eval_data->current_function->script_function->symbol.properties & SYMBOL_PROPERTY_STATIC;
			int ok=TRUE;
			//int idx_instruction=0;


			for(unsigned i=0; i < eval_data->current_function->instructions.size(); i++){

				Symbol *vis=NULL;
				EvalInstruction *instruction = eval_data->current_function->instructions[i];
				//is_static&=((instruction->vm_instruction.properties & MSK_INSTRUCTION_PROPERTY_ACCESS_TYPE_THIS)==0);

				if(		   ((	instruction->vm_instruction.properties & (MSK_INSTRUCTION_PROPERTY_ACCESS_TYPE_FIELD|MSK_INSTRUCTION_PROPERTY_ACCESS_TYPE_VECTOR)) == 0)
					    && ((	instruction->vm_instruction.byte_code == ByteCode::BYTE_CODE_LOAD_TYPE_FIND) || (	instruction->vm_instruction.properties & MSK_INSTRUCTION_PROPERTY_ACCESS_TYPE_THIS ))
				) { // try to solve symbol...

					std::string *symbol_to_find=&instruction->symbol.name;
					ScriptClass *sc_aux=NULL;

					if(instruction->vm_instruction.properties & MSK_INSTRUCTION_PROPERTY_ACCESS_TYPE_THIS){ // search the symbol within class.

						if(*symbol_to_find == SYMBOL_VALUE_SUPER){ // trivial super.
							Symbol *symbol_sf_foundf=NULL;
							std::string str_symbol_to_find = sf->symbol.name;

							for(int i = sf->symbol.idx_position-1; i >=0 && symbol_sf_foundf==NULL; i--){
								Symbol *symbol_member = (Symbol *)sf_class->symbol_members->items[i];
								if(symbol_member->properties & SYMBOL_PROPERTY_FUNCTION){
									ScriptFunction *sf=(ScriptFunction *)symbol_member->ref_ptr;
									bool match_params=(symbol_member->properties & SYMBOL_PROPERTY_C_OBJECT_REF?str_symbol_to_find == symbol_member->name:true);
									if(
											(sf->symbol.name == str_symbol_to_find)
										&& (match_params)
										){
										symbol_sf_foundf = symbol_member;
									}
								}
							}

							// ok get the super function...
							if(symbol_sf_foundf == NULL){
								EVAL_ERROR_POP_FUNCTION(instruction->instruction_source_info.file,instruction->instruction_source_info.line,"Cannot find parent function %s::%s",sf->symbol.name.c_str(),symbol_to_find->c_str());
							}
							//instruction->vm_instruction.byte_code=BYTE_CODE_LOAD_TYPE_VARIABLE;
							instruction->vm_instruction.value_op2=symbol_sf_foundf->idx_position;
							instruction->instruction_source_info.ptr_str_symbol_name =get_mapped_name(eval_data,str_symbol_to_find);
							instruction->vm_instruction.properties=MSK_INSTRUCTION_PROPERTY_ACCESS_TYPE_THIS;

						}else{ // is "this" symbol

							// is automatically created on vm...
							Symbol *symbol_function=sf_class->getSymbol(*symbol_to_find,ANY_PARAMS_SYMBOL_ONLY);
							if(symbol_function!=NULL){
								instruction->vm_instruction.value_op2=symbol_function->idx_position;
							}
						}
					}else if((sc_aux = eval_data->script_class_factory->getScriptClass(*symbol_to_find))!= NULL){

							instruction->vm_instruction.byte_code=BYTE_CODE_LOAD_TYPE_CLASS;
							instruction->vm_instruction.value_op2=(zs_int)sc_aux;

					}else{ // try find local symbol  ...
						bool local_found=false;
						ScriptFunction *script_function_found=NULL;
						//ScriptClass *sc=NULL;
						Symbol * sc_var = NULL;
						sc_var = instruction->symbol.scope->getSymbol(*symbol_to_find, NO_PARAMS_SYMBOL_ONLY,ScopeDirection::SCOPE_DIRECTION_DOWN);


						if(sc_var != NULL){ // local symbol found

							if(sc_var->n_params==NO_PARAMS_SYMBOL_ONLY){ // symbol is variable...
								if((vis=sf->getSymbol(sc_var->scope,*symbol_to_find))!=NULL){
									instruction->vm_instruction.byte_code=BYTE_CODE_LOAD_TYPE_VARIABLE;
									instruction->vm_instruction.value_op2=vis->idx_position;
									instruction->vm_instruction.properties |=MSK_INSTRUCTION_PROPERTY_ACCESS_TYPE_LOCAL;
									local_found=true;
								}
							}
							else{ // symbol is function...
								Symbol *function_symbol=sf->getSymbol(sc_var->scope,*symbol_to_find,NO_PARAMS_SYMBOL_ONLY);
								if(function_symbol!=NULL){
									script_function_found=(ScriptFunction *)function_symbol->ref_ptr;
									instruction->vm_instruction.byte_code=BYTE_CODE_LOAD_TYPE_FUNCTION;
									instruction->vm_instruction.value_op2=(zs_int)script_function_found; // store script function
									local_found =true;
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
				instruction_info.ptr_str_symbol_name=instruction->instruction_source_info.ptr_str_symbol_name;


				eval_data->current_function->script_function->instruction_source_info[i]=instruction_info;

			}

	lbl_exit_pop_function:

			// delete and popback function information...
			delete eval_data->current_function;
			eval_data->functions.pop_back();

			eval_data->current_function = NULL;
			if(eval_data->functions.size() > 0){
				eval_data->current_function = eval_data->functions.at(eval_data->functions.size()-1);
			}



			return ok;
		}
	}
}
