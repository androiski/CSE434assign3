all: udpclient udpserver

udpclient: udp_client.c
    gcc -o udpclient udp_client.c

udpserver: udp_server.c
    gcc -o udpserver udp_server.c
	> serverlog.txt
	
clean: 
	rm udpclient udpserver serverlog.txt