#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#define VERSION "check27 version 1.0 (linux)"

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

//TODO: change to approp location later, probably user/share
#define PDIR "/home/ani/Documents/check27"

void test_prog(int no_ip, char * op_dir, char * sl_dir, int print_diff);
int compile(char * filename);
int file_exists(char* path);
int file_cmp(char * op_dir, char * sl_dir);
int print_diff(char * opdir, char * sldir);

int main(int argc, char * argv[]){
    //Check for no files or options
    int progn = -1;
    char filename[500];
    int print_diff_bool=0;
    if(argc==1){
        puts("ERROR: No input files entered. Try \"check27 -H\" or \"check27 --help\"");
        return 1;
    }

    //check for version
    for(int i=1;i<argc;i++){
        if(!strcmp(argv[i],"-v") || !strcmp(argv[i],"--version")){
            puts(VERSION);
            return 0;
        }
    }

    //check for help argument
    for(int i=1;i<argc;i++){
        if(!strcmp(argv[i],"-H") || !strcmp(argv[i],"--help")){
            puts("USAGE: check27 prog_.c\n Fill in appropriate program number\n");
            return 0;
        }
    }

    //check for custom program number
    for(int i=1;i<argc;i++){
        if(!strcmp(argv[i],"-p") || !strcmp(argv[i],"--program")){
            //TODO: check if program arg is a number
            progn = atoi(argv[i+1]);
            }
    }

    //check for arg requesting program guidelines
    for(int i=1;i<argc;i++){
        if(!strcmp(argv[i],"-g") || !strcmp(argv[i],"--guidlines")){
            //TODO: check if program arg is a number
            char guidelines_path[500];
            sprintf(guidelines_path,"cat %s/prog%i/guidelines", PDIR, atoi(argv[i+1]));
            system(guidelines_path);
            return 0;
        }
    }

    //find filename
    for(int i=1;i<argc;i++){
        printf("%s", argv[i]);
        if(strstr(argv[i], ".c")!=NULL || strstr(argv[i],".cpp")!=NULL)
            strcpy(filename, argv[i]);
        printf("%s", filename);
    }

    //find param to print differece between files
    for(int i=1;i<argc;i++){
        if(!strcmp(argv[i],"-pd") || !strcmp(argv[i],"--print-difference")){
            print_diff_bool = 1;
        }
    }

    //Compile
    compile(filename);
    //get program number from last 3rd letter of filename
    if(progn==-1)
        progn = filename[strlen(filename)-3]-48;
    char progn_dir[500] = "";
    char ip_dir[500] = "";
    char op_dir[500];
    char sl_dir[500];


    strcpy(progn_dir, PDIR);
    strcat(progn_dir,"/prog");
    //puts(progn_dir);
    int path_size = strlen(progn_dir);
    progn_dir[path_size]=progn+48;
    progn_dir[path_size+1]='\0';
    //puts(progn_dir);


    //Generate op_dir and sl_dir from ip_dir
    strcpy(ip_dir, progn_dir);
    strcpy(op_dir,ip_dir);
    strcpy(sl_dir, ip_dir);

    strcat(ip_dir, "/ip/ip");
    strcat(op_dir, "/op/op");
    strcat(sl_dir, "/sl/sl");


    path_size = strlen(ip_dir);
    int key_pos = path_size;
    ip_dir[key_pos] = progn+48;
    ip_dir[path_size+1]='\0';

    op_dir[path_size] = progn+48;
    op_dir[path_size+1]='\0';

    sl_dir[path_size] = progn+48;
    sl_dir[path_size+1]='\0';

    path_size++;

    char sys_cmd[500];

    int no_ip=0;
    while(file_exists(ip_dir)){
        no_ip++;
        sprintf(sys_cmd,"./prog.out < %s > %s", ip_dir, op_dir);
        system(sys_cmd);

        ip_dir[key_pos]++;
        op_dir[key_pos]++;
        //puts(ip_dir);
    }


    test_prog(no_ip, op_dir, sl_dir, print_diff_bool);
    //system("cat diff.txt");
    system("rm prog.out");
    return 0;

}

int compile(char * filename){
    char sys_cmd[500];
    puts(PDIR);
    sprintf(sys_cmd, "gcc -O0 -std=c99 %s -o prog.out", filename);
    puts("Compiling...");
    system(sys_cmd);
    system("rm compile_err");
    //puts(sys_cmd);
    //check if comiled successfully]
    if(!file_exists("prog.out")){
        puts(ANSI_COLOR_RED "Compilation Failed.\n"ANSI_COLOR_RESET);
        exit(2);
    }
    //fclose(exec);
    puts(ANSI_COLOR_GREEN"Compilation Successful\n" ANSI_COLOR_RESET);
    return 0;

}

int file_exists(char* path){
    if(access(path, F_OK)!=-1)
        return 1;
    return 0;
}


void test_prog(int no_ip, char * op_dir, char * sl_dir, int print_diff_bool){

    int path_size = strlen(op_dir);
    int key_pos = path_size-1;
    for(int i=0; i<no_ip; i++){
        op_dir[key_pos] = sl_dir[key_pos] = i+48;
        //char test_no_str[50];
        if(!file_cmp(op_dir, sl_dir))
            printf(ANSI_COLOR_GREEN"Test:%i\n"ANSI_COLOR_RESET,i);
        else
            printf(ANSI_COLOR_RED"Test:%i\n"ANSI_COLOR_RESET,i);
        if(print_diff_bool)
            print_diff(op_dir, sl_dir);
    }
}

int print_diff(char * opdir, char * sldir){
    int err_flag=0;
    FILE * op = fopen(opdir,"r");
    FILE * sl = fopen(sldir,"r");
    char buffer1[100]="\0", buffer2[100]="\0";
    if(op==NULL){puts("ERROR: O/P file doesn't exist"); return 2;}
    if(sl==NULL){puts("ERROR: Solution file doesn't exist"); return 2;}

    printf("%-40s %-40s\r","YOUR OUTPUT", "EXPECTED OUTPUT");
    while(!feof(op) && !feof(sl)){
        //print likes side-by-side

        while(!feof(op)){
          fgets(buffer1, 100, op);
          buffer1[strcspn(buffer1, "\n")] = 0;
          if(buffer1[0]!='\0') break;
        }
        while(!feof(sl)){
          fgets(buffer2,100,sl);
          buffer2[strcspn(buffer2, "\n")] = 0;
          if(buffer2[0]!='\0') break;
        }


        if(feof(op) || feof(sl)) break;


        if(err_flag==0 && strcmp(buffer1,buffer2)) err_flag=1;

        if(err_flag==0)
        printf(ANSI_COLOR_GREEN"%-40s %-40s\r" ANSI_COLOR_RESET,buffer1, buffer2);
        else if(err_flag==1){
            printf(ANSI_COLOR_RED"%-40s %-40s\r" ANSI_COLOR_RESET,buffer1, buffer2);
            err_flag=2;
        }
        else if(err_flag==2)
            printf(ANSI_COLOR_YELLOW"%-40s %-40s\r",buffer1, buffer2);
    }
    //printf("");
    return 0;
}

int file_cmp(char * opdir, char * sldir){
    FILE * op = fopen(opdir,"r");
    FILE * sl = fopen(sldir,"r");
    char buffer1[100]="\0", buffer2[100]="\0";
    if(op==NULL){puts("ERROR: O/P file doesn't exist"); return 2;}
    if(sl==NULL){puts("ERROR: Solution file doesn't exist"); return 2;}

    while(!feof(op) && !feof(sl)){
      //get line and remove trailing \n from fgets
      while(!feof(op)){
        fgets(buffer1, 100, op);
        buffer1[strcspn(buffer1, "\n")] = 0;
        if(buffer1[0]!='\0') break;
      }
      while(!feof(sl)){
        fgets(buffer2,100,sl);
        buffer2[strcspn(buffer2, "\n")] = 0;
        if(buffer2[0]!='\0') break;
      }

      if(feof(op) || feof(sl)) break;
      if(strcmp(buffer1,buffer2)) return 1;
    }

    //remove newlines at end of file
    while(!feof(op)){
      fgets(buffer1, 100, op);
      if(buffer1[0]!='\n') break;
    }
    while(!feof(sl)){
      fgets(buffer2,100,sl);
      if(buffer2[0]!='\n') break;
    }

    //if both reach EOF at the same time
    printf("OP: %i, SL: %i\n %s %s\n", feof(op), feof(sl), buffer1, buffer2);
    if(feof(op) && feof(sl)) return 0;
    //if only one of the files has reached EOF
    return 1;

}
