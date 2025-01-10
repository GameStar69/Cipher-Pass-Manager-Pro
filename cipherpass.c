#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <termios.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <ctype.h> // Include ctype.h for islower, isupper, etc.

void printCentered(const char *text) {
    int width = 80;  // Assuming terminal width of 80 columns
    int len = strlen(text);
    int padding = (width - len) / 2;
    printf("%*s%s%*s\n", padding, "", text, padding, "");
}

// Function to turn off echoing in terminal
void disableEcho() {
    struct termios tty;
    tcgetattr(STDIN_FILENO, &tty);
    tty.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &tty);
}

// Function to turn on echoing in terminal
void enableEcho() {
    struct termios tty;
    tcgetattr(STDIN_FILENO, &tty);
    tty.c_lflag |= ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &tty);
}

// Caesar Cipher for Basic Encryption/Decryption (for illustration purposes)
void encryptPassword(char *password) {
    int shift = 3;  // Simple shift for Caesar Cipher
    for (int i = 0; password[i] != '\0'; i++) {
        password[i] = password[i] + shift;
    }
}

void decryptPassword(char *password) {
    int shift = 3;  // Simple shift for Caesar Cipher
    for (int i = 0; password[i] != '\0'; i++) {
        password[i] = password[i] - shift;
    }
}

// Caesar Cipher for Message Encryption/Decryption
void encryptMessage(char *message, int shift) {
    for (int i = 0; message[i] != '\0'; i++) {
        if (isalpha(message[i])) {
            char offset = isupper(message[i]) ? 'A' : 'a';
            message[i] = (message[i] - offset + shift) % 26 + offset;
        }
    }
}

void decryptMessage(char *message, int shift) {
    for (int i = 0; message[i] != '\0'; i++) {
        if (isalpha(message[i])) {
            char offset = isupper(message[i]) ? 'A' : 'a';
            message[i] = (message[i] - offset - shift + 26) % 26 + offset;
        }
    }
}

// Password Manager Functions
void generatePassword(char *password, int length, int useSymbols, int useLowercase, int useUppercase) {
    char characters[100] = "";

    if (useSymbols) {
        strcat(characters, "!@#$%^&*()");
    }
    if (useLowercase) {
        strcat(characters, "abcdefghijklmnopqrstuvwxyz");
    }
    if (useUppercase) {
        strcat(characters, "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    }
    strcat(characters, "0123456789");

    int charArraySize = strlen(characters);

    if (charArraySize == 0) {
        printf("You must select at least one character type.\n");
        password[0] = '\0';
        return;
    }

    srand(time(0));

    for (int i = 0; i < length; i++) {
        password[i] = characters[rand() % charArraySize];
    }
    password[length] = '\0'; // Null-terminate the string

    // Tips for stronger passwords
    if (!useSymbols || !useLowercase || !useUppercase) {
        printf("Tips: Using symbols, lowercase, and uppercase letters makes your password stronger.\n");
    }
}

int passwordMeetsCriteria(const char *password) {
    int hasLowercase = 0, hasUppercase = 0, hasSymbol = 0;

    for (int i = 0; password[i] != '\0'; i++) {
        if (islower(password[i])) {
            hasLowercase = 1;
        } else if (isupper(password[i])) {
            hasUppercase = 1;
        } else if (strchr("!@#$%^&*()", password[i]) != NULL) {
            hasSymbol = 1;
        }
    }

    return hasLowercase && hasUppercase && hasSymbol;
}

void storePassword(const char *username, const char *password) {
    FILE *file = fopen("passwords.txt", "a");
    if (file == NULL) {
        printf("Error opening file!\n");
        return;
    }
    char encryptedPassword[50];
    strcpy(encryptedPassword, password);
    encryptPassword(encryptedPassword);
    fprintf(file, "%s: %s\n", username, encryptedPassword);
    fclose(file);
}

void readPasswords() {
    FILE *file = fopen("passwords.txt", "r");
    if (file == NULL) {
        printf("No stored passwords found.\n");
        return;
    }
    char line[100];
    while (fgets(line, sizeof(line), file)) {
        char *username = strtok(line, ":");
        char *encryptedPassword = strtok(NULL, "\n");
        char decryptedPassword[50];
        strcpy(decryptedPassword, encryptedPassword);
        decryptPassword(decryptedPassword);
        printf("Username: %s, Password: %s\n", username, decryptedPassword);
    }
    fclose(file);
}

// Network Credential Auditor Function
void auditCredentials(const char *ip, const char *username, const char *password) {
    printf("Auditing %s with username: %s and password: %s\n", ip, username, password);

    // Validate the password meets criteria
    if (!passwordMeetsCriteria(password)) {
        printf("Credentials are not secure. Password does not meet criteria.\n");
        return;
    }

    // Simulated check (In real application, you would connect to a server or database)
    if (strcmp(username, "admin") == 0 && strcmp(password, "password123") == 0) {
        printf("Weak credentials detected for IP: %s\n", ip);
    } else {
        printf("Credentials appear to be secure for IP: %s\n", ip);
    }
    printf("Audit complete. (Simulation)\n");
}

int main() {
    int choice;
    char username[50];
    char password[50];
    int length;
    char ip[16];
    int useSymbols, useLowercase, useUppercase;
    char message[256];
    int shift;
    int subChoice;

    printCentered("Welcome to The");
    printCentered("'Cipher Password Manager and Network Auditor'");

    do {
        printf("\n");
        printf("1. Generate Password\n");
        printf("2. Store Password\n");
        printf("3. Read Stored Passwords\n");
        printf("4. Encrypt/Decrypt a Message\n");
        printf("5. Audit Network Credentials\n");
        printf("6. Exit\n");
        printf("Enter your choice: ");
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Please enter a number from 1 to 6.\n");
            scanf("%*[^\n]"); // Clear input buffer
            continue;
        }

        switch (choice) {
            case 1:
                printf("Enter desired length for the password: ");
                if (scanf("%d", &length) != 1) {
                    printf("Invalid input. Please enter a valid number.\n");
                    scanf("%*[^\n]"); // Clear input buffer
                    break;
                }

                if (length < 8) {
                    printf("Password length should be at least 8 characters.\n");
                } else {
                    printf("Include symbols? (1 for Yes, 0 for No): ");
                    if (scanf("%d", &useSymbols) != 1) {
                        printf("Invalid input. Please enter 1 or 0.\n");
                        scanf("%*[^\n]"); // Clear input buffer
                        break;
                    }
                    printf("Include lowercase letters? (1 for Yes, 0 for No): ");
                    if (scanf("%d", &useLowercase) != 1) {
                        printf("Invalid input. Please enter 1 or 0.\n");
                        scanf("%*[^\n]"); // Clear input buffer
                        break;
                    }
                    printf("Include uppercase letters? (1 for Yes, 0 for No): ");
                    if (scanf("%d", &useUppercase) != 1) {
                        printf("Invalid input. Please enter 1 or 0.\n");
                        scanf("%*[^\n]"); // Clear input buffer
                        break;
                    }

                    generatePassword(password, length, useSymbols, useLowercase, useUppercase);
                    printf("Generated Password: %s\n", password);
                }
                break;
            case 2:
                printf("Enter username: ");
                scanf("%s", username);
                printf("Enter password to store: ");
                disableEcho();
                scanf("%s", password);
                enableEcho();
                printf("\n");

                storePassword(username, password);
                printf("Password stored successfully.\n");
                break;
            case 3:
                printf("Stored Passwords:\n");
                readPasswords();
                break;
            case 4:
                printf("1. Encrypt a Message\n");
                printf("2. Decrypt a Message\n");
                printf("Enter your choice: ");
                if (scanf("%d", &subChoice) != 1) {
                    printf("Invalid input. Please enter 1 or 2.\n");
                    scanf("%*[^\n]"); // Clear input buffer
                    break;
                }

                switch (subChoice) {
                    case 1:
                        printf("Enter message to encrypt: ");
                        getchar(); // Clear newline character from input buffer
                        fgets(message, sizeof(message), stdin);
                        message[strcspn(message, "\n")] = '\0'; // Remove newline character
                        printf("Enter shift value: ");
                        if (scanf("%d", &shift) != 1) {
                            printf("Invalid input. Please enter a valid number.\n");
                            scanf("%*[^\n]"); // Clear input buffer
                            break;
                        }
                        encryptMessage(message, shift);
                        printf("Encrypted Message: %s\n", message);
                        break;
                    case 2:
                        printf("Enter message to decrypt: ");
                        getchar(); // Clear newline character from input buffer
                        fgets(message, sizeof(message), stdin);
                        message[strcspn(message, "\n")] = '\0'; // Remove newline character
                        printf("Enter shift value: ");
                        if (scanf("%d", &shift) != 1) {
                            printf("Invalid input. Please enter a valid number.\n");
                            scanf("%*[^\n]"); // Clear input buffer
                            break;
                        }
                        decryptMessage(message, shift);
                        printf("Decrypted Message: %s\n", message);
                        break;
                    default:
                        printf("Invalid choice. Please enter 1 or 2.\n");
                        break;
                }
                break;
            case 5:
                printf("Enter IP address to audit: ");
                scanf("%15s", ip);
                printf("Enter username: ");
                scanf("%s", username);
                printf("Enter password: ");
                disableEcho();
                scanf("%s", password);
                enableEcho();
                printf("\n");

                auditCredentials(ip, username, password);
                break;
            case 6:
                printCentered("Thank you for using the CipherPass Manager Pro!");
                printCentered("I hope you had a great experience.");
                printCentered("Exiting the program...\n");
                break;
            default:
                printf("Invalid choice. Please enter a number from 1 to 6.\n");
                break;
        }
    } while (choice != 6);

    return 0;
}
