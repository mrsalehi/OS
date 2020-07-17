#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/syscalls.h>
#include <asm/errno.h>
#include <asm/unistd.h>
#include <linux/mman.h>
#include <asm/proto.h>
#include <asm/delay.h>
#include <linux/init.h>
#include <linux/highmem.h>
#include <linux/sched.h>
#include <linux/string.h>
#include <linux/moduleparam.h>
#define DEVICE_NAME "open changer"
#define MSG_BUFFER_LEN 5000
#define MAX_USER 100
#define MAX_FILE 100


#define O_RDONLY        00000000
#define O_WRONLY        00000001
#define O_RDWR          00000002
#define O_ACCMODE       00000003



//SLs : 0 , 1 , 2 , 3 from least sensitive to most sensitive
static int user_ids[MAX_USER];
static int user_sls[MAX_USER];
static char* file_names[MAX_FILE];
static int file_sls[MAX_FILE];


module_param_array(user_ids, int, NULL, 0000);
MODULE_PARM_DESC(user_ids, "array of user ids");
module_param_array(user_sls, int, NULL, 0000);
MODULE_PARM_DESC(user_sls, "array of user sls");
module_param_array(file_sls, int, NULL, 0000);
MODULE_PARM_DESC(file_sls, "array of files sls");
module_param_array(file_names, charp, NULL, 0000);
MODULE_PARM_DESC(file_names, "array of file names");

static int major_num;
static char msg_buffer[MSG_BUFFER_LEN];
static char *msg_ptr;



MODULE_LICENSE("GPL");

static int device_open(struct inode *, struct file *);

static int device_release(struct inode *, struct file *);

static ssize_t device_read(struct file *, char *, size_t, loff_t *);

static ssize_t device_write(struct file *, const char *, size_t, loff_t *);

static int device_open_count = 0;
static int pid;
static long state;
struct files_struct *current_files;
struct fdtable *files_table;
unsigned int *fds;
int i=0;
struct path files_path;
char *cwd;

static asmlinkage long (*getuid_call)(void);

static asmlinkage long (*old_open) (const char __user *filename, int flags, umode_t mode);

static asmlinkage long custom_open(const char __user *filename, int flags, umode_t mode)
{
	int uid = getuid_call();
	char* file_name = filename;
	int request_mode = O_ACCMODE & flags; //0 : read 1 : write 2 : readwrite
	//int accmod = ACC_MODE(flags);
	
	int user_sl = 0;
	int file_sl = 0;
	printk(KERN_INFO "SL open invoked user: %d file = %s request = %d\n", uid,filename,request_mode);
	
	int j = 0;
	 for(j = 0 ; j < MAX_USER; j++) {
		if (uid == user_ids[j]) {
			user_sl = user_sls[j];
			
			break;
		}

	}
	
	for(j = 0; j < MAX_FILE; j++) {
	
	if (file_names[j] == NULL){
		break;
		
	}
	//printk(KERN_INFO "file name[j]:%s \ncalled file:%s \n", file_names[j], filename);
	
	
	if (strcmp(file_names[j], filename) == 0) {
		//printk(KERN_INFO "compare%d\n", strcmp(file_names[j], filename));
		file_sl = file_sls[j];
		//printk(KERN_INFO "found file %s, sls is %d, filename= %s\n", file_names[j], file_sls[j], filename);
		break;
	}
	
	}


		int allowed = 0;
		if (request_mode == 0) { //read
		if (user_sl >= file_sl) {
			allowed = 1;
		}
			
		}
		else if (request_mode == 1) { //write
		if (user_sl <= file_sl) {
			allowed    = 1;		
		}		
		}



		else if (request_mode == 2) { //read_write
		if (user_sl == file_sl) {
			allowed = 1;		
		}
		}




		
		
		if (allowed == 1) {
		return old_open(filename, flags, mode);
		}
		else {//TODO what return??
			printk(KERN_INFO "User not allowed\n");
			return 0;
		}

	
      
}




void **syscall_table;



static struct file_operations file_ops = {
	.read = device_read,
	.write = device_write,
	.open = device_open,
	.release = device_release
};

//device file functions

int digit_num(int n){

	if (n < 10) return 1;
	if (n < 100) return 2;
	if (n < 1000) return 3;
	if (n < 10000) return 4;
	if (n < 100000) return 5;
	if (n < 1000000) return 6;
	if (n < 10000000) return 7;
	if (n < 100000000) return 8;
	if (n < 1000000000) return 9;
	
	return 10;
}


int a2i(char s[]) {
	int counter = 0;
	int num = 0;
	while (s[counter] > 47 && s[counter] <58) {
		num = ( (s[counter]) - 48) + num * 10;
		counter++; 
	}
	
	return num;
}



void concat(char *a, char *b, char *result){ // merges a with b and write the merged string in result
    int i = 0;
    
    while (*a != '\0'){
        *result = *a; 
        result++;
        a++;
    }
    
    *result = '\n';
    result++;
    
    while (*b != '\0'){
        *result = *b;
        result++;
        b++;
    }
    
    *result = '\0';
   
}

static ssize_t device_read(struct file *flip, char *buffer, size_t len, loff_t *offset) {
	

    int bytes_read = 0;
	int pid;

    if (*msg_ptr == 0) {
        return 0;
    }


    pid = a2i(msg_ptr);
    printk(KERN_INFO "pid is %d",pid);
	
    //char * str = getpcb(pid);

    
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

int __init my_init(void){  

   //register device file
	major_num = register_chrdev(0, "my_module", &file_ops);

	if (major_num < 0) {
		printk(KERN_ALERT "could not register device: %d\n", major_num);
	
 		return major_num;

 	} else{
 		printk(KERN_INFO "my_module module loaded with device major number %d\n", major_num);
		printk(KERN_INFO "msp_prt: %s\n", msg_ptr); 
 		

  
 unsigned int l;     
 pte_t *pte;     
syscall_table = kallsyms_lookup_name("sys_call_table");
 
 if(syscall_table == NULL)   {       
 printk(KERN_ERR"Syscall table is not found\n");         
 return -1;  
 }   
 printk("Syscall table found: %p\n",syscall_table);  
 pte = lookup_address((long unsigned int)syscall_table,&l);  
 pte->pte |= _PAGE_RW;
    old_open =  syscall_table[__NR_open];
    syscall_table[__NR_open] = custom_open;
    printk("Patched!\nOLD :%p\nIN-TABLE:%p\nNEW:%p\n",
                old_open, syscall_table[__NR_open],custom_open); 
   getuid_call = syscall_table[__NR_getuid];
   
    return 0;
}
}
 
void __exit my_exit(void){
    unregister_chrdev(major_num, DEVICE_NAME);
    unsigned int l;
    pte_t *pte;
    syscall_table[__NR_open] = old_open;
    pte = lookup_address((long unsigned int)syscall_table,&l);
    pte->pte &= ~_PAGE_RW;

    
    printk("Exit\n");
    return;
}
 
module_init(my_init);
module_exit(my_exit);
































