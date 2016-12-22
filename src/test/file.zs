function bb(){
	return (([0,1]));
}

var g=new CVector();

var t=g;

for(var i=0; i < 10; i++)
{
	var h=bb();//new CVector();
	
	for(var j=0; j < h.size();j++){
		print(h[j]);
	}
	
}

var f=1;
// hola
/*new CInteger();

{
   var j=0;

   {
     var i=j;
     
     {
    	 
     }

   }

   //i=2; // incorrent out scope ...
}
*/
/*
function bb(){
	var j=0;
	
	return j;
}*/