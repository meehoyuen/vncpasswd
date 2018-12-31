all:vncpaswd

vncpasswd: vncpasswd.c d3des.c d3des.h
	gcc -o $@ $<

clean:
	rm -fr vncpasswd