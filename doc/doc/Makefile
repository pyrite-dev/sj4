.PHONY: all
.SUFFIXES: .ms .html

all: hinsi.html makedict.html

.ms.html:
	groff -ms -Kutf8 -Thtml < $< | sed -E 's/(&[^;]+;) /\1/g' > $@
