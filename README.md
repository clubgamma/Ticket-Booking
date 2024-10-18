# Ticket Booking System

A simple ticket booking system implemented in C that allows users to add and display bookings. The system saves the booking details to a binary file for persistent storage.


## Table of Contents

- [Features](#features)
- [Requirements](#requirements)
- [Installation](#installation)
- [Usage](#usage)
- [Code Structure](#code-structure)
- [Contribution Guidelines](#contribution-guidelines-)
- [General Rules](#general-rules)
- [Getting Started](#getting-started-)
- [Avoiding Conflicts](#avoiding-conflicts-syncing-your-fork)


## Features

- **Add Booking**: Users can enter details for new bookings, including ticket ID, name, destination, and price.
- **Display Bookings**: Users can view all the current bookings in a formatted table.
- **File Storage**: Booking details are saved to and loaded from a binary file (`bookings.dat`), ensuring data persistence between sessions.
- **User-Friendly Menu**: An interactive menu allows users to easily navigate between adding bookings and displaying them.

## Requirements

- C Compiler (e.g., GCC)
- Basic understanding of C programming

## Installation

1. Clone the repository:

   ```bash
   git clone https://github.com/your-username/ticket-booking-system.git
   cd ticket-booking-system
   ```

2. Compile the program:
   ```bash
   gcc -o booking booking.c
   ```

## Usage
1. Run the compiled program:
   ```bash
   ./booking
   ```

2. Follow the on-screen menu to add bookings or display existing ones.


## Usage with Docker

1. Build the Docker image:

   ```bash
   docker build -t ticket-booking-system .
   ```

2. Run the Docker container:

   ```bash
   docker run -it ticket-booking-system
   ```
   

## Code Structure
- ticket_booking_system.c: The main source file containing the implementation of the ticket booking system.
- bookings.dat: A binary file used for storing booking details (automatically created/updated by the program).

  
