#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <time.h> 

#define MAX_NAME_LENGTH 50
#define MAX_DESTINATION_LENGTH 50
#define FILENAME "bookings.dat"
#define PARTIAL_BOOKING_FILENAME "partial_booking.dat"
#define PROGRESS_FILENAME "booking.det"

struct Booking {
    int ticketID;
    char name[MAX_NAME_LENGTH];
    char destination[MAX_DESTINATION_LENGTH];
    float price;
};

struct PartialBooking {
    bool inProgress;
    struct Booking booking;
    int stage; // 0: ticketID, 1: name, 2: destination, 3: price
};

const char* indianCities[] = {
    "Mumbai", "Delhi", "Bangalore", "Hyderabad", "Chennai",
    "Kolkata", "Jaipur", "Ahmedabad", "Pune", "Lucknow"
};

const int numCities = sizeof(indianCities) / sizeof(indianCities[0]);

void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

bool isValidName(const char* name) {
    for (int i = 0; name[i]; i++) {
        if (!isalpha(name[i]) && !isspace(name[i])) {
            return false;
        }
    }
    return true;
}

void savePartialBooking(struct PartialBooking *partial) {
    FILE *file = fopen(PARTIAL_BOOKING_FILENAME, "wb");
    if (file == NULL) {
        printf("Error: Unable to save partial booking. Please try again.\n");
        return;
    }
    if (fwrite(partial, sizeof(struct PartialBooking), 1, file) != 1) {
        printf("Error: Failed to write partial booking data. Please try again.\n");
    }
    fclose(file);
}

bool loadPartialBooking(struct PartialBooking *partial) {
    FILE *file = fopen(PARTIAL_BOOKING_FILENAME, "rb");
    if (file == NULL) {
        return false;
    }
    bool success = (fread(partial, sizeof(struct PartialBooking), 1, file) == 1);
    fclose(file);
    return success;
}

void saveBookingProgress(struct PartialBooking *partial) {
    FILE *file = fopen(PROGRESS_FILENAME, "wb");
    if (file == NULL) {
        printf("Error: Unable to save booking progress. Please try again.\n");
        return;
    }
    if (fwrite(partial, sizeof(struct PartialBooking), 1, file) != 1) {
        printf("Error: Failed to write booking progress data. Please try again.\n");
    } else {
        printf("Booking progress saved successfully.\n");
    }
    fclose(file);
}

bool loadBookingProgress(struct PartialBooking *partial) {
    FILE *file = fopen(PROGRESS_FILENAME, "rb");
    if (file == NULL) {
        return false;
    }
    bool success = (fread(partial, sizeof(struct PartialBooking), 1, file) == 1);
    fclose(file);
    return success;
}

void generateReferenceNumber(char* refNumber, int ticketID) {
    time_t t;
    srand((unsigned) time(&t));
    int random = rand() % 1000;
    sprintf(refNumber, "REF-%d-%03d", ticketID, random);
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
        do {
            printf("Enter Ticket ID (positive integer, or 0 to pause): ");
            if (scanf("%d", &partial.booking.ticketID) != 1 || partial.booking.ticketID < 0) {
                printf("Error: Invalid Ticket ID. Please enter a positive integer.\n");
                clearInputBuffer();
                continue;
            }
            if (partial.booking.ticketID == 0) {
                savePartialBooking(&partial);
                printf("Booking paused. You can resume later.\n");
                return;
            }
            break;
        } while (1);
        clearInputBuffer();
        partial.stage = 1;
    }

    if (!resuming || partial.stage <= 1) {
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

    if (!resuming || partial.stage <= 2) {
        printf("Select Destination:\n");
        for (int i = 0; i < numCities; i++) {
            printf("%d. %s\n", i + 1, indianCities[i]);
        }
        int choice;
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
        strcpy(partial.booking.destination, indianCities[choice - 1]);
        clearInputBuffer();
        partial.stage = 3;
    }

    if (!resuming || partial.stage <= 3) {
        do {
            printf("Enter Price (positive number, or -1 to pause): ");
            if (scanf("%f", &partial.booking.price) != 1) {
                printf("Error: Invalid price. Please enter a number.\n");
                clearInputBuffer();
                continue;
            }
            if (partial.booking.price == -1) {
                savePartialBooking(&partial);
                printf("Booking paused. You can resume later.\n");
                return;
            }
            if (partial.booking.price < 0) {
                printf("Error: Price cannot be negative. Please enter a positive number.\n");
                continue;
            }
            break;
        } while (1);
        clearInputBuffer();
    }

    // Finalize booking
    FILE *file = fopen(FILENAME, "ab");
    if (file == NULL) {
        printf("Error: Unable to open bookings file. Booking not saved.\n");
        return;
    }

    if (fwrite(&partial.booking, sizeof(struct Booking), 1, file) != 1) {
        printf("Error: Failed to write booking data. Please try again.\n");
    } else {
        // Generate and display booking receipt
        generateReferenceNumber(bookingReference, partial.booking.ticketID);
        printf("Booking added successfully!\n");
        printf("\nReceipt:\n");
        printf("Booking Reference: %s\n", bookingReference);
        printf("Ticket ID: %d\n", partial.booking.ticketID);
        printf("Name: %s\n", partial.booking.name);
        printf("Destination: %s\n", partial.booking.destination);
        printf("Price: Rs. %.2f\n", partial.booking.price);
    }
    fclose(file);

    // Clear partial booking
    partial.inProgress = false;
    savePartialBooking(&partial);
    remove(PROGRESS_FILENAME); // Remove the progress file after successful booking
}
void displayBookings() {
    struct Booking booking;
    FILE *file = fopen(FILENAME, "rb");

    if (file == NULL) {
        printf("No bookings found or error opening file.\n");
        return;
    }

    printf("\n%-10s %-20s %-20s %-10s\n", "Ticket ID", "Name", "Destination", "Price");
    printf("----------------------------------------------------------\n");

    while (fread(&booking, sizeof(struct Booking), 1, file) == 1) {
        printf("%-10d %-20s %-20s Rs.%-9.2f\n", 
                booking.ticketID, booking.name, booking.destination, booking.price);
    }

    if (ferror(file)) {
        printf("Error occurred while reading the file.\n");
    } else if (feof(file)) {
        if (ftell(file) == 0) {
            printf("No bookings found.\n");
        }
    }

    fclose(file);
}

int main() {
    int choice;
    struct PartialBooking partial;

    while (1) {
        printf("\nTicket Booking System\n");
        printf("1. Add/Resume Booking\n");
        printf("2. Display Bookings\n");
        printf("3. Save Progress and Exit\n");
        printf("4. Exit without Saving\n");
        printf("Enter your choice: ");
        if (scanf("%d", &choice) != 1) {
            printf("Error: Invalid input. Please enter a number.\n");
            clearInputBuffer();
            continue;
        }
        clearInputBuffer();

        switch (choice) {
            case 1:
                addBooking();
                break;
            case 2:
                displayBookings();
                break;
            case 3:
                if (loadPartialBooking(&partial) && partial.inProgress) {
                    saveBookingProgress(&partial);
                    printf("Progress saved. Goodbye!\n");
                } else {
                    printf("No booking in progress to save. Goodbye!\n");
                }
                exit(0);
            case 4:
                printf("Exiting without saving. Goodbye!\n");
                exit(0);
            default:
                printf("Invalid choice. Please try again.\n");
        }
    }

    return 0;
}