PANDOC=pandoc

all: makenl.html whatsnew.html readme.html

makenl.html: makenl.md
	$(PANDOC) -o makenl.html makenl.md --self-contained --metadata pagetitle='MakeNL NODELIST Generator Program by Ben Baker'

whatsnew.html: whatsnew.md
	$(PANDOC) -o whatsnew.html whatsnew.md --self-contained --metadata pagetitle='MakeNL Revision History'

readme.html: readme.md
	$(PANDOC) -o readme.html readme.md --self-contained --metadata pagetitle='MakeNL read me first'

clean:
	rm -f makenl.html
	rm -f whatsnew.html
	rm -f readme.html
