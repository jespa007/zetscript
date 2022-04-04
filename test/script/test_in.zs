class A{
	_in(k){
		if(k<10 || k > 20){
			return false;
		}
		return true;
	}
	
}

System::assert(0 in [3,2,1,1,0] == true,"(0 in [3,2,1,1,0]) != true")
System::assert(20 in [3,2,1,1,0] == false,"(20 in [3,2,1,1,0]) != false")
System::assert("abc" in [3,2,1,1,"abc",0]==true,"\"abc\" in [3,2,1,1,\"abc\",0]!=true")
System::assert("abc" in [3,2,1,1,"abcd",0]==false,"\"abc\" in [3,2,1,1,\"abcd\",0]!=false")
System::assert(false in [3,2,1,1,"abc",0,false]==true,"false in [3,2,1,1,\"abc\",0,false]!=true")
System::assert(true in [3,2,1,1,"abc",0,true]==false,"true in [3,2,1,1,\"abc\",0,true]!=false")
System::assert('a' in {k:'a',a:0}==true,"'a' in {k:'a',a:0}!=true")
System::assert('b' in {k:'a',a:0}==false,"'b' in {k:'a',a:0}!=false")
System::assert("abc" in "aaaabcdef"==true,"\"abc\" in \"aaaabcdef\"!=true")
System::assert("abce" in "aaaabcdef"==false,"\"abce\" in \"aaaabcdef\"!=false")
System::assert('a' in "jdkjakdkk"==true,"'a' in \"jdkjakdkk\"!=true")
System::assert('b' in "jdkjakdkk"==false,"'b' in \"jdkjakdkk\"!=false")
System::assert(10 in new A()==true,"i in new A()!=true")
System::assert(0 in new A()==false,"0 in new A()!=false")


