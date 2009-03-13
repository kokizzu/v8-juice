include toc2.make
package.distclean_files += plugin-config.h juice-config.h

CBG.H := ClassBinder-MemFuncCallOpN.h
CBG.GEN := createMemFuncCallOp.sh
CBG.COUNT := 5# Generate MemFunCallOpN, where N is 1..$(CBG.COUNT)
ifeq (1,0)
$(CBG.GEN):
$(CBG.H): $(CBG.GEN) ClassBinder.h $(toc2.files.makefile)
	@echo "// GENERATED CODE! Edit at your own risk!" > $@; \
	x=1; while test $$x -le $(CBG.COUNT); do \
		bash $(CBG.GEN) $$x; \
		x=$$((x +1 )); \
	done >> $@ || exit; \
	echo "Created $@ for 1..$(CBG.COUNT) arguments."
all: $(CBG.H)
package.clean_files += $(CBG.H)
endif

package.install.headers := $(wildcard *.h)
