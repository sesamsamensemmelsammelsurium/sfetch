/*ART

Specify either a pre-cofigured option or a path for your ASCII art image

0 = "Linux", 1 = "OpenBSD", 2 = "Sfetch" or 3 for custom ascii art*/

static ART ascii = 0;

/*Your custom ascii art, change path to your liking. Not supposed to exceed 10 lines.*/

static char *imgpath = "../ascii/batman.txt"; 

/* Turn on color mode for ASCII header. 1 = on, 0 = off. */

Colors colors = {


            .toggle = 1,

};

/* Specify size of the ASCII art. */
 
#define MAX_LINES 10

/* Set color: RED = 0, GREEN = 1, BLUE = 2, MAGENTA = 3. Corresponding values might get overridden by your Terminal emulator. */

static COLORS set_c = 2;

/*MODULES */


/* Turn module on or off; 1 = Module on, 0 = Module off. */

Modules state = { 

            .art = 1, 
            .datetime = 1,
            .systemtime = 1,
            .ram = 1,
            .swap = 1,
            .cpu = 1,
            .kernel = 1,
            .procs = 1,
            .host = 1

};

/*Module order

Change module index n to show module on nth place */ 

void 
modules_()
{

modules[0] = render_art; //Display ASCII art
modules[2] = print_times; //Print system time, date and time
modules[5] = print_kernel; //Print kernel version
modules[3] = print_cpu; //Print CPU information
modules[4] = print_memory; //Print memory stats
modules[6] = print_procs; //Print active processes
modules[1] = print_user; //Print user and host

}

/*MISC

nlines = amount of seperator space between rows */ 

static unsigned int nlines = 2;

/* Not supposed to exceed a maximum size of 128. */

static size_t kern_size = 32;

/* Swap detection if no Swap is activated on the System, but modules turned on */

Swap detect = {

        .on = 0

};
