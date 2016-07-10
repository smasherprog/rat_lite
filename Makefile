.PHONY: clean All

All:
	@echo "----------Building project:[ Core - Linux_Debug ]----------"
	@cd "Core" && "$(MAKE)" -f  "Core.mk"
	@echo "----------Building project:[ Desktop_Server - Linux_Debug ]----------"
	@cd "Desktop_Server" && "$(MAKE)" -f  "Desktop_Server.mk" && "$(MAKE)" -f  "Desktop_Server.mk" PostBuild
clean:
	@echo "----------Cleaning project:[ Core - Linux_Debug ]----------"
	@cd "Core" && "$(MAKE)" -f  "Core.mk"  clean
	@echo "----------Cleaning project:[ Desktop_Server - Linux_Debug ]----------"
	@cd "Desktop_Server" && "$(MAKE)" -f  "Desktop_Server.mk" clean
