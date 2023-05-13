#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>

int main() {
   int fd = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_SYNC);
   if (fd < 0) {
      perror("open");
      return 1;
   }

   struct termios tty;
   memset(&tty, 0, sizeof(tty));
   if (tcgetattr(fd, &tty) != 0) {
      perror("tcgetattr");
      return 1;
   }

   cfsetospeed(&tty, B115200);
   cfsetispeed(&tty, B115200);

   tty.c_cflag |= (CLOCAL | CREAD);
   tty.c_cflag &= ~CSIZE;
   tty.c_cflag |= CS8;
   tty.c_cflag &= ~PARENB;
   tty.c_cflag &= ~CSTOPB;
   tty.c_cflag &= ~CRTSCTS;

   if (tcsetattr(fd, TCSANOW, &tty) != 0) {
      perror("tcsetattr");
      return 1;
   }

   FILE *fp = fopen("received_data.txt", "w+");
   if (fp == NULL) {
      perror("fopen");
      return 1;
   }

   while (1) {
      char buf[256];
      int n = read(fd, buf, sizeof(buf));
      if (n > 0) {
         // Convert each ASCII character to an integer
         for (int i = 0; i < n; i++) {
            int val = (int)(buf[i]);
            fprintf(fp, "%d\n", val);
         }
         fflush(fp);
      }
   }

   fclose(fp);
   close(fd);

   return 0;
}

