#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 8080

// Function prototypes
void *handleClient(void *arg);
void displayAvailableRooms();
void bookRoom(int roomNumber);
void checkOutRoom(int roomNumber);

// Global variables
int availableRooms[10] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1};  // 10 rooms initially available

int main() {
    int server_fd, client_socket, opt = 1;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    pthread_t tid;

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("Setsockopt failed");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind the socket to localhost port 8080
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Hotel management server is running on port %d...\n", PORT);

    while (1) {
        // Accept a new client connection
        if ((client_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }

        // Create a new thread for handling client
        if (pthread_create(&tid, NULL, handleClient, (void *)&client_socket) != 0) {
            perror("Thread creation failed");
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}

void *handleClient(void *arg) {
    int client_socket = *((int *)arg);
    char buffer[1024] = {0};
    int choice;

    // Read client's name
    read(client_socket, buffer, 1024);
    strtok(buffer, "\n");  // Remove newline character from userName

    // Display available rooms initially
    displayAvailableRooms();

    // Send menu options to client
    strcpy(buffer, "\n1. Check In\n2. Check Out\n3. Display Available Rooms\n4. Exit\nEnter your choice: ");
    send(client_socket, buffer, strlen(buffer), 0);

    // Receive and process client requests
    while (read(client_socket, &choice, sizeof(int))) {
        switch (choice) {
            case 1:
                bookRoom(client_socket);
                break;
            case 2:
                checkOutRoom(client_socket);
                break;
            case 3:
                displayAvailableRooms();
                break;
            case 4:
                close(client_socket);
                printf("Client disconnected.\n");
                pthread_exit(NULL);
            default:
                printf("Invalid choice received from client.\n");
        }
        // Send menu options to client again
        strcpy(buffer, "\n1. Check In\n2. Check Out\n3. Display Available Rooms\n4. Exit\nEnter your choice: ");
        send(client_socket, buffer, strlen(buffer), 0);
    }

    close(client_socket);
    pthread_exit(NULL);
}

void displayAvailableRooms() {
    printf("\nAvailable Rooms:\n");
    for (int i = 0; i < 10; ++i) {
        if (availableRooms[i] == 1) {
            printf("Room %d\n", i + 1);
        }
    }
}

void bookRoom(int client_socket) {
    int roomNumber;
    read(client_socket, &roomNumber, sizeof(int));

    if (roomNumber < 1 || roomNumber > 10) {
        printf("Invalid room number received from client.\n");
        return;
    }

    // Attempt to book the room
    if (availableRooms[roomNumber - 1] == 1) {
        availableRooms[roomNumber - 1] = 0;  // Mark room as booked
        printf("Room %d booked successfully.\n", roomNumber);
    } else {
        printf("Room %d is already booked. Please choose another room.\n", roomNumber);
    }
}

void checkOutRoom(int client_socket) {
    int roomNumber;
    read(client_socket, &roomNumber, sizeof(int));

    if (roomNumber < 1 || roomNumber > 10) {
        printf("Invalid room number received from client.\n");
        return;
    }

    // Mark the room as available
    availableRooms[roomNumber - 1] = 1;
    printf("Room %d checked out successfully.\n", roomNumber);
}

