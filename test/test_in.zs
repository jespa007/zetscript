class A{
	_exist(k){
		if(k<10 || k > 20){
			return false;
		}
		return true;
	}
	
}

0 in [3,2,1,1,0]
"a" in {k:'a',a:0}
0 in 10 // <-- invalid (2n is not iterator)
"abc" in "aaaabcdef"
'a' in "jdkjakdkk"
15 in new A()
0 in new A()

