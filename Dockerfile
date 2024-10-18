# Use an Alpine image with GCC installed
FROM alpine:latest

# Install build dependencies (GCC and build utilities)
RUN apk add --no-cache gcc musl-dev

# Set the working directory inside the container
WORKDIR /app

# Copy the C source file into the container
COPY . /app

# Compile the C program
RUN gcc -o booking booking.c

# Run the compiled binary as the default command
CMD ["./booking"]
