#include "objdb.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int read_size(int start_pos,char buffer[], int fd, int size){
    int check;
    int total_read=0;
    int left2read=size;

    memset(buffer,0,size);
    check=lseek(fd,start_pos,SEEK_SET);
    if(check<0){
        return -1;
    }

    while(total_read<size){
        left2read= size - total_read;
        check=read(fd,buffer+total_read,left2read);
        if(check<=0){
            return -1;
        }
        total_read+=check;
        
    }
    return 0;
}

int write_size(int start_pos,char buffer[], int fd, int size){
    int check;

    check=lseek(fd,start_pos,SEEK_SET);
    if(check<0){
        return -1;
    }
    write(fd,buffer,size);
    return 0;
}

int open_file(char db_name[], int fd_database){
    char buffer[9]={'\0'};
    int fd,n,check;
    char code[9]= "a1b2c3d4";

    if(fd_database!=0){
        close(fd_database);
    }

    fd=open(db_name, O_RDWR | O_CREAT, 0644);
    if(fd<0){
        return -2;
    }
    //check authenticity
    n=read(fd, buffer, sizeof(buffer)-1);
    if(n<0){
        return 42;
    }
    buffer[8]='\0';

    if(n!=0 && strcmp(buffer,code)!=0){
        return -1;
    }else if(strcmp(buffer,code)==0){
        return fd;
    }else if(n==0){
        //authenticate
        check=write(fd, code, sizeof(code));
        if(check<0){
            return 42;
        }
        return fd;
    }else{
        return -1;
    }
}

int find(char objname[], int fd_database){
    int counter_pos=0;
    int n, lenght,check;
    char buffer[256]={'\0'};

    check=lseek(fd_database, 9, SEEK_SET);
    if(check<0){
        return 42;
    }
    counter_pos+=9;

    while((n=read(fd_database, &lenght, sizeof(int)))!=0){
        memset(buffer, '\0', sizeof(buffer));
        if(n<0){
            return 42;
        }
        check=read(fd_database, buffer, lenght);
        if(check<0){
            return 42;
        }
        if(strcmp(buffer,objname)==0){
            return counter_pos;
        }else{
            counter_pos+=n+lenght;
            n=read(fd_database, &lenght, sizeof(int));
            if(n<0){
                return 42;
            }
            check=lseek(fd_database, lenght, SEEK_CUR);
            if(check<0){
                return 42;
            }
            counter_pos+=(n+lenght);
        }
    }
    return -1;
}

int find_main(char namepart[], int fd_database, char ***return_strings){
    int counter_pos=0;
    int n, lenght,check;
    char buffer[256];
    char **help_ptr=NULL;
    int i=0;
    int j;
    memset(buffer,0,sizeof(buffer));


    check=lseek(fd_database, 9, SEEK_SET);
    if(check<0){
        return -1;
    }
    counter_pos+=9;
    if(strcmp(namepart,"*")==0){
        while((n=read(fd_database, &lenght, sizeof(int))!=0)){
            if(n<0){
                return -1;
            }
            check=read(fd_database,buffer,lenght);
            if(check<0){
                return -1;
            }
            
            help_ptr=(char **)realloc((*return_strings),(i+1)*sizeof(char *));
            if(help_ptr==NULL){
                for(j=0;j<i;j++){
                    free((*return_strings)[j]);
                }
                return -1;
            }else{
                (*return_strings)=help_ptr;
            }

            (*return_strings)[i]=(char *)malloc(256*sizeof(char));
            strcpy((*return_strings)[i],buffer);
            check=read(fd_database, &lenght, sizeof(int));
            if(check<0){
                return -1;
            }

            check=lseek(fd_database,lenght, SEEK_CUR);
            if(check<0){
                return -1;
            }
            memset(buffer,0,sizeof(buffer));
            i++;
        }
    }else{
        while((n=read(fd_database, &lenght, sizeof(int))!=0)){
            if(n<0){
                return -1;
            }
            check=read(fd_database,buffer,lenght);
            if(check<0){
                return -1;
            }
            
            if(strstr(buffer,namepart)!=NULL){
                help_ptr=(char **)realloc((*return_strings),(i+1)*sizeof(char *));
                if(help_ptr==NULL){
                    for(j=0;j<i;j++){
                        free((*return_strings)[j]);
                    }
                    return -1;
                }else{
                    (*return_strings)=help_ptr;
                }
                (*return_strings)[i]=(char *)malloc(256*sizeof(char));
                strcpy((*return_strings)[i],buffer);
                i++;
            }
            
            check=read(fd_database, &lenght, sizeof(int));
            if(check<0){
                return -1;
            }
            check=lseek(fd_database,lenght, SEEK_CUR);
            if(check<0){
                return -1;
            }
            memset(buffer,0,sizeof(buffer));
        }
    }
    return i;
}

int import_file(char fname[], char objname[],int fd_database){
    int fd,find_res, namelenght,cur_pos, n,check;
    int lenght=0;
    char buffer[512]={'\0'};

    if(fd_database==0){
        return -1;
    }

    fd=open(fname, O_RDONLY);
    //file doesn't exist
    if(fd<0){
        return -2;
    }
    find_res=find(objname, fd_database);
    if(find_res==42){
        return 42;
    }
    //object exists
    if(find_res>0){
        close(fd);
        return -3;
    }

    check=lseek(fd_database,0,SEEK_END);
    if(check<0){
        return 42;
    }
    namelenght=strlen(objname);
    check=write(fd_database, &namelenght, sizeof(int));
    if(check<0){
        return 42;
    }
    check=write(fd_database, objname, namelenght);
    if(check<0){
        return 42;
    }
    
    cur_pos=lseek(fd, 0, SEEK_END);
    if(cur_pos<0){
        return 42;
    }
    check=lseek(fd,0,SEEK_SET);
    if(check<0){
        return 42;
    }
    lenght=cur_pos;

    check=write(fd_database, &lenght, sizeof(int));
    if(check<0){
        return 42;
    }

    while((n = read(fd, buffer, sizeof(buffer)))!=0){
        if(n<0){
            return 42;
        }
        check=write(fd_database, buffer, n);
        if(check<0){
            return 42;
        }
    }
    close(fd);
    return 0;
}

int delete_file(char objname[], int fd_database){
    int find_res,lenght,obj_pos;
    char buffer[512];
    int check;
    int count=0;
    int read_res;
    int cur_size;
    int write_res,i;
    memset(buffer,'\0',sizeof(buffer));
    
    if(fd_database==0){
        return -1;
    }

    find_res=find(objname, fd_database);
    if(find_res==42){
        return 42;
    }
    if(find_res<0){
        return -2;
    }

    cur_size=lseek(fd_database,0,SEEK_END);
    if(cur_size<0){
        return 42;
    }

    check=lseek(fd_database, find_res, SEEK_SET);
    if(check<0){
        return 42;
    }

    //skip name
    check=read(fd_database, &lenght, sizeof(int));
    if(check<0){
        return 42;
    }

    count+=check;
    check=lseek(fd_database, lenght, SEEK_CUR);
    if(check<0){
        return 42;
    }
    count+=lenght;

    //skip file
    check=read(fd_database,&lenght, sizeof(int));
    if(check<0){
        return 42;
    }

    count+=check;
    
    check=lseek(fd_database, lenght, SEEK_CUR);
    if(check<0){
        return 42;
    }
    count+=lenght;

    if(cur_size==(count+find_res)){
        check=ftruncate(fd_database,find_res);
        if(check<0){
            return 42;
        }
        return 0;
    }
    obj_pos=find_res;

    for(i=0;i<(cur_size-obj_pos-count)/512;i++){
        read_res=read_size(find_res+count,buffer,fd_database,512);
        if(read_res==-1){
            return 42;
        }
        
        write_res=write_size(find_res,buffer,fd_database,512);
        if(write_res==-1){
            return 42;
        }
        
        memset(buffer,0,sizeof(buffer));
        find_res+=512;
    }

    read_res=read_size(find_res+count,buffer,fd_database,(cur_size-find_res-count)%512);
    if(read_res==-1){
        return 42;
    }

    write_res=write_size(find_res,buffer,fd_database,(cur_size-find_res-count)%512);
    if(write_res==-1){
        return 42;
    }

    check=ftruncate(fd_database,cur_size-count);
    if(check<0){
        return 42;
    }

    return 0;
}

int close_file(int fd_database){
    if(fd_database==0){
        return -1;
    }else{
        close(fd_database);
        return 1;
    } 
}

void quit(int fd_database, int close_res){
    if(close_res==0){
        close_file(fd_database);
    }
}

int export_file(char objname[], char fname[], int fd_database){
    int find_res, fd_help, i;
    int lenght=0;
    char buffer[512];
    int check;
    int read_res;
    int write_res;
    int count=0;
    int write_pos=0;
    memset(buffer,'\0',sizeof(buffer));
    
    if(fd_database==0){
        return -1;
    }

    find_res=find(objname, fd_database);
    if(find_res==42){
        return 42;
    }
    if(find_res<0){
        //obj not found
        return -3;
    }

    fd_help=open(fname, O_RDONLY);
    if(fd_help!=-1){
        //file exists
        return -2;
    }else{
        close(fd_help);
    }

    fd_help=open(fname, O_WRONLY | O_CREAT, 0644);
    if(fd_help<0){
        //file can't be created
        return -2;
    }

    //goto obj
    check=lseek(fd_database, find_res, SEEK_SET);
    if(check<0){
        return 42;
    }
    //skip name
    check=read(fd_database, &lenght, sizeof(int));
    if(check<0){
        return 42;
    }
    count+=check;

    check=lseek(fd_database, lenght, SEEK_CUR);
    if(check<0){
        return 42;
    }
    count+=lenght;

    //read size of file
    check=read(fd_database, &lenght, sizeof(int));
    if(check<0){
        return 42;
    }
    count+=check;

    for(i=0;i<lenght/512;i++){       
        read_res=read_size(find_res+count,buffer,fd_database,512);
        if(read_res==-1){
            return 42;
        }
        write_res=write_size(write_pos,buffer,fd_help,512);
        if(write_res==-1){
            return 42;
        }
        count+=512;
        write_pos+=512;
        memset(buffer,'\0',sizeof(buffer));
    }

    read_res=read_size(find_res+count,buffer,fd_database,lenght%512);
    if(read_res==-1){
        return 42;
    }
    write_res=write_size(write_pos,buffer,fd_help,lenght%512);
    if(write_res==-1){
        return 42;
    }

    close(fd_help);
    return 0;
}