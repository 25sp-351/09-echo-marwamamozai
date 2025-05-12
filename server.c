#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#define DEFAULT_PORT 2345
#define BUFFER_SIZE 1024

// FUNC to make sure that it is able to handle teh client connection 
void *handle_client(void *arg) {
    int client_socket = *((int *)arg);
    char buffer[BUFFER_SIZE];
    
    while (1) {
        memset(buffer, 0, BUFFER_SIZE); // buff cleaere
        
        int bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        
        if (bytes_received <= 0) {
            break; 
        }

        // echo back to receiving client 
        send(client_socket, buffer, bytes_received, 0);
        

        printf("Received message: %s", buffer);
    }
    
    close(client_socket);
    return NULL;
}




// FUCNTION FOR MAIN SERVER 
void start_server(int port, int verbose) {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    
    // socket
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(1);
    }
    
    // Setting up the server address struct
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY; // Listen on all interfaces
    server_addr.sin_port = htons(port);

    
    // bind the socket to the address
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Bind failed");
        close(server_socket);
        exit(1);
    }

    //  this is to listen for incoming connections
    if (listen(server_socket, 1) == -1) {
        perror("Listen failed");
        close(server_socket);
        exit(1);
    }

    printf("Server listening on port %d...\n", port);
    
    //accept incoming client connections
    while (1) {
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_socket == -1) {
            perror("Accept failed");
            continue; 
        }

        //dreate a thread to handle each client
        pthread_t client_thread;
        pthread_create(&client_thread, NULL, handle_client, (void *)&client_socket);
        pthread_detach(client_thread); // detach thread so cleans up automatically
    }

    close(server_socket); // close server
}

// FUNC main func that takes care of arg and starts server
int main(int argc, char *argv[]) {
    int port = DEFAULT_PORT;
    int verbose = 0;

    // parse command arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-p") == 0 && i + 1 < argc) {
            port = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-v") == 0) {
            verbose = 1;
        }
    }

    // starting the server
    start_server(port, verbose);

    return 0;
}
