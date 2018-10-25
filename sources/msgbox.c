#include "uapi/asm-generic/errno.h"
#include "uapi/asm-generic/errno-base.h"
#include <linux/slab.h>

typedef struct _msg_t msg_t;

struct _msg_t{
  msg_t* previous;
  int length;
  char* message;
};

static msg_t *bottom = NULL;
static msg_t *top = NULL;

unsigned long flags;

int sys_msgbox_put( char *buffer, int length ) {
  printk("msgboxPut start.  \n");
  local_irq_save(flags);
  msg_t* msg = kmalloc(sizeof(msg_t), GFP_KERNEL);
  if (!msg) {
        printk("Unable to allocate memory for message! Out of memory!\n");
        printk("errno = -12 ENOMEM.\n");
        local_irq_restore(flags);
        return(-12);
  }
  msg->previous = NULL;
  msg->length = length;
  msg->message = kmalloc(length, GFP_KERNEL);
  if (!msg->message) {
        printk("Unable to allocate memory! Out of memory!\n");
        printk("errno = -12 ENOMEM.\n");
        local_irq_restore(flags);
        return(-12);
  }

  printk("msgboxPut BEGIN COPY.  \n");
  if(buffer == NULL){
    printk("Buffer is NULL! Bad address!\n");
    printk("errno = -14 EFAULT.\n");
          local_irq_restore(flags);
    return(-14);
  }
  ssize_t res;
  res = copy_from_user(msg->message, buffer, length);
  if(res) {
    printk("Whole message not copied! Bad address!   %d bytes not copied \n", res);
    printk("errno = -14 EFAULT.\n");
    local_irq_restore(flags);
    return(-14);
  }
  printk("Message copied. THE MESSAGE IS:  %s \n", msg->message);

  if (bottom == NULL) {
    bottom = msg;
    top = msg;
  } else {
    /* not empty stack */
    msg->previous = top;
    top = msg;
  }
  printk("msgboxPut SUCCESS.  \n");
  local_irq_restore(flags);
  return 0;
}

int sys_msgbox_get( char* buffer, int length ) {
  printk("msgboxGet start.  \n");
  if (top != NULL) {
    local_irq_save(flags);
    msg_t* msg = top;
    int mlength = msg->length;
    top = msg->previous;

    /* copy message */
    printk("The top message is:  %s \n", msg->message);
    printk("msgboxGet BEGIN COPY.  \n");
    if(buffer == NULL) {
        printk("Buffer is NULL! Bad address!\n");
        printk("errno = -14 EFAULT.\n");
        local_irq_restore(flags);
        return(-14);
    }
    if(msg == NULL||msg->message == NULL) {
        printk("Message is NULL! Bad address!\n");
        printk("errno = -14 EFAULT.\n");
        local_irq_restore(flags);
        return(-14);
    }
    if(length < mlength) {
        printk("Buffer is smaller than message! Invalid argument!\n");
        printk("errno = -22 EINVAL.\n");
        local_irq_restore(flags);
        return(-22);
    }

    copy_to_user(buffer, msg->message, mlength);

    /* free memory */
    kfree(msg->message);
    kfree(msg);

    printk("msgboxGet SUCCESS.  \n");
    local_irq_restore(flags);
    printk("msgboxGet local_irq_restore complete.  mlength= %d \n",mlength);
    return mlength;
  }
  return -1;
}