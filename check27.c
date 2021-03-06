#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <ctype.h>

#define VERSION "check27 version 1.0 (linux)"

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

//TODO: change to approp location later, probably user/share
#define PDIR "/usr/share/check27"

void test_prog(int no_ip, char * op_filepath, char * sl_filepath, int print_diff);
int compile(char * filename);
int file_exists(char* path);
int file_cmp(char * op_filepath, char * sl_filepath);
int print_diff(char * op_filepath, char * sl_filepath);

int main(int argc, char * argv[]){
    // Check for no files or options
    int progn = -1;
    char filename[500];
    char sys_cmd[500];
    int print_diff_bool=0;
    if(argc==1){
        puts("ERROR: No input files entered. Try \"check27 -H\" or \"check27 --help\"");
        return 1;
    }

    // check for version
    for(int i=1;i<argc;i++){
        if(!strcmp(argv[i],"-v") || !strcmp(argv[i],"--version")){
            puts(VERSION);
            return 0;
        }
    }

    // check for help argument
    for(int i=1;i<argc;i++){
        if(!strcmp(argv[i],"-H") || !strcmp(argv[i],"--help")){
            puts("USAGE: check27 prog_.c\n Fill in appropriate program number\n");
            return 0;
        }
    }

    // check for custom program number
    for(int i=1;i<argc;i++){
        if(!strcmp(argv[i],"-p") || !strcmp(argv[i],"--program")){
            //TODO: check if program arg is a number
            progn = atoi(argv[i+1]);
            }
    }

    // check for arg requesting program guidelines
    for(int i=1;i<argc;i++){
        if(!strcmp(argv[i],"-g") || !strcmp(argv[i],"--guidlines")){
            char guidelines_path[500];
            char sys_cmd[500];
            if(i+1>=argc){
                     puts("ERROR: Program ID not specified for guidlines");
                     return 2;
            }
            sprintf(guidelines_path,"%s/prog%s/guidelines", PDIR, argv[i+1]);
            if(!file_exists(guidelines_path)){
                printf("ERROR: No guidline entry for prog%s\n", argv[i+1]);
                return 2;
            }
            sprintf(sys_cmd, "cat %s", guidelines_path);
            system(sys_cmd);
            return 0;
        }
    }

    // find filename
    for(int i=1;i<argc;i++){
        printf("%s", argv[i]);
        if(strstr(argv[i], ".c")!=NULL || strstr(argv[i],".cpp")!=NULL)
            strcpy(filename, argv[i]);
        printf("%s", filename);
    }

    // find param to print difference between files
    for(int i=1;i<argc;i++){
        if(!strcmp(argv[i],"-pd") || !strcmp(argv[i],"--print-difference")){
            print_diff_bool = 1;
        }
    }

    // Compile
    compile(filename);
    // get program number from last 3rd letter of filename
    if(progn==-1)
        progn = filename[strlen(filename)-3]-48;
    //char progn_dir[500];
    char ip_filepath[500];
    char op_filepath[500];
    char sl_filepath[500];

    sprintf(ip_filepath, "%s/prog%i/ip/ip0", PDIR,progn);
    sprintf(sl_filepath, "%s/prog%i/sl/sl0", PDIR,progn);

    int path_size = strlen(ip_filepath);
    int key = path_size-1; // key for ip and sl
    sprintf(op_filepath, "./op/op0");
    path_size = strlen(op_filepath);
    int key_op = path_size-1;
    system("mkdir ./op");
    int no_ip=0;
    while(file_exists(ip_filepath)){
        no_ip++;
        sprintf(sys_cmd,"./prog.out < %s > %s", ip_filepath, op_filepath);
        system(sys_cmd);

        ip_filepath[key]++;
        op_filepath[key_op]++;
        //puts(ip_filepath);
    }

    // test against test cases, and print difference if -pd is passed
    test_prog(no_ip, op_filepath, sl_filepath, print_diff_bool);
    //system("cat diff.txt");

    // delete the output directory
    sprintf(sys_cmd, "rm -rf ./op");
    system(sys_cmd);
    system("rm prog.out");
    return 0;

}

int compile(char * filename){
    char sys_cmd[500];
    puts(PDIR);
    sprintf(sys_cmd, "gcc -O0 -std=c99 %s -o prog.out", filename);
    puts("Compiling...");
    system(sys_cmd);

    // check if comiled successfully
    if(!file_exists("prog.out")){
        printf(ANSI_COLOR_RED"Compilation Failed.\n"ANSI_COLOR_RESET);
        exit(2);
    }
    //fclose(exec);
    printf(ANSI_COLOR_GREEN"Compilation Successful.\n"ANSI_COLOR_RESET);
    return 0;

}

// function to check if a file exists
// doesn't check if its accessible
int file_exists(char* path){
    if(access(path, F_OK)!=-1)
        return 1;
    return 0;
}


void test_prog(int no_ip, char * op_filepath, char * sl_filepath, int print_diff_bool){

    int path_size = strlen(sl_filepath);
    // position of character number that determines which test case it is
    int key = path_size-1;
    path_size = strlen(op_filepath);
    int key_op = path_size-1;
    for(int i=0; i<no_ip; i++){
        op_filepath[key_op] = sl_filepath[key] = i+48;
        //char test_no_str[50];
        if(!file_cmp(op_filepath, sl_filepath))
            printf(ANSI_COLOR_GREEN"Test: %i\n"ANSI_COLOR_RESET,i);
        else
            printf(ANSI_COLOR_RED"Test: %i\n"ANSI_COLOR_RESET,i);
        if(print_diff_bool)
            print_diff(op_filepath, sl_filepath);
    }
}

// This function prints the differences between the output
// and solution generated by the program.
// Matching lines are coloured in green, line of difference is in red
// and the remaining lines are coloured yellow

int print_diff(char * op_filepath, char * sl_filepath){
    int err_flag=0;
    FILE * op = fopen(op_filepath,"r");
    FILE * sl = fopen(sl_filepath,"r");
    char buffer1[100]="\0", buffer2[100]="\0";
    if(op==NULL){puts("ERROR: O/P file doesn't exist"); return 2;}
    if(sl==NULL){puts("ERROR: Solution file doesn't exist"); return 2;}
    printf("%-40s %-40s\n","YOUR OUTPUT", "EXPECTED OUTPUT");
    while(!feof(op) || !feof(sl)){
        //gets the next non-empty line into buffer
        while(!feof(op)){
          fgets(buffer1, 100, op);
          //remove trailing \n from gets
          buffer1[strcspn(buffer1, "\n")] = 0;
          //remove trailing spaces in line
          if(strlen(buffer1)==0) continue;
          while(buffer1[strlen(buffer1)-1] == ' ' || buffer1[strlen(buffer1)-1] == '\t')
            buffer1[strlen(buffer1)-1]='\0';
          if(buffer1[0]!='\0') break;
        }
        while(!feof(sl)){
          fgets(buffer2,100,sl);
          buffer2[strcspn(buffer2, "\n")] = 0;
          if(strlen(buffer2)==0) continue;
          while(buffer2[strlen(buffer2)-1] == ' ' || buffer2[strlen(buffer2)-1] == '\t')
            buffer2[strlen(buffer2)-1]='\0';
          if(buffer2[0]!='\0') break;
        }


        //if(feof(op) || feof(sl)) break;

        // check if error encountered
        if(err_flag==0 && strcmp(buffer1,buffer2)) err_flag=1;
        //print in green if no err so far
        if(err_flag==0)
        printf(ANSI_COLOR_GREEN"%-40s %-40s\n"ANSI_COLOR_RESET,buffer1, buffer2);
        // print first error line encounted in red
        else if(err_flag==1){
            printf(ANSI_COLOR_RED"%-40s %-40s\n"ANSI_COLOR_RESET,buffer1, buffer2);
            err_flag=2;
        }
        // print in yellow if one error line has already been printed in red
        else if(err_flag==2)
            printf(ANSI_COLOR_YELLOW"%-40s %-40s\n"ANSI_COLOR_RESET,buffer1, buffer2);
        buffer1[0]='\0'; buffer2[0]='\0';
    }
    fclose(op);
    fclose(sl);
    return 0;
}

// Function checks if the given two files are the same
// after cleaning up by removing extra spaces, tabs etc.
int file_cmp(char * op_filepath, char * sl_filepath){
    FILE * op = fopen(op_filepath,"r");
    if(op==NULL){
        puts("ERROR: O/P file doesn't exist");
        return 2;
    }

    FILE * sl = fopen(sl_filepath,"r");
    if(sl==NULL){
        puts("ERROR: Solution file doesn't exist");
        fclose(op);
        return 2;
    }

    char buffer1[100]="", buffer2[100]="";

    while(!feof(op) && !feof(sl)){
      // get line and remove trailing \n from fgets
      while(!feof(op)){
        fgets(buffer1, 100, op);
        // remove trailing \n from gets
        buffer1[strcspn(buffer1, "\n")] = 0;
        // remove trailing spaces in line
        if(strlen(buffer1)==0) continue;
        while(buffer1[strlen(buffer1)-1] == ' ' || buffer1[strlen(buffer1)-1] == '\t')
          buffer1[strlen(buffer1)-1]='\0';
        if(buffer1[0]!='\0') break;
      }
      while(!feof(sl)){
        fgets(buffer2,100,sl);
        buffer2[strcspn(buffer2, "\n")] = 0;
        if(strlen(buffer2)==0) continue;
        while(buffer2[strlen(buffer2)-1] == ' ' || buffer2[strlen(buffer2)-1] == '\t')
          buffer2[strlen(buffer2)-1]='\0';
        if(buffer2[0]!='\0') break;
      }

      if(feof(op) || feof(sl)) break;
      if(strcmp(buffer1,buffer2)){
          fclose(op);
          fclose(sl);
          return 1;
      }
    }

    // remove newlines at end of file
    while(!feof(op)){
      fgets(buffer1, 100, op);
      if(buffer1[0]!='\n') break;
    }
    while(!feof(sl)){
      fgets(buffer2,100,sl);
      if(buffer2[0]!='\n') break;
    }

    // if both reach EOF at the same time
    if(feof(op) && feof(sl)){
        fclose(op);
        fclose(sl);
        return 0;
    }

    fclose(op);
    fclose(sl);
    // if only one of the files has reached EOF
    return 1;
}
