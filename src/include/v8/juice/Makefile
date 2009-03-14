include toc2.make
package.distclean_files += plugin-config.h juice-config.h

CB.GEN := createMemFuncCallOp.sh
CB.COUNT := 8# Generate overloads for ClassBinder funcs with N args is 1..$(CB.COUNT)
$(CB.GEN):

########################################################################
# $1 = part of the code to generate: BindMemFunc, MemFuncCallOps, or
# MemFuncCaller
define ClassBinder.GenCode
ClassBinder-$(1).h:
	@echo "// GENERATED CODE! Edit at your own risk!" > $$@; \
	x=1; while test $$$$x -le $$(CB.COUNT); do \
		bash $$(CB.GEN) $$$$x $(1) || exit; \
		x=$$$$((x +1 )); \
	done >> $$@; \
	echo "Created $$@ for 1..$$(CB.COUNT) arguments."
genclean-$(1):
	@echo "Cleaning up ClassBinder-$(1).* generated files."
	@-touch foo.bar; rm -f $$(wildcard ClassBinder-$(1).*) foo.bar
genclean: genclean-$(1)
gen: ClassBinder-$(1).h
endef

########################################################################
# Only generate ClassBinder-*.h on demand, not every build:
ifeq (1,1)
$(eval $(call ClassBinder.GenCode,BindMemFunc))
$(eval $(call ClassBinder.GenCode,MemFuncCaller))
$(eval $(call ClassBinder.GenCode,MemFuncCallOps))
endif
# ^^^ ClassBinder gen'd code

package.install.headers := $(wildcard *.h)
