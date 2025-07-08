
EXTERN	+=	"PROJECT_ROOT=$(PROJECT_ROOT)"
EXTERN	+=	"USE_SBIN=$(USE_SBIN)"

build clean locale install uninstall depend debug:
	$(MAKE) -C core $@ -- $(EXTERN)
	$(MAKE) -C main $@ -- $(EXTERN)


.PHONY: build clean locale install uninstall depend debug
# vim: noet
# end of file 
