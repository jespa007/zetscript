//1+(0+1>1+2?0+1:0+1)==0?0:1
(0>1?1:2)+2+3+4+5


/*var a=1;
var b;
var c;

var j=
	a>0 ?  			//if(a>0){
		(a>1) ? 	//	if(a>2){
			a+3		//		a+3
		: /*b=a > 4 ?	//	}else if(b=a>4){ // it also assigns boolean value into b in the middle of ternary 
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
//System::println("j:{0} b:{1} c:{2}",j,b,c);*/