all:
	make delete || true
	make -f wattsfarmer.mk
	make -f repeat-a.mk
	make -f masterballs.mk
	make -f wildareabreeding.mk
	make -f routebreeding.mk
	make -f releasebox.mk
	make -f move30days.mk

watts:
	make -f wattsfarmer.mk

repeat-a:
	make -f repeat-a.mk

balls:
	make -f masterballs.mk

wildarea:
	make -f wildareabreeding.mk

release:
	make -f releasebox.mk

route:
	make -f routebreeding.mk

movedays:
	make -f move30days.mk

delete:
	rm *.bin *.elf *.hex *.lss *.map *.sym *.eep
