MAKE = make --no-print-directory

define ECHO
	@printf "\033[;31m"; printf $1; printf "\033[0m\n"
endef

.PHONY: debug release profile clean

debug release profile clean:
	@$(call ECHO, "[build lib yut]")
	@$(MAKE) -f mak/libyut.mak

	@$(call ECHO , "[build lib common]")
	@$(MAKE) -f mak/libcommon.mak

	@$(call ECHO , "[build test]")
	@$(MAKE) -f mak/test.mak