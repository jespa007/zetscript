/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define ZS_NEW_OBJECT_VECTOR(zs) (ScriptObjectVector::newScriptObjectVector(zs)) //ScriptClass::getInstance()->getRegisteredClassByIdx(ScriptClass::getInstance()->getIdxClassVector())))//,(void *)(new ScriptObjectVector())))

namespace zetscript{

	class CZetgine;
	class  ScriptObjectVector: public ScriptObject{
	public:
		StackElement return_callc;

		template<typename T>
		static std::vector<T> toStdVector(ScriptObjectVector *v_in){
			std::vector<T> v_out;
			const char * dst_convert_type = typeid(T).name();
			zs_float aux_flt;
			zs_vector * elements = v_in->getAllElements();


			for(unsigned i = 0; i < elements->count; i++){

				StackElement *sv=elements->get(i);

				switch(sv->properties & MSK_STK_PROPERTY_TYPE_PRIMITIVES)
				{
					case MSK_STK_PROPERTY_ZS_FLOAT:
						if(ZS_STRCMP(dst_convert_type, ==,typeid(zs_float).name())){
							ZS_FLOAT_COPY(&aux_flt, &sv->stk_value);
							v_out.push_back(aux_flt);
						}else if(ZS_STRCMP(dst_convert_type, ==,typeid(int).name())){
							v_out.push_back((zs_int)sv->stk_value);
						}else{
							THROW_RUNTIME_ERROR("Error trying to cast element on std::vector<%s>",ZS_STR(zs_float));
							return v_out;
						}
						break;
					default:
					case MSK_STK_PROPERTY_UNDEFINED:
					case MSK_STK_PROPERTY_BOOL:
					case MSK_STK_PROPERTY_FUNCTION:
						THROW_RUNTIME_ERROR("type not implemented yet");
						return v_out;
						break;
					case MSK_STK_PROPERTY_ZS_INT:
						if(ZS_STRCMP(dst_convert_type, ==,typeid(zs_int).name()) || ZS_STRCMP(dst_convert_type, ==,typeid(zs_float).name())){// typeid(int).name()) || ){
							v_out.push_back((zs_int)sv->stk_value);
						}else{
							THROW_RUNTIME_ERROR("Error trying to cast element on std::vector<int>");
							return v_out;
						}
						break;
				}

			}
			return v_out;
		}

		static void    			pushSf(ScriptObjectVector *sv,StackElement  * stk);
		static StackElement *  	popSf(ScriptObjectVector *sv);

		static ScriptObjectVector * newScriptObjectVector(ZetScript *zs);
		static ScriptObjectVector * newScriptObjectVectorAdd(ZetScript *zs,ScriptObjectVector *v1,ScriptObjectVector *v2);

		ScriptObjectVector();

		virtual StackElement * 			getProperty(const std::string & property_name, int * idx=NULL);
		virtual StackElement * 	getElementAt(short idx);
		virtual size_t length();
		zs_vector * getAllElements();

		StackElement *			newSlot();
		void 					push(StackElement  * stk);
		StackElement *			pop();

		virtual ~ScriptObjectVector();
	protected:
		zs_vector 			stk_elements;


		bool 				eraseElementAt(short idx);

	private:


	};

}
