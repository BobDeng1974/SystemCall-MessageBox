#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "arch/x86/include/generated/uapi/asm/unistd_64.h"

int test1(void);
int test2(void);
int test3(void);
int test4(void);
int test5(void);

int main(int argc, char** argv) {

  int res = 0;
  int choice;

  printf("Which test would you like to run? \n");
  scanf(" %i", &choice);
  printf("Running test %i.\n", choice);

  if(choice == 1) {
	res = test1();
  }

  if(choice == 2) {
	res = test2();
  }

  if(choice == 3) {
	res = test3();
  }

  if(choice == 4) {
	res = test4();
  }

  if(choice == 5) {
	res = test5();
  }

return res;
}

int test1(void){
  char *in = "This is a stupid message.";

  printf("This test will attempt to create a message in which the length is incorrectly specified as a negative value. \n");

  /* Send a message containing 'in' */
  int result = syscall(__NR_msgbox_put, in, -1);
 
  return result;
}

int test2(void){
  char *in = NULL;

  printf("This test will attempt to create a message, passing a NULL value as the buffer parameter \n");

  /* Send a message containing 'in' */
  int result = syscall(__NR_msgbox_put, in, 50);
 
  return result;
}

int test3(void){
  char *in = "This is a stupid message.";
  char msg = NULL;

  printf("This test will attempt to read a message, passing a NULL value as the buffer parameter \n");

  /* Send a message containing 'in' */
  syscall(__NR_msgbox_put, in, strlen(in)+1);

  /* Read a message */
  int result = syscall(__NR_msgbox_get, msg, 50);
 
  return result;
}

int test4(void){
  char *in = "This is a stupid message.";
  char msg[50];

  printf("This test will attempt to read a message, passing a value smaller than the length of the message on the stack as the length parameter \n");

  /* Send a message containing 'in' */
  syscall(__NR_msgbox_put, in, strlen(in)+1);

  /* Read a message */
  int result = syscall(__NR_msgbox_get, msg, 1);
 
  return result;
}

int test5(void){
  char *in = "This is a stupid message.";
  char msg[50];
  int msglen;

  printf("This test will simply attempt to create a message, and then read that message off of the stack, using acceptable data as parameters. \n");

  /* Send a message containing 'in' */
  syscall(__NR_msgbox_put, in, strlen(in)+1);
 
  /* Read a message */
  msglen = syscall(__NR_msgbox_get, msg, 50);
 
  return 0;
}

