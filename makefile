SUBDIRECTORIES=index_connection

all:
	for d in $(SUBDIRECTORIES); do (cd $$d; $(MAKE) ); done

doc: doc-pdf doc-md
	git add README.tex
	git commit -m "updated readme"

doc-pdf:
	pdflatex README.tex	#> /dev/null
	rm README.aux README.log
	git add README.pdf

doc-md:
	pandoc -f latex -t markdown README.tex > README.md
	git add README.md

clean:
	for d in $(SUBDIRECTORIES); do (cd $$d; $(MAKE) clean ); done
