import unittest
from VMTranslater import *

class TestParser(unittest.TestCase):

    def basic(self, lines, rhs): 
        parser = Parser()
        parser.lines = lines
        for i in range(len(parser.lines)):
            parser.idx = i
            lhs = parser.get_type()
            self.assertEqual(lhs, rhs[i])

    def test_arithmetic(self):
        lines = ["add", "sub", "neg", "eq",
               "gt", "lt", "and", "or", "not"] 
        rhs = []
        for i in range(len(lines)):
            rhs.append((Type.ARITHMETIC, lines[i]))
        self.basic(lines, rhs)

    def test_pushpop(self):
        lines = ["push local 1", "pop temp 2"] 
        rhs = [(Type.PUSH, "local", "1"), (Type.POP, "temp", "2")]
        self.basic(lines, rhs)

    def test_label(self):
        lines = ["label fucj", "label fun"] 
        rhs = [(Type.LABEL, "fucj"), (Type.LABEL, "fun")]
        self.basic(lines, rhs)

    def test_goto(self):
        lines = ["goto loop", "goto end"] 
        rhs = [(Type.GOTO, "loop"), (Type.GOTO, "end")]
        self.basic(lines, rhs)

    def test_if(self):
        lines = ["if-goto loop", "if-goto end"] 
        rhs = [(Type.IF, "loop"), (Type.IF, "end")]
        self.basic(lines, rhs)

    def test_function(self):
        lines = ["function fun 1", "function fun2 2"] 
        rhs = [(Type.FUNCTION, "fun", "1"), (Type.FUNCTION, "fun2", "2")]
        self.basic(lines, rhs)

    def test_return(self):
        lines = ["return"] 
        rhs = [Type.RETURN]
        self.basic(lines, rhs)

    def test_call(self):
        lines = ["call fun 1", "call fun2 2"] 
        rhs = [(Type.CALL, "fun", "1"), (Type.CALL, "fun2", "2")]
        self.basic(lines, rhs)

    def test_arb(self):
        lines = ["//return", "return //123213231", "            return //1221312", "             "] 
        rhs = [None, Type.RETURN, Type.RETURN, None]
        self.basic(lines, rhs)

if __name__ == "__main__":
    unittest.main()



