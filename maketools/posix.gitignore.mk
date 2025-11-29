
.gitignore: makefile
	rm -f $@.tmp
	-cat $@ >$@.tmp
	echo "$@.tmp" >>$@.tmp
	for line in $(GITIGNORE_LINES); do \
		echo "$$line" >> $@.tmp; \
	done
	cat $@.tmp |sort |uniq >$@
	
build_gitignore: .gitignore

.PHONY: build_gitignore

# vim: noet
# end of file
