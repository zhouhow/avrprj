/*###########################################################
### File Name: unopro.c
### Author: zzh
### Mail: zhouhowellpsn@outlook.com
### Created Time: 2023-01-14
###########################################################*/

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>

#define SIZE 0x8000


int main(int argc, char *argv[])
{
  /* open port and get fd */
  if(argc != 3) 
  {
    printf(
      "Command line tool for unoPro !\n"
      "Usage: unoPro [baud] [port] !\n"
      "Optional BaudRate is 9600, 19200 and 38400 !\n"
    );
    return 1;
  }

  /* port and baud */
  char *port = argv[2];
  char *baud = argv[1];
  
  /* specify baudrate */
  long int speed = strtol(baud, NULL, 0);	
  if(!(speed == 9600 || speed == 19200 || speed == 38400))
  {
    printf("\033[1;33mBaudRate %s is not available !\033[0m\n", baud);
    printf("Optional BaudRate is 9600, 19200 and 38400 !\n");
    return 1;
  }

  /* try to connect */
  int fd = plug(speed, port);

  if(fd == -1)
  { /* can't open port */
    printf("\033[1;31mCan't open %s !\033[0m\n", port);
    return 1;
  }
  /* success open port */
  else  
  {
    printf("Success open %s, BaudRate is %ld !\n", port, speed);
    printf("Please type 'help' for usage\n");
  }
  
  /* command line and argument */
  char cli[32];
  char *ptr = cli;
  char arg[16][16];
  char *cmdv[] = { "dump", "write", "erase", "load", "help", "exit"};

  /* get command and argument */ 
  char *ps1 = ">>> ";
  printf("%s", ps1);
  gets(cli); 


  /* quit command */
  while(strcmp(cli, cmdv[5]) != 0)
  { 
    /* input char num */
    int num = strchr(cli, '\0')-cli;

    /* nothing */ 
    if(num == 0)
    {
      printf
      ( "\033[1;33mNothing input at all !\033[0m\n"
        "Please type 'help' for usage !\n"
      );
    }
    else
    { 
      /* get command and argument */
      int cmdc = 0;
      char seps[] = " ";

      char *tmp = strtok_r(cli, seps, &ptr);
      while(tmp != NULL)
      {
        strcpy(arg[cmdc], tmp);
        cmdc++;
        tmp = strtok_r(NULL, seps, &ptr);
      }
      
      if(cmdc != 0)
      { 
        /* get cmd index */
        int index = -1;
        index = getcmd(cmdv, arg[0]);

        if(index != -1)
        { 
          if(index == 4)
          { /* help command */
            if(cmdc == 1) usage();
            else printf("Do you mean 'help' ?\n");
          }
          else
          {
            if(cmdc == 3)
            { 
              /* address must like 01f0 */
              int addr = hex2int(arg[1]);
              int orgc = strchr(arg[1], '\0')-arg[1];
              
              if(orgc != 4 || addr == -1)
              { 
                printf("\033[1;33mAddress should be Hex without prefix, like 0100 !\033[0m\n");
                printf("However, your address: %s !\033[0m\n", arg[1]);
              }
              else process(fd, index, arg);
            }
            else
            { /* give example usage */
              printf("Do you mean '%s' ?\n", cmdv[index]);
              example(index);
            }

          }

        }
        else
        { /* unknown command */
          printf("\033[1;33m%s ?\033[0m\n", arg[0]);
          printf("Please type 'help' for usage !\n");
        }

        index = -1;
      }

      cmdc = 0;

    }

    /* get command ang argument */      
    printf("%s", ps1);
    gets(cli); 

  }
  
  /* close port */
  close(fd);
  return 0;

}


/* initial serial port */
int plug(long int baud, char *port)
{
  int fd = open(port, O_RDWR);

  /* success open port */
  if(fd != -1)
  { 
    /* get current port settings */
    struct termios ttyopt;
    tcgetattr(fd, &ttyopt);

    /* change port settings */
    ttyopt.c_cflag = 2237;
    ttyopt.c_iflag = 0;
    ttyopt.c_lflag = 0;
    ttyopt.c_oflag = 0;
    ttyopt.c_ospeed = 13;
    ttyopt.c_ispeed = 13;
    ttyopt.c_line = 0;
    
    /* baud setting */
    long int speed[] = {9600, 19200, 38400};
    int rate[] = {B9600, B19200, B38400};
    for(int j=0; j<3; j++)
    {
      if(baud == speed[j]) cfsetispeed(&ttyopt, rate[j]);
    }

    /* commit settings after change*/
    tcsetattr(fd, TCSANOW, &ttyopt);
  }

  return fd;
}


/* get command index */
int getcmd(char *cmdv[], char *cmd)
{

  for(int i=0; i<5; i++)
  {
    /* return command index */
    if(strcmp(cmd, cmdv[i]) == 0) return i;
  }
  /* unknown command */
  return -1;
}


/* example usage */
void example(int index)
{
  printf("Examples: ");
  if(index == 0) printf("dump 0100 02ff    Read 0x02ff bytes from 0x0100\n");
  else if(index == 1) printf("write 01f0 2ff635 Write 2fh,f6h,35h from 0x01f0\n");
  else if(index == 2) printf("erase 02f0 0100   Erase 0x0100 bytes from 0x02f0\n");
  else /* index == 3*/printf("load 01ff a.bin   Write file a.bin  from 0x01ff\n");
}


/* print help page */
void usage(void)
{
  printf
  ( "dump   [org]  [size]     Read some bytes from address\n"
    "write  [org]  [data]     Write some bytes to address\n"  
    "erase  [org]  [size]     Erase some bytes from address\n"
    "load   [org]  [file]     Write binary file to address\n"  
    "help                     Display this help page !\n"
    "exit                     Exit from this command tool\n"
  );
}


/* process command */
int process(int fd, int index, char arg[16][16])
{
  /* address and num */
  int org = hex2int(arg[1]);
  int data = hex2int(arg[2]);
  int arg2c = strchr(arg[2], '\0')-arg[2];

  if(index != 3 && data == -1)
  {
    printf("\033[1;33mSize and Data should be Hex without prefix, like 01f0, 01f3 !\033[0m\n");
    return 1;
  }
  
  /* dump command */
  if(index == 0)
  {
    /* read 16 times bytes once */
    int size = (data%16 != 0 ? (data/16 + 1)*16 : data);
    if(arg2c == 4 && size != 0 && size+org <= SIZE) dump(fd, arg[1], arg[2]);
    else
    {
      if(size+org > SIZE)
      {
        printf("\033[1;33m%sh + %sh > 8000h !\033[0m\n", arg[1], arg[2]);
        printf("Addresss is 0x0000 ~ 0x7fff, specify your argment again!\n");
      }
      if(arg2c != 4 || size == 0)
      {
        printf("\033[1;33mSize should be Hex without prefix and more than 0000, like 0100, 01f0 !\033[0m\n");
        printf("\033[1;33mHowever, your size: %s !\033[0m\n", arg[2]);
      }
    }
  }
  /* write command */
  else if(index == 1) 
  {
    int num = arg2c/2;

    if(num%2 == 0 && num != 0 && num+org <= SIZE)
    {
      mywrite(fd, arg[1], arg[2]);

      /* sleep for wrtite */
      for(int cur=1; cur<=num; cur++)
      {
      usleep(20*1000);
      printf("Total: %d Bytes, Written: %d Bytes\r", num, cur);
		  fflush(stdout);
      }
      printf("\n");
    }
    else
    {
      if(num+org > SIZE)
      {
        printf("\033[1;33m%sh + %dd > 8000h !\033[0m\n", arg[1], num);
        printf("Addresss is 0x0000 ~ 0x7fff, specify your argument again!\n");
      }

      if(num%2 != 0 || num == 0)
      {
        printf("\033[1;33mDate should be Even and  Hex without prefix, like 010001f0 !\033[0m\n");
        printf("\033[1;33mHowever, your data:%s !\033[0m\n", arg[2]);
      }
    }
  }
  /* erase command */
  else if(index == 2)
  {
    if(arg2c == 4 && data+org <= SIZE) erase(fd, arg[1], arg[2]);
    else
    {
      if(data+org > SIZE)
      {
        printf("\033[1;33m%sh + %sh > 8000h !\033[0m\n", arg[1], arg[2]);
        printf("Addresss is 0x0000 ~ 0x7fff, specify your argument again!\n");
      }
      if(arg2c != 4)
      {
        printf("\033[1;33mSize should be Hex without prefix, like 01f0 !\033[0m\n");
        printf("\033[1;33mHowever, your size:%s !\033[0m\n", arg[2]);
      }
    }
  }
  /* load command */
  else if(index == 3) 
  {
    FILE *file = fopen(arg[2], "rb");
    if(!file) printf("\033[1;33mFail to open %s !\033[0m\n", arg[2]);
    else
    {
      fseek(file, SEEK_SET, SEEK_END);
      int len  = ftell(file);

      if(len+org > SIZE)
      {
        printf("\033[1;33m%sh + %04x > 8000h !\033[0m\n", arg[1], len);
         printf("Addresss is 0x0000 ~ 0x7fff, specify your argument again!\n");
      }
      else load(fd, arg[1], file);
      /* close file */
      fclose(file);
    }
  } 
  else
  { /* argument error */    
    printf("\033[1;33mArgument specify error !\033[0m\n");
    printf("Please type 'help' for usage !\n");
  }
  return 0;          
}


/* convert hex to int */
/* input:07ff, output:2047 */
int hex2int(char *hex)
{
  int sum = 0;
  int num = strchr(hex, '\0')-hex;

	for(int j=0; j<num; j++)
	{
    if (hex[j] >= '0' && hex[j] <= '9') sum = 0;
    else if (hex[j] >= 'a' && hex[j] <= 'f') sum = 0;
    else if (hex[j] >= 'A' && hex[j] <= 'F') sum = 0; 
    /* some wrong char */
    else return -1;
	}
	
  char str[] = "0xffff";
  for(int j=0; j<4; j++) str[j+2] = hex[j];
  sum = strtol(str, NULL, 0);	

	return sum;
}


/* read some bytes */
/* org:0100, size:0030 */
void dump(int fd, char *org, char *size)
{
  char msg[128];
  char buf[256] = {'\0'};

  /* specify read operation */
  msg[0] = '0';
  msg[1] = '0';
  msg[10] = '\n';

  /* specify address and size */
  for(int i=0; i<4; i++)
  {
    msg[2+i] = org[i];
    msg[6+i] = size[i];
  }
  
  /* send message to device */
  write(fd, msg, 11);

  /* read some bytes */
  int num = read(fd, buf, sizeof(buf));
  int out = getout(buf);

  while(num > 0 && out == 0)
  {
    printf("%s", buf);
    for(int i=0; i<256; i++) buf[i] = '\0';
    num = read(fd, buf, sizeof(buf));
    out = getout(buf);
  }

  printf("%s", buf);
  printf("\n");
  
  /* flush serial buffer */
  tcflush(fd, TCIFLUSH);
  for(int i=0; i<256; i++) buf[i] = '\0';
  
}


/* exit from dump loop */
int getout(char *str)
{
	for(int j=0; j<4; j++)
	{
    if (str[j] == '!') return -1;
	}
	return 0;
}


/* erase some bytes */
void erase(int fd, char *org, char *size)
{
  char msg[128];

  /* specify erase operation */
  msg[0] = '0';
  msg[1] = 'e';
  msg[10] = '\n';

  /* specify address and size */
  for(int i=0; i<4; i++)
  {
    msg[2+i] = org[i];
    msg[6+i] = size[i];
  }
  
  /* send message to device */
  write(fd, msg, 11);

  /* sleep for erase */
  int sum = hex2int(size);

  for(int cur=0; cur<sum; cur++)
  {
    usleep(20*1000);
    printf("Total: %d Bytes, Erased: %d Bytes\r", sum, cur+1);
		fflush(stdout);
  }
  printf("\n");

}


/* write some bytes */
/* data: 2345 --> 0x23h, 0x45h */
void mywrite(int fd, char *org, char *data)
{
  char msg[128];

  /* specify write operation */
  msg[0] = '0';
  msg[1] = '1';
  /* num = 2 * bytes */
  int num = strchr(data, '\0')-data;
  msg[6+num] = '\n';
  
  /* specify address */
  for(int i=0; i<4; i++)
  {
    msg[2+i] = org[i];
  }

  /* specify data */
  for(int i=0; i<num; i++)
  {
    msg[6+i] = data[i];
  }

  /* send message to device */
  write(fd, msg, num+7);

}


/* write binary file */
void load(int fd, char *org, FILE *file)
{
  /* get file size */
  fseek(file, SEEK_SET, SEEK_END);
  int len  = ftell(file);
  /* seek file beginning */
	fseek(file, SEEK_SET, SEEK_SET );

	/* address and data */
  int bin;
  int sum = 0;
  char tmp[4];
  char data[96] = {'\0'};
  int addr0 = hex2int(org);
  
  /* write 48 bytes once */
  for(int addr=0; addr<len; addr += 48)
	{
		for(int k=0; k<48; k++)
		{
      if((bin = fgetc(file)) != EOF)
			{
				sprintf(tmp, "%02x", bin);
				data[2*k] = tmp[0];
				data[2*k+1] = tmp[1];
        /* real bytes num */
        sum += 1;
			}
		}

		/* current address and org */
		sprintf(org, "%04x", addr + addr0);

		/* write some bytes */
    mywrite(fd, org, data);

		/* clear data */
		for(int j=0; j< 96; j++)
		{
			data[j] = '\0';
		}

    /* sleep for wrtite */
    for(int cur=0; cur<=sum; cur++)
    {
      usleep(20*1000);
      printf("Total: %d Bytes, Burned: %d Bytes\r", len, cur+addr);
		  fflush(stdout);
    }
    sum = 0;

	}
  printf("\n");

}
