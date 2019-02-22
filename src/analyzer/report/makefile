LATEX := pdflatex -halt-on-error
FIGURES :=
.SUFFIXES : .tex .bbl .aux .eps .ps .dot .pdf

.tex.bbl:
	bibtex $*

.tex.aux:
	latex $*

%.eps: %.ps
	ps2epsi $< $@

%.ps: %.dot
	dot -Tps $< -o $@

%.pdf: %.dot
	dot -Tpdf $< -o $@

%.png: %.dot
	dot -Tpng $< -o $@

%.aux: %.tex
	$(LATEX) $<

#%.dot: %.dotpiece $(FIGHEAD) $(FIGFOOT)
#	cat $(FIGHEAD) $< $(FIGFOOT) > $@

# latex the paper (default)
new:	biblinks newpaper

# latex + bibtex the paper
bib:	bibpaper newpaper

# save a backup of tex files in BACKUPS
bak: 	backuppaper

biblinks: figures
	$(LATEX) paper.tex
	$(LATEX) paper.tex

bibsuppl: figures
	$(LATEX) suppl.tex
	$(LATEX) suppl.tex

cl: cover-letter.tex
	$(LATEX) cover-letter.tex

newpaper: figures biblinks
	$(LATEX) paper.tex

suppl: figures bibsuppl
	$(LATEX) suppl.tex


bibpaper: paper.aux paper.bbl
	latex paper.tex


backuppaper: 
	@if [ ! -d BACKUPS ]; then mkdir BACKUPS; fi;	\
	crtdate=`date '+%m-%d-%y--%H:%M:%S'`;		\
	bakdir=BACKUPS/$$crtdate;			\
	mkdir $$bakdir;					\
	cp *.tex $$bakdir

spell:
	@for i in *.tex; \
	do if [ $$i != "paper.tex" ]; then aspell -c $$i -p ./spell.lst; fi;\
	done

total:
	pdflatex paper.tex
	pdflatex paper.tex
	pdflatex paper.tex
	@echo ================================================================
	pdflatex paper.tex | grep -E 'arning|erfull'
	@echo ================================================================

figures: $(FIGURES)

errinjfig:
	gnuplot err-inj-fig/nve2latex.cmds
	epstopdf injectErrorFig.eps
	mv injectErrorFig.* figures


clean:
	rm -f *.dvi *.log *.aux *.blg *.bbl *~ err-inj-fig/*~
	rm -f paper.ps paper.pdf 
	rm -f injectErrorFig*
	rm -rf $(PDF_DIR)
