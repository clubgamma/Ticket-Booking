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

struct Booking
{
    int ticketID;
    char name[MAX_NAME_LENGTH];
    char currentLocation[MAX_DESTINATION_LENGTH];
    char destination[MAX_DESTINATION_LENGTH];
    int price;
    char category[MAX_NAME_LENGTH];
};

struct PartialBooking
{
    bool inProgress;
    struct Booking booking;
    int stage; // 0: ticketID, 1: name, 2: destination, 3: price
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

const int numCities = sizeof(indianCities) / sizeof(indianCities[0]);

void clearInputBuffer()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

bool isValidName(const char *name)
{
    for (int i = 0; name[i]; i++)
    {
        if (!isalpha(name[i]) && !isspace(name[i]))
        {
            return false;
        }
    }
    return true;
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

int getPriceForCity(const char *currentCity, const char *destinationCity, int n, int categoryIndex) {
    int current = -1, destination = -1;

    for (int i = 0; i < numCities; i++) {
        if (strcmp(currentCity, indianCities[i]) == 0) {
            current = i;
        }
        if (strcmp(destinationCity, indianCities[i]) == 0) {
            destination = i;
        }
    }

    if (current != -1 && destination != -1) {
        return ticketPrices[current][categoryIndex] * n; // Use category index for pricing
    }

    return -1; 
}

// int getPriceForCity(const char *currentCity, const char *destinationCity, int n)
// {
//     int current = -1, destination = -1;
//     int numCities = sizeof(indianCities) / sizeof(indianCities[0]);
//     for (int i = 0; i < numCities; i++)
//     {
//         if (strcmp(currentCity, indianCities[i]) == 0)
//         {
//             current = i;
//         }
//         if (strcmp(destinationCity, indianCities[i]) == 0)
//         {
//             destination = i;
//         }
//     }
//     if (current != -1 && destination != -1)
//     {
//         return (abs(ticketPrices[current] - ticketPrices[destination]) * n);
//     }
//     return -1;
// }

int unique_id(){       //Automatically Generate Unique, Non-Repeating Ticket IDs #16 by Vasu
    static int counter = 0;
    time_t now = time(NULL);
    return ((int)(now % 1234) | counter++);
}

void addBooking()
{
    struct PartialBooking partial;
    bool resuming = false;
    char bookingReference[20]; // To store the generated booking reference number

    if (loadPartialBooking(&partial) && partial.inProgress)
    {
        printf("You have a partially completed booking. Do you want to resume? (1: Yes, 0: No): ");
        int choice;
        if (scanf("%d", &choice) != 1)
        {
            printf("Error: Invalid input. Starting a new booking.\n");
            clearInputBuffer();
            partial.inProgress = false;
            partial.stage = 0;
        }
        else if (choice == 1)
        {
            resuming = true;
        }
        else
        {
            partial.inProgress = false;
            partial.stage = 0;
        }
        clearInputBuffer();
    }
    else
    {
        partial.inProgress = true;
        partial.stage = 0;
    }

    if (!resuming || partial.stage == 0)
    {
        partial.booking.ticketID = unique_id();
        partial.stage = 1;
    }

    if (!resuming || partial.stage <= 1)
    {
        do
        {
            printf("Enter Name (alphabets and spaces only, or 'pause' to pause): ");
            if (fgets(partial.booking.name, MAX_NAME_LENGTH, stdin) == NULL)
            {
                printf("Error: Failed to read input. Please try again.\n");
                continue;
            }
            partial.booking.name[strcspn(partial.booking.name, "\n")] = 0; // Remove newline
            if (strcmp(partial.booking.name, "pause") == 0)
            {
                savePartialBooking(&partial);
                printf("Booking paused. You can resume later.\n");
                return;
            }
            if (!isValidName(partial.booking.name))
            {
                printf("Error: Invalid name. Please use only alphabets and spaces.\n");
                continue;
            }
            break;
        } while (1);
        partial.stage = 2;
    }

    if (!resuming || partial.stage <= 2)
    {
        int currentChoice;
        //Implement 'Current Location' and Traveler Count Input for Accurate Price Calculation #15
        printf("Enter your current location :\n");      //fixes#15
        for (int i = 0; i < numCities; i++)
        {
            printf("%d. %s\n", i + 1, indianCities[i]);
        }

        do
        {
            printf("Enter the  number of your current location (1-%d):, or 0 to pause ", numCities);
            if (scanf("%d", &currentChoice) != 1 || currentChoice < 0 || currentChoice > numCities)
            {
                printf("Error: Invalid choice. Please enter a number between 0 and %d.\n", numCities);
                clearInputBuffer();
                continue;
            }
            if (currentChoice == 0)
            {
                savePartialBooking(&partial);
                printf("Booking paused. You can resume later.\n");
                return;
            }
            break;
        } while (1);
        strcpy(partial.booking.currentLocation, indianCities[currentChoice - 1]);
        clearInputBuffer();

        int choice;
        printf("Select Destination:\n");
        for (int i = 0; i < numCities; i++)
        {
            printf("%d. %s\n", i + 1, indianCities[i]);
        }
        do
        {
            printf("Enter the number of your destination (1-%d), or 0 to pause: ", numCities);
            if (scanf("%d", &choice) != 1 || choice < 0 || choice > numCities)
            {
                printf("Error: Invalid choice. Please enter a number between 0 and %d.\n", numCities);
                clearInputBuffer();
                continue;
            }
            if (choice == 0)
            {
                savePartialBooking(&partial);
                printf("Booking paused. You can resume later.\n");
                return;
            }
            break;
        } while (1);

        int n;
        do
        {
            printf("Enter how many number of Travelers : ");
            if (scanf("%d", &n) != 1 || n <= 0)
            {
                printf("Invalid input. Please enter a valid number of travelers (greater than 0).\n");
                while (getchar() != '\n')
                    continue;
            }
            break;
        } while (1);

        strcpy(partial.booking.destination, indianCities[choice - 1]);

        clearInputBuffer();

         int categoryChoice;

        printf("Select Ticket Category:\n");
       for (int i = 0; i < sizeof(ticketCategories)/sizeof(ticketCategories[0]); i++) {
            printf("%d. %s\n", i + 1, ticketCategories[i]);
        }

        do {
            printf("Enter the number of your selected category (1-%d): ", sizeof(ticketCategories)/sizeof(ticketCategories[0]));

            if (scanf("%d", &categoryChoice) != 1 || categoryChoice < 1 || categoryChoice > sizeof(ticketCategories)/sizeof(ticketCategories[0])) {
                printf("Error: Invalid choice. Please enter a number between 1 and %ld.\n", sizeof(ticketCategories)/sizeof(ticketCategories[0]));
                clearInputBuffer();
                continue;
            }
            break;

        } while(1);

        strcpy(partial.booking.category, ticketCategories[categoryChoice - 1]); // Store selected category

        partial.stage =3;

        // Calculate price based on selected category
        int categoryIndex = categoryChoice - 1; 

        partial.booking.price = getPriceForCity(partial.booking.currentLocation,
                                                 partial.booking.destination,
                                                 n,
                                                 categoryIndex);

        if (partial.booking.price != -1)
            printf("The price from %s to %s for %d traveler(s): Rs. %d\n",
                   partial.booking.currentLocation,
                   partial.booking.destination,
                   n,
                   partial.booking.price);
        else
            printf("Error: Could not find price for the selected city.\n");
   }
        
    //     partial.stage = 3;
    //     partial.booking.price = getPriceForCity(partial.booking.currentLocation, partial.booking.destination, n);
    //     printf("Price for the trip is : %d\n", partial.booking.price);
    //     if (partial.booking.price != -1)
    //     {
    //         printf("The price from %s to %s is Rs. %d\n", partial.booking.currentLocation, partial.booking.destination, partial.booking.price);
    //     }
    //     else
    //     {
    //         printf("Error: Could not find price for the selected city.\n");
    //     }
    // }

    // Finalize booking
      // Finalize booking
FILE *file = fopen(FILENAME, "ab");
if (file == NULL)
{
    printf("Error: Unable to open bookings file. Booking not saved.\n");
    return;
}

if (fwrite(&partial.booking, sizeof(struct Booking), 1, file) != 1)
{
    printf("Error: Failed to write booking data. Please try again.\n");
}
else
{
    // Generate and display booking receipt
    generateReferenceNumber(bookingReference, partial.booking.ticketID);
    printf("Booking added successfully!\n");
    printf("\nReceipt:\n");
    printf("Booking Reference: %s\n", bookingReference);
    printf("Ticket ID: %d\n", partial.booking.ticketID);
    printf("Name: %s\n", partial.booking.name);
    printf("Current Location: %s\n", partial.booking.currentLocation);
    printf("Destination: %s\n", partial.booking.destination);
    printf("Category: %s\n", partial.booking.category);
    printf("Price: Rs. %d\n", partial.booking.price);
}
fclose(file);

    // Clear partial booking
    partial.inProgress = false;
    savePartialBooking(&partial);
    remove(PROGRESS_FILENAME); 
    /* FILE *file = fopen(FILENAME, "ab");
    if (file == NULL)
    {
        printf("Error: Unable to open bookings file. Booking not saved.\n");
        return;
    }

    if (fwrite(&partial.booking, sizeof(struct Booking), 1, file) != 1)
    {
        printf("Error: Failed to write booking data. Please try again.\n");
    }
    else
    {
        // Generate and display booking receipt
        generateReferenceNumber(bookingReference, partial.booking.ticketID);
        printf("Booking added successfully!\n");
        printf("\nReceipt:\n");
        printf("Booking Reference: %s\n", bookingReference);
        printf("Ticket ID: %d\n", partial.booking.ticketID);
        printf("Name: %s\n", partial.booking.name);
        printf("currentLocation: %s\n",partial.booking.currentLocation);
        printf("Destination: %s\n", partial.booking.destination);
        printf("Category: %s\n", partial.booking.category);
        printf("Price: Rs. %d\n", partial.booking.price);
    }
    fclose(file);

    // Clear partial booking
    partial.inProgress = false;
    savePartialBooking(&partial);
    remove(PROGRESS_FILENAME); // Remove the progress file after successful booking
}
*/
void displayBookings()
{
    struct Booking booking;
    FILE *file = fopen(FILENAME, "rb");

    if (file == NULL)
    {
        printf("No bookings found or error opening file.\n");
        return;
    }

    printf("\n%-10s %-20s %-20s %-20s %s\n", "Ticket ID", "Name", "Current Location", "Destination", "Price");
    printf("--------------------------------------------------------------------------------------------------\n");

    while (fread(&booking, sizeof(struct Booking), 1, file) == 1)
    {
        printf("%-10d %-20s %-20s %-20s Rs.%d\n",
               booking.ticketID, booking.name, booking.currentLocation, booking.destination, booking.price);
    }

    if (ferror(file))
    {
        printf("Error occurred while reading the file.\n");
    }

    fclose(file);
    // struct Booking booking;
    // FILE *file = fopen(FILENAME, "rb");

    // if (file == NULL)
    // {
    //     printf("No bookings found or error opening file.\n");
    //     return;
    // }

    // printf("\n%-10s %-20s %-20s %s\n", "Ticket ID", "Name", "Destination", "Price");
    // printf("----------------------------------------------------------\n");

    // while (fread(&booking, sizeof(struct Booking), 1, file) == 1)
    // {
    //     printf("%-10d %-20s %-20s Rs.%d\n",
    //            booking.ticketID, booking.name, booking.destination, booking.price);
    // }

    // if (ferror(file))
    // {
    //     printf("Error occurred while reading the file.\n");
    // }
    // else if (feof(file))
    // {
    //     if (ftell(file) == 0)
    //     {
    //         printf("No bookings found.\n");
    //     }
    // }

    // fclose(file);
    
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
                printf("Price: Rs. %.2f\n", booking.price);
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
                printf("Price: Rs. %.2f\n", booking.price);
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
int main()
{
    int choice;
    struct PartialBooking partial;

    while (1)
    {
        printf("\nTicket Booking System\n");
        printf("1. Add/Resume Booking\n");
        printf("2. Display Bookings\n");
        printf("3. Save Progress and Exit\n");
        printf("4. Exit without Saving\n");
        printf("5. Search Bookings\n");
        printf("Enter your choice: ");
        if (scanf("%d", &choice) != 1)
        {
            printf("Error: Invalid input. Please enter a number.\n");
            clearInputBuffer();
            continue;
        }
        clearInputBuffer();

        switch (choice)
        {
        case 1:
            addBooking();
            break;
        case 2:
            displayBookings();
            break;
        case 3:
            if (loadPartialBooking(&partial) && partial.inProgress)
            {
                saveBookingProgress(&partial);
                printf("Progress saved. Goodbye!\n");
            }
            else
            {
                printf("No booking in progress to save. Goodbye!\n");
            }
            exit(0);
        case 4:
            printf("Exiting without saving. Goodbye!\n");
            exit(0);
        case 5:
            searchBookings();
            break;
        default:
            printf("Invalid choice. Please try again.\n");
        }
    }

    return 0;
}
