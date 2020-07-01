# Lexical Analyzer and Parser for Path_maker Language :gem:
Path_maker is a basic scripting language for creating directory trees. I used Backus-Naur form and recursive descent method to parse the source code which is given by the user.
Here is the BNF:<br>
* ProgramBegin() -> ifStatement() | ifnotStatement() |commandStatement() | NULL<br>
* ifStatement() -> "if" + pathStatement() + commandStatement() | blockStatement()<br>
* ifnotStatement() -> "ifnot" + pathStatement() + commandStatement() | blockStatement()<br>
* pathStatement() -> "<" + "abc...z,*,_,/" + ">"<br>
* commandStatement() -> "make" | "go" + pathStatement() + ";" + ProgramBegin()<br>
* blockStatement() -> "{" + ProgramBegin() + "}" + ProgramBegin()<br>

## How it works? :pencil:
Firstly, user enters source code name and then algorithm splits the source code to its tokens. After that we write these tokens to the lex file called "code.lex". Then, BNF comes in and recursive descent algorithm takes action. After all of that, if anywhere in the source code has errors, parser informs the user about the exact error(thanks to BNF) and stops, if there are no errors then parsing operation ends and indicates success message to the user.
Features of Path_maker:<br>
* "Path" is the only data type. Path constants are relative directory path expressions written in the form: <dir1/dir2/dir3> where dir1, dir2 and dir3 are directory names.
No file names are of any concern (just directories) • Directory names start with a letter (upper or lower case) and are made of any combination of letters, digits and underscore characters (only). (Punctuation characters are not allowed. Blank characters are not allowed either.) Directory names are not case sensitive so <AA> and <aa> are basically the same. (since that is the policy of most operating systems) Operator "\*" can be used instead of a directory name, and it indicates parent directory. It can be used multiple times before any other directory name. 
Example:   <\*/\*/mydirectory>    indicates that one should move up (to parent) twice and then choose mydirectory. Operator * can only be used at the beginning of path expressions.  <hi/\*/there> is not allowed. Operator "/" cannot be used at the beginning or the end of any path. So </hi/there> is not allowed. Neither is <hi/there/> allowed. Blanks in a path expression are ignored (unless they exist in a directory name (which is not allowed)) so   <    \*   /\*    /    mydirectory> is OK. 
* Variables: There are no variables in the language
* Basic Commands: The only two basic commands are “make” and “go”. Make  has the form: 
make  <myDirectoryPath>; 
It simply creates the directories in the myDirectoryPath. If the path already exists it does nothing (but gives a warning message). If the path partially exists, it completes the path. Example:  make <*/project1/data>    goes up once and then creates a directory called project1 and then creates another one called “data” inside it. 
“make” does not change the current (working) directory.  (This is what we do with go command) 
“go”   simply changes the current directory 
Syntax: go  <myPathExpression>; 
If the path does not exist, go does nothing. (Gives an error message but does not exit the execusion) It does not partially follow a path. Partial existence of any path is considered as inexistence.  
* Control Structures: There is an “if” clause and a similar “ifnot” clause 
if <path_expression>  command 
is the basic form of this clause where command can be a basic command or a block. “if” clause executes the command  if from the current directory the path  <path_expression> exists. 
“if” does not change the current directory. 
“ifnot” clause has the exact same structure  
ifnot <path_expression>  command 
but operates if the path <path_expression> does not exist. 
Blocks: A command can be a basic command (“make” or “go”) but it can also be a block. A block is a list of lines of code enclosed in {  } set brackets. Blocks may also be nested in one another. 
End of line character: Only “make” and “go” commands require an end of line character and it is ‘;’ (semi-colon) 
Keywords: Keywords are case sensitive and all are lowercase. They are: 
make, go, if, ifnot 
Symbols: < , > , { , } , / , *
* Input: Prompt for the source file name. Get the source file name from the user. Assuming the user gave the name x, you should open and read the source file with the extension “.pmk” like x.pmk
  
## Example :book:
* Sample Path_maker source code
```
make <test1/test2>;
go <file3/file2>;
make <*/file1>;
go <*>;
ifnot <*>
{
	make <emre/sahin/a123>;	
}
make <file/control>;
if <file/control> {
  go <*>;
  go <deneme/kontrol>;
  go <*>;
  make <berna/bircan>;
  
	if <musics/metallica> make <musicFile/musics>;
	if <berna/bircan2> make <gameFolder/bf1>;

	if <*/*/*/test1>
	{
    		make <me/you>;
    		go <me/you>;
    		make<we/are>;
	}

	ifnot <*/texas>
	{
    		make <me2/you2be>;
    		go <me2/you2be>;
    		make<we2/are2>;
	}
	go <*>;
	make <file/exists>;
}
if <file/exist> 
{
    make <this/is>;
    go <valid/path>;
    make<and/awesome>;
}
```
* Lexical Analyser output:
```
ENTER THE SOURCE FILE NAME: x
OPENING x.pmk FILE...
SUCCESS: x.pmk FILE IS READY!

Command Found: make
Path Data Type Found: <test1/test2>
End Of Line Found
Command Found: go
Path Data Type Found: <file3/file2>
End Of Line Found
Command Found: make
Path Data Type Found: <*/file1>
End Of Line Found
Command Found: go
Path Data Type Found: <*>
End Of Line Found
Control Structure Found: ifnot
Path Data Type Found: <*>
Left Block Structure Found
Command Found: make
Path Data Type Found: <emre/sahin/a123>
End Of Line Found
Right Block Structure Found
Command Found: make
Path Data Type Found: <file/control>
End Of Line Found
Control Structure Found: if
Path Data Type Found: <file/control>
Left Block Structure Found
Command Found: go
Path Data Type Found: <*>
End Of Line Found
Command Found: go
Path Data Type Found: <deneme/kontrol>
End Of Line Found
Command Found: go
Path Data Type Found: <*>
End Of Line Found
Command Found: make
Path Data Type Found: <berna/bircan>
End Of Line Found
Control Structure Found: if
Path Data Type Found: <musics/metallica>
Command Found: make
Path Data Type Found: <musicfile/musics>
End Of Line Found
Control Structure Found: if
Path Data Type Found: <berna/bircan2>
Command Found: make
Path Data Type Found: <gamefolder/bf1>
End Of Line Found
Control Structure Found: if
Path Data Type Found: <*/*/*/test1>
Left Block Structure Found
Command Found: make
Path Data Type Found: <me/you>
End Of Line Found
Command Found: go
Path Data Type Found: <me/you>
End Of Line Found
Command Found: make
Path Data Type Found: <we/are>
End Of Line Found
Right Block Structure Found
Control Structure Found: ifnot
Path Data Type Found: <*/texas>
Left Block Structure Found
Command Found: make
Path Data Type Found: <me2/you2be>
End Of Line Found
Command Found: go
Path Data Type Found: <me2/you2be>
End Of Line Found
Command Found: make
Path Data Type Found: <we2/are2>
End Of Line Found
Right Block Structure Found
Command Found: go
Path Data Type Found: <*>
End Of Line Found
Command Found: make
Path Data Type Found: <file/exists>
End Of Line Found
Right Block Structure Found
Control Structure Found: if
Path Data Type Found: <file/exist>
Left Block Structure Found
Command Found: make
Path Data Type Found: <this/is>
End Of Line Found
Command Found: go
Path Data Type Found: <valid/path>
End Of Line Found
Command Found: make
Path Data Type Found: <and/awesome>
End Of Line Found
Right Block Structure Found
SOURCE CODE TOKENS:
0) make
1) <test1/test2>
2) EndOfLine
3) go
4) <file3/file2>
5) EndOfLine
6) make
7) <*/file1>
8) EndOfLine
9) go
10) <*>
11) EndOfLine
12) ifnot
13) <*>
14) LeftCurlyBracket
15) make
16) <emre/sahin/a123>
17) EndOfLine
18) RightCurlyBracket
19) make
20) <file/control>
21) EndOfLine
22) if
23) <file/control>
24) LeftCurlyBracket
25) go
26) <*>
27) EndOfLine
28) go
29) <deneme/kontrol>
30) EndOfLine
31) go
32) <*>
33) EndOfLine
34) make
35) <berna/bircan>
36) EndOfLine
37) if
38) <musics/metallica>
39) make
40) <musicfile/musics>
41) EndOfLine
42) if
43) <berna/bircan2>
44) make
45) <gamefolder/bf1>
46) EndOfLine
47) if
48) <*/*/*/test1>
49) LeftCurlyBracket
50) make
51) <me/you>
52) EndOfLine
53) go
54) <me/you>
55) EndOfLine
56) make
57) <we/are>
58) EndOfLine
59) RightCurlyBracket
60) ifnot
61) <*/texas>
62) LeftCurlyBracket
63) make
64) <me2/you2be>
65) EndOfLine
66) go
67) <me2/you2be>
68) EndOfLine
69) make
70) <we2/are2>
71) EndOfLine
72) RightCurlyBracket
73) go
74) <*>
75) EndOfLine
76) make
77) <file/exists>
78) EndOfLine
79) RightCurlyBracket
80) if
81) <file/exist>
82) LeftCurlyBracket
83) make
84) <this/is>
85) EndOfLine
86) go
87) <valid/path>
88) EndOfLine
89) make
90) <and/awesome>
91) EndOfLine
92) RightCurlyBracket
----------


SOURCE CODE TOKEN TYPES:
0) Command
1) Path
2) EndOfLine
3) Command
4) Path
5) EndOfLine
6) Command
7) Path
8) EndOfLine
9) Command
10) Path
11) EndOfLine
12) ControlStructure
13) Path
14) LeftCurlyBracket
15) Command
16) Path
17) EndOfLine
18) RightCurlyBracket
19) Command
20) Path
21) EndOfLine
22) ControlStructure
23) Path
24) LeftCurlyBracket
25) Command
26) Path
27) EndOfLine
28) Command
29) Path
30) EndOfLine
31) Command
32) Path
33) EndOfLine
34) Command
35) Path
36) EndOfLine
37) ControlStructure
38) Path
39) Command
40) Path
41) EndOfLine
42) ControlStructure
43) Path
44) Command
45) Path
46) EndOfLine
47) ControlStructure
48) Path
49) LeftCurlyBracket
50) Command
51) Path
52) EndOfLine
53) Command
54) Path
55) EndOfLine
56) Command
57) Path
58) EndOfLine
59) RightCurlyBracket
60) ControlStructure
61) Path
62) LeftCurlyBracket
63) Command
64) Path
65) EndOfLine
66) Command
67) Path
68) EndOfLine
69) Command
70) Path
71) EndOfLine
72) RightCurlyBracket
73) Command
74) Path
75) EndOfLine
76) Command
77) Path
78) EndOfLine
79) RightCurlyBracket
80) ControlStructure
81) Path
82) LeftCurlyBracket
83) Command
84) Path
85) EndOfLine
86) Command
87) Path
88) EndOfLine
89) Command
90) Path
91) EndOfLine
92) RightCurlyBracket
----------
```



* Parser output:
```

      INCOMING WORD TYPE: make
EXECUTE: make <test1/test2>


      INCOMING WORD TYPE: go
EXECUTE: go <file3/file2>
FILE ERROR: DIRECTORY "file3/" IS NOT AVAILABLE!
FILE ERROR: DIRECTORY "file3/file2/" IS NOT AVAILABLE!


      INCOMING WORD TYPE: make
EXECUTE: make <*/file1>

WARNING: YOU CAN NOT GO UPPER THAN ROOT DIRECTORY



      INCOMING WORD TYPE: go
EXECUTE: go <*>

WARNING: YOU CAN NOT GO UPPER THAN ROOT DIRECTORY



      INCOMING WORD TYPE: ifnot
CHECK: ifnot <*> EXISTS,

WARNING: YOU ARE TRYING TO GO UP BUT YOU ARE ALREADY ON A ROOT DIRECTORY!

IFNOT STATUS: TRUE
EXECUTE: make <emre/sahin/a123>


      INCOMING WORD TYPE: RightCurlyBracket


      INCOMING WORD TYPE: make
EXECUTE: make <file/control>


      INCOMING WORD TYPE: if
CHECK: if <file/control> EXISTS,
CHECKING: file/
CHECKING: file/control/
IF STATUS: TRUE
EXECUTE: go <*>

WARNING: YOU CAN NOT GO UPPER THAN ROOT DIRECTORY



      INCOMING WORD TYPE: go
EXECUTE: go <deneme/kontrol>
FILE ERROR: DIRECTORY "deneme/" IS NOT AVAILABLE!
FILE ERROR: DIRECTORY "deneme/kontrol/" IS NOT AVAILABLE!


      INCOMING WORD TYPE: go
EXECUTE: go <*>

WARNING: YOU CAN NOT GO UPPER THAN ROOT DIRECTORY



      INCOMING WORD TYPE: make
EXECUTE: make <berna/bircan>


      INCOMING WORD TYPE: if
CHECK: if <musics/metallica> EXISTS,
CHECKING: musics/
FILE WARNING: DIRECTORY "musics/" IS NOT AVAILABLE!
CHECKING: musics/metallica/
FILE WARNING: DIRECTORY "musics/metallica/" IS NOT AVAILABLE!
IF STATUS: FALSE
CHECK: if <berna/bircan2> EXISTS,
CHECKING: berna/
CHECKING: berna/bircan2/
FILE WARNING: DIRECTORY "berna/bircan2/" IS NOT AVAILABLE!
IF STATUS: FALSE
CHECK: if <*/*/*/test1> EXISTS,

WARNING: YOU ARE TRYING TO GO UP BUT YOU ARE ALREADY ON A ROOT DIRECTORY!

IF STATUS: FALSE


      INCOMING WORD TYPE: ifnot
CHECK: ifnot <*/texas> EXISTS,

WARNING: YOU ARE TRYING TO GO UP BUT YOU ARE ALREADY ON A ROOT DIRECTORY!

IFNOT STATUS: TRUE
EXECUTE: make <me2/you2be>


      INCOMING WORD TYPE: go
EXECUTE: go <me2/you2be>


      INCOMING WORD TYPE: make
EXECUTE: make <we2/are2>


      INCOMING WORD TYPE: RightCurlyBracket


      INCOMING WORD TYPE: go
EXECUTE: go <*>


      INCOMING WORD TYPE: make
EXECUTE: make <file/exists>


      INCOMING WORD TYPE: RightCurlyBracket


      INCOMING WORD TYPE: RightCurlyBracket


      INCOMING WORD TYPE: RightCurlyBracket


      INCOMING WORD TYPE: if
CHECK: if <file/exist> EXISTS,
CHECKING: me2/file/
CHECKING: me2/file/exist/
FILE WARNING: DIRECTORY "me2/file/exist/" IS NOT AVAILABLE!
IF STATUS: FALSE


PARSER STATUS: DONE!

Process returned 0 (0x0)   execution time : 1.321 s
Press any key to continue.
```
