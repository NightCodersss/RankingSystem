export CXX=clang++
export PROJECTPATH=$(shell pwd)
export INCLUDEPATH=-I$(PROJECTPATH)/config_loader/ -I$(PROJECTPATH)/UbjsonCpp/include/ -I$(PROJECTPATH)/ -I$(PROJECTPATH)/common/
export LIBRARYPATH=-L$(PROJECTPATH)/UbjsonCpp/src -L$(PROJECTPATH)/config_loader -L$(PROJECTPATH)/common
SUBDIRECTORIES=config_loader common index_connection south_protocol ranking_server forward_index_server force_ranking_server

.PHONY: all $(SUBDIRECTORIES)

all: stop-servers $(SUBDIRECTORIES) 

$(SUBDIRECTORIES):
	$(MAKE) -C $@

stop-servers:
	./stopservers.sh || true

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

log_clean:
	rm -f ./ranking_server/log.log
	rm -f ./index_connection/log.log
	rm -f ./south_protocol/log.log

test:
	./ddos_script.py 100
	echo "38 is average for random permutation on 10 elements"

show_todo:
	echo "" > source_files
	find -name *.cpp >> source_files
	find -name *.hpp >> source_files
	grep "TODO" -ni `cat source_files` | sed "s/\:[^#]*\/\//\t/g"
	rm source_files

show_note:
	echo "" > source_files
	find -name *.cpp >> source_files
	find -name *.hpp >> source_files
	grep "NOTE" -ni `cat source_files` | sed "s/\:[^#]*\/\//\t/g"
	rm source_files

show_warn:
	echo "" > source_files
	find -name *.cpp >> source_files
	find -name *.hpp >> source_files
	grep "WARN" -ni `cat source_files` | sed "s/\:[^#]*\/\//\t/g"
	rm source_files
