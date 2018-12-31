all:gitconfig clean vncpasswd

gitconfig:
	@git config --global user.email "meeho@meeho.com"
	@git config --global user.name "meeho"

vncpasswd: vncpasswd.c d3des.c d3des.h
	@gcc -o $@ $^
	@strip $@
	@./vncpasswd

clean:
	@rm -fr vncpasswd
