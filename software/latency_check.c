#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <time.h>
#include <errno.h>

#define SERIAL_PORT "/dev/ttyS0"
#define BAUD_RATE B115200
#define NUM_TESTS 10

double current_time_ms() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000.0 + ts.tv_nsec / 1000000.0;
}

int main() {
    int serial_fd = open(SERIAL_PORT, O_RDWR | O_NOCTTY);
    if (serial_fd == -1) {
        perror("Failed to open serial port");
        return 1;
    }

    struct termios options;
    tcgetattr(serial_fd, &options);

    // Configure baud rate
    cfsetispeed(&options, BAUD_RATE);
    cfsetospeed(&options, BAUD_RATE);

    options.c_cflag |= (CLOCAL | CREAD);    // Enable receiver, set local mode
    options.c_cflag &= ~PARENB;             // No parity
    options.c_cflag &= ~CSTOPB;             // 1 stop bit
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;                 // 8 data bits
    options.c_cflag &= ~CRTSCTS;            // Disable hardware flow control

    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // Raw input
    options.c_iflag &= ~(IXON | IXOFF | IXANY);         // Disable software flow control
    options.c_oflag &= ~OPOST;                          // Raw output

    options.c_cc[VMIN] = 0;
    options.c_cc[VTIME] = 10;  // 1 second timeout

    tcflush(serial_fd, TCIOFLUSH);         // Clear input/output buffers
    tcsetattr(serial_fd, TCSANOW, &options);

    for (int i = 0; i < NUM_TESTS; i++) {
        char test_data[16];
        snprintf(test_data, sizeof(test_data), "ping%d", i);
        size_t data_len = strlen(test_data);

        // Flush input buffer to remove leftover data
        tcflush(serial_fd, TCIFLUSH);

        double start = current_time_ms();

        // Write data
        ssize_t bytes_written = write(serial_fd, test_data, data_len);
        if (bytes_written != data_len) {
            perror("Failed to write complete data");
            continue;
        }

        // Wait briefly to allow echo to come back (optional)
        //usleep(50000);  // 50ms

        // Read response
        char buffer[16] = {0};
        ssize_t total_read = 0;
        while (total_read < data_len) {
            ssize_t n = read(serial_fd, buffer + total_read, data_len - total_read);
            if (n > 0) {
                total_read += n;
            } else if (n == 0) {
                // Timeout
                break;
            } else {
                perror("Read error");
                break;
            }
        }

        double end = current_time_ms();

        if (total_read == data_len && memcmp(test_data, buffer, data_len) == 0) {
            printf("Test %d: Latency = %.2f ms\n", i, end - start);
        } else {
            printf("Test %d: Mismatch or Timeout\n", i);
            printf("  Sent:    '%s'\n", test_data);
            printf("  Received:'%.*s'\n", (int)total_read, buffer);
        }
    }

    close(serial_fd);
    return 0;
}
