all:
	clang++ -o main main.cpp -std=c++14 -lboost_system

doc:
	pdflatex README.tex
	rm README.aux README.log

clean:
	rm -f *.o
