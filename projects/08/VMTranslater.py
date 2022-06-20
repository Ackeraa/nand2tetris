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
        pass

    def write_init(self):
        pass

    def write_arithmetic(self):
        pass

    def write_push(self):
        pass

    def write_pop(self):
        pass

    def write_label(self):
        pass

    def write_goto(self):
        pass

    def write_if(self):
        pass

    def write_call(self):
        pass

    def write_return(self):
        pass

    def write_function(self):
        pass

    def close_file(self):
        pass

if __name__ == "__main__":
    parser = Parser()
    parser.read_file()

    writer = CodeWriter()
    
    while parser.has_more():
        parser.advance()
        op, arg1, arg2 = parser.get_type()
        writer.write(op, arg1, arg2)
    writer.close_file()

