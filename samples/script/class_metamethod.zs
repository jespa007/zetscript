class Vector{

	Vector(_x=0,_y=0){
		this.x=_x;
		this.y=_y;
	}
	
	_set(_obj){
		this.x=_obj[0];
		this.y=_obj[1];
	}

	static _equ(_obj1,_obj2){
		
		
		if((_obj1 instanceof A)
				   &&
		  !(_obj2 instanceof A)
				){
			
			return _obj1.__a__ == _obj2;	
		}
		else if(!(_obj1 instanceof A)
			&&
			(_obj2 instanceof A)
		){
			return _obj1==_obj2.__a__;
		}else if(_obj1 instanceof A && _obj2 instanceof A){
			return _obj1.__a__==_obj2.__a__;
		}
		
		System::error("I don't how to compare {0} and {1}",typeof _obj1, typeof _obj2)
				
	}
	
	// defines _add metamethod...
	static _add(v1,v2){
		
		return new Vector(
				v1.x+v2.x
				,v1.y+v2.y);
	}
	
	_postinc(){
		this.x++;
		this.y++;
	}

	_to_string(){ // a way to have a custom string output
		return "["+this.x+","+this.y+"]";
	}
};

var v=(new Vector(3,4))+(new Vector(5,6))
Console::outln(v++) // it will read vector [x,y] and postincrement


