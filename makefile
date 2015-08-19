export PROJECTPATH=/home/asio/rankingsystem/
SUBDIRECTORIES=config_loader index_connection south_protocol ranking_server generatingserver

.PHONY: all $(SUBDIRECTORIES)

all: $(SUBDIRECTORIES)

$(SUBDIRECTORIES):
	$(MAKE) -C $@

generatingserver: config_loader
ranking_server: config_loader 
south_protocol: config_loader 
index_connection: config_loader 

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
