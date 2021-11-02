import sys

def load_code():
    filename = sys.argv[1]
    with open(filename, 'r') as f:
        return f.readlines()

def save_encode(code):
    filename = sys.argv[1].split(".")[0] + ".hack"
    with open(filename, 'w') as f:
        for line in code:
            f.write(line + "\n")

def check_int(s):
    if s[0] == '-':
        return s[1:].isdigit()
    return s.isdigit()

def pre_process(code):
    code2 = []
    symbols = {}

    # pre-defined
    symbols["SP"] = 0
    symbols["LCL"] = 1
    symbols["ARG"] = 2 
    symbols["THIS"] = 3 
    symbols["THAT"] = 4 
    symbols["SCREEN"] = 16384
    symbols["KBD"] =  24576
    for i in range(16):
        symbols["R" + str(i)] = i

    # scan
    for line in code:
        line = line.strip()
        # blank line
        if line.startswith("//") or len(line) == 0:
            continue
        line = line.split("//")[0]
        line = line.replace(" ", "")
        if line[0] == "(":
            # label symbol
            symbols[line[1:-1]] = len(code2)
        else:
            code2.append(line)

    return code2, symbols

def complie(code, symbols):
    encode = []
    addressn = 16
    for line in code:
        # command type
        if line.startswith("@"):
            command_type = "A"
            if check_int(line[1:]):
                symbol = int(line[1:])
            else:
                if line[1:] in symbols:
                    symbol = symbols[line[1:]]
                else:
                    #variabel symbol
                    symbols[line[1:]] = addressn
                    addressn += 1
                    symbol = symbols[line[1:]]
        else:
            command_type = "C"
            if "=" in line:
                dest, comp = line.split("=")
                jump = "null"
            else:
                comp, jump = line.split(";")
                dest = "null"
            
        # encode
        if command_type == "A":
            if symbol >= 0:
                binary = format(symbol, '#017b')
            else:
                binary = bin((1<<15) + symbol)

            encode.append('0' + binary[2:])
        else:
            # dest
            if dest == "null":
                dest_code = "000"
            elif dest == "M":
                dest_code = "001"
            elif dest == "D":
                dest_code = "010"
            elif dest == "MD":
                dest_code = "011"
            elif dest == "A":
                dest_code = "100"
            elif dest == "AM":
                dest_code = "101"
            elif dest == "AD":
                dest_code = "110"
            elif dest == "AMD":
                dest_code = "111"
            else:
                print("FUCK dest_code")

            # jump
            if jump == "null":
                jump_code="000"
            elif jump == "JGT":
                jump_code="001"
            elif jump == "JEQ":
                jump_code="010"
            elif jump == "JGE":
                jump_code="011"
            elif jump == "JLT":
                jump_code="100"
            elif jump == "JNE":
                jump_code="101"
            elif jump == "JLE":
                jump_code="110"
            elif jump == "JMP":
                jump_code="111"
            else:
                print("FUCK jump_code")

            # comp
            if comp == "0":
                comp_code = "0101010"
            elif comp == "1":
                comp_code = "0111111"
            elif comp == "-1":
                comp_code = "0111010"
            elif comp == "D":
                comp_code = "0001100"
            elif comp == "A":
                comp_code = "0110000"
            elif comp == "!D":
                comp_code = "0001101"
            elif comp == "!A":
                comp_code = "0110001"
            elif comp == "-D":
                comp_code = "0001111"
            elif comp == "-A":
                comp_code = "0110011"
            elif comp == "D+1":
                comp_code = "0011111"
            elif comp == "A+1":
                comp_code = "0110111"
            elif comp == "D-1":
                comp_code = "0001110"
            elif comp == "A-1":
                comp_code = "0110010"
            elif comp == "D+A":
                comp_code = "0000010"
            elif comp == "D-A":
                comp_code = "0010011"
            elif comp == "A-D":
                comp_code = "0000111"
            elif comp == "D&A":
                comp_code = "0000000"
            elif comp == "D|A":
                comp_code = "0010101"
            elif comp == "M":
                comp_code = "1110000"
            elif comp == "!M":
                comp_code = "1110001"
            elif comp == "-M":
                comp_code = "1110011"
            elif comp == "M+1":
                comp_code = "1110111"
            elif comp == "M-1":
                comp_code = "1110010"
            elif comp == "D+M":
                comp_code = "1000010"
            elif comp == "D-M":
                comp_code = "1010011"
            elif comp == "M-D":
                comp_code = "1000111"
            elif comp == "D&M":
                comp_code = "1000000"
            elif comp == "D|M":
                comp_code = "1010101"
            else:
                print("FUCK comp_code")

            encode.append("111" + comp_code + dest_code + jump_code)

    return encode

if __name__ == '__main__':
    ugly_code = load_code()
    code, symbols = pre_process(ugly_code)
    encode = complie(code, symbols)
    save_encode(encode)
    '''
    for kv in symbols.items():
        print(kv)
    '''
