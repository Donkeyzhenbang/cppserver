server:
	g++ socket_client.cpp -o client && g++ socket_server.cpp -o server 
clean:
	rm client server