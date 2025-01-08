#include <stdio.h>

typedef struct {
    char username[50];
    char password[50];
} Credentials;

int main(){

	Credentials newCred;
    printf("Enter New Username: ");
    scanf("%s", newCred.username);
    printf("Enter New Password: ");
    scanf("%s", newCred.password);

    FILE *file = fopen("credentials.txt", "w");
    if (file == NULL) {
        printf("Error: Could not open the file for writing.\n");
        return 0;;
    }

    fwrite(&newCred, sizeof(Credentials), 1, file);
    fclose(file);

    printf("Credentials updated successfully!\n");
    
	return 0;
}
