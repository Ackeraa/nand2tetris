@256
D=A
@SP
M=D
@Sys.init
0;JMP
(SimpleFunction.test)
@2
D=A
@END_1
D;JEQ
@R14
M=D
(REPEAT_1)
@0
D=A
@SP
A=M
M=D
@SP
M=M+1
@R14
MD=M-1
@REPEAT_1
D;JGT
(END_1)
@LCL
D=M
@0
A=D+A
D=M
@SP
A=M
M=D
@SP
M=M+1
@LCL
D=M
@1
A=D+A
D=M
@SP
A=M
M=D
@SP
M=M+1
@SP
AM=M-1
D=M
@SP
AM=M-1
D=D+M
M=D
@SP
M=M+1
@SP
AM=M-1
M=!M
@SP
M=M+1
@ARG
D=M
@0
A=D+A
D=M
@SP
A=M
M=D
@SP
M=M+1
@SP
AM=M-1
D=M
@SP
AM=M-1
D=D+M
M=D
@SP
M=M+1
@ARG
D=M
@1
A=D+A
D=M
@SP
A=M
M=D
@SP
M=M+1
@SP
AM=M-1
D=M
@SP
AM=M-1
D=M-D
M=D
@SP
M=M+1
@LCL
D=M
@FRAME
M=D
@5
A=D-A
D=M
@RET
M=D
@SP
AM=M-1
D=M
@ARG
A=M
M=D
@ARG
D=M
D=D+1
@SP
M=D
@FRAME
AM=M-1
D=M
@THAT
M=D
@FRAME
AM=M-1
D=M
@THIS
M=D
@FRAME
AM=M-1
D=M
@ARG
M=D
@FRAME
AM=M-1
D=M
@LCL
M=D
@RET
A=M
0;JMP