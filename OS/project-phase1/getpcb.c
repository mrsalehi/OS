#include <linux/init.h>
#include <linux/types.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/rcupdate.h>
#include <linux/dcache.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/kernel.h>
#include <linux/fdtable.h>
#include <linux/fs.h>
#include <linux/fs_struct.h>
#include <linux/time.h>
#include <linux/sched/task.h>
#define DEVICE_NAME "getpcb"
#define MSG_BUFFER_LEN 15

MODULE_LICENSE("GPL");

static int major_num;
static char msg_buffer[MSG_BUFFER_LEN];
static char *msg_ptr;


static int device_open(struct inode *, struct file *);

static int device_release(struct inode *, struct file *);

static ssize_t device_read(struct file *, char *, size_t, loff_t *);

static ssize_t device_write(struct file *, const char *, size_t, loff_t *);
static int device_open_count = 0;
static int pid;
static long state;

// Used for getting the PCB of process as a struct
struct task_struct *t;

/* Variables used to get open files 

Code from:
	https://tuxthink.blogspot.com/2012/05/module-to-print-open-files-of-process.html 
*/
struct files_struct *current_files;
struct fdtable *files_table;
unsigned int *fds;
int i=0;
struct path files_path;
char *cwd;


/* Variables used to get time and real_start_time */
//struct timespec start_time;  // Used for monotonic time
//struct timespec real_start_time;  // Used for boot based time
u64 start_time;
u64 real_start_time;


static struct file_operations file_ops = {
	.read = device_read,
	.write = device_write,
	.open = device_open,
	.release = device_release
};


/* Converts a string to integer */
int a2i(char* s) {
	
	int sign = 1;
	if(*s == '-'){
		sign = -1;
		s ++;
	}

	int num = 0;
	while (*s) {
		num = ( (*s) - '0') + num * 10;
		s++; 
	}

	return num * sign;
}


/* Prints state of the process */
void print_state(void){
	printk(KERN_INFO "State of the process %d: %ld", t->pid, t->state);
}


/* Prints the list of open files in the process */
void print_open_files(void){
	char *buf =(char *) kmalloc(GFP_KERNEL,100*sizeof(char)); 

	current_files = t->files; /* gets open files */
	files_table = files_fdtable(current_files);
	printk(KERN_ALERT "Open files:");
 	
	while(files_table->fd[i] != NULL) { 
 		files_path = files_table->fd[i]->f_path;
 		cwd = d_path(&files_path,buf,100*sizeof(char));
 		printk(KERN_ALERT "Open file with fd %d  %s", i, cwd);
 		i++;
 	}
}


/* Prints start_time and ral_start_time */
void print_time(void){
	
	/*start_time = t->start_time;
	real_start_time = t->real_start_time; */
	
	/* https://stackoverflow.com/questions/8304259/formatting-struct-timespec */
/*
	printk(KERN_INFO "Start time: %lld.%.9ld", (long long)start_time.tv_sec, start_time.tv_nsec);
	printk(KERN_INFO "Real Start time: %lld.%.9ld", (long long)real_start_time.tv_sec, real_start_time.tv_nsec); */
}


/*  Prints context switch counts: nvcsw, nivcsw
	Read about the difference between these counts at:
	https://stackoverflow.com/questions/61530040/what-are-nivcsw-and-nvcsw-fields-in-task-struct

*/
void print_csw(void){
	printk(KERN_INFO "Number of Voluntary context switches: %ld", t->nvcsw);
	printk(KERN_INFO "Number of InVoluntary context switches: %ld", t->nivcsw);
}



void getpcb(int pid){
	/* Access the task_struct using the code available at:
		https://stackoverflow.com/questions/56531880/how-does-the-kernel-use-task-struct
	*/
	read_lock(&tasklist_lock);
	t = find_task_by_vpid(pid);

	if (t)
		get_task_struct(t);

	read_unlock(&tasklist_lock);

	if (t == NULL) {
    		printk(KERN_INFO "Process %d not found!", pid);
	} else {
		print_state();       /* prints the state */
		print_open_files();  /* prints the open files */
		print_time();        /* prints start_time and real_start_time */
		print_csw();          /* prints context switch related data */
	}

	return 0;
}




static ssize_t device_read(struct file *flip, char *buffer, size_t len, loff_t *offset) {

    int bytes_read = 0;
	int pid;

    if (*msg_ptr == 0) {
        return 0;
    }


    pid = a2i(msg_ptr);
    getpcb(pid);

    //msg_ptr = str;
    while (len && *msg_ptr) {
        put_user(*(msg_ptr++), buffer++);
        len--;
        bytes_read++;
    }

    return bytes_read;
}



static ssize_t device_write(struct file *file, const char __user * buffer, 
			    size_t length, loff_t * offset) {

	int i;

	#ifdef DEBUG
		printk(KERN_INFO "device_write(%p,%s,%d)", file, buffer, length);
	#endif

	for (i = 0; i < length && i < MSG_BUFFER_LEN; i++)
		get_user(msg_buffer[i], buffer + i);

	msg_ptr = msg_buffer;

	return i;

}

static int device_open(struct inode *inode, struct file *file) {


 if (device_open_count) {

 return -EBUSY;

 }

 device_open_count++;

 try_module_get(THIS_MODULE);

 return 0;

}






static int device_release(struct inode *inode, struct file *file) {

 device_open_count--;

 module_put(THIS_MODULE);

 return 0;

}

static int __init my_module_init(void) {

	major_num = register_chrdev(0, "my_module", &file_ops);

	if (major_num < 0) {
		printk(KERN_ALERT "could not register device: %d\n", major_num);
	
 		return major_num;

 	} else{
 		printk(KERN_INFO "my_module module loaded with device major number %d\n", major_num);
		printk(KERN_INFO "msp_prt: %s\n", msg_ptr);
 		return 0;

	}

} 

static void __exit my_module_exit(void) {

	/* Unregister the character device */
	unregister_chrdev(major_num, DEVICE_NAME);
	printk(KERN_INFO "Goodbye!\n");
}




module_init(my_module_init);

module_exit(my_module_exit);

