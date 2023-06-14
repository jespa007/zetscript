# ZetScript

## Simple Script engine for C++

<h2>Introduction</h2>

<p>ZetScript is a&nbsp;programming language with an API that allows bind C++ code in script side. ZetScript includes&nbsp;the following features:</p>

<ul>
	<li>Virtual Machine</li>
	<li>Language syntax close to Javascript</li>
	<li>MSVC++ 32/64 bits MSVC 2015/2017 or build tools v141</li>
	<li>Linux/MinGW 32/64 bits, g++ 4.8.1 or above</li>
	<li>CMake 3.15 or above</li>
	<li>Dynamic Garbage collector</li>
	<li>Straightforward way to expose C++ types and call functions from C++ to ZetScript and Viceversa</li>
	<li>Override operators through metamethods</li>
</ul>

<p>A&nbsp;helloworld in shown in the following code,</p>

<pre lang="c++">
#include &quot;ZetScript.h&quot;

using namespace zetscript;

// Function C++ to be called from ZetScript
void sayHelloworld(ZetScript *_zs){
	printf(&quot;Hello World!\n&quot;);
}

int main(){

	// ZetScript instantiation
	ZetScript zs;

	// register function 'sayHelloworld'
	zs.registerFunction(&quot;sayHelloworld&quot;,sayHelloworld);

	// Evaluates ZetScript code that calls 'sayHelloworld'
	zs.eval(&quot;sayHelloworld();&quot;); 
	
	return 0;
}</pre>

<p style="text-align: center"><em>List 1.1</em></p>

<p>The code presented on list 1.1 it registers <strong><em> </em></strong><em>sayHelloworld</em> function. Then, it evaluates a code <em>sayHelloworld();</em>&nbsp; that, simply, calls C++ function <em>sayHelloword</em> from ZetScript.</p>

<ul>
</ul>

<h2>Install</h2>

<p>From the source code included in this article, the install is quite easy, it uses cmake method so to configure the project depending which compiler is installed,</p>

<pre>
cmake CMakeLists.txt</pre>

<p>&nbsp;</p>

<blockquote class="quote">
<div class="op">Quote:</div>

<p>Note:&nbsp;MSVC 2017, it has a feature a Open Folder that easily configures a CMakeFile project just opening the folder where the project is, avoiding configure the project in the same directory and leave a lot of files and directories related with configuration</p>
</blockquote>

<h2>Language overview</h2>

<h3>Built-in types</h3>

<p>Zetscript has a built in types Integers, Float, Boolean, String, Array and Object.</p>

<pre lang="javascript">
var i=10; // Integer
var f=0.5; // Float
var s=&quot;a string&quot;; // String
var b=true; // Boolean

var array=[1,0.5, &quot;a string&quot;, true]; // Array

// Object
var object={ 
	i: 10,
	f: 0.5,
	s: &quot;a string&quot;,
	b: true,
	v: [1,0.5, &quot;a string&quot;, true]
};</pre>

<p>&nbsp;</p>

<h3>Scope</h3>

<p>ZetScript has an easy concept of global scope declaring variables in the top of evaluating scope. Local variables are within blocks like function or loops. Local vars are destroyed when it exits from block, unless is not referenced by other variable</p>

<pre lang="javascript">
var i= 0; // global var (never is destroyed when is declared)

{ // starting block --&gt; declaring local variables starts here. 
  // You can access also to global var.

	var j= 2; // local var 
	// ...

} // ending block --&gt; j is destroyed
</pre>

<h3>Conditionals</h3>

<p>Zetscript supports if-else and switch conditionals</p>

<pre lang="javascript">
// if-else conditional
var number=5;
if(number &lt; 10){
	Console::outln(&quot;number less than 10&quot;);
}else{
	Console::outln(&quot;number greater equal than 10&quot;);
}

// switch conditional
switch(number){
case 0:
case 1: 
	Console::outln(&quot;number is 0 or 1&quot;);
	break;
case 2:
	Console::outln(&quot;number is 2&quot;);
	break;
default:
	Console::outln(&quot;number is : &quot;+number);
	break;
}</pre>

<h3>Loops</h3>

<p>Zetscript supports while,do-while and for as loops iterators</p>

<pre lang="javascript">
var i=0;
// while loop
while(i &lt; 10){
	Console::outln(&quot;i:&quot;+i);
	i++;
}

// do-while loop
do{
	Console::outln(&quot;i:&quot;+i);
	i++;
}while(i &lt; 20);

// for loop
for(var j=0; j &lt; 10; j++){
	Console::outln(&quot;j:&quot;+i);
}

</pre>

<h3>Classes and inheritance</h3>

<p>Zetscript supports define custom types. Function and variable members are referenced as fields. Inside the function is referenced as <em><strong>this </strong></em>keyword and out is accessed with field operator (i.e <em><strong>.</strong></em>). Also is possible to post declare static variables and functions members. 
ZetScript also supports inheritance.</p>

<pre lang="javascript">
// A class example
class Test{
	var data0=10; // Variable members 'data1' is initialized as 10
	
	constructor(){
		this.data1=5;
	}
	
	initData2(_a){
		this.data2=a;  // Variable member 'data2' is initialized as '_a'
	}
};

// Postdeclaration of constant Test::MY_CONSTANT;
const Test::MY_CONSTANT=10; 

// Postdeclaration of function member Test::function2
function Test::add(){ 
	return this.data0+this.data1+this.data2+Test::MY_CONSTANT; // returns 'data1' + 'data2' + Test::MY_CONSTANT
}

// A inheritance class example. 
// TestExtended inherites data1,data2,function1 and function2. 
class TestExtended extends Test{
	initData2(a){
		initData2(a+2); // Calls 'Test::function1(2)'
		this.data3=this.add(); // returns 'data1' + 'data2' + Test::MY_CONSTANT = 5 +
	}
	

};

var t=new TestExtended(); // instances TestExtended class</pre>

<p style="text-align: center">&nbsp;</p>

<h3>Calling script function from c++</h3>

<p>Once you have evaluated the script you can call function script from c++&nbsp;<b>until up 6 parameters</b>. To call script function from c++ it can be done through the function&nbsp;<i>ZetScript::zs->registerFunction</i>. Just pass the name of the function with the c++ template casting and it creates an std::function that links the script function. Also it is possible to link/call function member from instancied object on global scope.</p>

<pre lang="c++">
#include &quot;ZetScript.h&quot;

using namespace zetscript;

int main(){

	ZetScript zs = new ZetScript(); // instance zetscript

	zs-&gt;eval(
		&quot;class Test{&quot;
		&quot;	var data1;&quot;
		&quot;	function function1(arg){&quot;
		&quot;		Console::outln(\&quot;calling Test.Function:\&quot;+arg);&quot;
		&quot;	}&quot;
		&quot;};&quot;
		&quot;&quot;
		&quot;function delete_test(){&quot;
		&quot;	delete test;&quot;
		&quot;	Console::outln(\&quot;test variable was deleted\&quot;);&quot;
		&quot;}&quot;
		&quot;&quot;
		&quot;var test=new Test();&quot;
	);

	
    	// instance function delete_test function.
	std::function&lt;void()&gt;  * delete_test=zs->registerFunction&lt;void ()&gt;(&quot;delete_test&quot;); 

    	// instance member function test.function1.
	std::function&lt;void(int)&gt; * test_function1=zs->registerFunction&lt;void (int)&gt;(&quot;test.function1&quot;); 
	
   	// it calls &quot;test.function&quot; member function with 10 as parameter.
	(*test_function1)(10); 

    	// it calls &quot;delete_test&quot; function with no parameters
	(*delete_test)(); 

	// delete functions when they are used anymore
	delete 	test_function1;
	delete 	delete_test;
	delete  zs;

}</pre>

<p>&nbsp;</p>

<h2>API overview</h2>

<h3>Bind C++ variables</h3>

<p>Zetscrip can bind basic C&nbsp;types as int, float, bool and string types to operate in the script side.</p>

<pre lang="c++">
#include &quot;ZetScript.h&quot;

using namespace zetscript;

int main(){

	int i=10;
	std::string	string_var = &quot;in c++&quot;;
	bool b=false;
	zs_float f=5.0;

	ZetScript *zs = new ZetScript(); // instance zetscript

    	zs->registerVariable(&quot;i&quot;,i); // it registers int variable called i
    	zs->registerVariable(&quot;b&quot;,b); // it registers bool var ble called b
    	zs->registerVariable(&quot;f&quot;,f); // it registers float variable called f
	zs->registerVariable(&quot;string_var&quot;,string_var); // it registers string variable called string_var

	zs-&gt;eval(
		&quot;i+=10;&quot; // i+=10 =&gt; i=20
		&quot;b=!b;&quot; //  b=!b  =&gt; b=true
		&quot;f+=10.5;&quot; // f+=10.5 =&gt; f = 15.5
		&quot;string_var+=\&quot; and in script\&quot;;&quot; // concatenates &quot; and in script
		&quot;Console::outln(\&quot;string_var:\&quot;+string_var);&quot; // prints &quot;string_var:in c++ and in script
	);
	
	delete zs;
	return 0;
}</pre>

<h3>Bind C++ classes and its members</h3>

<p>Binding C++ class in Zetscript is done easyly with&nbsp;<i>zs->registerClass</i>&nbsp;method. To bind variables and functions members it can be done through <i>register_C_VariableMember</i>&nbsp;and&nbsp;<i>zs->registerMemberFunction</i>&nbsp;respectively. In script you can instance the C++ class in script side through operator&nbsp;<strong><i>new</i></strong>. When the instanced C Class variable is not used anymore the user has to delete it with operator&nbsp;<i><strong>delete</strong>.</i></p>

<pre lang="c++">
#include &quot;ZetScript.h&quot;

using namespace zetscript;

class MyClass{
public:
	int data1;

	void function1(int arg){
		this-&gt;data1 = arg;
		printf(&quot;Int argument is %i\n&quot;,this-&gt;data1);
	}
};

//-------------------------------------------------
// wraping functions for MyClass

MyClass *MyClassWrap_new(){
	return new MyClass;
}

//--------------------------------------------------------
// GET/SET for data1 (read & write)
void MyClassWrap_set_data1(MyClass *_this, zs_int v){
	_this->data1=v;
}

zs_int MyClassWrap_get_data1(MyClass *_this){
	return _this->data1;
}
//--------------------------------------------------------
// GET for data2 (only read)
zs_int MyClassWrap_get_data2(MyClass *_this){
	return _this->data2;
}
//--------------------------------------------------------
// SET for data3 (only write)
void MyClassWrap_set_data3(MyClass *_this, zs_int v){
	_this->data3=v;
}

void MyClassWrap_function0(MyClass *_this){
	_this->function0();
}

// register function1 named function1 in script side as function member.
void MyClassWrap_function1(MyClass *_this, zs_int v){
	_this->function1(v);
}

void MyClassWrap_delete(MyClass *_this){
	delete _this;
}

// wraping functions for MyClass
//-------------------------------------------------


class MyClassExtend:public MyClass{
public:
	float data2;

	void function2(float * arg){
		this-&gt;data2 = *arg;
		printf(&quot;Float argument is %.02f\n&quot;,this-&gt;data2);
	}
};

//-------------------------------------------------
// wraping functions for MyClassExtend

MyClassExtend *MyClassExtendWrap_new(){
	return new MyClassExtend;
}

zs_int MyClassExtendWrap_get_data4(MyClassExtend *_this){
	return _this->data4;
}

void MyClassExtendWrap_set_data4(MyClassExtend *_this, zs_int v){
	_this->data4=v;
}

void MyClassExtendWrap_function2(MyClassExtend *_this, zs_float *f){
	_this->function2(*f);
}

void MyClassExtendWrap_delete(MyClassExtend *_this){
	delete _this;
}

// wraping functions for MyClassExtend
//-------------------------------------------------

int main(){

	ZetScript *zs = new ZetScript(); // instance zetscript
	
	try{

		// register MyClass with name MyClass in script side.
		zs->register<MyClass>("MyClass",MyClassWrap_new,MyClassWrap_delete);

		 // register MyClassExtend with name MyClassExtend in script side.
		zs->registerClass<MyClassExtend>("MyClassExtend",MyClassExtendWrap_new,MyClassExtendWrap_delete);

		// register data1 named data1 in script side as variable member and read/write.
		zs->registerMemberAttributeSetter<MyClass>("data1",&MyClassWrap_set_data1);
		zs->registerMemberAttributeGetter<MyClass>("data1",&MyClassWrap_get_data1);

		// register data2 named data1 in script side as variable member (only read).
		zs->registerMemberAttributeGetter<MyClass>("data2",&MyClassWrap_get_data2);

		// register data1 named data1 in script side as variable member (only write).
		zs->registerMemberAttributeSetter<MyClass>("data3",&MyClassWrap_set_data3);

		// register function0 named function1 in script side as function member.
		zs->registerMemberFunction<MyClass>("function0",&MyClassWrap_function0);

		// register function1 named function1 in script side as function member.
		zs->registerMemberFunction<MyClass>("function1",&MyClassWrap_function1);


		// register data2 named data1 in script side as variable member.
		zs->registerMemberAttributeSetter<MyClassExtend>("data4",&MyClassExtendWrap_set_data4);
		zs->registerMemberAttributeGetter<MyClassExtend>("data4",&MyClassExtendWrap_get_data4);

		// register function2 named function2 in script side as function member.
		zs->registerMemberFunction<MyClassExtend>("function2",&MyClassExtendWrap_function2);

		// once all vars and functions are registered, tell that MyClassExtend is base of MyClass
		zs->classInheritsFrom<MyClassExtend,MyClass>();

	}catch(std::exception & ex){
		fprintf(stderr,"register error: %s\n",ex.what());
		exit(-1);
	}

	try{

		zs->eval(
			"class ScriptMyClassExtend extends MyClassExtend{\n"
			"	var data5;\n"
			"	function function0(){\n"
			"		super();\n"
			"   }\n"
			"	function function1(arg){\n"
			"		var i=this.data1;\n"
			"		super(this.data1+arg);\n"
			"		Console::outln(\"result => \"+i+\"+\"+arg+\"=\"+this.data1);\n"
			"   }\n"
			"};\n"
			"class ScriptMyClassExtend2 extends ScriptMyClassExtend{\n"
			"	var data6;\n"
			"	function function5(arg){\n"
			"		var i=this.data1;\n"
			"   }\n"
			"};\n"
			"var myclass = new ScriptMyClassExtend2();\n" // instances MyClassExtend
			"myclass.function0();\n" // it prints "function0"
			"myclass.function1(12);\n" // it prints "Int argument is 12"
			"myclass.function2(0.5);\n" // it prints "Float argument is 0.5"
			"Console::outln(\"data1:\"+myclass.data1);\n" // it prints "data1:12"
			"Console::outln(\"data2:\"+myclass.data2);\n" // it prints "data2:0.5"
		);

	}catch(std::exception & ex){
		fprintf(stderr,"runtime error: %s\n",ex.what());
		exit(-1);

	}
	
	delete zs;

	return 0;
}</pre>

<p style="text-align: center"><em>List 1.2</em></p>

<h3>Inheritance c++ Class on Script Class</h3>

<p>An important feature of ZetScript is that it has supports c++ class inheritance for script class. <em><strong>this </strong></em>and <em><strong>super()&nbsp;</strong></em>keywords works as a normal behavior.</p>

<p>From list 1.2 we present an example script that <em>ScripMyClassExtends </em>class is inherited by <em>MyClassExtends </em>class (from C++),</p>

<pre lang="javascript">
class ScriptMyClassExtended extends MyClassExtend{ // &lt;-- inheritances MyClassExtend (c++)
  function1(arg1){
    Console::outln(&quot;script argument is &quot;+arg1)
    super(this.data1+arg1); // &lt;-- calls MyClassExtend::function1 (c++) from list 1.2
  }
}

var myclass=new ScriptMyClassExtend();
Myclass.function1(5);

</pre>

<p style="text-align: center"><em>List 1.3</em></p>

<p>The code shown at list 1.3 will print,</p>

<pre>
script argument is 5
c++ argument is 15 </pre>

<p>&nbsp;</p>

<h2>Metamethods</h2>

<p>ZetScript implements metamethods to map operators or other operations&nbsp;through objects. Currently, it&nbsp;supports the following metamethods:</p>

<ul>
	<li>_equ (aka ==)</li>
	<li>_not_equ (aka !=)</li>
	<li>_lt (aka &lt;)</li>
	<li>_lte&nbsp;(aka &gt;=)</li>
	<li>_gt&nbsp;(aka &gt;)</li>
	<li>_gte&nbsp;(aka &gt;=)</li>
	<li>_not&nbsp;(aka !)</li>
	<li>_neg&nbsp;(aka -)</li>
	<li>_add&nbsp;(aka +)</li>
	<li>_div&nbsp;(aka /)</li>
	<li>_mul&nbsp;(aka *)</li>
	<li>_mod&nbsp;(aka %)</li>
	<li>_and&nbsp;(aka &amp;)</li>
	<li>_or&nbsp;(aka |)</li>
	<li>_xor&nbsp;(aka ^)</li>
	<li>_shl&nbsp;(aka &lt;&lt;)</li>
	<li>_shr&nbsp;(aka &gt;&gt;)&nbsp;</li>
	<li>_set (aka =)</li>
</ul>

<p>&nbsp;</p>

<p>For example, if in script side we want to do the operation + for an object we have to declare the function _add with two parameters, as we can see in the following code,</p>

<pre lang="javascript">
class MyNumber{
  var num;
  MyNumber(_n){
    this.num=_n;
  }
  static _add(op1,op2){
    return new MyNumber(op1.num+op2.num);
  }
};

var n1 = new MyNumber (20);
var n2 = new MyNumber (10);
var n3 =n1+n2;

Console::outln(&quot;n1 (&quot;+n1.num+&quot;) n2 (&quot;+n2.num+&quot;) = &quot;+n3.num);</pre>

<p style="text-align: center"><em>List 1.4</em></p>

<p>the same can be applied for C++ class. We have to register _add function in the C++ object,</p>

<pre lang="c++">
#include &quot;ZetScript.h&quot;

using namespace zetscript;

class MyNumber{
public:
	int num;
	MyNumber(){
		this-&gt;num=0;
	}
	MyNumber(int _n){
		this-&gt;num=_n;
	}
	void set(int _n){
		this-&gt;num=_n;
	}
};

//-------------------------------------------------
// wraping functions

MyNumber *MyNumberWrap_new(){
	return new MyNumber;
}

zs_int MyNumberWrap_num_get(MyNumber *_this, zs_int _num){
	return _this->num;
}

void MyNumberWrap_set(MyNumber *_this, zs_int _num){
	_this->num=_num;
}

MyNumber * MyNumberWrap_add(MyNumber *op1, MyNumber *op2){
	return new MyNumber(op1-&gt;num + op2-&gt;num);
}

void MyNumberWrap_delete(MyNumber *_this){
	delete _this;
}
	
// wrapping functions
//----------------------------------------------------

int main(){
	ZetScript *zs = new ZetScript();

	// register class MyNumber
	zs->registerClass&lt;MyNumber&gt;(&quot;MyNumber&quot;,MyNumberWrap_new,MyNumberWrap_delete);

	// register member attribute getter (only read)
	zs->registerMemberAttributeGetter&lt;MyNumber&gt;(&quot;num&quot;,&amp;MyNumberWrap_num_get);

	// register constructor through function MyNumber::set
	zs->registerMemberFunction&lt;MyNumber&gt;(&quot;MyNumber&quot;,&amp;MyNumberWrap_set);

	// register static function _add as metamethod
	zs->registerStaticMemberFunction&lt;MyNumber&gt;(&quot;_add&quot;,&amp;MyNumberWrap_add);

	try{
		zs-&gt;eval(
			&quot;var n1 = new MyNumber (20);\n&quot;
			&quot;var n2 = new MyNumber (10); \n&quot;
			&quot;var n3 =n1+n2; \n &quot;
			&quot;Console::outln(\&quot;n1 (\&quot;+n1.num+\&quot;) + n2 (\&quot;+n2.num+\&quot;) = \&quot;+n3.num);\n&quot;
		);
	}catch(std::exception & ex){
		fprintf(stderr,ex.what());
	}
	
	delete zs;
	
	return 0;
}
</pre>

<p style="text-align: center"><em>List 1.5</em></p>

<p>&nbsp;</p>

<blockquote class="quote">
<div class="op">Quote:</div>

<p><strong>Note that the codes</strong>&nbsp;shown at list 1.4 and 1.5 the _add function is static due operates from two objects and it doesn&#39;t the object member itself.</p>
</blockquote>

<h2>Save state/Clear</h2>

<p>Sometimes is useful to reset the script cleaning global variables or restore&nbsp;from one point&nbsp;when, for example,&nbsp;C++&nbsp;application is restarted.</p>

<p>ZetScript supports a way to save current compiled state. This operation saves AST nodes, registered C functions, variables and classes.</p>

<p>To save current state we have to invoke <em>CState::saveState</em>. This function returns an index that tells compiled state index saved.</p>

<pre>
zs->saveState()</pre>

<p>To clear current state and restore all registered variables/functions/classes,etc after last save state invoke **clear**</p>

<pre>
zs->clear()</pre>

<p>&nbsp;</p>

