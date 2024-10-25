#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <time.h>

#define MAX_NAME_LENGTH 50
#define MAX_DESTINATION_LENGTH 50
#define MAX_SEATS 50
#define MAX_ROUTES 100 
#define FILENAME "bookings.dat"
#define PARTIAL_BOOKING_FILENAME "partial_booking.dat"
#define PROGRESS_FILENAME "booking.det"
#define MAX_PROMO_CODE_LENGTH 20

struct Booking
{
    int ticketID;
    char name[MAX_NAME_LENGTH];
    char currentLocation[MAX_DESTINATION_LENGTH];
    char destination[MAX_DESTINATION_LENGTH];
    int price;
    char category[MAX_NAME_LENGTH];
    int seats[MAX_SEATS];
    int bookedSeat;
     char promoCode[MAX_PROMO_CODE_LENGTH]; 
    float discount;
};

struct PartialBooking
{
    bool inProgress;
    struct Booking booking;
    int stage; // 0: ticketID, 1: name, 2: destination, 3: price
};

struct Routs {
    char currentLocation[MAX_DESTINATION_LENGTH];
    char destination[MAX_DESTINATION_LENGTH];
    bool seatAvailability[MAX_SEATS]; // Array to track availability for this specific route
};

struct RouteSeatAvailability {
    char currentLocation[MAX_DESTINATION_LENGTH];
    char destination[MAX_DESTINATION_LENGTH];
    bool seatAvailability[MAX_SEATS]; // Array to track availability for this specific route
};

struct Feedback {
    int ticketID;
    char name[MAX_NAME_LENGTH];
    int rating; // Rating out of 5
    char comments[200];
};

const char *indianCities[] = {
    "Mumbai", "Delhi", "Bangalore", "Hyderabad", "Chennai",
    "Kolkata", "Jaipur", "Ahmedabad", "Pune", "Lucknow"};

//int ticketPrices[] = {1500, 1300, 1200, 1100, 1150, 1250, 1400, 1350, 1600, 900};

const char *ticketCategories[] = {"Standard", "VIP"}; // Ticket categories
int ticketPrices[][2] = { // Prices for each category
    {1500, 2500}, // Standard and VIP prices for Mumbai
    {1300, 2300}, // Delhi
    {1200, 2200}, // Bangalore
    {1100, 2100}, // Hyderabad
    {1150, 2150}, // Chennai
    {1250, 2250}, // Kolkata
    {1400, 2400}, // Jaipur
    {1350, 2350}, // Ahmedabad
    {1600, 2600}, // Pune
    {900, 1900}   // Lucknow
};

void showMenu();
void handleInput();
const int numCities = sizeof(indianCities) / sizeof(indianCities[0]);
struct Routs routeSeatAvailability[MAX_ROUTES]; 
int routeCount = 0;  // Keep track of unique routes

void printCentered(const char* str, int width) {
    int len = strlen(str);
    int spaces = (width - len) / 2; 
    if (spaces < 0) spaces = 0; // In case the string is longer than width
    printf("%*s%s\n", spaces, "", str); // Print spaces and the string
}

void clearInputBuffer()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

void initializeSeats() {
    for (int i = 0; i < MAX_ROUTES; i++) {
        for (int j = 0; j < MAX_SEATS; j++) {
            routeSeatAvailability[i].seatAvailability[j] = true; // Initialize all seats to available
        }
    }
    struct Booking booking;
    FILE *file = fopen(FILENAME, "rb");

    if (file != NULL) {
        while (fread(&booking, sizeof(struct Booking), 1, file) == 1) {
            for (int j = 0; j < MAX_SEATS && booking.seats[j] != 0; j++) {
                // Find the route and mark the seat as booked
                for (int r = 0; r < routeCount; r++) {
                    if (strcmp(routeSeatAvailability[r].currentLocation, booking.currentLocation) == 0 &&
                        strcmp(routeSeatAvailability[r].destination, booking.destination) == 0) {
                        routeSeatAvailability[r].seatAvailability[booking.seats[j] - 1] = false;
                        break; // Break to prevent unnecessary looping
                    }
                }
            }
        }
        fclose(file);
    }
}

void addRoute(const char* currentLocation, const char* destination) {
    // Initialize a new route if it doesn't exist
    for (int i = 0; i < routeCount; i++) {
        if (strcmp(routeSeatAvailability[i].currentLocation, currentLocation) == 0 &&
            strcmp(routeSeatAvailability[i].destination, destination) == 0) {
            return; // Route already exists
        }
    }
    // If route does not exist, initialize it
    strcpy(routeSeatAvailability[routeCount].currentLocation, currentLocation);
    strcpy(routeSeatAvailability[routeCount].destination, destination);
    for (int i = 0; i < MAX_SEATS; i++) {
        routeSeatAvailability[routeCount].seatAvailability[i] = true; // Set all seats to available
    }
    routeCount++;
}

bool isSeatAvailableForRoute(const char* currentCity, const char* destinationCity, int seatNum) {
    for (int i = 0; i < routeCount; i++) {
        if (strcmp(routeSeatAvailability[i].currentLocation, currentCity) == 0 &&
            strcmp(routeSeatAvailability[i].destination, destinationCity) == 0) {
            return routeSeatAvailability[i].seatAvailability[seatNum - 1]; // Checking availability for specific route
        }
    }
    return false; // If route is not found, return false
}

void bookSeatRoute(const char* currentCity, const char* destinationCity, int seatNum) {
    for (int i = 0; i < routeCount; i++) {
        if (strcmp(routeSeatAvailability[i].currentLocation, currentCity) == 0 &&
            strcmp(routeSeatAvailability[i].destination, destinationCity) == 0) {
            routeSeatAvailability[i].seatAvailability[seatNum - 1] = false; // Mark the seat as booked
            return; // Break once you've found the route
        }
    }
}

void freeSeatRoute(const char* currentCity, const char* destinationCity, int seatNum) {
    for (int i = 0; i < routeCount; i++) {
        if (strcmp(routeSeatAvailability[i].currentLocation, currentCity) == 0 &&
            strcmp(routeSeatAvailability[i].destination, destinationCity) == 0) {
            routeSeatAvailability[i].seatAvailability[seatNum - 1] = true; // Mark the seat as available again
            return; // Exit once you've freed the seat
        }
    }
}

void displayAvailableSeats(const char* currentCity, const char* destinationCity) {
    printf("\n +-------------------------------------------------------------------------------------------------------------------+\n");
    printf(" |    Available Seats for %s to %s:  | \n", currentCity, destinationCity);
    printf(" +-------------------------------------------------------------------------------------------------------------------+\n");

    for (int i = 0; i < routeCount; i++) {
        if (strcmp(routeSeatAvailability[i].currentLocation, currentCity) == 0 &&
            strcmp(routeSeatAvailability[i].destination, destinationCity) == 0) {
            printf(" | ");
            bool foundSeat = false;
            for (int j = 0; j < MAX_SEATS; j++) {
                if (routeSeatAvailability[i].seatAvailability[j]) {
                    printf("%d ", (j + 1)); // 1-indexed seat numbers
                    foundSeat = true;
                }
            }
            if (!foundSeat) {
                printf("No available seats.");
            }
            printf("\n +-------------------------------------------------------------------------------------------------------------------+\n");
            break; // Break once you've found the relevant route
        }
    }
}


bool isValidName(const char *name)
{
    if (isspace(name[0])) {
        return false;
    }
    for (const char *p = name; *p; p++) {
        if (*p != ' ' && !isalpha(*p)) {
            return false;
        }
    }
    // Check if name contains at least one non-space character
    for (const char *p = name; *p; p++) {
        if (*p != ' ') {
            return true;
        }
    }
    return false;
}

void savePartialBooking(struct PartialBooking *partial)
{
    FILE *file = fopen(PARTIAL_BOOKING_FILENAME, "wb");
    if (file == NULL)
    {
        printf("Error: Unable to save partial booking. Please try again.\n");
        return;
    }
    if (fwrite(partial, sizeof(struct PartialBooking), 1, file) != 1)
    {
        printf("Error: Failed to write partial booking data. Please try again.\n");
    }
    fclose(file);
}

bool loadPartialBooking(struct PartialBooking *partial)
{
    FILE *file = fopen(PARTIAL_BOOKING_FILENAME, "rb");
    if (file == NULL)
    {
        return false;
    }
    bool success = (fread(partial, sizeof(struct PartialBooking), 1, file) == 1);
    fclose(file);
    return success;
}

void saveBookingProgress(struct PartialBooking *partial)
{
    FILE *file = fopen(PROGRESS_FILENAME, "wb");
    if (file == NULL)
    {
        printf("Error: Unable to save booking progress. Please try again.\n");
        return;
    }
    if (fwrite(partial, sizeof(struct PartialBooking), 1, file) != 1)
    {
        printf("Error: Failed to write booking progress data. Please try again.\n");
    }
    else
    {
        printf("Booking progress saved successfully.\n");
    }
    fclose(file);
}

bool loadBookingProgress(struct PartialBooking *partial)
{
    FILE *file = fopen(PROGRESS_FILENAME, "rb");
    if (file == NULL)
    {
        return false;
    }
    bool success = (fread(partial, sizeof(struct PartialBooking), 1, file) == 1);
    fclose(file);
    return success;
}

void generateReferenceNumber(char *refNumber, int ticketID)
{
    time_t t;
    srand((unsigned)time(&t));
    int random = rand() % 1000;
    sprintf(refNumber, "REF-%d-%03d", ticketID, random);
}

int unique_id(){       //Automatically Generate Unique, Non-Repeating Ticket IDs #16 by Vasu
    static int counter = 0;
    time_t now = time(NULL);
    return ((int)(now % 1234) | counter++);
}

void printBookingSummary(const struct PartialBooking *partial, int n) {
    printf("\n +----------------------------------------------+\n");
    printf(" |               Booking Summary                |\n");
    printf(" +----------------------------------------------+\n");
    printf(" | Ticket ID: %d                               |\n", partial->booking.ticketID);
    printf(" | Name: %s                                     |\n", partial->booking.name);
    printf(" | Current Location: %s                     |\n", partial->booking.currentLocation);
    printf(" | Destination: %s                           |\n", partial->booking.destination);
    printf(" | Number of Travelers: %d                       |\n", n);
    printf(" | Category: %s                             |\n", ticketCategories[partial->booking.category[0]]);

    printf(" | Seats Booked: ");
    for (int j = 0; j < n; j++) {
        printf("%d ", partial->booking.seats[j]);
    }

    printf("                             |\n");
    printf(" | Final Price: Rs. %-32d |\n", partial->booking.price);
    printf(" +----------------------------------------------+\n");
}

void Discount(struct Booking *booking, const char *promoCode) {
    if (strcmp(promoCode, "meet") == 0) {
        int discount = 10;
        booking->discount = discount;
        booking->price -= (booking->price * booking->discount / 100);
        printf("You got %d%% discount...\n",discount);
    } else {
        booking->discount = 0; 
    }
}

void addBooking() {
    struct PartialBooking partial;
    bool resuming = false;
    char bookingReference[20]; // To store the generated booking reference number

    if (loadPartialBooking(&partial) && partial.inProgress) {
        printf("You have a partially completed booking. Do you want to resume? (1: Yes, 0: No): ");
        int choice;
        if (scanf("%d", &choice) != 1) {
            printf("Error: Invalid input. Starting a new booking.\n");
            clearInputBuffer();
            partial.inProgress = false;
            partial.stage = 0;
        } else if (choice == 1) {
            resuming = true;
        } else {
            partial.inProgress = false;
            partial.stage = 0;
        }
        clearInputBuffer();
    } else {
        partial.inProgress = true;
        partial.stage = 0;
    }

    if (!resuming || partial.stage == 0) {
        partial.booking.ticketID = unique_id();
        partial.stage = 1;
    }

    if (!resuming || partial.stage <= 1) {
             printf("\n");
        // Get Name
        do {
            printf("Enter Name (alphabets and spaces only, or 'pause' to pause): ");
            if (fgets(partial.booking.name, MAX_NAME_LENGTH, stdin) == NULL) {
                printf("Error: Failed to read input. Please try again.\n");
                continue;
            }
            partial.booking.name[strcspn(partial.booking.name, "\n")] = 0; // Remove newline
            if (strcmp(partial.booking.name, "pause") == 0) {
                savePartialBooking(&partial);
                printf("Booking paused. You can resume later.\n");
                return;
            }
            if (!isValidName(partial.booking.name)) {
                printf("Error: Invalid name. Please use only alphabets and spaces.\n");
                continue;
            }
            break;
        } while (1);
        partial.stage = 2;
    }
 printf("\n");
    if (!resuming || partial.stage <= 2) {
        int currentChoice;

        // Current Location Input
        printf("Enter your current location:\n");
        for (int i = 0; i < numCities; i++) {
            printf("%d. %s\n", i + 1, indianCities[i]);
        }

        do {
            printf("Enter the number of your current location (1-%d), or 0 to pause: ", numCities);
            if (scanf("%d", &currentChoice) != 1 || currentChoice < 0 || currentChoice > numCities) {
                printf("Error: Invalid choice. Please enter a number between 0 and %d.\n", numCities);
                clearInputBuffer();
                continue;
            }
            if (currentChoice == 0) {
                savePartialBooking(&partial);
                printf("Booking paused. You can resume later.\n");
                return;
            }
            break;
        } while (1);

        strcpy(partial.booking.currentLocation, indianCities[currentChoice - 1]);
        addRoute(partial.booking.currentLocation, ""); // Add route initialization
        clearInputBuffer();

        int choice;
 printf("\n");
        // Destination Input
        printf("Select Destination:\n");
        for (int i = 0; i < numCities; i++) {
            printf("%d. %s\n", i + 1, indianCities[i]);
        }

        do {
            printf("Enter the number of your destination (1-%d), or 0 to pause: ", numCities);
            if (scanf("%d", &choice) != 1 || choice < 0 || choice > numCities) {
                printf("Error: Invalid choice. Please enter a number between 0 and %d.\n", numCities);
                clearInputBuffer();
                continue;
            }
            if (choice == 0) {
                savePartialBooking(&partial);
                printf("Booking paused. You can resume later.\n");
                return;
            }
            break;
        } while (1);
 printf("\n");
        // Check for same pickup and destination
        if (currentChoice - 1 == choice - 1) {
            printf("Pickup point and destination cannot be the same.\n");
            return;
        }

        strcpy(partial.booking.destination, indianCities[choice - 1]);

        // Add the route for new booking
        addRoute(partial.booking.currentLocation, partial.booking.destination);

        clearInputBuffer();

        int n;
 printf("\n");
        do {
            // Traveler Count Input
            printf("Enter how many travelers: ");
            if (scanf("%d", &n) != 1 || n <= 0) {
                printf("Invalid input. Please enter a valid number of travelers (greater than zero).\n");
                clearInputBuffer();
            } else {
                break;
            }
        } while (1);
        clearInputBuffer();

        int categoryChoice;
 printf("\n");
        // Ticket Category Selection
        printf("Select Ticket Category:\n");
        for (int i = 0; i < sizeof(ticketCategories) / sizeof(ticketCategories[0]); i++) {
            printf("%d. %s\n", i + 1, ticketCategories[i]);
        }
 printf("\n");
        do {
            printf("Enter the number of your selected category (1-%ld): ", sizeof(ticketCategories) / sizeof(ticketCategories[0]));
            if (scanf("%d", &categoryChoice) != 1 || categoryChoice < 1 || categoryChoice > sizeof(ticketCategories) / sizeof(ticketCategories[0])) {
                printf("Error: Invalid choice. Please enter a number between 1 and %ld.\n", sizeof(ticketCategories) / sizeof(ticketCategories[0]));
                clearInputBuffer();
                continue;
            }
            break;
        } while (1);

        // Initialize booked seats to 0
        for (int i = 0; i < MAX_SEATS; i++) {
            partial.booking.seats[i] = 0; // Initialize unbooked seats with 0
        }

        for (int i = 0; i < n; i++) {
            int seatNum;
            displayAvailableSeats(partial.booking.currentLocation, partial.booking.destination); // Show available seats before booking
            do {
                printf("Enter seat number for traveler %d: ", i + 1);
                if (scanf("%d", &seatNum) != 1 || seatNum < 1 || seatNum > MAX_SEATS || !isSeatAvailableForRoute(partial.booking.currentLocation, partial.booking.destination, seatNum)) {
                    printf("Error: Invalid or unavailable seat number. Please select an available seat (1-%d).\n", MAX_SEATS);
                    clearInputBuffer();
                    continue;
                }
                break;
            } while (1);
            partial.booking.seats[i] = seatNum; // Store the booked seat number
            bookSeatRoute(partial.booking.currentLocation, partial.booking.destination, seatNum); // Mark seat as booked
        }

        partial.booking.price = ticketPrices[currentChoice - 1][categoryChoice - 1]; // Updated by Meet

        // Display summary before finalizing booking
        printBookingSummary(&partial, n); // Improved summary display

        char confirm[10];

        clearInputBuffer();
 printf("\n");
        do {
            printf("Do you want to confirm this booking? (yes/no): ");
            if (fgets(confirm, sizeof(confirm), stdin) == NULL) {
                printf("Error: Failed to read input. Please try again.\n");
                continue; // Retry prompt in case of error
            }

            // Remove newline character that fgets adds
            confirm[strcspn(confirm, "\n")] = '\0';

            for (int i = 0; confirm[i]; i++) {
                confirm[i] = tolower(confirm[i]);
            }

            if (strcmp(confirm, "yes") == 0) {
                // Mark the seats as booked
                for (int j = 0; j < n; j++) {
                    bookSeatRoute(partial.booking.currentLocation, partial.booking.destination, partial.booking.seats[j]);
                }

                char promoCode[20];
                printf("Enter promotional code (if any) OR Press Enter: ");
                fgets(promoCode, sizeof(promoCode), stdin);
                promoCode[strcspn(promoCode, "\n")] = 0; 

                Discount(&partial.booking, promoCode);
                
                FILE *file = fopen(FILENAME, "ab");
                if (file == NULL) {
                    printf("Error: Unable to open bookings file. Booking not saved.\n");
                    return;
                }

                if (fwrite(&partial.booking, sizeof(struct Booking), 1, file) != 1) {
                    printf("Error: Failed to write booking data. Please try again.\n");
                } else {
                    generateReferenceNumber(bookingReference, partial.booking.ticketID);
                    recordFeedback(partial.booking.ticketID, partial.booking.name);
                    printf("\nBooking added successfully!\n");

                    const int width = 90; 
                    printf("\n");
                    
                    // Receipt
                    printCentered("+----------------------------------------------+", 50);
                    printCentered("|                   Receipt                    |", 50);
                    printCentered("+----------------------------------------------+", 50);
                    printf(" | Ticket ID: %-33d |\n", partial.booking.ticketID);
                    printf(" | Name: %-38s |\n", partial.booking.name);
                    printf(" | Current Location: %-26s |\n", partial.booking.currentLocation);
                    printf(" | Destination: %-31s |\n", partial.booking.destination);
                    printf(" | Number of Travelers: %-23d |\n", n);
                    printf(" | Category: %-34s |\n", ticketCategories[partial.booking.category[0]]);
                    printf("| Seats Booked: ");
                    for (int j = 0; j < n; j++) {
                         printf("%d ", partial.booking.seats[j]);
                    }
                    printf("%*s |\n", (width - strlen("| Seats Booked: ") - (n * 2) - 1), ""); // Calculate space to keep in line
                    printf(" | Price: Rs. %-32d  |\n", partial.booking.price);
                    printCentered("+----------------------------------------------+", 50);
                }
                fclose(file);

                // Clear partial booking
                partial.inProgress = false;
                savePartialBooking(&partial);
                remove(PROGRESS_FILENAME);
                break;
            }
             else if (strcmp(confirm, "no") == 0) {

                for (int j = 0; j < n; j++) {
                      freeSeatRoute(partial.booking.currentLocation, partial.booking.destination, partial.booking.seats[j]);
                    }
                printf("Booking cancelled. You can make changes or start over.\n");
                return;
            } else {
                printf("Invalid input. Please enter 'yes' or 'no'.\n");
            }

        } while (1);
    }
}

void displayBookings() {
    struct Booking booking;
    FILE *file = fopen(FILENAME, "rb");

    if (file == NULL) {
        printf("No bookings found or error opening file.\n");
        return;
    }

    // Adjusted header to include "Booked Seat"
    printf("\n +----------------------------------------------------------------------------------------------+\n");
    printf(" | %-10s %-20s %-20s %-20s %-10s %-6.5s |\n", "Ticket ID", "Name", "Current Location", "Destination", "Booked Seat", "Price");
    printf(" +----------------------------------------------------------------------------------------------+\n");

    // Read and display each booking entry
    while (fread(&booking, sizeof(struct Booking), 1, file) == 1) {
        int bookedCount = 0;

        // Count how many seats were booked
        for (int i = 0; i < MAX_SEATS; i++) {
            if (booking.seats[i] != 0) { // Count only non-zero seats
                bookedCount++;
            }
        }

        // Display booking information
        printf(" | %-10d %-20s %-20s %-20s %-10d Rs.%d |\n",
               booking.ticketID, booking.name, booking.currentLocation,
               booking.destination, bookedCount, booking.price);
    }
    printf(" +----------------------------------------------------------------------------------------------+\n");

    // Check for any read errors
    if (ferror(file)) {
        printf("Error occurred while reading the file.\n");
    }

    fclose(file);
}

void searchBookings()
{
    int searchChoice;
    printf("\nSearch by:\n");
    printf("1. Ticket ID\n");
    printf("2. Name\n");
    printf("Enter your choice: ");
    if (scanf("%d", &searchChoice) != 1)
    {
        printf("Error: Invalid input. Please enter a valid number.\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();

    struct Booking booking;
    FILE *file = fopen(FILENAME, "rb");
    if (file == NULL)
    {
        printf(" error opening file.\n");
        return;
    }

    bool found = false;
    if (searchChoice == 1)
    {
        int ticketID;
        printf("Enter Ticket ID to search: ");
        if (scanf("%d", &ticketID) != 1)
        {
            printf("Error: Invalid input.\n");
            clearInputBuffer();
            fclose(file);
            return;
        }
        clearInputBuffer();

        while (fread(&booking, sizeof(struct Booking), 1, file) == 1)
        {
            if (booking.ticketID == ticketID)
            {
                printf("\nBooking Found:\n");
                printf("Ticket ID: %d\n", booking.ticketID);
                printf("Name: %s\n", booking.name);
                printf("Destination: %s\n", booking.destination);
                printf("Category: %s\n",booking.category);
                printf("Price: Rs. %d\n", booking.price);
                found = true;
            }
        }
    }
    else if (searchChoice == 2)
    {
        char name[MAX_NAME_LENGTH];
        printf("Enter Name to search: ");
        fgets(name, MAX_NAME_LENGTH, stdin);
        name[strcspn(name, "\n")] = 0; // Remove newline

        while (fread(&booking, sizeof(struct Booking), 1, file) == 1)
        {
            if (strcasecmp(booking.name, name) == 0)
            {
                printf("\nBooking Found:\n");
                printf("Ticket ID: %d\n", booking.ticketID);
                printf("Name: %s\n", booking.name);
                printf("Destination: %s\n", booking.destination);
                printf("Category: %s",booking.category);
                printf("Price: Rs. %d\n", booking.price);
                found = true;
            }
        }
    }
    else
    {
        printf("Invalid choice.\n");
        fclose(file);
        return;
    }

    if (!found)
    {
        printf("No booking found with the given criteria.\n");
    }

    fclose(file);
}

// exit without save functionality 
void removeLastBooking() {
    struct Booking *bookings = NULL;
    int count = 0;
    FILE *file = fopen(FILENAME, "rb");
    if (file == NULL) {
        return; 
    }

    // Count the number of bookings and read them into an array
    while (!feof(file)) {
        bookings = realloc(bookings, (count + 1) * sizeof(struct Booking));
        if (fread(&bookings[count], sizeof(struct Booking), 1, file) == 1) {
            count++;
        }
    }
    fclose(file);

    // If there are bookings, overwrite the file excluding the last booking
    if (count > 1) {
        file = fopen(FILENAME, "wb");
        for (int i = 0; i < count - 1; i++) {
            fwrite(&bookings[i], sizeof(struct Booking), 1, file);
        }
        fclose(file);
    } else {
        // If there was only one booking, just remove the file
        remove(FILENAME);
    }

    free(bookings); // Free the allocated array
}

void displayCities() {
    printf("Available Cities:\n");
    for (int i = 0; i < numCities; i++) {
        printf("%d. %s\n", i + 1, indianCities[i]);
    }
}

bool cityExists(const char* cityName) {
    for (int i = 0; i < numCities; i++) {
        if (strcasecmp(indianCities[i], cityName) == 0) {
            return true;
        }
    }
    return false;
}

int selectCity() {
    int choice;

    while (1) {
        printf("Please select a city (1-%d) or 0 to skip: ", numCities);
        if (scanf("%d", &choice) != 1 || choice < 0 || choice > numCities) {
            printf("Error: Invalid input. Please enter a number between 0 and %d.\n", numCities);
            clearInputBuffer();
        } else {
            break; // Valid input received
        }
    }

    clearInputBuffer(); 
    return choice;
}

void modifyBooking() {
    int ticketID;
    printf("Enter Ticket ID to modify: ");
    if (scanf("%d", &ticketID) != 1) {
        printf("Error: Invalid input.\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();

    struct Booking booking;
    FILE *file = fopen(FILENAME, "rb");
    FILE *tempFile = fopen("temp.dat", "wb");
    if (file == NULL || tempFile == NULL) {
        printf("Error opening file.\n");
        if (file) fclose(file);
        if (tempFile) fclose(tempFile);
        return;
    }

    bool found = false;
    while (fread(&booking, sizeof(struct Booking), 1, file) == 1) {
        if (booking.ticketID == ticketID) {
            found = true;
            printf("Current Booking Details:\n");
            printf("Name: %s\n", booking.name);
            printf("Current Location: %s\n", booking.currentLocation);
            printf("Destination: %s\n", booking.destination);
            printf("Price: Rs. %d\n", booking.price);

            // Modify the Name
            printf("Enter new name (leave blank for no change): ");
            char newName[MAX_NAME_LENGTH];
            fgets(newName, MAX_NAME_LENGTH, stdin);
            newName[strcspn(newName, "\n")] = 0;
            if (strlen(newName) > 0) {
                strncpy(booking.name, newName, MAX_NAME_LENGTH);
            }

            // Display Indian Cities and Modify Current Location
            for (int i = 0; i < numCities; i++) {
                printf("%d. %s\n", i + 1, indianCities[i]);
            }
            printf("current location : ");
            int currentLocationChoice = selectCity();
            if (currentLocationChoice > 0) {
                strncpy(booking.currentLocation, indianCities[currentLocationChoice - 1], MAX_DESTINATION_LENGTH);
            } else if (currentLocationChoice == 0) {
                printf("No change to the current location.\n");
            }

            // Display Cities and Modify Destination Location
            for (int i = 0; i < numCities; i++) {
                printf("%d. %s\n", i + 1, indianCities[i]);
            }

            printf("destination location : ");
            int newDestChoice = selectCity();
            if (newDestChoice > 0) {
                strncpy(booking.destination, indianCities[newDestChoice - 1], MAX_DESTINATION_LENGTH);
            } else if (newDestChoice == 0) {
                printf("No change to the destination.\n");
            }

            // Modify Number of Travelers
            int n;
            do {
                printf("Enter new number of travelers: ");
                if (scanf("%d", &n) != 1 || n <= 0) {
                    printf("Invalid input. Please enter a valid number of travelers (greater than zero).\n");
                    clearInputBuffer();
                } else {
                    break;
                }
            } while (1);
            clearInputBuffer();

            // Modify Ticket Category
            int categoryChoice;
            printf("Select Ticket Category:\n");
            for (int i = 0; i < sizeof(ticketCategories) / sizeof(ticketCategories[0]); i++) {
                printf("%d. %s\n", i + 1, ticketCategories[i]);
            }

            do {
                printf("Enter the number of your selected category (1-%ld): ", sizeof(ticketCategories) / sizeof(ticketCategories[0]));
                if (scanf("%d", &categoryChoice) != 1 ||
                    categoryChoice < 1 ||
                    categoryChoice > sizeof(ticketCategories) / sizeof(ticketCategories[0])) {
                    printf("Error: Invalid choice. Please enter a number between 1 and %ld.\n", sizeof(ticketCategories) / sizeof(ticketCategories[0]));
                    clearInputBuffer();
                    continue;
                }
                break;

            } while (1);

            int currentLocationIndex = -1, destinationIndex = -1;
            for (int i = 0; i < numCities; i++) {
                if (strcmp(booking.currentLocation, indianCities[i]) == 0) {
                    currentLocationIndex = i;
                }
                if (strcmp(booking.destination, indianCities[i]) == 0) {
                    destinationIndex = i;
                }
            }

            if (currentLocationIndex != -1 && destinationIndex != -1) {
                booking.price = ticketPrices[currentLocationIndex][categoryChoice - 1];
            } else {
                printf("Error: Unable to find current location or destination in the cities list.\n");
                fclose(file);
                fclose(tempFile);
                return;
            }

            // Write updated booking to temp file
            fwrite(&booking, sizeof(struct Booking), 1, tempFile);
            printf("Booking modified successfully!\n");
        } else {
            // Write unchanged booking to temp file
            fwrite(&booking, sizeof(struct Booking), 1, tempFile);
        }
    }

    if (!found) {
        printf("Booking with Ticket ID %d not found.\n", ticketID);
    }

    fclose(file);
    fclose(tempFile);
    remove(FILENAME);
    rename("temp.dat", FILENAME); // Replace original file with updated file
}

void cancelBooking() {
    int ticketID;
    printf("Enter Ticket ID to cancel: ");
    if (scanf("%d", &ticketID) != 1) {
        printf("Error: Invalid input.\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();

    struct Booking booking;
    FILE *file = fopen(FILENAME, "rb");
    FILE *tempFile = fopen("temp.dat", "wb");
    if (file == NULL || tempFile == NULL) {
        printf("Error opening file.\n");
        if (file) fclose(file);
        if (tempFile) fclose(tempFile);
        return;
    }

    bool found = false;
    while (fread(&booking, sizeof(struct Booking), 1, file) == 1) {
        if (booking.ticketID == ticketID) {
            found = true;
            printf("Booking with Ticket ID %d has been canceled successfully!\n", ticketID);
        } else {
            // Write unchanged booking to temp file
            fwrite(&booking, sizeof(struct Booking), 1, tempFile);
        }
    }

    if (!found) {
        printf("Booking with Ticket ID %d not found.\n", ticketID);
    }

    fclose(file);
    fclose(tempFile);
    remove(FILENAME);
    rename("temp.dat", FILENAME); // Replace original file with updated file
}

void PopularDestinations()
{
    struct Booking booking;
    FILE *file = fopen(FILENAME, "rb");

    int destinationCounts[MAX_DESTINATION_LENGTH];

    memset(destinationCounts, 0, sizeof(destinationCounts));

    if (file == NULL)
    {
        printf("Error opening bookings file.\n");
        return;
    }

    while (fread(&booking, sizeof(struct Booking), 1, file) == 1)
    {
        for (int i = 0; i < numCities; i++)
        {
            if (strcmp(booking.destination, indianCities[i]) == 0)
            {
                destinationCounts[i]++;
                break;
            }
        }
    }

    fclose(file);

    printf("\n+---------------------------------------------+\n");
    printCentered("|            Popular Destinations             |", 45);
    printf("+---------------------------------------------+\n");

    for (int i = 0; i < numCities; i++) {
        if (destinationCounts[i] > 0) {
            printf("| %-31s: %d bookings |\n", indianCities[i], destinationCounts[i]);
        }
    }
    printf("+---------------------------------------------+\n");
}

void RevenueStatistics()
{
    struct Booking booking;
    FILE *file = fopen(FILENAME, "rb");
    int totalRevenue = 0;

    if (file == NULL)
    {
        printf("Error opening bookings file.\n");
        return;
    }

    while (fread(&booking, sizeof(struct Booking), 1, file) == 1)
    {
        totalRevenue += booking.price;
    }

    fclose(file);
    printf("\n+---------------------------------------------+\n");
    printCentered("|            Revenue Statistics               |", 45);
    printf("+---------------------------------------------+\n");
    printf("| Total Revenue from Bookings: Rs. %-10d |\n", totalRevenue);
    printf("+---------------------------------------------+\n");
}

void generateReports() {
     printf("\n+---------------------------------------------+\n");
    printCentered("|           Reports and Analytics             |", 45);
    printf("+---------------------------------------------+\n");

    PopularDestinations();
    RevenueStatistics();

    printf("+---------------------------------------------+\n");
}

void recordFeedback(int ticketID, const char* name) {
    struct Feedback feedback;
    feedback.ticketID = ticketID;
    strncpy(feedback.name, name, MAX_NAME_LENGTH);

    do {
        printf("Rate your experience (1-5): ");
        if (scanf("%d", &feedback.rating) != 1 || feedback.rating < 1 || feedback.rating > 5) {
            printf("Invalid input. Please enter a number between 1 and 5.\n");
            clearInputBuffer();
        } else {
            break; // valid input
        }
    } while (1);

    clearInputBuffer(); 
    printf("Enter Your Feedback (max 200 characters): ");
    fgets(feedback.comments, sizeof(feedback.comments), stdin);
    feedback.comments[strcspn(feedback.comments, "\n")] = 0; // Remove newline

    FILE *file = fopen("feedbacks.dat", "ab");
    if (file == NULL) {
        printf("Error saving feedback. Please try again.\n");
        return;
    }
    fwrite(&feedback, sizeof(struct Feedback), 1, file);
    fclose(file);
    printf("Thank you for your feedback!\n");
}

void displayFeedbacks() {
    struct Feedback feedback;
    FILE *file = fopen("feedbacks.dat", "rb");

    if (file == NULL) {
        printf("+-----------------------------------------------+\n");
        printf("| No feedbacks available.                        |\n");
        printf("+-----------------------------------------------+\n");
        return;
    }

    printf("\nFeedbacks:\n");
    printf("+----------------------------------------------------------------------+\n");
    printf("| Ticket ID | Name                | Rating |      Comments             |\n");
    printf("+----------------------------------------------------------------------+\n");

    while (fread(&feedback, sizeof(struct Feedback), 1, file) == 1) {
        printf("| %-10d | %-18s | %-6d | %s\n",
               feedback.ticketID, feedback.name, feedback.rating, feedback.comments);
    }
    printf("+----------------------------------------------------------------------+\n");

    fclose(file);
}


int main()
{
     system("color 78");
    while(1){
        showMenu();
        TransportMode();
    }
    return 0;
}

void showMenu(){
    printf("\n");
          printCentered("\033[30m+-----------------------------------------------+", 120);
        printCentered("\033[30m|           Ticket Booking System               |", 120);
        printCentered("\033[30m+-----------------------------------------------+", 120);
        printCentered("\033[30m| 1. Add/Resume Booking                         |", 120);
        printCentered("\033[30m| 2. Display Bookings                           |", 120);
        printCentered("\033[30m| 3. Save Progress and Exit                     |", 120);
        printCentered("\033[30m| 4. Exit without Saving                        |", 120);
        printCentered("\033[30m| 5. Search Bookings                            |", 120);
        printCentered("\033[30m| 6. Modify Booking                             |", 120);
        printCentered("\033[30m| 7. Cancel Booking                             |", 120);
        printCentered("\033[30m| 8. View Report                                |", 120);
        printCentered("\033[30m| 9. View Feedbacks                             |", 120);
        printCentered("\033[30m| 10. Exit.                                     |", 120);
        printCentered("\033[30m+-----------------------------------------------+", 120);
}

void TransportMode() {
    int Choice;

    printf("\nSelect mode of transport:\n");
    printf("1. Bus\n");
    printf("2. Train\n");
    printf("Enter your choice: ");

    if (scanf("%d", &Choice) != 1 || (Choice < 1 || Choice > 2)) {
        printf("Invalid choice. Please choose 1 for Bus or 2 for Train.\n");
        clearInputBuffer(); 
        return;
    }
    clearInputBuffer(); 

    printf("You selected: %s\n", Choice == 1 ? "Bus" : "Train");

    if (Choice == 1) {
        handleInput(); 
    } else {
        handleInput(); 
    }
}

void handleInput(){
    int choice;
    struct PartialBooking partial;

    while(1){
        printf("Enter your choice: ");
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Please enter a number.\n");
            clearInputBuffer();
            continue;
        }
        clearInputBuffer();
        switch (choice)
        {
        case 1:
            addBooking();
            showMenu();
            break;
        case 2:
            displayBookings();
            showMenu();
            break;
        case 3:
            if (partial.inProgress) {
                saveBookingProgress(&partial);
                printf("Progress saved.\n");

                int exitChoice;
                do {
                    printf("Do you want to exit? (1: Yes, 0: No): ");
                    if (scanf("%d", &exitChoice) != 1 || (exitChoice != 0 && exitChoice != 1)) {
                        printf("Invalid input. Please enter 1 for Yes or 0 for No.\n");
                        clearInputBuffer();
                        continue;
                    }
                    clearInputBuffer();
                } while (exitChoice != 0 && exitChoice != 1);

                if (exitChoice == 1) {
                    printf("Goodbye!\n");
                    exit(0);
                } else {
                    continue;
                }
            } else {
                printf("No booking in progress to save. Goodbye!\n");
                exit(0);
            }
            showMenu();
            break;
        case 4:
            printf("Exiting without saving.\n");
            if (remove(PARTIAL_BOOKING_FILENAME) == 0) {
                printf("Unsaved progress cleared.\n");
            } else {
                printf("No unsaved progress to clear.\n");
            }

            int exitChoice2;
            do {
                printf("Do you want to exit? (1: Yes, 0: No): ");
                if (scanf("%d", &exitChoice2) != 1 || (exitChoice2 != 0 && exitChoice2 != 1)) {
                    printf("Invalid input. Please enter 1 for Yes or 0 for No.\n");
                    clearInputBuffer();
                    continue;
                }
                clearInputBuffer();
            } while (exitChoice2 != 0 && exitChoice2 != 1);

            if (exitChoice2 == 1) {
                printf("Goodbye!\n");
                exit(0);
            } else {
                // If the user chooses not to exit, continue with the program
                continue;

            }
            break;
        case 5:
            searchBookings();
            showMenu();
            break;
        case 6:
            modifyBooking();
            showMenu();
            break;
        case 7:
            cancelBooking();
            showMenu();
            break;
        case 8:
            generateReports();
            showMenu();
            break;
        case 9:
            displayFeedbacks();
            showMenu();
            break;
        case 10:
            exit(0);
            break;
        default:
            printf("Invalid choice. Please try again.\n");
        }
    }
}
