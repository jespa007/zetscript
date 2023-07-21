/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
namespace zetscript{

	class RefObjectScriptObject : public ScriptObject{
	public:
		RefObjectScriptObject(ZetScript *_zs, ScriptObject *_so_ref);
		void setRefObject(ScriptObject *_ref_object);
		ScriptObject *getRefObject();
		~RefObjectScriptObject();


	protected:
		zs_list_node<RefObjectScriptObject *> *ref_object_node;
		ScriptObject *ref_object;

		void dettachRefObject();
	};

}
