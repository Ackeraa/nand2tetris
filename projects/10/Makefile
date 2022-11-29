objects = obj/JackAnalyzer.o \
					obj/JackTokenizer.o \
					obj/CompilationEngine.o \
					obj/SymbolTable.o \
					obj/VMWriter.o \
					obj/CodeWriter.o	\
					obj/Utils.o

obj/%.o: src/%.cpp
	mkdir -p obj
	g++ -c -o $@ $<

JackCompiler: $(objects)
	g++ -o JackCompiler $(objects)

clean:
	rm -rf obj JackCompiler

