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
   gcc -o ticket_booking_system ticket_booking_system.c
   ```

## Usage
1. Run the compiled program:
   ```bash
   ./ticket_booking_system
   ```

2. Follow the on-screen menu to add bookings or display existing ones.

## Code Structure
- ticket_booking_system.c: The main source file containing the implementation of the ticket booking system.
- bookings.dat: A binary file used for storing booking details (automatically created/updated by the program).


# Contribution Guidelines 📚

Welcome to Club Gamma project! We appreciate your interest and contributions. Please read the following guidelines carefully to ensure a smooth collaboration.

## General Rules:

- **Creativity Allowed:** Feel free to submit pull requests that break the rules—we might just merge them anyway!
- **No Build Steps:** Avoid adding build steps like `npm install` to maintain simplicity as a static site.
- **Preserve Existing Content:** Do not remove existing content.
- **Code Style:** Your code can be neat, messy, simple, or complex. As long as it works, it's welcome.
- **Add Your Name:** Remember to add your name to the `contributorsList` file.
- **Keep it Small:** Aim for small pull requests to minimize merge conflicts and streamline reviews.

## Getting Started 🤗🚀

1. **Fork the Repository:**
   - Use the fork button at the top right of the repository page.

2. **Clone Your Fork:**
   - Clone the forked repository to your local machine.

   ```bash
   git clone https://github.com/clubgamma/Internet-Speed-Tester.git
   ```

3. **Navigate to the Project Directory:**

   ```bash
   cd Internet-Speed-Tester
   ```

4. **Create a New Branch:**

   ```bash
   git checkout -b my-new-branch
   ```

5. **Make Your Changes:**
  
   ```bash
   git add .
   ```

6. **Commit Your Changes:**

   ```bash
   git commit -m "Relevant message"
   ```

7. **Push to Your Branch:**

   ```bash
   git push origin my-new-branch
   ```

8. **Create a Pull Request:**
   - Go to your forked repository on GitHub and create a pull request to the main repository.

## Avoiding Conflicts {Syncing Your Fork}

To keep your fork up-to-date with the main repository and avoid conflicts:

1. **Add Upstream Remote:**

   ```bash
   git remote add upstream https://github.com/clubgamma/Sudoku.git
   ```

2. **Verify the New Remote:**

   ```bash
   git remote -v
   ```

3. **Sync Your Fork with Upstream:**

   ```bash
   git fetch upstream
   git merge upstream/main
   ```

   This will pull in changes from the parent repository and help you resolve any conflicts.

4. **Keep Updated:**
   - Regularly pull changes from the upstream repository to keep your fork updated.


###  Add your name
Add your name to the `CONTRIBUTING.md` file using the below convention:

```markdown
#### Name: [YOUR NAME](GitHub link)
- Place: City, State, Country
- Bio: Who are you?
- GitHub: [GitHub account name](GitHub link)
```

We look forward to your contributions and thank you for being a part of our community!
  
