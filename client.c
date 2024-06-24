#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080

int main() {
    int sock = 0, choice;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};
    char userName[100];

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        return -1;
    }

    // Connect to server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection Failed");
        return -1;
    }

    // Get user name
printf("****************************************** ");
printf("\n***********WELCOME TO THE HOTEL***********\n ");
printf("****************************************** \n");

    printf("Enter your name: ");
    fgets(userName, sizeof(userName), stdin);
    strtok(userName, "\n");  // Remove newline character from userName

    // Send user name to server
    send(sock, userName, strlen(userName), 0);

    // Receive and display initial menu options from server
    read(sock, buffer, 1024);
    printf("%s", buffer);

    // Loop to interact with server
    while (1) {
        // Get user's choice
        scanf("%d", &choice);
        getchar();  // Consume newline character left in input buffer

        // Send choice to server
        send(sock, &choice, sizeof(int), 0);

        // Process server responses
        switch (choice) {
            case 1:
            case 2:
                // Ask for room number
                printf("Enter room number: ");
                scanf("%d", &choice);
                getchar();  // Consume newline character left in input buffer
                send(sock, &choice, sizeof(int), 0);
                break;
            case 4:
                close(sock);
                printf("Disconnected from server.\n");
                return 0;
            default:
                break;
        }

        // Receive and display menu options again from server
        read(sock, buffer, 1024);
        printf("%s", buffer);
    }

    return 0;
}

