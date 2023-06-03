#include "zetscript.h"

using zetscript::ZetScript;
using zetscript::ClassScriptObject;
using zetscript::zs_int;

// Class Point to register
struct Point{
	int x,y;

	Point(){
		x=0;
		y=0;
	}

   Point(int _x, int _y){
		x=_x;
		y=_y;
	}
};

//------------------------------
// Point class functions to register

// defines new function Point ClassScriptObject
Point *Point_new(ZetScript *_zs){
	return new  Point();
}

void Point_constructor(ZetScript *_zs, Point *_this, zs_int _x, zs_int _y){
	_this->x=_x;
	_this->y=_y;
}

// defines getter property Point::x ClassScriptObject
zs_int Point_get_x(ZetScript *_zs, Point *_this){
	return _this->x;
}

// defines getter property Point::y ClassScriptObject
zs_int Point_get_y(ZetScript *_zs, Point *_this){
	return _this->y;
}

// defines delete function Point ClassScriptObject
void Point_delete(ZetScript *_zs, Point *_this){
	delete _this;
}


//
//------------------------------

int main()
{
	ZetScript zs;

   // Register class Point
	zs.registerClass<Point>("Point",Point_new,Point_delete);

	// Register constructor
	zs.registerConstructor<Point>(Point_constructor);

	// register property getter Point::x
	zs.registerMemberPropertyGetter<Point>("x",Point_get_x);

	// register property getter Point::y
	zs.registerMemberPropertyGetter<Point>("y",Point_get_y);

	// Evaluates function 'returnPoint' that returns an instance of registered type 'Point'
	zs.eval(
		"// 'returnPoint' instances 'Point' type\n"
		"function returnPoint(){\n"
		"   return new Point(10,20);\n"
		"}\n"
	);

	// It binds 'returnPoint' as '(ClassScriptObject *)(void)'
	auto returnPoint=zs.bindScriptFunction<ClassScriptObject *()>("returnPoint");

	// Calls ZetScript function which it returns 'ClassScriptObject *' reference
	auto class_script_object_point=returnPoint();

	// Prints the contents by console.
	printf("From zetscript object : %s\n",class_script_object_point->toString().c_str());

	// Cast ZetScript class object to 'Point' type pointer
	auto point=class_script_object_point->to<Point *>();

	// Prints the contents of 'Point' type by console.
	printf("From C++ pointer type : point->x=%i point->y=%i\n",point->x,point->y);

	// 'unrefLifetimeObject' it decreases the reference count of script object to tell is not used anymore
	zs.unrefLifetimeObject(class_script_object_point);

 	return 0;
}
