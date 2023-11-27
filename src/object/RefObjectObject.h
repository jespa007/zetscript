/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
namespace zetscript{

	class RefObjectObject : public Object{
	public:
		RefObjectObject(ZetScript *_zs, Object *_so_ref);
		void setRefObject(Object *_ref_object);
		Object *getRefObject();
		~RefObjectObject();


	protected:
		ListNode<RefObjectObject *> *ref_object_node;
		Object *ref_object;

		void dettachRefObject();
	};

}
