

var vv= 1+function(){
return 1;
}();

var kk=1+[1,2][0];

var fun = function (l,j,k){
	var b=0;
	
	function hola2(k,l){
		var ff=0;
	}
	
	hola2(1+1,2+2,3+3); // invalid: hola2 only takes 2 parameters
	
	return b;
};

var i=3+fun(1+2+3*7)+2; // val but j & k are not defined.

var array=[0,1,1];

array[0]++;


print("Hola:"+array[0]);


