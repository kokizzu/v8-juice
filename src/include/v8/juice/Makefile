include toc2.make
package.distclean_files += plugin-config.h juice-config.h

CB.GEN := createMemFuncCallOp.sh
CB.COUNT := 10# Generate overloads for ClassBinder funcs with N args is 1..$(CB.COUNT)
$(CB.GEN):

########################################################################
# $1 = prefix
# $2 = part of the code to generate: BindMemFunc, MemFuncCallOps, or
# MemFuncCaller
define CallOps.GenCode
# Maintenance reminder: don't give these any deps so that they won't
# be auto-generated on each build. We generate them when we need them
# and store a copy in source control.
$(1)-$(2).h:
	@echo "// GENERATED CODE! Edit at your own risk!" > $$@; \
	echo "#if !defined(DOXYGEN)" >> $$@; \
	x=1; while test $$$$x -le $$(CB.COUNT); do \
		bash $$(CB.GEN) $$$$x $(2) || exit; \
		x=$$$$((x +1 )); \
	done >> $$@; \
	echo "#endif // if !defined(DOXYGEN)" >> $$@; \
	echo "Created $$@ for 1..$$(CB.COUNT) arguments."
genclean-$(1)-$(2):
	@echo "Cleaning up $(1)-$(2).* generated files."
	@-touch foo.bar; rm -f $$(wildcard $(1)-$(2).*) foo.bar
genclean: genclean-$(1)-$(2)
genclean-$(1): genclean-$(1)-$(2)
gen: $(1)-$(2).h
gen-$(1): $(1)-$(2).h
endef


########################################################################
# Only generate ClassBinder-*.h on demand, not every build:
ifeq (1,1)
$(eval $(call CallOps.GenCode,ClassBinder,BindMemFunc))
$(eval $(call CallOps.GenCode,ClassBinder,MemFuncCaller))
$(eval $(call CallOps.GenCode,ClassBinder,MemFuncCallOps))
endif
# ^^^ ClassBinder gen'd code


########################################################################
#
FWD.GEN := createForwarders.sh
FWD.COUNT := 10# Generate overloads for ClassBinder funcs with N args is 1..$(FWD.COUNT)
$(FWD.GEN):

########################################################################
# $1 = prefix
# $2 = part of the code to generate: MemFuncForwarder
define Forwarder.GenCode
# Maintenance reminder: don't give these any deps so that they won't
# be auto-generated on each build. We generate them when we need them
# and store a copy in source control.
.PHONY: $(1)
$(1)-$(2).h:
	@echo "// GENERATED CODE! Edit at your own risk!" > $$@; \
	echo "#if !defined(DOXYGEN)" >> $$@; \
	x=1; while test $$$$x -le $$(FWD.COUNT); do \
		bash $$(FWD.GEN) $$$$x $(2) || exit; \
		x=$$$$((x +1 )); \
	done >> $$@; \
	echo "#endif // if !defined(DOXYGEN)" >> $$@; \
	echo "Created $$@ for 1..$$(FWD.COUNT) arguments."
genclean-$(1)-$(2):
	@echo "Cleaning up $(1)-$(2).* generated files."
	@-touch foo.bar; rm -f $$(wildcard $(1)-$(2).*) foo.bar
genclean: genclean-$(1)-$(2)
gen: $(1)-$(2).h
$(1): $(1)-$(2).h
clean-$(1): genclean-$(1)-$(2)
endef
$(eval $(call Forwarder.GenCode,forwarding,MemFuncForwarder))
$(eval $(call Forwarder.GenCode,forwarding,TMemFuncForwarder))
$(eval $(call Forwarder.GenCode,forwarding,FunctorForwarder))
$(eval $(call Forwarder.GenCode,forwarding,FunctionForwarder))
$(eval $(call Forwarder.GenCode,forwarding,CtorForwarder))

package.install.headers := $(wildcard *.h)
