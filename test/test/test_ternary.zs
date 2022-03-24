var a=1;
var b;

var j=
	a>0 ?  			//if(a>0){
		(a>1) ? 	//	if(a>2){
			a+3		//		a+3
		: b=a > 4 ?	//	}else if(b=a>4){ // it also assigns the result of ternary into b
			a+5		// 		a+2	5			
		: 			//	}else{
		  a>3?		//		if(a>3){
		   a+4    	//          a+4
		    :		//      }else{
		   a+2     	//          a+2
		    		//      }
		     		//	}
     :				//}else{
    	 a+1		//		a+1
    	 			//}
// Should be j=3 and b=3 (b=a>4 ? a+5:a>3?a+4:a+2 --> it assigns the result of ternary which is correct. Note is not recommended doing assignations during ternari op ) 
Console::outln("j:{0} b:{1}",j,b);
System::assert(j==3,"j!=3");
System::assert(b==3,"b!=3");
		  
		  
