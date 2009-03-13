include toc2.make
package.distclean_files += plugin-config.h juice-config.h

CB1.H := ClassBinder-MemFuncCallOpN.h
CB2.H := ClassBinder-BindMemFunc.h
CB.GEN := createMemFuncCallOp.sh
CB.COUNT := 5# Generate MemFunCallOpN, where N is 1..$(CB.COUNT)
ifeq (0,1)# only enable this to rebuild the ClassBinder gen'd code
$(CB.GEN):
# FIXME: combine this code into a single `define` block:
$(CB1.H): $(CB.GEN) ClassBinder.h $(toc2.files.makefile)
	@echo "// GENERATED CODE! Edit at your own risk!" > $@; \
	echo "#if ! defined(DOXYGEN)" >> $@; \
	x=1; while test $$x -le $(CB.COUNT); do \
		bash $(CB.GEN) $$x MemFuncCallOps; \
		x=$$((x +1 )); \
	done >> $@ || exit; \
	echo "#endif //if !defined(DOXYGEN)" >> $@; \
	echo "Created $@ for 1..$(CB.COUNT) arguments."
$(CB2.H): $(CB.GEN) ClassBinder.h $(toc2.files.makefile)
	@echo "// GENERATED CODE! Edit at your own risk!" > $@; \
	echo "#if ! defined(DOXYGEN)" >> $@; \
	x=1; while test $$x -le $(CB.COUNT); do \
		bash $(CB.GEN) $$x BindMemFunc; \
		x=$$((x +1 )); \
	done >> $@ || exit; \
	echo "#endif //if !defined(DOXYGEN)" >> $@; \
	echo "Created $@ for 1..$(CB.COUNT) arguments."
all: $(CB1.H) $(CB2.H)
package.clean_files += $(CB1.H) $(CB2.H)
endif
# ^^^ ClassBinder gen'd code

package.install.headers := $(wildcard *.h)
