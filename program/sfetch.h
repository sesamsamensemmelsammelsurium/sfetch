#define VERSION "0.1"

#define KERNSIZE 128
#define CPUSIZE 128
#define IMGSIZE 1024
#define MB (1024 * 1024)

#define DEFAULT "\x1b[0m"
#define RED "\x1b[1;31m"
#define GREEN "\x1b[1;32m"
#define BLUE "\x1b[1;34m"
#define MAGENTA "\x1b[1;35m"
#define WHITE "\x1b[1;37m"
#define RESET "\x1b[H"

#define DAYS sys.uptime / 86400
#define HOURS (sys.uptime % 86400) / 3600
#define MINUTES (sys.uptime % 3600) / 60
#define SECONDS sys.uptime % 60

#define TOTAL_RAM (sys.totalram * sys.mem_unit) / MB
#define FREE_RAM (sys.freeram * sys.mem_unit) / MB
#define TOTAL_SWAP (sys.totalswap * sys.mem_unit) / MB
#define FREE_SWAP (sys.freeswap * sys.mem_unit) / MB
#define PROCESSES sys.procs

struct sysinfo sys;

char *default_path[3] = {"../ascii/linux.txt", "../ascii/openbsd.txt", "../ascii/sfetch.txt"};
char *install_path[3] = {"/usr/local/share/ascii/linux.txt", "/usr/local/share/ascii/openbsd.txt", "/usr/local/share/ascii/sfetch.txt"};
int (*modules[7])(FILE*);

char *switch_colors(char *str);
void module_order();
int print_times(FILE*);
int print_memory(FILE*);
int print_kernel(FILE*);
int print_procs(FILE*);
int print_cpu(FILE*);
int print_user(FILE*);
int print_address(FILE *);
void print_info();
int render_art(FILE*);
void check_error(FILE*);
int check_path(char*);
void colorize(const char*, char*);
char* fetch_time();
void separator(int);
char* strip_newline(char*);
char* switch_colors(char*);

typedef enum {
                  LINUX,
                  OPENBSD,
                  SFETCH,
                  CUSTOM,

              } ART;

typedef enum {

                  R,
                  G,
                  B,
                  M,

              } COLORS;

#if defined(__TINYC__)
  #pragma pack(push, 1)
  typedef struct modules {

    unsigned int art        : 1;
    unsigned int datetime   : 1;
    unsigned int systemtime : 1;
    unsigned int ram        : 1;
    unsigned int swap       : 1;
    unsigned int cpu        : 1;
    unsigned int kernel     : 1;
    unsigned int procs      : 1;
    unsigned int host       : 1;
 
  } Modules;

  typedef union Colors {

    unsigned int toggle : 1;

  } Colors;

  typedef struct mdls {

    unsigned int active : 4;

  } ActiveMods;

  typedef union swap {

    unsigned int on : 1;

  } Swap;
#pragma pack(pop)

#elif defined(__GNUC__) || defined(__clang__) 
  typedef struct modules {
 
    unsigned int art        : 1;
    unsigned int datetime   : 1;
    unsigned int systemtime : 1;
    unsigned int ram        : 1;
    unsigned int swap       : 1;
    unsigned int cpu        : 1;
    unsigned int kernel     : 1;
    unsigned int procs      : 1;
    unsigned int host       : 1;

  } __attribute__((packed)) Modules;

  typedef union Colors {

    unsigned int toggle : 1;

  } __attribute__((packed)) Colors;

  typedef struct mdls {

    unsigned int active : 4;

  } __attribute__((packed)) ActiveMods;

  typedef union swap {

    unsigned int on : 1;

  } __attribute__((packed)) Swap;
#endif

ActiveMods mdls = {

      .active = 0

};
