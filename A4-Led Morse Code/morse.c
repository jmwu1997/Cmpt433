#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/miscdevice.h>		// for misc-driver calls.
#include <linux/delay.h> 			// for msleep()
#include <linux/proc_fs.h>			// /proc
#include <linux/leds.h>				// LED access
#include <asm/uaccess.h>			// copy_from_user() and copy_to_user()
#include <linux/kfifo.h>


#define MY_DEVICE_FILE "morse-code"
#define DOTTIME_DEFAULT 200
#define DOTTIME_LOWER 1                 // ms
#define DOTTIME_UPPER 2000              // ms

//Morse code
//dot is 1 unit
//dash is 3 unit
//space between same letter is 1 unit
// space between letters is 3 unit
// space between words is 7 unit
static unsigned int time = 200;
static DECLARE_KFIFO(queue, char, 8192);
module_param(time, int, S_IRUGO);
MODULE_PARM_DESC(time, "dot time");
DEFINE_LED_TRIGGER(led_trigger);

static unsigned short morsecode_codes[] = {
		0xB800,	// A 1011 1  .-
		0xEA80,	// B 1110 1010 1 -...
		0xEBA0,	// C 1110 1011 101
		0xEA00,	// D 1110 101
		0x8000,	// E 1
		0xAE80,	// F 1010 1110 1
		0xEE80,	// G 1110 1110 1
		0xAA00,	// H 1010 101
		0xA000,	// I 101
		0xBBB8,	// J 1011 1011 1011 1
		0xEB80,	// K 1110 1011 1
		0xBA80,	// L 1011 1010 1
		0xEE00,	// M 1110 111
		0xE800,	// N 1110 1
		0xEEE0,	// O 1110 1110 111
		0xBBA0,	// P 1011 1011 101
		0xEEB8,	// Q 1110 1110 1011 1
		0xBA00,	// R 1011 101
		0xA800,	// S 1010 1
		0xE000,	// T 111
		0xAE00,	// U 1010 111
		0xAB80,	// V 1010 1011 1
		0xBB80,	// W 1011 1011 1
		0xEAE0,	// X 1110 1010 111
		0xEBB8,	// Y 1110 1011 1011 1
		0xEEA0	// Z 1110 1110 101
};



/******************************************************
 * LED
 ******************************************************/





static void led_unregister(void)
{
	led_trigger_unregister_simple(led_trigger);
}

static void on_led (void)
{
	led_trigger_event(led_trigger, LED_FULL);

}

static void off_led(void)
{
	led_trigger_event(led_trigger, LED_OFF);
}

static void led_register(void)
{
	led_trigger_register_simple("morse-code", &led_trigger);
	off_led();
}




/******************************************************
 * Callbacks
 ******************************************************/
// return the corresponding code
static unsigned short convert(char ch)
{
	unsigned short ret = 0;

    // lowercase
	if (ch >= 'a' && ch <= 'z') {
		ret = morsecode_codes[ch-'a'];
	}
	// uppercase
	else if (ch >= 'A' && ch <= 'Z') {
		ret = morsecode_codes[ch-'A'];
	}
	else if (ch == ' '){
		kfifo_put(&queue, ' ');
		msleep(DOTTIME_DEFAULT*3);
		return 0;
	}

	return ret;
}



static void lightled(unsigned short ret)
{
	//check first character is 1 or 0
	if (ret & 0x8000) {
		on_led();
		msleep(DOTTIME_DEFAULT);
	}
	else {
		off_led();
		msleep(DOTTIME_DEFAULT);
	}
			
			

}



static ssize_t my_write(struct file* file, const char *buff, size_t count, loff_t* ppos)
{
	int i=0,countl=0;
	char ch;
	unsigned short ret = 0;
	_Bool checkloop = false;

	for (i = 0; i < count; i++) {
		if (checkloop){
			kfifo_put(&queue, ' ');
			msleep(DOTTIME_DEFAULT*3);
		}
		if (raw_copy_from_user(&ch, &buff[i], sizeof(ch))) {
			return -EFAULT;
		}
		ret = convert(ch);
		
		while (ret!=0) {
		   lightled(ret);
		   if (countl <= 0){
		   		// check first 3 input 111
				if ((ret & 0xE000) == 0xE000){
					kfifo_put(&queue, '-');
					countl = 3;
				// check first input 1
				}
				else if ((ret & 0x8000) == 0x8000) {
					kfifo_put(&queue, '.');

		   		}
		   }	
		   
		   // move 1 to next
		   ret <<= 1;
		   countl --;
		}
		//turn off led after every letter	
		off_led();
		checkloop = true;
	}


	// insert end of char before exiting
	kfifo_put(&queue, '\n');
	return count;  
}

// Read the status as it happens:
static ssize_t my_read(struct file *file, char *buf, size_t count, loff_t *f_pos)
{
	int num_bytes_read = 0;
	if (kfifo_to_user(&queue, buf, count, &num_bytes_read)) {
		return -EFAULT;
	}
	return num_bytes_read;
}

// Callbacks:  (structure defined in /linux/fs.h)
struct file_operations my_fops = {
		.owner    = THIS_MODULE,
		.write    = my_write,
		.read     = my_read,
};


// Character Device info for the Kernel:
static struct miscdevice my_miscdevice = {
		.minor    = MISC_DYNAMIC_MINOR,         // Let the system assign one.
		.name     = MY_DEVICE_FILE,                // /dev/.... file.
		.fops     = &my_fops             // Callback functions.
};

/******************************************************
 * Driver initialization and exit:
 ******************************************************/

// Initialize the driver.
static int __init my_init(void)
{
	int ret;
	printk(KERN_INFO "----> a4 morsecode driver init().\n", MY_DEVICE_FILE);
    INIT_KFIFO(queue);
	ret = misc_register(&my_miscdevice);

	if (ret) {
		printk(KERN_INFO " failed to register driver (code %d)\n", ret);

	}
	led_register();

    
	return ret;
}


static void __exit my_exit(void)
{
	printk(KERN_INFO "<---- a4 morsecode driver exit().\n");
	// Unregister misc driver
	misc_deregister(&my_miscdevice);
	//remove_proc_entry(MY_DEVICE_FILE, NULL);
	led_unregister();
}



module_init(my_init);
module_exit(my_exit);

MODULE_AUTHOR("Kelvin & Levana");
MODULE_DESCRIPTION("a4 Morse Code LED flash driver");
MODULE_LICENSE("GPL");
