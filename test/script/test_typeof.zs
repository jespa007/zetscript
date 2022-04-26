class A{}

System::assert(typeof 0==int,"typeof 0 != int")
System::assert(typeof 1.0==float,"typeof 1.0 != float")
System::assert(typeof true==bool,"typeof true != bool")
System::assert(typeof "str"==String,"typeof \"str\" != String")
System::assert(typeof [0,1,2]==Vector,"typeof [0,1,2] != Vector")
System::assert(typeof {a:0,b:1}==Object,"typeof {a:0,b:1} != Object")
System::assert(typeof new A()==A,"typeof new A() != A")




