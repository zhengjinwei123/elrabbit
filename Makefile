MAKE = make --no-print-directory

define ECHO
	@printf "\033[;31m"; printf $1; printf "\033[0m\n"
endef

.PHONY: debug release profile clean

debug release profile clean:
	@$(call ECHO, "[build libtinyxml2]")
	@$(MAKE) -f mak/libtinyxml2.mak $@

	@$(call ECHO, "[build session_server]")
	@$(MAKE) -f mak/session_server.mak $@