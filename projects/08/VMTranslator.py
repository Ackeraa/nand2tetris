import sys
import os
from enum import Enum

Type = Enum("Type", "ARITHMETIC PUSH POP LABEL GOTO IF FUNCTION RETURN CALL")
class Parser:

    def __init__(self, filename="Sys.vm"):
        self.filename = filename
        self.staticpre = os.path.split(filename)[1][:-2]
        self.lines = []
        self.idx = -1

    def read_file(self):
        with open(self.filename, 'r') as f:
            self.lines = f.readlines()

    def has_more(self):
        return self.idx < len(self.lines) - 1

    def advance(self):
        self.idx += 1

    def get_type(self):
        line = self.lines[self.idx].strip()
        if line.startswith("//") or len(line) == 0:
            return None
        line = line.split("//")[0].strip().split(" ")
        n = len(line)
        if n == 1:
            op = line[0]
            if op == "return":
                return Type.RETURN, ""
            elif op in ["add", "sub", "and", "or", "not", "neg", "eq", "gt", "lt"]:
                return Type.ARITHMETIC, line[0]
            else:
                raise Exception("Wrong command: " + str(line))
        elif n == 2:
            op, arg1 = line
            if op == "label":
                return Type.LABEL, arg1
            elif op == "if-goto":
                return Type.IF, arg1
            elif op == "goto":
                return Type.GOTO, arg1
            else:
                raise Exception("Wrong command: " + str(line))

        elif n == 3:
            op, arg1, arg2 = line
            if op == "push":
                if arg1 == "static":
                    arg2 = self.staticpre+arg2
                return Type.PUSH, arg1, arg2
            elif op == "pop":
                if arg1 == "static":
                    arg2 = self.staticpre+arg2
                return Type.POP, arg1, arg2
            elif op == "call":
                return Type.CALL, arg1, arg2
            elif op == "function":
                return Type.FUNCTION, arg1, arg2
            else:
                raise Exception("Wrong command: " + str(line))
        else:
            raise Exception("Wrong command: " + str(line))

class CodeWriter:

    def __init__(self, filename, is_single):
        self.filename = filename
        self.idx = 0
        self.func = "_"
        self.ret_addr = 0
        self.push_cmds = [ "D=M", "@SP", "A=M", "M=D", "@SP", "M=M+1" ]
        self.pop_cmds = [ "@SP", "AM=M-1", "D=M" ]
        self.latt = {"local": "LCL", "argument": "ARG", "this": "THIS", "that": "THAT"}

        if is_single:
            self.cmds = []
        else:
            self.cmds = self.trans_init()

    def write_file(self):
        with open(self.filename, 'w') as f:
            for cmd in self.cmds:
                f.write(cmd+ "\n")

    def trans(self, line):
        if line is None:
            return
        self.idx += 1
        tp = line[0]
        line = line[1:]
        if tp == Type.ARITHMETIC:
            cmd = self.trans_arithmetic(line)
        elif tp == Type.PUSH:
            cmd = self.trans_push(line)
        elif tp == Type.POP:
            cmd = self.trans_pop(line)
        elif tp == Type.LABEL:
            cmd = self.trans_label(line)
        elif tp == Type.GOTO:
            cmd = self.trans_goto(line)
        elif tp == Type.IF:
            cmd = self.trans_if(line)
        elif tp == Type.FUNCTION:
            cmd = self.trans_function(line)
        elif tp == Type.RETURN:
            cmd = self.trans_return()
        else:   # Type.CALL
            cmd = self.trans_call(line)

        self.cmds.extend(cmd)

    def trans_init(self):
        cmds = [
                "@256",
                "D=A",
                "@SP",
                "M=D",
                *self.trans_call(["Sys.init", "0"]),
            ]

        return cmds

    def trans_arithmetic(self, line):
        op = line[0]
        if op in ["add", "sub", "and", "or"]:
            if op == "add":
                cmd = "D=D+M" 
            elif op == "sub":
                cmd = "D=M-D"
            elif op == "and":
                cmd = "D=D&M"
            else:   # or
                cmd = "D=D|M"
            cmds = [
                    *self.pop_cmds, 
                    "@SP",
                    "AM=M-1",
                    cmd,
                    "M=D",
                    "@SP",
                    "M=M+1",
                 ]
        elif op == "neg" or op == "not":
            cmd = "M=-M" if op == "neg" else "M=!M"
            cmds = [
                    "@SP",
                    "AM=M-1",
                    cmd,
                    "@SP",
                    "M=M+1",
                ]
        else:    # op in ["eq", "gt", "lt"]
            if op == "eq":
                cmd = "D;JEQ"
            elif op == "gt":
                cmd = "D;JGT"
            else:   # lt
                cmd = "D;JLT"
            cmds = [
                    *self.pop_cmds,
                    "@SP",
                    "AM=M-1",
                    "D=M-D",
                    f"@TRUE_{self.idx}",
                    cmd,
                    "D=0",
                    f"@END_{self.idx}",
                    "0;JMP",
                    f"(TRUE_{self.idx})",
                    "D=-1",  # 0xFFFF
                    f"(END_{self.idx})",
                    *self.push_cmds[1:],
                ]

        return cmds

    def trans_push(self, line):
        arg1, arg2 = line
        if arg1 == "constant":
            cmds = [
                    "@"+arg2,
                    "D=A",
                    *self.push_cmds[1:]
                ] 
        elif arg1 in self.latt.keys():    # LCL, ARG, THIS, THAT
            # push local i -> addr = lcl+i; *sp=*addr; sp++
            cmds = [
                    "@"+self.latt[arg1],
                    "D=M",
                    "@"+arg2,
                    "A=D+A",
                    *self.push_cmds,
                ]
        elif arg1 == "pointer":
            cmd = "@THIS" if arg2 == "0" else "@THAT"
            cmds = [
                    cmd,
                    *self.push_cmds,
                ]
        elif arg1 == "static":
            cmds = [
                    "@"+arg2,
                    *self.push_cmds,
                ]
        else:   # arg1 == "temp"
            cmds = [
                    "@5",
                    "D=A",
                    "@"+arg2,
                    "A=A+D",
                    *self.push_cmds,
                ]

        return cmds

    def trans_pop(self, line):
        arg1, arg2 = line
        if arg1 in self.latt.keys():    # LCL, ARG, THIS, THAT
            # pop local i -> addr=LCL+i; sp--; *addr=*sp;
            cmds = [
                    "@"+self.latt[arg1],
                    "D=M",
                    "@"+arg2,
                    "D=D+A",
                    "@R13",
                    "M=D",
                    *self.pop_cmds,
                    "@R13",
                    "A=M",
                    "M=D",
                ]
        elif arg1 == "pointer":
            # pop pointer 0 -> sp--; *This=*sp
            cmd = "@THIS" if arg2 == "0" else "@THAT"
            cmds = [
                    *self.pop_cmds,
                    cmd,
                    "M=D",
                ]
        elif arg1 == "static":
            cmds = [
                    *self.pop_cmds,
                    "@"+arg2,
                    "M=D",
                ]
        else:   # arg1 == "temp"
            # pop temp i -> addr=5+i; sp--; *addr=*sp
            cmds = [
                    "@5",
                    "D=A",
                    "@"+arg2,
                    "D=D+A",
                    "@R13",
                    "M=D",
                    *self.pop_cmds,
                    "@R13",
                    "A=M",
                    "M=D",
                ]

        return cmds

    def trans_label(self, line):
        label = self.func + "$" + line[0]
        return ["("+label+")"]

    def trans_goto(self, line):
        label = self.func + "$" + line[0]
        cmds = [
                "@"+label,
                "0;JMP",
            ]

        return cmds

    def trans_if(self, line):
        label = self.func + "$" + line[0]
        cmds = [
                *self.pop_cmds,
                "@"+label,
                "D;JNE",
            ]
        
        return cmds

    def trans_call(self, line):
        self.ret_addr += 1
        f, n = line
        cmds = [
                f"@return_{self.ret_addr}",
                "D=A",
                *self.push_cmds[1:],
                "@LCL",
                *self.push_cmds,
                "@ARG",
                *self.push_cmds,
                "@THIS",
                *self.push_cmds,
                "@THAT",
                *self.push_cmds,
                # ARG = SP-n-5
                "@SP",
                "D=M",
                "@"+n,
                "D=D-A",
                "@5",
                "D=D-A",
                "@ARG",
                "M=D",
                # LCL = SP
                "@SP",
                "D=M",
                "@LCL",
                "M=D",
                # goto f
                "@"+f,
                "0;JMP",
                f"(return_{self.ret_addr})",
            ]

        return cmds

    def trans_function(self, line):
        f, k = line
        self.func = f
        cmds = [
                "("+f+")",
                "@"+k,
                "D=A",
                f"@END_{self.idx}",
                "D;JEQ",
                "@R14",
                "M=D",
                f"(REPEAT_{self.idx})",
                *self.trans_push(["constant", "0"]),
                "@R14",
                "MD=M-1",
                f"@REPEAT_{self.idx}",
                "D;JGT",
                f"(END_{self.idx})",
            ]

        return cmds

    def trans_return(self):
        cmds = [
                # FRAME = LCL
                "@LCL",
                "D=M",
                "@FRAME",
                "M=D",
                # RET = *(FRAME - 5)
                "@5",
                "A=D-A",
                "D=M",
                "@RET",
                "M=D",
                # *ARG = POP()
                *self.pop_cmds,
                "@ARG",
                "A=M",
                "M=D",
                # SP = ARG+1
                "@ARG",
                "D=M",
                "D=D+1",
                "@SP",
                "M=D",
                # THAT = *(FRAME-1)
                "@FRAME",
                "AM=M-1",
                "D=M",
                "@THAT",
                "M=D",
                # THIS = *(FRAME-2)
                "@FRAME",
                "AM=M-1",
                "D=M",
                "@THIS",
                "M=D",
                # ARG = *(FRAME-3)
                "@FRAME",
                "AM=M-1",
                "D=M",
                "@ARG",
                "M=D",
                # LCL = *(FRAME-4)
                "@FRAME",
                "AM=M-1",
                "D=M",
                "@LCL",
                "M=D",
                # goto RET
                "@RET",
                "A=M",
                "0;JMP",
            ]

        return cmds

if __name__ == "__main__":
    
    path = sys.argv[1]
    files = []
    is_single = True
    if path.endswith(".vm"):
        files.append(path)
        out_file = path[:-2]+"asm"
    else:
        for file in os.listdir(path):
            if file.endswith(".vm"):
                files.append(os.path.join(path, file))
        out_file = os.path.join(path, os.path.split(path)[1]+".asm")
        is_single = False

    writer = CodeWriter(out_file, is_single)

    for file in files:
        parser = Parser(file)
        parser.read_file()

        while parser.has_more():
            parser.advance()
            cmd = parser.get_type()
            writer.trans(cmd)

    writer.write_file()

