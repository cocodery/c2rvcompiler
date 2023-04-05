grammar SysY;

compilationUnit
    :   translationUnit? EOF
    ;

translationUnit
    :   externalDeclaration+
    ;

externalDeclaration
    :   decl
    |   funcDef
    |   ';'
    ;

decl
    :   constDecl 
    |   varDecl
    ;

constDecl
    :   'const' bType constDef (',' constDef)* ';' 
    ;

bType
    :   'int' 
    |   'float'
    ;

constDef
    :   Identifier ('[' constExp ']')* '=' constInitVal
    ;

constInitVal
    :   constExp # scalarConstInitVal
    |   '{' (constInitVal (',' constInitVal)* )? '}' # listConstInitVal
    ;

varDecl 
    :   bType varDef (',' varDef)* ';'
    ;

varDef
    :   Identifier ('[' constExp ']')* # uninitVarDef
    |   Identifier ('[' constExp ']')* '=' initVal # initVarDef
    ;

initVal
    :   exp # scalarInitVal
    |   '{' (initVal (',' initVal)* )? '}' # listInitval
    ;

funcDef
    :   funcType Identifier '(' (funcFParams)? ')' block
    ;

funcType
    :   'void' 
    |   'int' 
    |   'float'
    ;

funcFParams
    :   funcFParam (',' funcFParam)*
    ;

funcFParam
    :   bType Identifier ('[' ']' ('[' constExp ']')* )?
    ;

block
    :   '{' blockItemList? '}'
    ;

blockItemList
    :   blockItem+
    ;

blockItem
    :   decl
    |   stmt
    ;

stmt
    :   assignStmt
    |   expStmt
    |   blockStmt
    |   seleStmt
    |   loopStmt
    |   jumpStmt
    ;

assignStmt
    :   lVal assignOp exp ';'
    ;


assignOp
    :   '=' | '*=' | '/=' | '%=' | '+=' | '-=' | '<<=' | '>>=' | '&=' | '^=' | '|='
    ;

expStmt
    :   (exp)? ';'
    ;

blockStmt
    :   block
    ;

seleStmt
    :   If '(' condExp ')' stmt (Else stmt)? # ifStmt
    ;

loopStmt
    :   While '(' condExp ')' stmt # whileLoop
    |   Do stmt While '(' condExp ')' ';' #DoWhileLoop
    ;

jumpStmt    
    :   Continue ';' # continueStmt
    |   Break ';' # breakStmt
    |   Return (exp)? ';' #returnStmt
    ;

exp
    :   condExp
    ;

constExp
    :   condExp 
    ;

lVal
    :   Identifier ('[' exp ']')*
    ;

primaryExp
    :   '(' exp ')' # primaryExp1
    |   lVal # primaryExp2
    |   number # primaryExp3
    ;

number
    :   IntLiteral # number1
    |   FloatLiteral # number2
    ; 

funcRParams
    :   funcRParam (',' funcRParam)*
    ;

funcRParam
    :   exp
    ;

unaryExp
    :   primaryExp # unary1
    |   Identifier '(' (funcRParams)? ')' # unary2
    |   unaryOp unaryExp # unary3
    ;

unaryOp 
    :   '&' | '*' | '+' | '-' | '~' | '!'
    ;

mulExp
    :   unaryExp (mulOp unaryExp)*
    ;

mulOp
    :   '*' | '/' | '%'
    ;

addExp
    :   mulExp (addOp mulExp)*
    ;

addOp
    :   '+' | '-'
    ;    

shiftExp
    :   addExp (shiftOp addExp)*
    ;

shiftOp
    :   '<<' | '>>'
    ;

relExp
    :   shiftExp (relOp shiftExp)*
    ;

relOp
    :   '<' | '>' | '<=' | '>='
    ;

eqExp
    :   relExp (eqOp relExp)*
    ;

eqOp
    :   '==' | '!='
    ;

andExp
    :   eqExp ('&' eqExp)*
    ;

exOrExp
    :   andExp ('^' andExp)*
    ;

inOrExp
    :   exOrExp ('|' exOrExp)*
    ;

lAndExp
    :   inOrExp ('&&' inOrExp)*
    ;

lOrExp
    :   lAndExp ('||' lAndExp)*
    ;

condExp
    :   lOrExp ('?' exp ':' condExp)?
    ;

// keyword
Int : 'int';
Void: 'void';
Const: 'const';
Return : 'return';
If : 'if';
Else : 'else';
While : 'while';
For : 'for';
Do : 'do';
Break : 'break';
Continue : 'continue';


// operator
Lparen : '(' ;
Rparen : ')' ;
Lbrkt : '[' ;
Rbrkt : ']' ;
Lbrace : '{' ;
Rbrace : '}' ;
Comma : ',' ;
Semicolon : ';';

Minus : '-';
Addition : '+';
Exclamation : '!';
Multiplication : '*';
Division : '/';
Modulo : '%';
LAND : '&&';
LOR : '||';
EQ : '==';
NEQ : '!=';
LT : '<';
LE : '<=';
GT : '>';
GE : '>=';

// integer, identifier
IntLiteral
    :   [0-9]+
    |   ('0x'|'0X')[0-9a-fA-F]+
    ;

FloatLiteral
    :   ([0-9]+|[0-9]*'.'[0-9]*)([eE]('+'|'-')?[0-9]+)?[fFlL]?
    |   ('0x'|'0X')([0-9a-fA-F]*'.'[0-9a-fA-F]*)([pP]('+'|'-')?[0-9]+)?[fFlL]?
    ;


Identifier
    :   [a-zA-Z_][a-zA-Z_0-9]*
    ;

Whitespace
    :   [ \t]+
        -> skip
    ;

Newline
    :   (   '\r' '\n'?
        |   '\n'
        )
        -> skip
    ;

BlockComment
    :   '/*' .*? '*/'
        -> skip
    ;

LineComment
    :   '//' ~[\r\n]*
        -> skip
    ;
