
PROG=	tests-ls-snprintlu
SRCS=	main.c
OBJS=	main.o
MAN=	

run:
	LD_LIBRARY_PATH="$(LSPATH):$(TAPPATH)" $(TAPPATH)/harness ./$(PROG)

include $(PROJECT_ROOT)/maketools/posix.prog.mk
# vim: noet
# end of file 
