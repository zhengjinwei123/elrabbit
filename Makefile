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

	@$(call ECHO, "[build demo_server1]")
	@$(MAKE) -f mak/demo_server.mak $@

	@$(call ECHO, "[build demo_client1]")
	@$(MAKE) -f mak/demo_client.mak $@

	@$(call ECHO, "[build c11]")
	@$(MAKE) -f mak/c11.mak $@