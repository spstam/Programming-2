#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdlib.h>
#include "objdb.h"

int main(){
    char *fname;
    int open_res, import_res, export_res, delete_res,i;
    int close_res=1;
    int fd_database=0;
    char *db_name;
    char choice, help_char;
    char objname[256], namepart[256];
    int find_res;

    while(1){
        scanf(" %c", &choice);
        switch(choice){
            case 'o':
                scanf("%ms", &db_name);
                open_res=open_file(db_name, fd_database);
                if(open_res==-2){
                    fprintf(stderr,"\nError opening %s.\n", db_name);
                }else if(open_res==-1){
                    fprintf(stderr,"\nInvalid db file %s.\n", db_name);
                }else if(open_res==42){
                    free(db_name);
                    return 42;
                }else{
                    fd_database=open_res;
                    close_res=0;
                }
                free(db_name);
                break;
            case 'i':
                scanf("%ms %s", &fname, objname);
                import_res=import_file(fname, objname, fd_database);
                switch(import_res){
                    case -1:
                        fprintf(stderr, "\nNo open db file.\n");
                        break;
                    case -2:
                        fprintf(stderr,"\nFile %s not found.\n", fname);
                        break;
                    case -3:
                        fprintf(stderr,"\nObject %s already in db.\n", objname);
                        break;
                    case 42:
                        free(fname);
                        return 42;
                }
                free(fname);
                break;
            case 'f':
                scanf("%s", namepart);
                if(fd_database==0){
                    fprintf(stderr, "\nNo open db file.\n");
                }else{
                    char **return_strings=(char **)malloc(sizeof(char *));
                    find_res=find_main(namepart, fd_database, &return_strings);
                    if(find_res==-1){
                        free(return_strings);
                        return 42;
                    }else{
                        printf("\n##\n");
                        for(i=0;i<find_res;i++){
                            printf("%s\n", return_strings[i]);
                        }
                        for(i=0;i<find_res;i++){
                            free(return_strings[i]);
                        }
                    }
                    free(return_strings);      
                }
                break;
            case 'e':
                scanf("%s %ms", objname, &fname);
                export_res=export_file(objname, fname, fd_database);
                switch(export_res){
                    case -1:
                        fprintf(stderr,"\nNo open db file.\n");
                        break;
                    case -2:
                        fprintf(stderr,"\nCannot open file %s.\n", fname);
                        break;
                    case -3:
                        fprintf(stderr,"\nObject %s not in db.\n", objname);
                        break;
                    case 42:
                        free(fname);  
                        return 42;
                }  
                free(fname);  
                break;
            case 'd':
                scanf("%s", objname);
                delete_res=delete_file(objname, fd_database);
                if(delete_res==-1){
                    fprintf(stderr,"\nNo open db file.\n");
                }else if(delete_res==-2){
                    fprintf(stderr,"\nObject %s not in db.\n", objname);
                }else if(delete_res==42){
                    return 42;
                }
                break;
            case 'c':
                close_res=close_file(fd_database);
                if(close_res==-1){
                    fprintf(stderr,"\nNo open db file.\n");
                }   
                fd_database=0;
                break;
            case 'q':
                quit(fd_database, close_res);
                return 0;
                break;
            default:
              fprintf(stderr,"\nInvalid command.\n");
                do{
                  scanf("%c", &help_char);
                }while(help_char!='\n');
                break;
        }
    }
}
