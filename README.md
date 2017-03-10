# ZetScript

<h2>Description</h2>


ZetScript is a basic script engine for your c++ project.




<h2>Performance</h2>



<b>for_test.zs</b> has this code,

<code>
var j=0;for(var i=0; i < 10000000; ++i){i*2;}
</code>

Generates this following asm,



[01:00]	LOAD	VAR(i)<br>
[01:01]	LOAD	CONST(10000000)<br>
[01:02]	LT	[01:00],[01:01]<br>
[01:03]	JNT	[0004:-001]<br>
[02:00]	LOAD	VAR(i)<br>
[02:00]	LOAD	CONST(2)<br>
[02:04]	MUL	[02:00],[02:03]<br>
[02:05]	POP_SCOPE(2)<br>
[03:00]	LOAD	++VAR(i)<br>
[03:01]	JMP	[0001:-001]<br>





Total 12 instructions


Is performed 10000000 times within 1.5 seconds so,

(10*10000000)/2.0=  => 50MIPS ~ better than Shit with a horse 

Comparison tables https://en.wikipedia.org/wiki/Instructions_per_second

TABLE OPTIMIZATION

Optimization|   ms  |   MIPS   | Desc
------------|-------|--------------------------------------------------------
No opt      |  2700 |  37 MIPS | N/A 
1st         |  2000 |  50 MIPS | Get rid of function access as much as possible and enums






