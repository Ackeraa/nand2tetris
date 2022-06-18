import sys

def load_code():
    filename = sys.argv[1]
    with open(filename, 'r') as f:
        return f.readlines()

def save_code(code):
    filename = sys.argv[1].split(".")[0] + ".asm"
    with open(filename, 'w') as f:
        for line in code:
            f.write(line + "\n")

def check_int(s):
    if s[0] == '-':
        return s[1:].isdigit()
    return s.isdigit()

def pre_process(code):
    code2 = []
    # scan
    for line in code:
        line = line.strip()
        # blank line
        if line.startswith("//") or len(line) == 0:
            continue
        line = line.split("//")[0]
        code2.append(line)

    return code2


def compile(code):
    asmcode = []
    for i, line in enumerate(code):
        line = line.split(" ")
        n = len(line)
        if n == 1:    # Arithmetic
            op = line[0]
            if op in ["add", "sub", "and", "or"]:
                if op == "add":
                    cmd = "D=D+M" 
                elif op == "sub":
                    cmd = "D=M-D"
                elif op == "and":
                    cmd = "D=D&M"
                elif op == "or":
                    cmd = "D=D|M"
                cmds = [
                        "@SP",
                        "M=M-1",
                        "A=M",
                        "D=M",
                        "@SP",
                        "M=M-1",
                        "A=M",
                        cmd,
                        "M=D",
                        "@SP",
                        "M=M+1",
                     ]
            elif op == "neg" or op == "not":
                cmd = "M=-M" if op == "neg" else "M=!M"
                cmds = [
                        "@SP",
                        "M=M-1",
                        "A=M",
                        cmd,
                        "@SP",
                        "M=M+1",
                    ]
            elif op in ["eq", "gt", "lt"]:
                if op == "eq":
                    cmd = "D;JEQ"
                elif op == "gt":
                    cmd = "D;JGT"
                elif op == "lt":
                    cmd = "D;JLT"
                cmds = [
                        "@SP",
                        "M=M-1",
                        "A=M",
                        "D=M",
                        "@SP",
                        "M=M-1",
                        "A=M",
                        "D=M-D",
                        f"@TRUE_{i}",
                        cmd,
                        "D=0",
                        f"@END_{i}",
                        "0;JMP",
                        f"(TRUE_{i})",
                        "D=-1",  #0xFFFF
                        f"(END_{i})",
                        "@SP",
                        "A=M",
                        "M=D",
                        "@SP",
                        "M=M+1",
                    ]
        elif n == 2:  #  
            pass
        elif n == 3:  #
            op, arg1, arg2 = line
            if op == "push":
                if arg1 == "constant":
                    cmds = [
                            "@"+arg2,
                            "D=A",
                            "@SP",
                            "A=M",
                            "M=D",
                            "@SP",
                            "M=M+1",
                        ] 
                elif arg1 == "local" or arg1 ==  "argument":
                    # push local 1 -> lcl+=1; *sp=*lcl; sp++
                    if arg1 == "local":
                        cmd = "@"+"LCL"
                    elif arg1 == "argument":
                        cmd = "@"+"ARG"
                    cmds = [
                            cmd,
                            "M=M+"+arg2,
                            "A=M",
                            "D=M",
                            "@SP",
                            "A=M",
                            "M=D",
                            "@SP",
                            "M=M+1",
                        ]
                elif arg1 == "static" or arg1 == "pointer":
                    if arg1 == "static":
                        cmd = "@"+sys.argv[0]+"."+arg2
                    elif arg1 == "pointer":
                        cmd = "@THIS" if arg2 == "0" else "@THAT"
                    cmds = [
                            cmd,
                            "A=M",
                            "D=M",
                            "@SP",
                            "A=M",
                            "M=D",
                            "@SP",
                            "M=M+1",
                        ]
                elif arg1 == "temp":
                    cmds = [
                            "@5",
                            "D=A",
                            "@"+arg2,
                            "A=A+D",
                            "D=M"
                            "@SP",
                            "A=M",
                            "M=D",
                            "@SP",
                            "M=M+1",
                        ]
            elif op == "pop":
                if arg1 == "local" or arg1 ==  "argument":
                    # pop local i -> sp--; addr=LCL+i; *addr=*sp;
                    if arg1 == "local":
                        cmd = "@"+"LCL"
                    elif arg1 == "argument":
                        cmd = "@"+"ARG"
                    cmds = [
                            "@SP",
                            "M=M-1",
                            "A=M",
                            "R13=M",
                            cmd,
                            "D=A",
                            "@"+arg2,
                            "A=D+A",
                            "M=R13"
                        ]
                elif arg1 == "static" or arg1 == "pointer":
                    # pop pointer 0 -> sp--; *This=*sp
                    if arg1 == "static":
                        cmd = "@"+sys.argv[0]+"."+arg2
                    elif arg1 == "pointer":
                        cmd = "@THIS" if arg2 == "0" else "@THAT"
                    cmds = [
                            "@SP",
                            "M=M-1",
                            "A=M"
                            "D=M",
                            cmd,
                            "A=M",
                            "M=D",
                        ]
                elif arg1 == "temp":
                    # pop temp i -> sp--; addr=5+i; *addr=*sp
                    cmds = [
                            "@SP",
                            "M=M-1"
                            "A=M",
                            "R13=M"
                            "@5",
                            "D=A",
                            "@"+arg2,
                            "A=D+A",
                            "M=R13"
                        ]
            else:
                pass
        else:
            print("FUCK this")

        asmcode.extend(cmds)

    return asmcode


if __name__ == '__main__':
    ugly_code = load_code()
    code = pre_process(ugly_code)
    asmcode = compile(code)
    save_code(asmcode)
