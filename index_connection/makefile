all:
	clang++ -o main main.cpp -std=c++14 -lboost_system

doc: doc-pdf doc-md
	git add README.tex
	git commit -m "updated readme"

doc-pdf:
	pdflatex README.tex	
	rm README.aux README.log
	git add README.pdf

doc-md:
	pandoc -f latex -t markdown README.tex > README.md
	git add README.md

clean:
	rm -f *.o
