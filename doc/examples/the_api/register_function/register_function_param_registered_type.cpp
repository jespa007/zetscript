#include "zetscript.h"

using zetscript::ZetScript;
using zetscript::zs_int;

// C structure to be registered
struct Point{
	int x,y;

	Point(){
		x=0;
		y=0;
	}
};

//------------------------------
// WRAP POINT

// defines new function for Point object
Point *Point_new(ZetScript *_zs){
	return new  Point();
}

// defines setter property for Point::x
void Point_set_x(ZetScript *_zs, Point *_this, zs_int _x){
	_this->x=_x;
}

// defines setter property for Point::y
void Point_set_y(ZetScript *_zs, Point *_this, zs_int _y){
	_this->y=_y;
}

// defines getter property for Point::x
zs_int Point_get_x(ZetScript *_zs, Point *_this){
	return _this->x;
}

// defines getter property for Point::y
zs_int Point_get_y(ZetScript *_zs, Point *_this){
	return _this->y;
}

// defines delete function for Point object
void Point_delete(ZetScript *_zs, Point *_this){
	delete _this;
}

// WRAP POINT
//------------------------------

// C function the accepts native Point
void mul10Point(ZetScript *_zs, Point *_point){
	// initialize x and y
	_point->x*=10;
	_point->y*=10;

}

int main(){
	ZetScript zs;

	// Register class Point
	zs.registerClass<Point>("Point",Point_new,Point_delete);


	// Register property setter Point::x
	zs.registerMemberPropertySetter<Point>("x",Point_set_x);

	// Register property setter Point::y
	zs.registerMemberPropertySetter<Point>("y",Point_set_y);    

	// Register property getter Point::x
	zs.registerMemberPropertyGetter<Point>("x",Point_get_x);

	// Register property getter Point::y
	zs.registerMemberPropertyGetter<Point>("y",Point_get_y);

	// Register native function mulPoint named as 'mulPoint'
    zs.registerFunction("mul10Point",mul10Point);

    // Eval script that calls native function 'mulPoint'
    zs.eval(
        "var point=new Point();\n"
        "point.x=10;\n"
        "point.y=20;\n"
        "Console::outln(\"before : \"+point);\n"
        "mul10Point(point)\n"
        "Console::outln(\"after : \"+point);"
 	);

    return 0;
}
