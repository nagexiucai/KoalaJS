.PHONY: clean All

All:
	@echo "----------Building project:[ TinyJSMobile - Debug ]----------"
	@"$(MAKE)" -f  "TinyJSMobile.mk"
clean:
	@echo "----------Cleaning project:[ TinyJSMobile - Debug ]----------"
	@"$(MAKE)" -f  "TinyJSMobile.mk" clean
