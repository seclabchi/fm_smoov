.PHONY: clean All

All:
	@echo "----------Building project:[ audiod - Debug ]----------"
	@"$(MAKE)" -f  "audiod.mk" && "$(MAKE)" -f  "audiod.mk" PostBuild
clean:
	@echo "----------Cleaning project:[ audiod - Debug ]----------"
	@"$(MAKE)" -f  "audiod.mk" clean
