import sys
from enum import Enum

Type = Enum("Type", "ARITHMETIC PUSH POP LABEL GOTO IF FUNCTION RETURN CALL")
class Parser:

    def __init__(self):
        self.lines = []
        self.idx = 0

    def read_file(self):
        self.filename = sys.argv[1]
        with open(self.filename, 'r') as f:
            self.lines = f.readlines()

    def has_more(self):
        return self.idx < len(self.lines)

    def advance(self):
        self.idx += 1

    def get_type(self):
        line = self.lines[self.idx].strip().split("//")[0].strip().split(" ")
        n = len(line)
        if line == [""]:
            return None
        elif n == 1:
            op = line[0]
            if op == "return":
                return Type.RETURN
            elif op in ["add", "sub", "and", "or", "not", "neg", "eq", "gt", "lt"]:
                return Type.ARITHMETIC, line[0]
            else:
                raise Exception("Wrong command" + str(line))
        elif n == 2:
            op, arg1 = line
            if op == "label":
                return Type.LABEL, arg1
            elif op == "if-goto":
                return Type.IF, arg1
            elif op == "goto":
                return Type.GOTO, arg1
            else:
                raise Exception("Wrong command" + str(line))

        elif n == 3:
            op, arg1, arg2 = line
            if op == "push":
                return Type.PUSH, arg1, arg2
            elif op == "pop":
                return Type.POP, arg1, arg2
            elif op == "call":
                return Type.CALL, arg1, arg2
            elif op == "function":
                return Type.FUNCTION, arg1, arg2
            else:
                raise Exception("Wrong command" + str(line))
        else:
            raise Exception("Wrong command" + str(line))

class CodeWriter:

    def __init__(self):
        self.filename = None

    def open_file(self):
        self.filename = sys.argv[1].split(".")[0] + ".asm"
        pass

    def write(self, line):
        if line is None:
            return
        op = line[0]
#Type = Enum("Type", "ARITHMETIC PUSH POP LABEL GOTO IF FUNCTION RETURN CALL")
        if op == Type.ARITHMETIC:
            self.write(line)
        elif op == Type.PUSH:
            self.write_push(line)
        elif op == Type.POP:
            self.write_pop(line)
        elif op == Type.LABEL:
            self.write_label(line)
        elif op == Type.GOTO:
            self.write_goto(line)
        elif op == Type.IF:
            self.write_if(line)
        elif op == Type.FUNCTION:
            self.write_function(line)
        elif line[0] == Type.RETURN:
            self.write_return(line)
        else:
            self.write_call(line)


            

    def write_init(self, line):
        pass

    def write_arithmetic(self, line):
        pass

    def write_push(self, line):
        pass

    def write_pop(self, line):
        pass

    def write_label(self, line):
        pass

    def write_goto(self, line):
        pass

    def write_if(self, line):
        pass

    def write_call(self, line):
        pass

    def write_return(self, line):
        pass

    def write_function(self, line):
        pass

    def close_file(self, line):
        pass

if __name__ == "__main__":
    parser = Parser()
    parser.read_file()

    writer = CodeWriter()
    
    while parser.has_more():
        parser.advance()
        cmd = parser.get_type()
        writer.write(cmd)
    writer.close_file()

