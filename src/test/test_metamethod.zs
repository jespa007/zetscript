class CVector2{
	var x,y;
	
	function CVector2(x,y){
		this.x=x;
		this.y=y;
	}
	
	function _add(v){
		this.x += v.x;		
		this.y += v.y;
	}
};

var v1=new CVector2(0,1),v2=new CVector2(1,0);

var v3=v1+v2; // add a+b and creates a new object class CVector (implicit)



