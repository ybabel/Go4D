DOCXX = ../doc++

html:
	$(DOCXX) doc.dxx

tex:
	$(DOCXX) -v -t -o doc.tex -ep a4wide doc.dxx
	latex doc
	latex doc

ps:	tex
	dvips -o doc.ps doc.dvi

clean:
	\rm -rf doc.ps doc.tex doc.aux doc.toc doc.dvi doc.log core *.swp *~	\
		*.html *.gif dxx* *.class gifs.db .1.html
