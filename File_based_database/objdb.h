#ifndef _OBJDB_H
#define _OBJDB_H

int read_size(int start_pos,char buffer[], int fd, int size);
int write_size(int start_pos,char buffer[], int fd, int size);
extern int open_file(char fname[], int fd_database);
extern int find(char objname[], int fd_database);
extern int find_main(char namepart[], int fd_database,char ***return_strings);
extern int import_file(char fname[], char objname[],int fd_database);
extern int delete_file(char objname[], int fd_database);
extern int close_file(int fd_database);
extern void quit(int fd_database, int close_res);
extern int export_file(char objname[], char fname[], int fd_database);

#endif