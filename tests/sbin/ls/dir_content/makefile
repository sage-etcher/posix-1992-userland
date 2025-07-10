
PROG=	tests-ls-dir_content
SRCS=	main.c
OBJS=	main.o $(LSLIB)
MAN=	

run:
	LD_LIBRARY_PATH="$(LSPATH):$(TAPPATH)" $(TAPPATH)/harness ./$(PROG)

include $(PROJECT_ROOT)/maketools/posix.prog.mk
# vim: noet
# end of file 
