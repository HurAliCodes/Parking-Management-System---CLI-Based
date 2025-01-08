#include <conio.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <time.h>
#include <stdlib.h>

#define MAX_ROWS 10
#define MAX_COLS 10
#define MAX_CAPACITY 5
#define WAITLIST_CAPACITY 5

typedef struct {
    char username[50];
    char password[50];
} Credentials;

int parkingArea[MAX_ROWS][MAX_COLS];
//Default Rows
int rows = 5, cols = 5;
int parkedVehicles = 0;
int waitinglistcount=0;

typedef struct {
	int address;
	char type[10];
	char plateNumber[10];
	int charges;
	int duration;
	time_t t;
	time_t u;
} vehicle;

vehicle parked[MAX_ROWS * MAX_COLS] = {0};
vehicle waitingList[WAITLIST_CAPACITY] = {0};
vehicle history[1000];
int historyCount = 0;

void animate(char str[],int length);
void setColor();
void fillArea();
void selection();
void adminLogin();
void adminMenu();
void createParkingArea();
void displayParkingArea(int n);
void displayParkedVehiclesList();
void displayBanner();
void searchVehicle();
void viewHistory();
int extra_services(int total);
void userMenu();
void parkVehicle();
void unparkVehicle();
void header();
void exit();
void loadVehicles();
void saveVehicles();
void saveVehicles();
void notify(const char *message);
void shiftVehicleFromWaitlist();
void generateAdminReport();

void setColor(int textColor, int backgroundColor) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	int colorCode = (backgroundColor << 4) | textColor;
	SetConsoleTextAttribute(hConsole, colorCode);
}


void loadVehicles() {
    FILE *fptr = fopen("projectParked.txt", "r");
    if (fptr == NULL) {
        fillArea();
        return;
    }
    
    int index = 0;
    vehicle tempVehicle;
    int maxAddress = 0;

    while (fscanf(fptr, "%d %s %s %d %d %ld", &tempVehicle.address, tempVehicle.type, tempVehicle.plateNumber, 
                  &tempVehicle.charges, &tempVehicle.duration, &tempVehicle.t) == 6) {
        if (tempVehicle.address > maxAddress) {
            maxAddress = tempVehicle.address;
        }
        parked[index++] = tempVehicle;
        parkedVehicles++;
    }
	if(maxAddress>5){
    	rows = (maxAddress / MAX_COLS) + 1; 
    	cols = (maxAddress % MAX_COLS) + 1; 
	}

    if (rows > MAX_ROWS) rows = MAX_ROWS;
    if (cols > MAX_COLS) cols = MAX_COLS;

    fillArea();

    rewind(fptr);
    while (fscanf(fptr, "%d %s %s %d %d %ld", &tempVehicle.address, tempVehicle.type, tempVehicle.plateNumber, 
                  &tempVehicle.charges, &tempVehicle.duration, &tempVehicle.t) == 6) {
        int row = tempVehicle.address / cols;
        int col = tempVehicle.address % cols;

        if (row < rows && col < cols) { 
            parkingArea[row][col] = -1; 
        } 
    }
    
    fclose(fptr);
}



int main() {
	loadVehicles();

	displayBanner();

	selection();

	return 0;
}

void shiftVehicleFromWaitlist() {
    if (waitinglistcount > 0) {
        vehicle vehicleToPark = waitingList[0];  // Get the first vehicle in the waiting list

        // Find the first available parking space
        int found = 0;
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                if (parkingArea[i][j] >= 0) {  // Available space
                    parkingArea[i][j] = -1;  // Mark the space as occupied
                    vehicleToPark.address = i * cols + j;  // Assign parking address
                    vehicleToPark.charges = 0;
                    vehicleToPark.t = time(NULL);  // Set parking time
                    
                    // Move vehicle to parking
                    parked[parkedVehicles++] = vehicleToPark;

                    // Shift all other vehicles in the waiting list up by one
                    for (int k = 1; k < waitinglistcount; k++) {
                        waitingList[k - 1] = waitingList[k];
                    }

                    waitinglistcount--;  // Decrease the waiting list count

                    notify(" A vehicle from the waiting list has been parked.");
                    setColor(2, 0);
                    printf("\n Vehicle with plate %s is now parked at address %d.\n", vehicleToPark.plateNumber, vehicleToPark.address);
                    setColor(7, 0);

                    found = 1;
                    break;
                }
            }
            if (found) break;
        }

        if (!found) {
            // No available parking space for the vehicle in the waiting list
            notify("\n No available parking space to shift from the waiting list.");
        }
    } else {
        // No vehicles in the waiting list
        notify("\n No vehicles are currently in the waiting list.");
    }
}



void notify(const char *message) {
	setColor(4,0);
	printf("\n\a [Notification]: %s\n", message);
	setColor(7,0);
	Sleep(1000);
}



void saveVehicles() {
	FILE * fptr;
	fptr = fopen("projectParked.txt", "w");
	for(int i=0; i<parkedVehicles; i++) {
		fprintf(fptr, "%d %s %s %d %d %ld\n", parked[i].address, parked[i].type, parked[i].plateNumber, parked[i].charges, parked[i].duration, parked[i].t);
	}
	fclose(fptr);
}



void animate(char str[],int length) {
	for(int i=0; i<length; i++) {
		printf("%c",str[i]);
		Sleep(40);
	}
}

void fillArea() {
	int val = 0;
	for(int i=0; i<rows; i++) {
		for(int j=0; j<cols; j++) {
			parkingArea[i][j] = val++;
		}
	}
}

void createParkingArea() {

	system("cls");
	header();

	printf("\n\n Create Your Own Parking Space: \n");
	printf("===============================\n\n");

row:

	printf(" Enter number of rows (1-10): ");
	scanf("%d", &rows);

	if(rows>10) {
		setColor(12,0);
		printf("Please Enter the rows between 1 to 10! \n");
		setColor(7,0);
		goto row;
	}

col:

	printf("\n Enter number of columns (1-10): ");
	scanf("%d", &cols);

	if(cols>10) {
		setColor(12,0);
		printf("Please Enter the columns between 1 to 10! \n");
		setColor(7,0);
		goto col;
	}

	fillArea();
	parkedVehicles=0;
	for(int i=0;i<MAX_ROWS*MAX_COLS;i++){
		parked[i] = {0};
	}
	setColor(2,0);
	printf("\n Parking Area Created Succesfully ! \n\n");
	setColor(7,0);
	displayParkingArea(1);
}

void displayParkingArea(int n) {
	int totalWidth = 122;
	int cellWidth = 10;
	int parkingAreaWidth = cols * (cellWidth + 2);
	int leftPadding = (totalWidth - parkingAreaWidth) / 2;
	printf("\n\n");

	printf("%*s", leftPadding,"");
	printf("|");
	setColor(0, 15);
	int textPaddingLeft = (parkingAreaWidth - strlen("Parking Area")) / 2;
	int textPaddingRight = parkingAreaWidth - strlen("Parking Area") - textPaddingLeft;

	printf("%*s", textPaddingLeft-2, "");
	printf("Parking Area");
	printf("%*s", textPaddingRight-2, "");
	setColor(7, 0);
	printf("|\n");

	printf("%*s", leftPadding, "");
	for (int j = 0; j < cols; j++) {
		printf("==========");
		if (j != cols - 1)
			printf("==");
	}
	printf("\n");

	for (int i = 0; i < rows; i++) {

		printf("%*s", leftPadding, "");
		for (int j = 0; j < cols; j++) {
			printf("|");
			if (parkingArea[i][j] < 0)
				setColor(15, 4);
			else
				setColor(15, 2);
			printf("        ");
			setColor(7, 0);
			printf("|  ");
		}
		printf("\n");

		printf("%*s", leftPadding, "");
		for (int j = 0; j < cols; j++) {
			printf("|");
			if (parkingArea[i][j] < 0) {
				setColor(15, 4);
				printf("   VH   ");
			} else {
				setColor(15, 2);
				printf("   %02d   ", parkingArea[i][j]);
			}
			setColor(7, 0);
			printf("|  ");
		}
		printf("\n");

		printf("%*s", leftPadding, "");
		for (int j = 0; j < cols; j++) {
			printf("|");
			if (parkingArea[i][j] < 0)
				setColor(15, 4);
			else
				setColor(15, 2);
			printf("        ");
			setColor(7, 0);
			printf("|  ");
		}
		printf("\n");

		if (i != rows - 1) {
			printf("\n");
		}
	}
	setColor(15, 0);

	printf("%*s", leftPadding, "");
	for (int j = 0; j < cols; j++) {
		printf("==========");
		if (j != cols - 1)
			printf("==");
	}

	if(n!=0) {
		printf("\n\n\n\n Press b to go back.");

		char choice = '0';
		while (!(choice == 'b' || choice == 'B')) {
			choice = getch();
		}
	}
}

void displayBanner() {
	system("color F0");
	char wel[50] = "W E L C O M E   TO";
	char par[50] = "P A R K I N G";
	char man[50] = "M A N A G E M E N T";	
	char sys[50] = "S Y S T E M";
	printf("*                                                                                                                     *\n");
	printf("*                                                                                                                     *\n");
	printf("*                                                                                                                     *\n");
	printf("*                                                                                                                     *\n");
	printf("*                                                                                                                     *\n");
	printf("*                                                                                                                     *\n");
	printf("*                                                                                                                     *\n");
	printf("*                                                                                                                     *\n");
	printf("*                                                                                                                     *\n");
	printf("*                                                                                                                     *\n");
	printf("*                                                                                                                     *\n");
	printf("*                                                 ");
	animate(wel,strlen(wel));
	printf("                                                  *\n");
	printf("*                                                                                                                     *\n");
	printf("*                                                    ");
	animate(par,strlen(par));
	printf("                                                    *\n");
	printf("*                                                                                                                     *\n");
	printf("*                                                 ");
	animate(man,strlen(man));
	printf("                                                 *\n");
	printf("*                                                                                                                     *\n");
	printf("*                                                     ");
	animate(sys,strlen(sys));
	printf("                                                     *\n");
	printf("*                                                                                                                     *\n");
	printf("*                                                                                                                     *\n");
	printf("*                                                                                                                     *\n");
	printf("*                                                                                                                     *\n");
	printf("*                                                                                                                     *\n");
	printf("*                                                                                                                     *\n");
	printf("*                                                                                                                     *\n");
	printf("*                                                                                                                     *\n");
	printf("*                                                                                                                     *\n");
	printf("*                                                                                                                     *\n");
	printf("*                                              Press Any Key To Continue                                              *\n");
	printf("*                                                                                                                     *");

	setColor(7,0);
	getch();
	MessageBeep(MB_OK);

}

void header() {
	setColor(4,0);
	printf("=======================================================================================================================\n||");
	setColor(15,4);
	printf("                                              Parking Management System                                            ");
	setColor(4,0);
	printf("||\n=======================================================================================================================\n");
	setColor(7,0);
}

void selection() {
	char choice;
selectAgain:
	system("cls");
	header();
	printf("\n Select Account to Login: \n");
	printf(" ========================== \n\n");
	printf(" [1] Admin \n");
	printf(" [2] User \n");
	printf(" [3] Exit \n");

	printf("\n Enter the choice: ");
	scanf(" %c",&choice);

	switch(choice) {
		case '1':
			MessageBeep(MB_OK);
			adminLogin();
			break;
		case '2':
			MessageBeep(MB_OK);
			userMenu();
			break;
		case '3':
			MessageBeep(MB_OK);
			exit();
			break;
		default:
			setColor(4,0);
			printf("\n\a Invalid Option! Enter Again");
			setColor(7,0);
			Sleep(500);
			goto selectAgain;
	}
}

void displayParkedVehiclesList() {
	system("cls");

	setColor(4,0);
	printf("-----------------------------------------------------------------------------------------------------------------------\n|");
	setColor(15,4);   ;
	printf("                                                    Vehicle List                                                      ");
	setColor(4,0);
	printf("|\n-----------------------------------------------------------------------------------------------------------------------\n");
	setColor(7,0);

	printf("========================================================================================================================\n");
	for(int i=0; i<parkedVehicles; i++) {
		if(i==0) {
			printf("| SR NO.| Type\t| Plate Number\t| Parking Address | Total Charges\t| Time of Parking\t| Date of Parking\t|\n");
			printf("========================================================================================================================\n");
		}
		struct tm time = *localtime(&parked[i].t);
		int hours;
		char format[3];
		if(time.tm_hour > 12) {
			hours = time.tm_hour - 12;
			strcpy(format,"pm");
		} else {
			hours = time.tm_hour;
			strcpy(format,"am");
		}

		printf("| %d.\t| %s\t| %08s\t| %02d\t\t  | %d Rs.\t\t| %02d:%02d %s\t\t| %d-%02d-%02d\t\t|\n", i+1, parked[i].type, parked[i].plateNumber, parked[i].address, parked[i].charges, hours, time.tm_min, format, time.tm_mday, time.tm_mon+1, time.tm_year+1900);
	}
	printf("------------------------------------------------------------------------------------------------------------------------\n");
	printf("\n\n\n Press b to go back");
	char c = '0';
	while(c!='b')
		c =getch();

	adminMenu();

}

void adminLogin() {
	system("cls");
	char username[100];
	char password[100];
	
	setColor(4,0);
	printf("-----------------------------------------------------------------------------------------------------------------------\n|");
	setColor(15,4);
	printf("                                                     Admin Login                                                     ");
	setColor(4,0);
	printf("|\n-----------------------------------------------------------------------------------------------------------------------\n");
	setColor(7,0);
	printf("\n\n");
	int count = 3;
	
	Credentials cred;
    FILE *file = fopen("credentials.txt", "rb");
    if (file == NULL) {
		strcpy(cred.username, "owner");
		strcpy(cred.password, "1234");
    }
    else{
    	fread(&cred, sizeof(Credentials), 1, file);
	}
    fclose(file);

	
	while (1) {
		printf(" > Enter Username: ");
		scanf("%s", username);
		fflush(stdin);
		printf("\n > Enter Password: ");
		scanf("%s", password);

		fflush(stdin);

		if (strcmp( cred.username, username) == 0 && strcmp(cred.password , password)==0) {
			break;
		} 
		else {
			count--;
			setColor(4, 0);
			printf("\n\a Incorrect Username or PIN! %d tries left.\n\n", count);
			setColor(7, 0);
			Sleep(1000);

			if (count <= 0) {
				setColor(4, 0);
				printf("\n\a Too Many Wrong Attempts! Going back to main menu...\n\n");
				setColor(7, 0);
				Sleep(1000);
				system("cls");
				selection();
				break;
			}
		}
	}

	setColor(2,0);
	MessageBeep(MB_OK);
	printf("\n Login Successful!");
	setColor(7,0);
	Sleep(800);

	adminMenu();
}

void adminMenu() {

	system("cls");
	char choice;
	setColor(4,0);
	printf("-----------------------------------------------------------------------------------------------------------------------\n|");
	setColor(15,4);
	printf("                                                     Admin Menu                                                      ");
	setColor(4,0);
	printf("|\n-----------------------------------------------------------------------------------------------------------------------\n");
	setColor(7,0);

	printf("\n\n");

	printf(" Select Option: \n");
	printf(" ============== \n\n");
	printf(" [1] Create Parking Area \n");
	printf(" [2] Empty Parking Area \n");
	printf(" [3] View Parking Area \n");
	printf(" [4] Parked Vehicle List\n");
	printf(" [5] Get Vehicle Details \n");
	printf(" [6] Generate Report\n");
	printf(" [7] View History \n");
	printf(" [8] Quit to Main Menu \n\n");
	printf(" ============== \n");

	printf(" \n Enter Your Choice > ");
	scanf(" %c",&choice);

	switch(choice) {
		case '1':
			system("cls");
			createParkingArea();
			adminMenu();
			break;
		case '2':
			char clr;
			printf("\n Are You Sure You Want to Clear Parking Area? (Y / N): ");
			scanf(" %c",&clr);
			if(tolower(clr) == 'y') {
				fillArea();
				for(int i=0; i < parkedVehicles; i++) {
					parked[i] = {0};
				}
				parkedVehicles = 0;
				setColor(2,0);
				printf("\n Parking Area Emptied Seccessfully !");
				setColor(7,0);
				Sleep(1500);
			}
			adminMenu();
			break;
		case '3':
			system("cls");
			displayParkingArea(1);
			adminMenu();
			break;
		case '4':
			displayParkedVehiclesList();
			break;
		case '5':
			searchVehicle();
			break;
		case '6':
			generateAdminReport();
			break;
		case '7':
			viewHistory();
			break;
		case '8':
			MessageBeep(MB_OK);
			selection();
			break;
		default:
			setColor(4,0);
			printf("\n\a Invalid Option");
			setColor(7,0);
			Sleep(800);
			adminMenu();
	}
}

void userMenu() {
	system("cls");
	char choice;
	setColor(4,0);
	printf("-----------------------------------------------------------------------------------------------------------------------\n|");
	setColor(15,4);
	printf("                                                      User Menu                                                      ");
	setColor(4,0);
	printf("|\n-----------------------------------------------------------------------------------------------------------------------\n");
	setColor(7,0);
	printf("\n");

	printf(" Select Option: \n");
	printf(" ============== \n\n");
	printf(" [1] Park Vehicle \n");
	printf(" [2] UnPark Vehicle \n");
	printf(" [3] View Parking Area \n");
	printf(" [4] Quit to Main Menu \n\n");
	printf(" ============== \n");

	printf(" \n Enter Your Choice > ");
	scanf(" %c",&choice);

	switch(choice) {
		case '1':
			MessageBeep(MB_OK);
			parkVehicle();
			break;
		case '2':
			MessageBeep(MB_OK);
			unparkVehicle();
			break;
		case '3':
			system("cls");
			displayParkingArea(1);
			userMenu();
			break;
		case '4':
			MessageBeep(MB_OK);
			selection();
			break;
	}

}

void parkVehicle() {
	system("cls");
	header();
	int total;

	if (parkedVehicles >= rows * cols) {
		if (waitinglistcount < WAITLIST_CAPACITY) {
			vehicle newVehicle;
			int vehType;

			printf("\n Enter vehicle type \n\n");
			printf(" [1] Car\n");
			printf(" [2] Bike\n");
			printf(" [3] Bus\n");
			printf(" [4] Truck\n\n");
			printf(" Enter Your Choice Number > ");
			scanf("%d",&vehType);
	
			switch (vehType) {
				case 1:
					strcpy(newVehicle.type, "Car");
					break;
				case 2:
					strcpy(newVehicle.type, "Bike");
					break;
				case 3:
					strcpy(newVehicle.type, "Bus");
					break;
				case 4:
					strcpy(newVehicle.type, "Truck");
					break;
				default:
					setColor(4, 0);
					printf("\n\a Invalid Choice! Enter Again.");
					setColor(7, 0);
					Sleep(1500);
					parkVehicle();
					return;
			}

			printf("\n Enter plate number: ");
			scanf("%s", newVehicle.plateNumber);

			printf("\n Enter Time in Hours for which you want to park Vehicle: ");
			scanf("%d", &newVehicle.duration);

			waitingList[waitinglistcount++] = newVehicle;
			setColor(4,0);
			notify("\n Parking area is full. You have been added to the waiting list!");
			setColor(7,0);
			Sleep(1000);
			userMenu();
			return;
		} else {
			notify("\n Parking area and waiting list are both full.");
			userMenu();
			return;
		}
	}


	vehicle newVehicle;
	int vehType;

	printf("\n Enter vehicle type \n\n");
	printf(" [1] Car\n");
	printf(" [2] Bike\n");
	printf(" [3] Bus\n");
	printf(" [4] Truck\n\n");
	printf(" Enter Your Choice Number > ");
	fflush(stdin);
	scanf("%d",&vehType);

	switch(vehType) {
		case 1:
			strcpy(newVehicle.type,"Car");
			break;
		case 2:
			strcpy(newVehicle.type,"Bike");
			break;
		case 3:
			strcpy(newVehicle.type,"Bus");
			break;
		case 4:
			strcpy(newVehicle.type,"Truck");
			break;
		default:
			setColor(4,0);
			printf("\n\a Invalid Choice ! Enter Again.");
			setColor(7,0);
			Sleep(1500);
			parkVehicle();
	}

	printf("\n Enter plate number: ");
	scanf("%s", newVehicle.plateNumber);

	printf("\n Enter Time in Hours for which you want to park Vehicle: ");
	scanf("%d",&newVehicle.duration);

	char choice;
	fflush(stdin);
	printf("\n Do you want to park manually (M) or automatically (A)? ");
	scanf(" %c", &choice);
	choice = tolower(choice);

	if (choice == 'm' ) {
		int address;
		displayParkingArea(0);
		printf("\n\n Enter the parking address (0 to %d): ", rows * cols - 1);
		scanf("%d", &address);

		int row = address / cols;
		int col = address % cols;

		if (address < 0 || address >= rows * cols || parkingArea[row][col] == -1) {
			setColor(4, 0);
			printf("\n\a Invalid or already occupied parking spot. Please try again.\n");
			Sleep(1000);
			parkVehicle();
			setColor(7, 0);
		} else {
			parkingArea[row][col] = -1;
			newVehicle.address = address;
			newVehicle.charges = 0;
			newVehicle.t = time(NULL);

			parked[parkedVehicles++] = newVehicle;

			setColor(2, 0);
			printf("\n Vehicle parked successfully at address: %d\n\n", address);
			setColor(7, 0);
		}
	} else if (choice == 'a') {
		int found = 0;
		int i,j;
		for ( i = 0; i < rows; i++) {
			for (j = 0; j < cols; j++) {
				if (parkingArea[i][j] >= 0) {
					newVehicle.address = parkingArea[i][j];
					parkingArea[i][j] = -1;
					newVehicle.charges = 0;
					newVehicle.t = time(NULL);
					parked[parkedVehicles++] = newVehicle;
					found = 1;
					break;
				}
			}
			if (found)
				break;
		}

		if (found) {
			setColor(2, 0);
			printf("\n Vehicle parked successfully at address: %d\n\n", newVehicle.address);
			setColor(7, 0);
		} else {
			setColor(4, 0);
			printf("\n\a Error: Could not find a parking space.\n");
			setColor(7, 0);
		}
	} else {
		setColor(4, 0);
		printf("\n\a Invalid choice. Returning to menu...\n");
		setColor(7, 0);
		Sleep(800);
		userMenu();
		return;
	}

	if (strcmp(newVehicle.type, "Car") == 0) {
		printf(" Charges for Car parking are RS 250 per hour.\n");
		total = 250;
	} else if (strcmp(newVehicle.type, "Bike") == 0) {
		printf(" Charges for Bike parking are Rs 100 per hour.\n");
		total = 100;
	} else if (strcmp(newVehicle.type, "Bus") == 0) {
		printf(" Charges for Bus parking are RS 500 per hour.\n");
		total = 500;
	} else if(strcmp(newVehicle.type, "Truck") == 0) {
		printf(" Charges for Truck parking are Rs 600 per hour.\n");
		total = 600;
	}

	total = extra_services(total);

	parked[parkedVehicles-1].charges += total;
    saveVehicles();
	printf("\n Press b to Continue..");
	char c;
	while(tolower(c)!='b') {
		c = getch();
	}



	userMenu();
}


int calculateCharges(vehicle unparkedVehicle) {
	double durationInSeconds = difftime(unparkedVehicle.u, unparkedVehicle.t);
	int hours = (int)(durationInSeconds / 3600);
	if (hours < 1) hours = 1; // Minimum charge for 1 hour

	if (strcasecmp(unparkedVehicle.type, "Car") == 0) {
		return hours * 250;
	} else if (strcasecmp(unparkedVehicle.type, "Bike") == 0) {
		return hours * 100;
	} else if (strcasecmp(unparkedVehicle.type, "Bus") == 0) {
		return hours * 500;
	} else if (strcasecmp(unparkedVehicle.type, "Truck") == 0) {
		return hours * 600;
	}

	return 0; // Default case
}

void calculateDuration(double durationInSeconds, int *hours, int *minutes, int *seconds) {
	*hours = (int)(durationInSeconds / 3600);
	*minutes = ((int)durationInSeconds % 3600) / 60;
	*seconds = (int)durationInSeconds % 60;
}


void unparkVehicle() {
	system("cls");
	header();

	if (parkedVehicles == 0) {
		setColor(4, 0);
		printf("\n\a No vehicles are currently parked.\n");
		setColor(7, 0);
		Sleep(1500);
		userMenu();
		return;
	}

	vehicle unparkedVehicle;
	displayParkingArea(0);
	int address;
	printf("\n\n Enter the parking address (0 to %d) to unpark: ", rows * cols - 1);
	scanf("%d", &address);

	int row = address / cols;
	int col = address % cols;

	if (address < 0 || address >= rows * cols || parkingArea[row][col] != -1) {
		setColor(4, 0);
		printf("\n\a Invalid parking address or no vehicle is parked at this spot.\n");
		Sleep(1000);
		unparkVehicle();
		setColor(7, 0);
	} else {

		for (int i = 0; i < parkedVehicles; i++) {
			if (parked[i].address == address) {
				unparkedVehicle = parked[i];

				for (int j = i; j < parkedVehicles - 1; j++) {
					parked[j] = parked[j + 1];
				}

				parkedVehicles--;
				parkingArea[row][col] = address;

				break;
			}
		}
	}



	unparkedVehicle.u = time(NULL);
	shiftVehicleFromWaitlist();
	int charges = calculateCharges(unparkedVehicle);
	unparkedVehicle.charges += charges;
	double durationInSeconds = difftime(unparkedVehicle.u, unparkedVehicle.t);
	int hours, minutes, seconds;
	calculateDuration(durationInSeconds, &hours, &minutes, &seconds);

	setColor(2, 0);
	printf("\n Vehicle unparked successfully\n");
	setColor(7, 0);

	printf("\n ********** Unpark Vehicle Receipt **********\n\n");
	printf(" Vehicle Type: %s\n", unparkedVehicle.type);
	printf(" Plate Number: %s\n", unparkedVehicle.plateNumber);
	printf(" Parking Address: %d\n", unparkedVehicle.address);
	printf(" Time of Parking: %s", ctime(&unparkedVehicle.t));
	printf(" Duration Parked: %d hours, %d minutes, and %d seconds\n", hours, minutes, seconds);
	printf(" Time of UnParking: %s",ctime(&unparkedVehicle.u) );
	printf(" Total Charges: %d Rs.\n\n", unparkedVehicle.charges);
	printf(" **********************************************\n");

	history[historyCount++] = unparkedVehicle;

	FILE *fptrUnpark;
	fptrUnpark=fopen("projectUnparked.txt", "w");
	if(fptrUnpark==NULL) {
		printf("\nError in opening the file\n");

	} else {
		fprintf(fptrUnpark,"\n ********** Unpark Vehicle Receipt **********\n\n");
		fprintf(fptrUnpark," Vehicle Type: %s\n", unparkedVehicle.type);
		fprintf(fptrUnpark," Plate Number: %s\n", unparkedVehicle.plateNumber);
		fprintf(fptrUnpark," Parking Address: %d\n", unparkedVehicle.address);
		fprintf(fptrUnpark," Time of Parking: %s", ctime(&unparkedVehicle.t));
		fprintf(fptrUnpark," Time of UnParking: %s",ctime(&unparkedVehicle.u) );
		fprintf(fptrUnpark, " Duration Parked: %d hours, %d minutes, and %d seconds\n", hours, minutes, seconds);
		fprintf(fptrUnpark," Total Charges: %d Rs.\n\n", unparkedVehicle.charges);
		fprintf(fptrUnpark," **********************************************\n");
	}
	fclose(fptrUnpark);
    saveVehicles();
	printf("\n Press b to Continue..");

	char c;
	while(tolower(c)!='b') {
		c = getch();
	}
    
	userMenu();
}

int extra_services(int total) {
	int charge = 0, tyreoption;
	int extraservice, serviceoption;
service:
	printf("\n --------------------------------------------");
	printf("\n Do you want extra services for your vehicle?\n\n");
	printf(" Press [1] for Yes\n");
	printf(" Press [2] for No \n\n");

	printf(" Enter Option > ");

	scanf("%d", &extraservice);

	if (extraservice == 2) {
		printf("\n You selected No extra services.\n");
		return total;
	} else if (extraservice != 1) {
		setColor(4,0);
		printf("\n\a Invalid option.\n");
		setColor(7,0);
		goto service;
	}

	system("cls");

	setColor(4,0);
	printf("-----------------------------------------------------------------------------------------------------------------------\n|");
	setColor(15,4);
	printf("                                                    Extra Services                                                    ");
	setColor(4,0);
	printf("|\n-----------------------------------------------------------------------------------------------------------------------\n");
	setColor(7,0);

	printf("\n Extra services we provide:\n\n"
	       " 1. Wash\n"
	       " 2. Repairing\n"
	       " 3. Battery charging\n"
	       " 4. Fuel refueling\n"
	       " 5. Tyre changing\n\n"
	       " Choose an option: ");
	scanf("%d", &serviceoption);

	int car_wash[] = {500, 200, 1000, 800};
	int repairing[] = {1000, 500, 1500, 1200};
	int battery_charging[] = {300, 200, 1000,3000};
	int refueling[] = {7000, 2000, 50000, 40000};
	int tyre_changing[] = {12000, 2000, 25000, 18000};

	int *service_charges[] = {
		car_wash, repairing, battery_charging, refueling, tyre_changing
	};

	int total_index = (total == 250) ? 0 : (total == 100) ? 1 : (total == 500) ? 2 :3;

	if (serviceoption == 5) {
		printf("\n How many tyres do you want to change: ");
		scanf("%d", &tyreoption);
		charge = tyreoption * tyre_changing[total_index];
	} else if (serviceoption >= 1 && serviceoption <= 4) {
		charge = service_charges[serviceoption - 1][total_index];
	} else {
		setColor(4,0);
		printf("\n\a Invalid service option.\n");
		setColor(7,0);
		goto service;
	}
	printf("\n Your Extra Services have been noted ! \n");
	printf("\n Total Extra Charges For Extra Services are: %d\n",charge);
	return total + charge;
}

void searchVehicle() {
	system("cls");
	setColor(4,0);
	printf("-----------------------------------------------------------------------------------------------------------------------\n|");
	setColor(15,4);
	printf("                                                  Search Vehicle                                                      ");
	setColor(4,0);
	printf("|\n-----------------------------------------------------------------------------------------------------------------------\n");
	setColor(7,0);

	char plate[50];
	printf("\n Enter Plate Number to Search: ");
	scanf("%s", plate);

	int found = 0;
	for (int i = 0; i < parkedVehicles; i++) {
		if (strcmp(parked[i].plateNumber, plate) == 0) {
			found = 1;
			struct tm time = *localtime(&parked[i].t);

			printf("\n ********** Vehicle Details **********\n");
			printf(" Vehicle Type: %s\n", parked[i].type);
			printf(" Plate Number: %s\n", parked[i].plateNumber);
			printf(" Parking Address: %d\n", parked[i].address);
			printf(" Time of Parking: %02d:%02d\n", time.tm_hour, time.tm_min);
			printf(" Date of Parking: %02d-%02d-%04d\n", time.tm_mday, time.tm_mon + 1, time.tm_year + 1900);
			printf(" Total Charges: %d Rs.\n", parked[i].charges);
			printf(" *************************************\n");
			break;
		}
	}

	if (!found) {
		setColor(4, 0);
		printf("\n\a No vehicle found with the plate number: %s\n", plate);
		setColor(7, 0);
	}

	printf("\n Press 'b' to go back...");
	while (tolower(getch()) != 'b');
	adminMenu();
}

void viewHistory() {
	system("cls");
	setColor(4,0);
	printf("-----------------------------------------------------------------------------------------------------------------------\n|");
	setColor(15,4);   ;
	printf("                                                        History                                                       ");
	setColor(4,0);
	printf("|\n-----------------------------------------------------------------------------------------------------------------------\n");
	setColor(7,0);
	if (historyCount == 0) {
		printf("\n No history available.\n");
		printf("\n Press 'b' to go back...");
		while (tolower(getch()) != 'b');
		adminMenu();
		return;
	}

	printf("------------------------------------------------------------------------------------------------------------------------\n");
	printf(" SR\t| Plate Number\t| Vehicle Type\t| Charges\t| Time of Parking\t\t| Time of Unparking\t\t|\n");
	printf("------------------------------------------------------------------------------------------------------------------------\n");

	for (int i = 0; i < historyCount; i++) {
		struct tm parkTime = *localtime(&history[i].t);
		struct tm unparkTime = *localtime(&history[i].u);

		printf(" %d\t| %8s\t| %-10s\t| %d\t\t| %02d:%02d %02d-%02d-%04d\t\t| %02d:%02d %02d-%02d-%04d\t\t|\n",
		       i + 1,
		       history[i].plateNumber,
		       history[i].type,
		       history[i].charges,
		       parkTime.tm_hour, parkTime.tm_min, parkTime.tm_mday, parkTime.tm_mon + 1, parkTime.tm_year + 1900,
		       unparkTime.tm_hour, unparkTime.tm_min, unparkTime.tm_mday, unparkTime.tm_mon + 1, unparkTime.tm_year + 1900);
	}
	printf("------------------------------------------------------------------------------------------------------------------------\n");
	printf("\n Press 'b' to go back...");
	while (tolower(getch()) != 'b');
	adminMenu();
}

void generateAdminReport() {
    system("cls");

    // Initialize Metrics
    int totalProfit = 0;
    int totalVehiclesHandled = parkedVehicles + historyCount;
    double utilizationPercentage = ((double)parkedVehicles / (rows * cols)) * 100;

    // Calculate Total Profit from History
    for (int i = 0; i < historyCount; i++) {
        totalProfit += history[i].charges;
    }

    // Add charges from currently parked vehicles
    for (int i = 0; i < parkedVehicles; i++) {
        totalProfit += parked[i].charges;
    }

    // Display Report Header
    setColor(4, 0);
    printf("-----------------------------------------------------------------------------------------------------------------------\n|");
    setColor(15, 4);
    printf("                                              Parking System Report                                                  ");
    setColor(4, 0);
    printf("|\n-----------------------------------------------------------------------------------------------------------------------\n");
    setColor(7, 0);

    // Display Summary
    printf("\nSummary:   \n");
    printf("-------------\n");
    printf("Total Profit Earned: Rs. %d\n", totalProfit);
    printf("Total Vehicles Handled: %d\n", totalVehiclesHandled);
    printf("Current Parking Utilization: %.2f%%\n", utilizationPercentage);
    printf("Vehicles Currently Parked: %d\n", parkedVehicles);
    printf("Vehicles in Waiting List: %d\n", waitinglistcount);
    printf("Historical Records: %d\n\n", historyCount);

    // Display Details of Parked Vehicles
    printf("Details of Parked Vehicles:\n");
    printf("-----------------------------------------------------------------\n");
    printf("| SR | Plate Number | Vehicle Type | Parking Address | Charges  |\n");
    printf("-----------------------------------------------------------------\n");
    for (int i = 0; i < parkedVehicles; i++) {
        printf("| %2d | %12s | %12s | %16d | %7d |\n", i + 1, parked[i].plateNumber, parked[i].type, parked[i].address, parked[i].charges);
    }
    printf("-----------------------------------------------------------------\n");

    // Ask User to Save Report
    char choice;
    printf("\nDo you want to save this report to a file? (Y/N): ");
    scanf(" %c", &choice);

    if (tolower(choice) == 'y') {
        FILE *reportFile = fopen("AdminReport.txt", "w");
        if (reportFile == NULL) {
            setColor(4, 0);
            printf("\nError: Unable to save the file.\n");
            setColor(7, 0);
            return;
        }

        // Write Report to File
        fprintf(reportFile, "Parking System Report\n");
        fprintf(reportFile, "=====================\n\n");
        fprintf(reportFile, "Summary:\n");
        fprintf(reportFile, "---------\n");
        fprintf(reportFile, "Total Profit Earned: Rs. %d\n", totalProfit);
        fprintf(reportFile, "Total Vehicles Handled: %d\n", totalVehiclesHandled);
        fprintf(reportFile, "Current Parking Utilization: %.2f%%\n", utilizationPercentage);
        fprintf(reportFile, "Vehicles Currently Parked: %d\n", parkedVehicles);
        fprintf(reportFile, "Vehicles in Waiting List: %d\n", waitinglistcount);
        fprintf(reportFile, "Historical Records: %d\n\n", historyCount);

        fprintf(reportFile, "Details of Parked Vehicles:\n");
        fprintf(reportFile, "-----------------------------------------------------------------\n");
        fprintf(reportFile, "| SR | Plate Number | Vehicle Type | Parking Address  | Charges |\n");
        fprintf(reportFile, "-----------------------------------------------------------------\nn");
        for (int i = 0; i < parkedVehicles; i++) {
            fprintf(reportFile, "| %2d | %12s | %12s | %16d | %7d |\n", i + 1, parked[i].plateNumber, parked[i].type, parked[i].address, parked[i].charges);
        }
        fprintf(reportFile, "-----------------------------------------------------------------\n");

        fclose(reportFile);
        setColor(2, 0);
        printf("\nReport saved successfully to 'AdminReport.txt'.\n");
        setColor(7, 0);
    } else {
        printf("\nReport was not saved.\n");
    }

    printf("\nPress 'b' to go back to the admin menu...");
    while (tolower(getch()) != 'b');
    adminMenu();
}

void exit() {
	saveVehicles();
	system("cls");
	printf("\n\n\n\n\n\n\n\n");
	setColor(12,0);
	printf("                                      Thank You! Hope You are satisfied with our services                              ");
	printf("\n\n\n\n\n");
	setColor(14,0);
	printf("                                                  Please Come Again later!                                             ");
	printf("\n\n\n\n\n");
	setColor(2,0);
	printf("                                                          Good Bye!                                                    ");
	printf("\n\n\n\n\n\n\n\n\n");
	setColor(7,0);
}