# ZetScript

<h2>Description</h2>


ZetScript is a basic script engine for your c++ project.




<h2>Performance</h2>



<b>for_test.zs</b> has this code,

<code>
var j=0;for(var i=0; i < 10000000; ++i){j=i*2;}
</code>

Generates this following asm,



[01:00]	LOAD	VAR(i)<br>
[01:01]	LOAD	CONST(0)<br>
[01:02]	MOV	[01:00],[01:01]<br>
[02:00]	LOAD	VAR(i)<br>
[02:01]	LOAD	CONST(10000000)<br>
[02:02]	LT	[02:00],[02:01]<br>
[02:03]	JNT	[0255:0005]<br>
[03:00]	LOAD	VAR(j)<br>
[03:01]	LOAD	VAR(i)<br>
[03:02]	LOAD	CONST(2)<br>
[03:03]	MUL	[03:01],[03:02]<br>
[03:04]	MOV	[03:00],[03:03]<br>
[03:05]	POP_SCOPE(2)<br>
[04:00]	LOAD	VAR(i)++<br>
[04:01]	JMP	[0255:0002]<br>






Total 15 instructions


Is performed 10000000 times within 0.777 seconds so,

(15*10000000)/0.777=  => 193MIPS ~ better than Shit with a horse 

Comparison tables https://en.wikipedia.org/wiki/Instructions_per_second

TABLE OPTIMIZATION

Optimization|   ms  |   MIPS   | Desc
------------|-------|----------|---------------------------------------------
No opt      |  2700 |  37 MIPS | N/A 
1st         |  2000 |  50 MIPS | std::vector to plain C
2st         |  1400 |  71 MIPS | Get rid of function access as much as possible
3st         |  1321 |  75 MIPS | Integrate Memory manage
4rt         |  1135 |  88 MIPS | Optimize main loop with continues and statments/instructions iteration ends with NULL
5rt         |  0.777|  193 MIPS| Optimize main loop 2nd







