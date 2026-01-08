/* Copyright (c) 2025 sesamsamensemmelsammelsurium

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the 
following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following 
disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following 
disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products 
derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE 
USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. */ 

#define _GNU_SOURCE

#include <sys/sysinfo.h>
#include <sys/stat.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>

#include "sfetch.h"
#include "config.h"

/*Fetch system information and display it on the terminal screen*/
int
main(int argc, char** argv)
{

  for (int i = 1; i < argc; ++i)
  {

      if(argv[i][0] == '-')
        switch (argv[i][1])
        {
        case 'i':
          print_info();
          break;
        case 'v':
          printf("\nsfetch version %s", VERSION);
          break;
        default:
          fprintf(stderr, "Unknown option: %s\n", argv[i]);
        }

  }

  return 0;

}

/*Print the info*/
void
print_info()
{

    if(sysinfo(&sys) < 0)
    {
      fprintf(stderr, "Problem with fetching sys info.");
      exit(EXIT_FAILURE);
    }
 
    static const size_t max_mods = 7;

    modules_();
 
    for(size_t i = 0; i < max_mods; i++) 
    {
      if (modules[i] != NULL)
          modules[i](stdout);
    }

    if(mdls.active == 0)
    {
      separator(nlines);
      printf("Welcome to sfetch %s. Please activate some modules.", VERSION);
      separator(nlines);
    }

}


/*Get active processes if module is activated*/
int
print_procs(FILE *fd)
{

  if(state.procs)
  {

    mdls.active += 1;

    if(colors.toggle)
      switch_colors("Active Processes: ");
    else 
      fprintf(fd, "\tActive Processes: ");
 
    if(fprintf(fd, "%hu", PROCESSES) < 0)
      fprintf(stderr, "Couldn't display processes");

    separator(nlines);
 
  }

  return 0;

}

/*Get datetime and uptime if modules are activated*/
int
print_times(FILE* fd)
{

  char *currt = fetch_time();

  if(state.datetime)
  {

    mdls.active += 1;

    if(colors.toggle)
      switch_colors("Date & Time: ");
    else
      fprintf(fd, "\tDate & Time: ");

    if(fprintf(fd, "%s", strip_newline(currt)) < 0)
      fprintf(stderr, "Couldn't display datetime");

    separator(nlines);

  }

  if(state.systemtime)
  {

    mdls.active += 1;

    if(colors.toggle)
      switch_colors("System Uptime: ");
    else 
      fprintf(fd, "\tSystem Uptime: ");
  
    if(fprintf(fd, "%.2lu:%.2lu:%.2lu:%.2lu (d, h, m & s)",
      DAYS,
      HOURS,
      MINUTES,
      SECONDS) < 0)
      fprintf(stderr, "Couldn't display Uptime");

    separator(nlines);

  }

  return 0;

}

/*Get RAM and Swap if modules are activated*/
int
print_memory(FILE* fd)
{

  if(state.ram)
  {

    mdls.active += 1;

    if(colors.toggle)
      switch_colors("Available RAM: ");
    else
      fprintf(fd, "\tAvailable RAM: ");

    if(fprintf(fd, "%luMiB (of %luMiB)", FREE_RAM, TOTAL_RAM) < 0)
      fprintf(stderr, "Couldn't display RAM");

    separator(nlines);

  }
 
  if((state.swap && TOTAL_SWAP) || (state.swap && TOTAL_SWAP == 0 && detect.on))
  {

    mdls.active += 1;

    if(colors.toggle)
      switch_colors("Available Swap: ");
    else
      fprintf(fd, "\tAvailable Swap: ");

    if(fprintf(fd, "%luMiB (of %luMiB)", FREE_SWAP, TOTAL_SWAP) < 0)
      fprintf(stderr, "Couldn't display Swap");

    separator(nlines);
 
  }

  return 0;

}

/*Get print_cpu if module is activated*/
int
print_cpu(FILE *fd)
{

  if(state.cpu)
  {
 
    mdls.active += 1;

    FILE *print_cpuinfo = fopen("/proc/cpuinfo", "r");

    check_error(print_cpuinfo);

    char *res = malloc(CPUSIZE);

    if(!res)
    {
      fprintf(stderr, "\tCouldn't allocate memory");
      exit(EXIT_FAILURE);
    }

    static const char *sub = "model name";

    while (fgets(res, CPUSIZE, print_cpuinfo) != NULL)
      if(strstr(res, sub) != NULL)
      {

        char *model_name = strchr(res, ':');

        if (model_name)
        {
              if(colors.toggle) 
                switch_colors("CPU:");
              else 
                fprintf(fd, "\tCPU:");
              if(fprintf(fd, "%s", strip_newline(model_name+1)) < 0)
                fprintf(stderr, "Couldn't display CPU");
        }

        separator(nlines);

        break;


      }

    free(res);
    fclose(print_cpuinfo);

  }
 
  return 0;

}

/*Get Kernel version if module is activated*/
int
print_kernel(FILE *fd)
{

  if(state.kernel)
  {
    
    if(kern_size <= KERNSIZE)
    {
 
      char *kern = malloc(kern_size);

      mdls.active += 1;

      FILE* fp = fopen("/proc/version", "r");

      check_error(fp);

      if(!kern)
      {
        fprintf(stderr, "Couldn't allocate memory");
        exit(EXIT_FAILURE);
      }

      static const char *sub = "Linux";
 
      while (fgets(kern, kern_size, fp) != NULL)
        if(strstr(kern, sub) != NULL)
        {
          char *ver = strstr(kern, "version");
 
          size_t len = sizeof("version");

          if (ver)
          {
            if(colors.toggle)
              switch_colors("Kernel: ");
            else
              fprintf(fd, "\tKernel: ");
            if(fprintf(fd, "%s", ver+len) < 0)
              fprintf(stderr, "Couldn't display Kernel version");

            separator(nlines);
          }
      }

            free(kern);
            fclose(fp);

    }

  }

    return 0;

}

/* Print user and host */
int
print_user(FILE *fd)
{

  char host[512];

  uid_t uid = getuid();

  struct passwd *user = getpwuid(uid);

  if(state.host)
  {
    if(gethostname(host, sizeof(host)) == 0 && user != NULL)
    {
      if(colors.toggle)
        switch_colors("User: ");
      else
        fprintf(fd, "User: ");


      if(fprintf(fd, "%s->%s", user->pw_name, host) < 0)
        fprintf(stderr, "Couldn't display hostname");
    }

  }

  separator(nlines); 
  return 0;

}

/* Render art if module is activated */
/* FILE* has no purpose here, but must be present for very hacky reasons. 
   Identifier must stay for tcc compatibility. */
int
render_art(FILE* fd) 
{

  if(state.art)
  {

    mdls.active += 1;

    FILE *path = NULL;
 
    switch(ascii)
    {
    case LINUX:
      if(!(path = fopen(default_path[0], "r")))
      {
        path = fopen(install_path[0], "r");
        check_error(path);
      }
      break;

    case OPENBSD:
      if(!(path = fopen(default_path[1], "r")))
      {
        path = fopen(install_path[1], "r");
        check_error(path);
      }
      break;

    case SFETCH:
      if(!(path = fopen(default_path[2], "r")))
      {
        path = fopen(install_path[2], "r");
        check_error(path);
      }
      break;

    case CUSTOM:
      if (!check_path(imgpath))
      {
        path = fopen(imgpath, "r");
        check_error(path);
      } else { printf("%s\n\timgpath not a .txt file?%s", RED, DEFAULT); separator(nlines); return -1; }
      break;

    default:
      if(!(path = fopen(default_path[2], "r")))
          fprintf(stderr, "default path missing");
  }

  char img[IMGSIZE];

  int linesread = 0;

  while(fgets(img, sizeof(img), path) != NULL && linesread <= MAX_LINES)
  {

        ++linesread;

        /* If color option is set, print colored ASCII, else ignore */
        if (colors.toggle)
          switch_colors(img);
        else 
          printf("\t%s", img);

  }
 
  fclose(path);
  separator(nlines);
 
  }

  return 0;

}

/*Helper function: strip newline */
char*
strip_newline(char *str)
{

    size_t len = strlen(str);

    if (len > 0 && str[len - 1] == '\n')
        str[len - 1] = '\0';

    return str;

}

/*Helper function: check if a .txt file is provided*/
int
check_path(char *path)
{

  static const size_t check_chars = 4;

  static char *example = "/usr/local/share/ascii/example.txt"; 

  return strncmp(path + strlen(path) - check_chars, example + strlen(example) - check_chars, check_chars);

}

/*Helper function: fetch current date and time*/
char*
fetch_time()
{

  time_t current_time;

	if(time(&current_time) < 0)
    fprintf(stderr, "Couldn't retrieve time");

  char *r_time = ctime(&current_time);

  return r_time;

}

/*Helper function: separating lines*/
void
separator(int nlines)
{

    for (int i = 0; i < nlines; i++)
      printf("\n");

}

/*Helper function: check for FILE* errors */
void
check_error(FILE* fd)
{

  if(!fd) { fprintf(stderr, "Couldn't load file!\n"); exit(EXIT_FAILURE); }

}

/*Helper function: Colorize output */
void
colorize(const char* color, char* str)
{

  printf("\t%s%s\x1b[0m", color, str);

}

/*Helper function: Handle colors */
char*
switch_colors(char* str)
{

  switch(set_c)
    {
      case R:
        colorize(RED, str);
          break;

      case G:
        colorize(GREEN, str);
        break;

      case B:
        colorize(BLUE, str);
        break;

      case M:
        colorize(MAGENTA, str);
        break;

      default:
        colorize(WHITE, str);
        break;

        }

    return str;

}
