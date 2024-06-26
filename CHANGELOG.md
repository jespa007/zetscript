
---
Version: 2.1.0
Date: 2024-01-14

- #375 Specify stack size in ScriptEngine::ScriptEngine constructor
- #373 Change ScriptEngine::registerConstant ScriptEngine::registerConstant[Type]


---
Version: 2.0.3
Date: 2024-01-14

- #367 Fixed bug double detach returning objects that shares within function
- Update README.md 

---
Version: 2.0.2
Date: 2024-01-14

- #365 Fixed bug returning objects from containers leaves a double dereference and delete of the object itself in the container


---
Version: 2.0.2
Date: 2024-01-14

- #365 Fixed bug returning objects from containers leaves a double dereference and delete of the object itself in the container


---
Version: 2.0.1
Date: 2024-01-11

- #360 Fixed bug try to share constant script object
- #362 Fixed Bug segfault on returning string from class
- #363 Fixed Bug segfault on returning value from property getter with parameter
- Added benchmarks README.md

---
Version: 2.0.0
Date: 2024-01-04

Main improvements: 

- Massive update that rewrites a internal architecture, organize and clean code
- VM speed up ~x2.5
- VM save memory stack ~x2
- Optimize size/compile times ~x2
- Implements multiple Return/Assignments
- Implements arguments with options: default, variable arguments and by reference
- Implementation of properties with custom metamethods
- Implements keywords 'const'
- Implements operators 'in','typeof'
- Implements builtin iterators for String, Vector and Objects
- Implements builtin modules: 'System', 'Math', 'Json' and 'DateTime'
- The use of "function" keyword on class function members are not mandatory
- switch-case eval expressions
- Implements builtin concatenation with vector and object types through operator '+' 
- Full documentation and working examples

Solved issues
 
- #64 Implement "include" preprocessor feature 
- #65 added shared/static link cmake option through -DLINK=[SHARED|STATIC]
- #66 Added memory leak detection manager (only for GNU toolchain).  It needs memmanager https://github.org/jespa007/memmanager and pass -DMEMMANAGER parameter on cmake
- #67 Compact all utils source in one called CZetScriptUtils
- #68 implement try/catch. CZetScript getErrorMsg is not used anymore. catch will return message error and line
- #69 Fix memory leaks on delete keyword
- #70 concatenate string with return result from variable dot access it gives the function instead of result.
- #71 Improve cmake print messages 
- #72 post declare variable members it gives "already declared variable error"
- #73 fixed return; fails 
- #74 vectors are not deallocated if they are initialized on functions and declared as global (may be it has to deallocate on clearGlobalVariables ?)
- #75 create vars within for/while scope gives memory leaks and there's no control about nomber of open/close claudators on main function 
- #76 Fix print parent variable on access member error
- #78 Allow N instances of zetscript instead of single one 
- #80 initialize vars within class doesn't tell any error or warning 
- #81 Fixed final parentesis/claudator lack detector 
- #82 Allow "this" do assign or pass parameter as well as any object 
- #85,#103 Implement for(..in..)  
- #86 Implement access vector element by string 
- #87 vector: Fixed error "push" strings into vector 
- #88 struct: Fixed fail to find none of its elements 
- #89 string as function parameter: Fixed doesn't parse \" within string
- #90 for: Support for no pre/conditional and post expressions.
- #91 Changed name vector::add by vector::push  
- #92 Support for StringVar to const char conversion as parameter into C function 
- #94 for/while/switch: break support 
- #95 for: obligate to put "var" on pre for expression 
- #96 for: continue keyword support
- #98 removed statment concept 
- #99 register C constant feature
- #100 Improved virtual machine speed (Improved ~ x1.4) 
- #101 Adapt inheritance feature developed on 1.1.3 version
- #102 Bug: can use variable before define var itself
- #103 Adapt foreach to for(.. in .. )
- #104 optimize embeded container ScriptVariable 
- #105 Get rid AST Node limitation
- #106 Study whether is posible parse + compile in single step 
- #107 Way to get the symbol_value at run time without ASTNode
- #108 Wayt to get filename + line without AST_NODE 
- #109 Optimize parseExpression 
- #110 Change concept structure by dictionary
- #111 C stuff in the global scope when is reevaluated 
- #112 replace std::bind 
- #116 CMakeLists: Make permament static link and add -DFCOMPILE
- #114 Turn CZetScript class into helper functions inside zetscript namespace enhancement feature
- #117 Make MACROS register_C_XXXX all uppercase
- #119 CEval as eval (all functions are C functions within zs member)
- #118 split CZetScriptUtils file in io_utils, path_utils, logger as static function
- #113 Replace try/catch by getError (canceled)
- #120 Put all common function used by user in zs namespace and the rest in zs::core
- #121 CEval,CScriptClassFactory and CZetScript as instanceable
- #122 Restore CScriptClassFactory
- #124 Implement Integer helper class
- #125 Implement String helper class
- #126 Save pure c function instead of std::function for speed improvement
- #128 Implement register_c_function_as_member 
- #129 Migrate new C++ coding style
- #130 Feature add variables into instanced class (i.e this.a=0;)
- #131 Adapt language to ECMA-6 
- #132 do not requires ; char as end expression 
- #133 Perform SUB instead of NEG+ADD
- #134 Implement Math helper class
- #135 eval negative identifier and constants tokens and perform LOAD with negative option or load negative constants instead of doing NEG.
- #136 Clear unusued scopes
- #137 Callback function support
- #138 remove all std::vector and std::string access on VM
- #139 Implement const operator
- #140 Implement Array methods  
- #141 limit n params only for c functions 
- #142 Try to optimize bind function member 
- #144 Implement System helper class PART1
- #145 Implement eval Array object
- #146 Implement eval Dictionary object
- #147 Implement bind class const static variable
- #148 replace all writeError functions by throw and remove error param 
- #150 String operator + with any stackelement
- #154 Fix continue, break keyword 
- #153 fix DO-WHILE keyword
- #152 fix WHILE keyword
- #151 fix FOR keyword 
- #155 Check compile Visual Studio 2015, 2019
- #156 Fix SWITCH  
- #157 Fix +=,-=,/= etc operator
- #159 Implements zetscript documentation asciidoctor 
- #160 Update Invader ZetScript 2.0 
- #161 Merge symbol+functions in one container !!!
- #162 Implement eval Function object 
- #164 implement length property built-in for get length of vector/dictionary/string instead of call size()
- #166 control the increment of vm_stk_current/vm_str_current  
- #168 When semicolon on expression is missing it shows wrong filename 
- #169 Fix/Check metamethods
- #170 Fix class 
- #171 test returns stack var with 0 refs (solved by #230, user is in charge to destroy lifetime objects) 
- #172 get rid super scope
- #173 Manage this context automatically on anonymouse function created on class
- #175 Arguments as E/S 
- #174 Expression optimization: Evaluate constant operations
- #177 Implement saveCurrentState
- #179 Bug if-else fails {max scope reached} 
- #180 Return vector or dictionary not deref its user properties 
- #181 Implement var args 
- #182 Make StringStringScriptObject iterable like Array 
- #183 Implement ScriptObject methods
- #184 Implement + for Array 
- #185 parser char as integer
- #186 Implement StriptVarString methods
- #187 Implement Console helper class
- #189 give call stacktrace from exception 
- #190 eval_expression operator fails
- #192 Fix multiassigment 
- #193 Check fix ternari
- #194 Detect literal or assigments within literals  
- #195 Change ScriptVar by Script and remove ScriptVarDictionnary
- #196 Do not extend property when it access 
- #197 Avoid vector/string objects to add properties through string 
- #199 Check fix instanceof 
- #200 Implement count variable
- #201 System.print: convert any stackelement in string if argument itself is not string var  
- #202 getSymbolRecursive it iterates twice if scope direction is set as ScopeDirection::SCOPE_DIRECTION_BOTH Fix
- #203 Implement operator + object
- #204 Implement indexs as ArrayScriptObject in ScriptObject 
- #205 desglosse BYTE_LOAD to BYTE_LOAD_XXXX 
- #206 Override global functions 
- #207 ObjectString: Implement operator [] 
- #208 Implement Float helper class
- #209 Implement Boolean helper class  
- #210 Fix check keyword delete
- #211 Fix check directive import 
- #212 Fix accessors betwen assigns 
- #213 Replace int by intptr
- #214 replace throw_script_error by other smart way to manage exceptions
- #216 Accept .,() and [] as operator 
- #217 div/mod issues 
- #218 Encapsulate std::string in ObjectString 
- #219 Create iterator 
- #220 Set information for non-existant properties in store instruction 
- #222 do as an option to show system functions 
- #223 get static function through class 
- #224 implement keyword static
- #215 Implement typeof operator
- #227 implement DateTime class
- #229 Implement toJson(bool formated=false) for vector and Object
- #230 Control return ScriptObject[String] values that are not deallocated by user 
- #231 register class passing C function constructor and destructor
- #232 Implement setter/getter for member variables 
- #233 Implement operator IN 
- #234 Back to class name constructor 
- #235 Make optional function keyword as member function class
- #236 Implement sequence expressions
- #237 Optimize memory
- #238 Change behaviour save vars 
- #239 change absolute jmp offset by relative jmp offset 
- #240 Implement Multiple Assignment
- #241 Optimize VM Stack Based operations
- #243 Expression optimization: immediate register/constant load on operations 
- #244 Optimize memory for constants
- #245 Get rid of MSK_STACK_ELEMENT_UNDEFINED/MSK_STACK_ELEMENT_STRING
- #247 Optimization: move type field load variable to byte load_variable_XXXX 
- #248 Restructure script object elements
- #249 Allow function default arg value 
- #252 Allow var class with optional initialization
- #253 Get rid RESET STACK on each expression 
- #254 VM optimization: Immediate load if member or function are members 
- #255 Set Strict Bind static funcions 
- #256 Make compatibility back loops, while, switch, pre/post increment etc 
- #257 Set as local variable not found in the block if declared after its using
- #258 Access object properties by other type rather string feature
- #259 Erase user properties only by remove operator
- #261 zs_test: test every single simple check to within script and then launch complex scripts and stop if any fails
- #262 Select compile double or float precision config
- #263 ObjectString: Implement operator + feature feature
- #264 Implement passing args by reference
- #265 Implement zs_test_strict
- #266 Optimize using Push/Pop scope 
- #267 Implement Builtin helper class Json
- #269 Implement String methods 
- #270 Get rid of C++ functions binding. Allow only C functions or optimized way
- #271 Register member variables through getter/setter instead of the variable 
- #272 Separate built-in properties and user properties
- #275 Change VirtualMachine class by struct and C functions  
- #276 Implement built-in iterators
- #277 optimize access stk_vm_current 
- #279 Support array access for ObjectScriptObject 
- #281 Console Helper Class 
- #282 Improve the meaning of printing errors 
- #284 Seffault when native function returns object as NULL 
- #285 No finding static function 
- #288 ScriptStringScriptObject::format: First implementation format argument with ':' 
- #289 Example bind "object" from C code
- #290 Json::serialize: expose attributes metamethods that implements getters 
- #291 vm_call_function_native: Support std::string return type
- #292 Make mandatory use of static function for operator metamethods with two args
- #293 Make operation with assigment operations it does indefined behaviour on calling funcionts and its results
- #294 undefined behaviour when call delete 
- #295 Allow switch to have expressions/variables in the case 
- #296 Create inc/dec metamethods and include in attribute metamethods 
- #297 Vector/Object & class by default it gives all public members in json format by default when prints or do toString()
- #298 Do not store function members in Object 
- #299 Do not allocate for attribute member in script object 
- #300 Use linear array instead of Vector in vm runtime 
- #301 Solve inherited class call overrided function
- #303 Implement ZS_BYTE_CODE_IMMEDIATE_CALL ZS_BYTE_CODE_THIS_CALL
- #304 Cache scope data
- #305 Simplify VMScope info optimitzation
- #306 Get rid PUSH/POP begining function optimitzation
- #307 Solve all symbols after each eval
- #308 Save memory: get rid of std::string and std::map 
- #309 Create new from type info stk variable 
- #311 Instruction: Add property reset stack if last instruction of expression is CALL or STORE
- #312 Direct call if metamethod is action or function without any params optimitzation 
- #313 Change names Property -> Field / Attribute -> Property 
- #314 Save n paramater/return count for each call in instruction->value_op1
- #315 Test stack element limits
- #316 default functions generates extra byte codes 
- #317 Change concept of class by type 
- #302 Improve MapString
- #319 Detect when function member object is called and its own object is destroyed 
- #323 Implement operator "==" for Array 
- #323 Implement operator "==" for Array
- #324 Mandatory ScriptEngine *_script_engine first param for to all binding functions 
- #325 evaluation inherited class doesn't update references from its parent 
- #326 Constructor member as named "constructor" 
- #330 When try to access property member from native class it gives a non apropiate message 
- #331 Operation to find native member functions for call always returns first function from instance and ignores inherited functions with same name and diferents signatures
- #332 Operation XXX "in" YYYY "==" boolean_value, not evals first "in" and then "=="
- #333 for in doesn't work for function calls  
- #335 Generate library name with version
- #334 Implement concatenated call  
- #337 Implement MapInt 
- #336 Solve problem ciclic references 
- #339 Conditionals evaluates whole condition instead of first breaking condition improvement
- #340 StackOverflow core dumped in deep recursion 
- #346 ZetScript it can allow register two same C++ types 
- #350 Extend from script from native type should be instantiable 
- #353 Memory peak on exit for-in 
- #355 Constant comparison string vs integer it throws a internal error that is not show 
- #357 Change SET to ASSIGN to have a better concept of assignment instead of set.
- #356 When call is not possible for incompatible binding when it prints possible candidates add TEXT if function is MEMBER or STATIC

---
Version: 1.3.0
Date: 2018-03-30

- #48 Optimize compile process.
- #47 Implement interactive console (zs).
- #46 Fixed seg fault on destroy objects in struct.
- #49 doing operations of returning values doesnt't work.
- #39 Script function and script classes are not clean after call cleancompile
- #53 Not do pop scope on returns inside body 
- #56 Show only line if file == DEFAULT_FILENAME or not found
- #57 it gives "Expected symbol after ','" message on void structs
- #54 Memory leaks detection (only for new keywords that involve C class) 
- #55 Implement add_attr/remove_attr functions for CStructVariable 
- #58 Removed clear compile information on saveState. 
- #59 Return undefined on NULL pointers after call C function with value. 
- #60 seg fault on calling first a function call and then register a variable
- #61 Clean whoke stack array on clean compile  
- #62 VM speed improve ~ x2

Added fibonacci test (fib.zs)

---
Version: 1.2.0
Date: 2018-02-13

- #30 ASTNode.h right push/pop pragma.
- #31 Configure output library as shared. 
- #28 Fixed ciclic dependency in class_C_baseof function on already registered classes.
- #34 Added support for unsigned int and intptr_t types. 
- #35 Fixed. Bug bad scope index on push/pop ast node 
- #36 Fixed. It doesn't register a class if it tries to register some variable base of class that has not registered by base_of_class 
- #37 Fixed. When symbols are not found during searching on its linking, it does a memory corruption 
- #38 Fixed. Bug double free pointer on set/restore state
- #39 Fixed. Script function and script classes are not clean after call cleancompile function.
- #41 More detail information about error accessing function "XXXXX" when prints "Expected scriptvar".
- #43 Improvement split eval process into parse, compile and execute functions.
- #40 Support detection class base of param type in C function calls.
- #41 Fixed/Implemented. Due virtual classes changes its memory map at run-time, function and variables cannot ensure the same pointer as base class so
     has been decided to disable heritate all functions/variables from parent classes (only C).
     Due 41, now we have to pass class type on both register_C_MemberFunction/register_C_VariableMember (+1 feature version)
- #45 Fixed. Seg. fault on returning wrong C pointer derived class. A helper function has been implemented to return right pointer in function of type parameter.

- Due float it cannot be the same size as the any param of c functions, float type is disable since 1.2. Instead do float *. 
- Fixed. Seg.faults on returning strings * from C functions.
- Check if C function can be registered. C function should be registered at last elements. 
- VM: It prints more information when user wants to access to member function into primitive. Currently it gives the error 
- Support converting pointers C functions if can derive from base class.
- Fixed. Seg.faults on autodelete C pointers given by some C function.
- Fixed. Assignment of C variables of type int/number independent if source script variable is int or number.
- vscript2vector: support for converting elements to numbers (aka floats).

- Removed assign hereritance of runtime stack var on assigment op.
- Fixed. Seg. fault on restore/save states.
- Renamed zg_variable.x to zs_variable.

---
Version: 1.1.3
Date: 2017-11-19

- Make compatible binding functions with MSVC compiler.

---
Version: 1.1.2
Date: 2017-11-19

- Solved bug #25: Fixed error on keyword after case/default keyword

---
Version: 1.1.1
Date: 2017-11-12

- #21 bind functions already implemented 
- #22 cast vector from/to stk implemented
- #23 Fixed bug
- #24 Fixed bug

---
Version: 1.1.0
Date: 2017-11-11

- Script function calls usign std::function.

---
Version: 1.0.2
Date: 2017-11-05

- Solved some bugs
- print comulative errors getErrorMsg()

---
Version: 1.0.1
Date: 2017-11-04

- Solved some bugs

---
Version: 1.0.0
Date: 2017-11-04

- First version
 
