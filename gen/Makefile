CC = g++
CCFLAGS = -g
FLEX = flex
BISON = bison
LATEX = latex
DVIPS = dvips

all: Testcthulhu

clean:
	rm -f *.o cthulhu.dvi cthulhu.aux cthulhu.log cthulhu.ps Testcthulhu

distclean:
	 rm -f *.o Absyn.C Absyn.H Test.C Parser.C Parser.H Lexer.C Skeleton.C Skeleton.H Printer.C Printer.H cthulhu.l cthulhu.y cthulhu.tex cthulhu.dvi cthulhu.aux cthulhu.log cthulhu.ps Testcthulhu Parser.output

Testcthulhu: Absyn.o Lexer.o Parser.o Printer.o Test.o Skeleton.o
	@echo "Linking Testcthulhu..."
	${CC} ${CCFLAGS} *.o -o Testcthulhu
        
Absyn.o: Absyn.C Absyn.H
	${CC} ${CCFLAGS} -c Absyn.C

Lexer.C: cthulhu.l
	${FLEX} -oLexer.C cthulhu.l

Parser.C: cthulhu.y
	${BISON}  cthulhu.y -v -o Parser.C

Lexer.o: Lexer.C Parser.H
	${CC} ${CCFLAGS} -c Lexer.C 

Parser.o: Parser.C Absyn.H
	${CC} ${CCFLAGS} -c Parser.C

Printer.o: Printer.C Printer.H Absyn.H
	${CC} ${CCFLAGS} -c Printer.C

Skeleton.o: Skeleton.C Skeleton.H Absyn.H
	${CC} ${CCFLAGS} -c Skeleton.C

Test.o: Test.C Parser.H Printer.H Absyn.H
	${CC} ${CCFLAGS} -c Test.C

cthulhu.dvi: cthulhu.tex
	${LATEX} cthulhu.tex

cthulhu.ps: cthulhu.dvi
	${DVIPS} cthulhu.dvi -o cthulhu.ps

