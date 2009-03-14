include toc2.make
package.distclean_files += plugin-config.h juice-config.h

CB.GEN := createMemFuncCallOp.sh
CB.COUNT := 5# Generate overloads for ClassBinder funcs with N args is 1..$(CB.COUNT)
$(CB.GEN):

########################################################################
# $1 = part of the code to generate: BindMemFunc, MemFuncCallOps, or
# MemFuncCaller
define ClassBinder.GenCode
ClassBinder-$(1).h: ClassBinder.h $(CB.GEN) $(toc2.files.makefile)
	@echo "// GENERATED CODE! Edit at your own risk!" > $$@; \
	x=1; while test $$$$x -le $$(CB.COUNT); do \
		bash $$(CB.GEN) $$$$x $(1) || exit; \
		x=$$$$((x +1 )); \
	done >> $$@; \
	echo "Created $$@ for 1..$$(CB.COUNT) arguments."
package.clean_files += ClassBinder-$(1).h
all: ClassBinder-$(1).h
endef

########################################################################
# Only generate ClassBinder-*.h on demand, not every build:
ifeq (0,1)
$(eval $(call ClassBinder.GenCode,BindMemFunc))
$(eval $(call ClassBinder.GenCode,MemFuncCaller))
$(eval $(call ClassBinder.GenCode,MemFuncCallOps))
endif
# ^^^ ClassBinder gen'd code

package.install.headers := $(wildcard *.h)
