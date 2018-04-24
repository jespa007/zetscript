<p>You can also run an interactive console <a href="http://zetscript.org/zs.html">here</a>.</p>

<h2>Introduction</h2>

<p>ZetScript is a&nbsp;programming language with an API that allows bind C++ code in script side. ZetScript includes&nbsp;the following features:</p>

<ul>
	<li>Virtual Machine</li>
	<li>Language syntax close to Javascript</li>
	<li>MSVC++ 32/64 bits MSVC 2015/2017 or build tools v141</li>
	<li>Linux/MinGW 32/64 bits, g++ 4.8 or above</li>
	<li>Save/Load state support</li>
	<li>Dynamic Garbage collector</li>
	<li>Straightforward way to bind C++ variables, functions, classes and its members</li>
	<li>The library size is 1Mb on gnu toolchain and 372KB on MSVC++</li>
	<li>Override operators through metamethods</li>
</ul>

<p>A&nbsp;helloworld in shown in the following code,</p>

<pre lang="c++">
#include &quot;CZetScript.h&quot;

using namespace zetscript;

void say_helloworld(){
	printf(&quot;Hello World!&quot;);
}

int main(){

	CZetScript *zs = CZetScript::getInstance(); // instance zetscript

	register_C_Function(&quot;say_helloworld&quot;,say_helloworld);

	zs-&gt;eval(&quot;say_helloworld();&quot;); // Call c function and prints hello world!
	return 0;
}</pre>

<p style="text-align: center"><em>List 1.1</em></p>

<p>The code presented on list 1.1 it registers C function<strong><em> </em></strong><em>say_helloworld</em> with just one line of code. Then, it calls&nbsp;<em>eval </em>function that evaluates the string <em>say_helloworld();</em>&nbsp;that means execute&nbsp;a call&nbsp;of C funtion <em>say_helloword</em> from the script engine.</p>

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

<h3><br />
Yet another script language ?</h3>

<p>Few years ago, I decided to start work with scripting because I needed to be faster in terms of production. Of course, because I always try to be a good engineer I started to play with all script engines that already are in this world.&nbsp;<br />
<br />
Some of them were faster but it I didn&#39;t like its syntax. Others, the sintaxis was good but slow at run-time, etc. After spent a lot of time playing with them and trying to decide what&#39;s kind of script engine could be a better candidate for my project I finished to know that none of them fitted at all for my neededs.&nbsp;<br />
<br />
So two years ago decided to start my own script engine, but apart of putting my effort with my neededs I read from some forums what it expects to have from a good scrip engine, I mixed the both things together and It became ZetScript.<br />
<br />
At the end I decided to put ZetScript free because I like make easy life for the people, just in case they find useful this tool.</p>

<h2>Language overview</h2>

<h3>Built-in types</h3>

<p>Zetscript has a built in basic types like integers, numbers, booleans and containers as vectors and structures.</p>

<pre lang="jscript">
var i=10; //integer
var f=0.5; // number
var s=&quot;a string&quot;; // string
var b=true; // boolean

var vector=[1,0.5, &quot;a string&quot;, true]; // vector

var struct={ // structure
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

<pre lang="jscript">
// if-else conditional
var number=5;
if(number &lt; 10){
	print(&quot;number less than 10&quot;);
}else{
	print(&quot;number greater equal than 10&quot;);
}

// switch conditional
switch(number){
case 0:
case 1: 
	print(&quot;number is 0 or 1&quot;);
	break;
case 2:
	print(&quot;number is 2&quot;);
	break;
default:
	print(&quot;number is : &quot;+number);
	break;
}</pre>

<h3>Loops</h3>

<p>Zetscript supports while,do-while and for as loops iterators</p>

<pre lang="javascript">
var i=0;
// while loop
while(i &lt; 10){
	print(&quot;i:&quot;+i);
	i++;
}

// do-while loop
do{
	print(&quot;i:&quot;+i);
	i++;
}while(i &lt; 20);

// for loop
for(var j=0; j &lt; 10; j++){
	print(&quot;j:&quot;+i);
}

</pre>

<h3>Classes and inheritance</h3>

<p>Zetscript supports class and inheritance. member and function variables inside class scope are referenced through <em><strong>this </strong></em>keyword. Also it can include variables/functions in post class declaration&nbsp;through operator <strong>::</strong>&nbsp;. Inheritance supports&nbsp;<strong><em>super()</em></strong> function in order to call parent function. To instance class is done through <strong>new </strong>operator.</p>

<pre lang="javascript">
// A class example
class Test{
	var data1;
	
	function function1(a){
		this.data1 =a;
		print(&quot;calling from Test. Data1:&quot;+this.data1);
	}
};

// include member variable data2
var Test::data2; 

// include function function function2
function Test::function2(){ 
	this.data2=&quot;a string&quot;;
}

// A inheritance class example. 
// TestExtended inherites data1,data2,function1 and function2. 
class TestExtended: Test{
	var data3;
	function function1(a){
		super(2); // it calls Test::function1(2)
		this.data1+=5; // Now data1=5+2 = 7
		print(&quot;calling from TestExtended. Data1:&quot;+this.data1);
	}
	
	function function3(){ // 
		this.data3=6;
		print(&quot;data3 is &quot;+this.data3);
	}
};

var t=new TestExtended(); // instances TestExtended class</pre>

<p style="text-align: center">&nbsp;</p>

<h3>Calling script function from c++</h3>

<p>Once you have evaluated the script you can call function script from c++&nbsp;<b>until up 6 parameters</b>. To call script function from c++ it can be done through the function&nbsp;<i>CZetScript::bind_function</i>. Just pass the name of the function with the c++ template casting and it creates an std::function that links the script function. Also it is possible to link/call function member from instancied object on global scope.</p>

<pre lang="c++">
#include &quot;CZetScript.h&quot;

using namespace zetscript;

int main(){

	CZetScript *zs = CZetScript::getInstance(); // instance zetscript

	zs-&gt;eval(
		&quot;class Test{&quot;
		&quot;	var data1;&quot;
		&quot;	function function1(arg){&quot;
		&quot;		print(\&quot;calling Test.Function:\&quot;+arg);&quot;
		&quot;	}&quot;
		&quot;};&quot;
		&quot;&quot;
		&quot;function delete_test(){&quot;
		&quot;	delete test;&quot;
		&quot;	print(\&quot;test variable was deleted\&quot;);&quot;
		&quot;}&quot;
		&quot;&quot;
		&quot;var test=new Test();&quot;
	);

	
    // instance function delete_test function.
	std::function&lt;void()&gt;  * delete_test=bind_function&lt;void ()&gt;(&quot;delete_test&quot;); 

    // instance member function test.function1.
	std::function&lt;void(int)&gt; * test_function1=bind_function&lt;void (int)&gt;(&quot;test.function1&quot;); 
	
    // it calls &quot;test.function&quot; member function with 10 as parameter.
	(*test_function1)(10); 

    // it calls &quot;delete_test&quot; function with no parameters
	(*delete_test)(); 

	// delete functions when they are used anymore
	delete 	test_function1;
	delete 	delete_test;

}</pre>

<p>&nbsp;</p>

<h2>API overview</h2>

<h3>Bind C++ variables</h3>

<p>Zetscrip can bind basic C&nbsp;types as int, float, bool and string types to operate in the script side.</p>

<pre lang="c++">
#include &quot;CZetScript.h&quot;

using namespace zetscript;

int main(){

    int i=10;
	string	string_var = &quot;in c++&quot;;
&nbsp;   bool b=false;
&nbsp;   float f=5.0;

	CZetScript *zs = CZetScript::getInstance(); // instance zetscript

    register_C_Variable(&quot;i&quot;,i); // it registers int variable called i
    register_C_Variable(&quot;b&quot;,b); // it registers bool var ble called b
    register_C_Variable(&quot;f&quot;,f); // it registers float variable called f
	register_C_Variable(&quot;string_var&quot;,string_var); // it registers string variable called string_var

	zs-&gt;eval(
        &quot;i+=10;&quot; // i+=10 =&gt; i=20
        &quot;b=!b;&quot; //  b=!b  =&gt; b=true
        &quot;f+=10.5;&quot; // f+=10.5 =&gt; f = 15.5
		&quot;string_var+=\&quot; and in script\&quot;;&quot; // concatenates &quot; and in script
		&quot;print(\&quot;string_var:\&quot;+string_var);&quot; // prints &quot;string_var:in c++ and in script
	);
	return 0;
}</pre>

<h3>Bind C++ classes and its members</h3>

<p>Binding C++ class in Zetscript is done easyly with&nbsp;<i>register_C_Class</i>&nbsp;method. To bind variables and functions members it can be done through <i>register_C_VariableMember</i>&nbsp;and&nbsp;<i>register_C_FunctionMember</i>&nbsp;respectively. In script you can instance the C++ class in script side through operator&nbsp;<strong><i>new</i></strong>. When the instanced C Class variable is not used anymore the user has to delete it with operator&nbsp;<i><strong>delete</strong>.</i></p>

<pre>
#include &quot;CZetScript.h&quot;

using namespace zetscript;

class MyClass{
public:
	int data1;

	void function1(int arg){
		this-&gt;data1 = arg;
		printf(&quot;Int argument is %i\n&quot;,this-&gt;data1);
	}
};

class MyClassExtend:public MyClass{
public:
	float data2;

	void function2(float * arg){
		this-&gt;data2 = *arg;
		printf(&quot;Float argument is %.02f\n&quot;,this-&gt;data2);
	}
};

int main(){

	CZetScript *zs = CZetScript::getInstance(); // instance zetscript
	
    // register MyClass with name MyClass in script side.
	register_C_Class&lt;MyClass&gt;(&quot;MyClass&quot;); 

    // register MyClassExtend with name MyClassExtend in script side.
	register_C_Class&lt;MyClassExtend&gt;(&quot;MyClassExtend&quot;); 
	
    // tell that MyClassExtend is base of MyClass
	class_C_baseof&lt;MyClassExtend,MyClass&gt;(); 

    // register data1 named data1 in script side as variable member.
	register_C_VariableMember&lt;MyClassExtend&gt;(&quot;data1&quot;,&amp;MyClass::data1); 

    // register function1 named function1 in script side as function member.
	register_C_FunctionMember&lt;MyClassExtend&gt;(&quot;function1&quot;,&amp;MyClass::function1); 

    // register data2 named data1 in script side as variable member.
	register_C_VariableMember&lt;MyClassExtend&gt;(&quot;data2&quot;,&amp;MyClassExtend::data2); 

    // register function2 named function2 in script side as function member.
	register_C_FunctionMember&lt;MyClassExtend&gt;(&quot;function2&quot;,&amp;MyClassExtend::function2); 



	zs-&gt;eval(
		&quot;var myclass = new MyClassExtend();&quot; // instances MyClassExtend
		&quot;myclass.function1(12);&quot; // it prints &quot;Int argument is 12&quot;
		&quot;myclass.function2(0.5);&quot; // it prints &quot;Float argument is 0.5&quot;
		&quot;print(\&quot;data1:\&quot;+myclass.data1);&quot; // it prints &quot;data1:12&quot;
		&quot;print(\&quot;data2:\&quot;+myclass.data2);&quot; // it prints &quot;data2:0.5&quot;
		&quot;delete myclass;&quot; // delete script var with c pointers attached inside.
	);

	return 0;
}</pre>

<p style="text-align: center"><em>List 1.2</em></p>

<h3>Inheritance c++ Class on Script Class</h3>

<p>An important feature of ZetScript is that it has supports c++ class inheritance for script class. <em><strong>this </strong></em>and <em><strong>super()&nbsp;</strong></em>keywords works as a normal behavior.</p>

<p>From list 1.2 we present an example script that <em>ScripMyClassExtends </em>class is inherited by <em>MyClassExtends </em>class (from C++),</p>

<pre lang="jscript">
class ScriptMyClassExtended: MyClassExtend{ // &lt;-- inheritances MyClassExtend (c++)
  function function1(arg1){
    print(&quot;script argument is &quot;+arg1)
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

<pre lang="c++">
class MyNumber{
  var num;
  function MyNumber(_n){
    this.num=_n;
  }
  function _add(op1,op2){
    return new MyNumber(op1.num+op2.num);
  }
};

var n1 = new MyNumber (20);
var n2 = new MyNumber (10);
var n3 =n1+n2;

print(&quot;n1 (&quot;+n1.num+&quot;) n2 (&quot;+n2.num+&quot;) = &quot;+n3.num);</pre>

<p style="text-align: center"><em>List 1.4</em></p>

<p>the same can be applied for C++ class. We have to register _add function in the C++ object,</p>

<pre lang="c++">
#include &quot;CZetScript.h&quot;

using namespace zetscript;

class MyNumber{
public:
&nbsp;&nbsp; &nbsp;int num;
&nbsp;&nbsp; &nbsp;MyNumber(){
&nbsp;&nbsp; &nbsp;&nbsp;&nbsp; &nbsp;this-&gt;num=0;
&nbsp;&nbsp; &nbsp;}
&nbsp;&nbsp; &nbsp;MyNumber(int _n){
&nbsp;&nbsp; &nbsp;&nbsp;&nbsp; &nbsp;this-&gt;num=_n;
&nbsp;&nbsp; &nbsp;}
&nbsp;&nbsp; &nbsp;void set(int _n){
&nbsp;&nbsp; &nbsp;&nbsp;&nbsp; &nbsp;this-&gt;num=_n;
&nbsp;&nbsp; &nbsp;}
&nbsp;&nbsp; &nbsp;static MyNumber * _add(MyNumber *op1, MyNumber *op2){
&nbsp;&nbsp; &nbsp;&nbsp;&nbsp; &nbsp;return new MyNumber(op1-&gt;num + op2-&gt;num);
&nbsp;&nbsp; &nbsp;}
};
int main(){
&nbsp;&nbsp; &nbsp;CZetScript *zs = CZetScript::getInstance();

&nbsp;&nbsp; &nbsp;// register class MyNumber
&nbsp;&nbsp; &nbsp;register_C_Class&lt;MyNumber&gt;(&quot;MyNumber&quot;);

&nbsp;&nbsp; &nbsp;// register variable member num
&nbsp;&nbsp; &nbsp;register_C_VariableMember&lt;MyNumber&gt;(&quot;num&quot;,&amp;MyNumber::num);

&nbsp;&nbsp; &nbsp;// register constructor through function MyNumber::set
&nbsp;&nbsp; &nbsp;register_C_FunctionMember&lt;MyNumber&gt;(&quot;MyNumber&quot;,&amp;MyNumber:: set);

&nbsp;&nbsp; &nbsp;// register static function _add as metamethod
&nbsp;&nbsp; &nbsp;register_C_StaticFunctionMember&lt;MyNumber&gt;(&quot;_add&quot;,&amp;MyNumber::_add);
&nbsp;&nbsp; &nbsp;
&nbsp;&nbsp; &nbsp;if(!zs-&gt;eval(
&nbsp;&nbsp; &nbsp;&nbsp;&nbsp; &nbsp;&quot;var n1 = new MyNumber (20);\n&quot;
&nbsp;&nbsp; &nbsp;&nbsp;&nbsp; &nbsp;&quot;var n2 = new MyNumber (10); \n&quot;
&nbsp;&nbsp; &nbsp;&nbsp;&nbsp; &nbsp;&quot;var n3 =n1+n2; \n &quot;
&nbsp;&nbsp; &nbsp;&nbsp;&nbsp; &nbsp;&quot;print(\&quot;n1 (\&quot;+n1.num+\&quot;) + n2 (\&quot;+n2.num+\&quot;) = \&quot;+n3.num);\n&quot;
&nbsp;&nbsp; &nbsp;)){
&nbsp;&nbsp; &nbsp;&nbsp;&nbsp; &nbsp;fprintf(stderr,ZS_GET_ERROR_MSG());
&nbsp;&nbsp; &nbsp;}
&nbsp;&nbsp; &nbsp;return 0;
}
</pre>

<p style="text-align: center"><em>List 1.5</em></p>

<p>&nbsp;</p>

<blockquote class="quote">
<div class="op">Quote:</div>

<p><strong>Note that the codes</strong>&nbsp;shown at list 1.4 and 1.5 the _add function is static due operates from two objects and it doesn&#39;t the object member itself.</p>
</blockquote>

<h2>Save/Restore state</h2>

<p>Sometimes is useful to reset the script cleaning global variables or restore&nbsp;from one point&nbsp;when, for example,&nbsp;C++&nbsp;application is restarted.</p>

<p>ZetScript supports a way to save current compiled state. This operation saves AST nodes, registered C functions, variables and classes.</p>

<p>To save current state we have to invoke <em>CState::saveState</em>. This function returns an index that tells compiled state index saved.</p>

<pre>
int idx=CState::saveState()</pre>

<p>To restore a previous state we have to invoke CState::restoreState passing compiled state index.</p>

<pre>
CState::setState(idx)</pre>

<p>&nbsp;</p>

<h2>Performance</h2>

<p>I had checked ZetScript performance 1.3.0 version with other script languages. Because Lua is the fastest and one of the most scripting language used I chose&nbsp;this&nbsp;to compare Fibonacci calculation time with N=34.</p>

<p>A Fibonacci script implemented in ZetScript is,</p>

<pre lang="jscript">
function fibR(n)
{
    if (n &lt; 2) { 
         return n; 
    }

    return fibR(n-2)+fibR(n-1);
}

print(&quot;fib: &quot; + (fibR(34)) );</pre>

<p style="text-align: center"><em>List 1.6</em></p>

<p>and the equivalent&nbsp;in Lua is,</p>

<pre lang="lua">
function fibR(n)

    if (n &lt; 2) then return n end
    return (fibR(n-2) + fibR(n-1))
end

print(&quot;fib: &quot; .. fibR(34))</pre>

<p style="text-align: center"><em>List 1.7</em></p>

<p>&nbsp;</p>

<p>And I have compared them through the <code>time</code> command already available in linux and the tests were made on a computer with i5-2450 CPU with&nbsp;2.50GHz and 8GB of RAM.</p>

<p>The result was the following,</p>

<ul>
	<li>Lua: <strong>1355ms</strong></li>
	<li>ZetScript: <strong>3110ms</strong></li>
</ul>

<p>So It can say that <strong>Lua is</strong> 3110/1355=<strong>2,29 times faster</strong> than ZetScript in this test.</p>

<p>&nbsp;</p>

<h2>Conclusions</h2>

<p>I have presented a new programming language, but is not quite new because is close to Javascript so many people can find confortable using it. Furthermore, Zetscript API exposes C++ code in script side in a straighforward way so is quite productive in general.</p>

<p>The language is new and of course, even though it have had a several tests, more bugs can be found. I would happy if people starts to use it and gives its feedback.</p>

<p>Maybe It will have success or not. I did this project for my needs but I freed in case &nbsp;other people can find useful for their projects.</p>

<p>If someone finds interesting this project it can go to these sites for further information:</p>

<ul>
	<li><a href="http://zetscript.org" target="zetscript">ZetScript webpage</a></li>
	<li><a href="http://zetscript.org/forum" target="zetscript">ZetScript Forum</a></li>
</ul>

<p>Also there&#39;s an example of using ZetScript,</p>

<ul>
	<li><a href="https://www.codeproject.com/Tips/1215529/Game-engine-using-SDL-and-ZetScript">Game Engine with SDL2 and ZetScript</a></li>
</ul>

<h2>Changes since 1.3</h2>

<ul>
	<li>It implements&nbsp;an interactive console.</li>
	<li>Added add/remove attributes on struct variable</li>
	<li>Optimized eval&nbsp;process</li>
	<li>Improve virtual machine speed ~x2</li>
	<li>Minor bug fixes.</li>
</ul>

<h2>Changes since 1.2</h2>

<ul>
	<li>eval process can be split in parse/compile and execute (see seccion 2.4 from ZetScript documentation)</li>
	<li>Due virtual classes changes its memory map at run-time, function and variables cannot ensure the same pointer as base class so has been decided to disable heritate all functions/variables from parent classes (only C).&nbsp; Due that change, now we have to pass class type on both <code>register_C_FunctionMember</code> and <code>register_C_VariableMember</code>.</li>
	<li>ZetScript 1.1.3 supported automatic register of parent functions/variables but, due of problem of virtual functions it cannot do since 1.2. Derived classes has to re-register parent functions/variables in order to use in script side.</li>
	<li>ZetScript 1.1.3 allowed pass float type as arguments but this only works 100% for x32 builds. So to avoid confusions I decided to constraint pass float types as pointer (i.e float *).</li>
</ul>

<h2>History</h2>

<ul>
	<li><strong>2018-04-20 ZetScript 1.3.0:</strong> Has been implemented an interactive console (zs.exe), it has improve virtual machine speed and a minor bug fixes. (see HISTORY for more information). Added performance test comparison with Lua script language in this article.</li>
	<li><strong>2018-02-21&nbsp;ZetScript 1.2.0:</strong> It has some features and a major bug fix so, as far I could test, is stable version. Despite I prefer version 1.1.3 I don&#39;t recommend use it because it doesn&#39;t work on virtual classes (it has segmentation fault as a hell) and also it doesn&#39;t work passing float for functions with 2 or more arguments on x64 builds.</li>
	<li><strong>2017-12-01&nbsp;ZetScript 1.1.3:</strong> First release</li>
</ul>
