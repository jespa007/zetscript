function f(a,b=0,c){
	
	System::assert(b==0,"b!=0");
	System::assert(c==undefined,"c!=undefined");
}

f(0)

