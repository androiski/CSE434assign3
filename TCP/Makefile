all: tcpclient tcpserver

tcpclient: tcp_client.c
    gcc -o tcpclient tcp_client.c

tcpserver: tcp_server.c
    gcc -o tcpserver tcp_server.c
clean: 
	rm tcpclient tcpserver 