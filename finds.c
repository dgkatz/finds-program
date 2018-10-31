#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>

#define NO_OF_CHARS 256

struct Args {
  int f;
  int c;
  int h;
  int S;
  int l;
};

static char *fullpath;
static size_t pathlen;

int main(int argc, char * const argv[])
{
  char *searchTerm; 
  char *pathname;
  struct Args flags;
  int c;

  memset(&flags, 0, sizeof(flags));

  while ((c = getopt (argc, argv, "p:f:ls:")) != -1)
    switch (c)
      {
      case 'p':
        pathname = optarg; 
	break;
      case 'f':
        flags.f = 1;
	if (strcmp(optarg, "c")  == 0) {
	  flags.c = 1;
	} else if (strcmp(optarg, "h") == 0) {
	  flags.h = 1;
	} else if (strcmp(optarg, "S") == 0) {
	  flags.S = 1;
	} else {
	  fprintf(stderr, "Invalid option argument for -f: %s\n", optarg);
	}
        break;
      case 'l':
        flags.l = 1;
        break;
      case 's':
	searchTerm = optarg;
	break;
      case '?':
	if(optopt == 'p')
	  fprintf(stderr, "Invalid pathname\n");
        else if (isprint (optopt))
          fprintf (stderr, "Unknown option `-%c'.\n", optopt);
        else
          fprintf (stderr,
                   "Unknown option character `\\x%x'.\n",
                   optopt);
        return 1;
      default:
        abort ();
      }

  fullpath = path_alloc(&pathlen);

  if (pathlen <= strlen(pathname)) {
    pathlen = strlen(pathname) * 2;
    if ((fullpath = realloc(fullpath, pathlen)) == NULL)
      err_sys("realloc failed");
  }
  strcpy(fullpath, pathname);
  myftw(flags, searchTerm);
}

//bool strmatch(char str[], char pattern[], 
//              int n, int m) 
//{ 
  //  int prev_state = -1;
  //int state = 0;
  //char prev_char = str[0];
  //char current_char = str[0];
  
  //for (int i = 0; i <= n; i++) {
    //current_char = str[i];
    
    //switch (pattern[state]) {
    //case ".":
    //prev_state = state;
    //state ++;
    //prev_char = current_char;
    //case "*":
    //if ((current_char == prev_char)){
    //prev_state = prev_state;
    //state = state;
    //prev_char = current_char;
    //}
    //else{
    //	prev_state = state;
    //	state ++;
    //	prev_char = current_char;
    //}
    //}
    // IDK where to go from here.
//} 

int search(const char* pathname, struct Args flags, char* term) {
  int match = 0; 
  FILE *fp;
  char temp[80];

   
  char *ext = strrchr (pathname, '.');
  if (ext == NULL) {
    ext = "";
  }

  if (flags.f){
    if (flags.c && (strcmp(ext, ".c") == 0)) {
      fp = fopen(pathname, "r");
    }
    else if (flags.h && (strcmp(ext, ".h") == 0)) {
      fp = fopen(pathname, "r");
    }
    else if (flags.S && (strcmp(ext, ".S") == 0)) {
      fp = fopen(pathname, "r");
    }
    else { return 0;}
  }
  else if (flags.f == 0) {
      fp = fopen(pathname, "r");
  }
  
  if (fp == NULL) { 
    return; 
  }

  //  while (fgets(temp, 80, fp) != NULL) {
  //    if (strmatch(term, temp, strlen(term), strlen(temp))) {
  //      fprintf(stdout, "%s", temp);
  //      fprintf(stdout, "%s\n", pathname);
  //    }
  //  }

  bool found = false;
  while (fgets(temp, 80, fp) != NULL) {
    if ((strstr(temp, term)) != NULL) {
      fprintf(stdout, "%s", temp);
      found = true;
    }
  }
  if (found) {
    fprintf(stdout, "%s\n", pathname);
  }
  if (fp) { fclose(fp); }
  return 0; 
}

int myftw(struct Args flags, char* term) {
  struct stat statbuf;
  struct dirent *dirp;
  DIR *dp;
  int ret, n;

  if (lstat(fullpath, &statbuf) < 0) { 
    fprintf(stderr, "find: '%s' : No such file or directory\n", fullpath); 
  }

  if (S_ISDIR(statbuf.st_mode) == 0) {
    return search(fullpath, flags, term); 
  }
 
  n = strlen(fullpath);
  if (n + NAME_MAX + 2 > pathlen) {
    pathlen *= 2;
    if ((fullpath = realloc(fullpath, pathlen)) == NULL) { 
      fprintf(stderr, "Path reallocation failed"); 
    }
  }
  fullpath[n++] = '/';
  fullpath[n] = 0;
  
  if ((dp = opendir(fullpath)) == NULL) { return; }

  while ((dirp = readdir(dp)) != NULL) {
    if ((strcmp(dirp->d_name, ".")) ==  0 || (strcmp(dirp->d_name, "..") == 0)) { continue; }
    strcpy(&fullpath[n], dirp->d_name);
    
    lstat(fullpath, &statbuf);
    if (S_ISDIR(statbuf.st_mode) != 0) { myftw(flags, term); }

    if (ret = search(fullpath, flags, term) != 0) { break; }
  }
  
  fullpath[n-1] = 0;
  
  if (closedir(dp) < 0) { fprintf(stderr, "Cant close directory %s\n", fullpath); }
  
  return; 
}
