var a=1;
var b;
var c;

var j=
	a>0 ?  			//if(a>0){
		(c=a>2) ? 	//	if(a>2){
			a+3		//		a+3
		: b=a > 4 ?	//	}else if(b=a>4){ // it also assigns boolean value into b in the middle of ternary 
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
print("j:"+j+" b:"+b+" c:"+c);