server:
	g++ socket_client.cpp utils.cpp -o client && g++ socket_server.cpp utils.cpp -o server 
clean:
	rm client server