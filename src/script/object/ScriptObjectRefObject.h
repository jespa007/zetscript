/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
namespace zetscript{

	class ScriptObjectRefObject : public ScriptObject{
	public:
		ScriptObjectRefObject();
		void setRefObject(ScriptObject *_ref_object);
		ScriptObject *getRefObject();
		~ScriptObjectRefObject();


	protected:
		zs_list_node<ScriptObjectRefObject *> *ref_object_node;
		ScriptObject *ref_object;

		void dettachRefObject();
	};

}
