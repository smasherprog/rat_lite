.PHONY: clean All

All:
	@echo "----------Building project:[ Desktop_Client - Mac_Debug ]----------"
	@cd "Desktop_Client" && "$(MAKE)" -f  "Desktop_Client.mk"
clean:
	@echo "----------Cleaning project:[ Desktop_Client - Mac_Debug ]----------"
	@cd "Desktop_Client" && "$(MAKE)" -f  "Desktop_Client.mk" clean
