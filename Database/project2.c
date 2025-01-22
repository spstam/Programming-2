/* ONOMATEPWNYMO: Stamoulis Spyridon
   AEM:03775
*/
#include"project2.h"
#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<string.h>

typedef struct name{
  char first_name[NAMESIZE];
  char sirname[NAMESIZE];
}student_name;

typedef struct subj_list{
  unsigned short int subj_code;
  struct subj_list *next;
}subject_list;

typedef struct student{
  unsigned long int aem;
  student_name name;
  unsigned short int num_owe_subj;
  subject_list *head;
  struct student *next;
  struct student *prev;
}credentials;

typedef struct hash{
  credentials *ptr_to_struct;
  int number;
}hash_struct;

typedef struct student_info{
  /*student info useful for basic functions 
  such as add,rmv...*/
  int courses;
  unsigned long int aem;
  student_name name;
}stud_info;


typedef struct add_parameters{
  //general info for the whole program
  int table_size;
  int num_of_students;
  int min_hash_size;
  int hash_size;
  int sorted;
  credentials **ptr_ptr_table;
  hash_struct *hash_table;
  int step;
  stud_info student;

  /*useful for handling the subject list
  of each student*/
  unsigned short int subject_code;
  
  //useful for find and sorted to print in stderr
  unsigned long int comparisons;
  
  /*useful for find in rmv so we know the possition of
  the subject we need to rmv*/
  int position_rmv;
}add_param;

/************************
Find student through aem
*************************/
credentials *find(add_param *parameters){
  int i;
  parameters->comparisons=0;

  for(i=0;i<parameters->num_of_students;i++){
    parameters->comparisons++;
    if(parameters->ptr_ptr_table[i]->aem==parameters->student.aem){
        parameters->position_rmv=i;
        return parameters->ptr_ptr_table[i];
    }
  }
  return NULL;
}

credentials *find_sorted(add_param *parameters){
    int left=0;
    int right=parameters->num_of_students-1;
    int middle;
    parameters->comparisons=0;
    
    while(left<=right){
        middle=(left+right)/2;
        if(parameters->student.aem==parameters->ptr_ptr_table[middle]->aem){
            parameters->comparisons++;
            parameters->position_rmv=middle;
            return (parameters->ptr_ptr_table[middle]);
        }
        if(parameters->student.aem>parameters->ptr_ptr_table[middle]->aem){
            parameters->comparisons= parameters->comparisons + 2;
            left=middle+1;
        }else if(parameters->student.aem<parameters->ptr_ptr_table[middle]->aem){
            parameters->comparisons= parameters->comparisons + 2;
            right=middle-1;
        }
    }
    return NULL;
}

/******
rehash
******/
unsigned long int hash(char *str){
  unsigned long int hash = 5381;
  int c;

  while ((c = *str++)) {
    hash = ((hash << 5) + hash) + c;
  }
  return hash;
}

void make_hash_table(add_param *parameters){
  unsigned long int categorize;
  int i;
  credentials *curr;

  for(i=0;i<parameters->num_of_students;i++){

    categorize=hash(parameters->ptr_ptr_table[i]->name.sirname)%(parameters->hash_size);

    for(curr=parameters->hash_table[categorize].ptr_to_struct->next;
    curr!=parameters->hash_table[categorize].ptr_to_struct; curr=curr->next){

        if((strcmp(curr->name.sirname,parameters->ptr_ptr_table[i]->name.sirname)<0)||
        ((strcmp(curr->name.sirname,parameters->ptr_ptr_table[i]->name.sirname)==0)&&
        (curr->aem < parameters->ptr_ptr_table[i]->aem))){
            continue;
        }
        else{
            break;            
        }
    }
    parameters->ptr_ptr_table[i]->next=curr;
    parameters->ptr_ptr_table[i]->prev=curr->prev;
    curr->prev->next=parameters->ptr_ptr_table[i];
    curr->prev=parameters->ptr_ptr_table[i];
    parameters->hash_table[categorize].number++;
  }
} 

/***************
Add new student
****************/
credentials **add(add_param *parameters){
    credentials **help_ptr=NULL;
    credentials *find_result,*curr;
    hash_struct *help_pointer;
    unsigned long int categorize;
    int i,j;

    if(parameters->num_of_students!=0){
      if(parameters->sorted==0){
        find_result=find(parameters);
      }else{
        find_result=find_sorted(parameters);
      }
      if(find_result!=NULL){
          return NULL;
      }
    }
    if(parameters->num_of_students>= parameters->table_size){
        help_ptr=(credentials **)realloc(parameters->ptr_ptr_table,(parameters->table_size+parameters->step)*sizeof(credentials *));
        if(help_ptr==NULL){
            return NULL;
        }else{
            parameters->ptr_ptr_table=help_ptr;
        }

        for(i=parameters->table_size;i<parameters->table_size+parameters->step;i++){
            parameters->ptr_ptr_table[i]=NULL;
        }
        parameters->table_size+=parameters->step;
    }

    i=parameters->num_of_students;

    parameters->ptr_ptr_table[i]=(credentials *)malloc(sizeof(credentials));
    if(parameters->ptr_ptr_table[i]==NULL){
      if(help_ptr==NULL){
        parameters->table_size-=parameters->step;
        for(j=parameters->table_size;j<parameters->step+parameters->table_size;j++){
          free(parameters->ptr_ptr_table[j]);
        }
      }
      return NULL;
    }else{
        parameters->ptr_ptr_table[i]->aem=parameters->student.aem;
        strncpy(parameters->ptr_ptr_table[i]->name.first_name,parameters->student.name.first_name,NAMESIZE);
        strncpy(parameters->ptr_ptr_table[i]->name.sirname,parameters->student.name.sirname,NAMESIZE);
        parameters->ptr_ptr_table[i]->num_owe_subj=parameters->student.courses;
        parameters->num_of_students=parameters->num_of_students+1;
        parameters->sorted=0;
        parameters->ptr_ptr_table[i]->head=NULL;


      categorize=hash(parameters->student.name.sirname)%(parameters->hash_size);

        for(curr=parameters->hash_table[categorize].ptr_to_struct->next;
        curr!=parameters->hash_table[categorize].ptr_to_struct; curr=curr->next){
            if((strcmp(curr->name.sirname,parameters->student.name.sirname)<0)||((strcmp(curr->name.sirname,parameters->student.name.sirname))==0
            && (curr->aem < parameters->student.aem))){
                continue;
            }else{
                break;
            }
        }
        parameters->ptr_ptr_table[i]->next=curr;
        parameters->ptr_ptr_table[i]->prev=curr->prev;
        curr->prev->next=parameters->ptr_ptr_table[i];
        curr->prev=parameters->ptr_ptr_table[i];
        parameters->hash_table[categorize].number++;

      if(parameters->num_of_students/(double)parameters->hash_size>=HIGH_LOAD){
        help_pointer=(hash_struct*)realloc(parameters->hash_table,sizeof(hash_struct)*(parameters->hash_size)*2);
        if(help_pointer!=NULL){
          parameters->hash_table=help_pointer;
          parameters->hash_size=parameters->hash_size*2;
        
          for(i=parameters->hash_size/2; i<parameters->hash_size;i++){
            parameters->hash_table[i].ptr_to_struct=(credentials*)malloc(sizeof(credentials)*1);
          }

          for(i=0;i<parameters->hash_size;i++){
            parameters->hash_table[i].number=0;
            parameters->hash_table[i].ptr_to_struct->aem=0;
            parameters->hash_table[i].ptr_to_struct->num_owe_subj=0;
            strcpy(parameters->hash_table[i].ptr_to_struct->name.sirname,"\0");
            strcpy(parameters->hash_table[i].ptr_to_struct->name.first_name,"\0");
            parameters->hash_table[i].ptr_to_struct->next=parameters->hash_table[i].ptr_to_struct;
            parameters->hash_table[i].ptr_to_struct->prev=parameters->hash_table[i].ptr_to_struct;
          }      
          make_hash_table(parameters);
        }

      }
    }
    return parameters->ptr_ptr_table;
}

/**********************
Print all student info
**********************/
void print(add_param *parameters){
    int i;

    printf("\n##\n");
    for(i=0;i<parameters->table_size;i++){
        if(parameters->ptr_ptr_table[i]==NULL){
            continue;
        }else{
            printf("%lu ", parameters->ptr_ptr_table[i]->aem);
            printf("%s ", parameters->ptr_ptr_table[i]->name.first_name);
            printf("%s ", parameters->ptr_ptr_table[i]->name.sirname);
            printf("%d\n", parameters->ptr_ptr_table[i]->num_owe_subj);
        }
    }

}

/***********************
Modify a student's info
************************/
int mod(add_param *parameters){
  credentials *find_result;

  if(parameters->sorted==0){
    find_result=find(parameters);
  }else{
    find_result=find_sorted(parameters);
  }
  if(find_result==NULL){
    return -1;
  }

  find_result->num_owe_subj=parameters->student.courses;
  return 0;
}

/***********************
Clear all students' info
************************/
void clear(add_param *parameters){
    int i;
    credentials **help_ptr;
    subject_list *curr, *temp;
    hash_struct *help_pointer;

    for(i=0;i<parameters->num_of_students;i++){
        curr=parameters->ptr_ptr_table[i]->head;
        while(curr!=NULL){
          temp=curr;
          curr=curr->next;
          free(temp);
        }
        free(parameters->ptr_ptr_table[i]);
        parameters->ptr_ptr_table[i]=NULL;
    }
    parameters->num_of_students=0;
    parameters->table_size=0;
    help_ptr=(credentials **)realloc(parameters->ptr_ptr_table,0);
    parameters->ptr_ptr_table=help_ptr;
    //free sentinels
    for(i=parameters->min_hash_size;i<parameters->hash_size;i++){
        free(parameters->hash_table[i].ptr_to_struct);
    }
    help_pointer=(hash_struct*)realloc(parameters->hash_table,sizeof(hash_struct)*(parameters->min_hash_size));
    parameters->hash_table=help_pointer;
    parameters->hash_size=parameters->min_hash_size;
    for(i=0;i<parameters->hash_size;i++){
        parameters->hash_table[i].number=0;
        parameters->hash_table[i].ptr_to_struct->aem=0;
        parameters->hash_table[i].ptr_to_struct->num_owe_subj=0;
        strcpy(parameters->hash_table[i].ptr_to_struct->name.sirname,"\0");
        strcpy(parameters->hash_table[i].ptr_to_struct->name.first_name,"\0");
        parameters->hash_table[i].ptr_to_struct->next=parameters->hash_table[i].ptr_to_struct;
        parameters->hash_table[i].ptr_to_struct->prev=parameters->hash_table[i].ptr_to_struct;
    } 
}

/************
Quit program
************/
void quit(add_param *parameters){
    int i;

    clear(parameters);
    for(i=0;i<parameters->hash_size;i++){
      free(parameters->hash_table[i].ptr_to_struct);
    }
    free(parameters->hash_table);

    free(parameters->ptr_ptr_table);
    parameters->ptr_ptr_table=NULL; 
}

/***********************************
Sort students's info using their aem
************************************/
void sort(add_param *parameters) {
  credentials *temp;
  int i, j;

  parameters->comparisons=0;

  for (i = 1; i < parameters->num_of_students; i++) {
    temp = parameters->ptr_ptr_table[i];
    j = i - 1;
    while (j >= 0) {
      parameters->comparisons++;
      if(parameters->ptr_ptr_table[j]->aem <= temp->aem){
        break;
      }else{
        parameters->ptr_ptr_table[j+1] = parameters->ptr_ptr_table[j];
        j = j - 1;
      }
    }
    parameters->ptr_ptr_table[j+1] = temp;
  }
  //sorted
  parameters->sorted=1;
}

/***********************
Remove a student's info
***********************/
int rmv(add_param *parameters){
  credentials *find_result;
  credentials **help_ptr;
  long unsigned int categorize;
  subject_list *curr,*temp;
  int i;
  hash_struct *help_pointer;

  if(parameters->sorted==0){
    find_result=find(parameters);
  }else{
    find_result=find_sorted(parameters);
  }
  if(find_result==NULL){
    return -1;
  }else{
    i=parameters->position_rmv;
    curr=find_result->head;
    while(curr!=NULL){
      temp=curr;
      curr=curr->next;
      free(temp);
    }
    //rmv from hash table
    categorize=hash(find_result->name.sirname)%parameters->hash_size;
    parameters->hash_table[categorize].number--;

    find_result->next->prev=find_result->prev;
    find_result->prev->next=find_result->next;
    free(find_result);

    parameters->ptr_ptr_table[i]=parameters->ptr_ptr_table[parameters->num_of_students-1];
    parameters->ptr_ptr_table[parameters->num_of_students-1]=NULL;
    parameters->sorted=0;
    parameters->num_of_students=parameters->num_of_students-1;
    
    
    if((parameters->num_of_students/(double)parameters->hash_size<=LOW_LOAD)&&
      ((parameters->hash_size/2)>=(parameters->min_hash_size))){
        //free sentinels
        for(i=parameters->hash_size/2;i<parameters->hash_size;i++){
            free(parameters->hash_table[i].ptr_to_struct);
        }
        help_pointer=(hash_struct*)realloc(parameters->hash_table,sizeof(hash_struct)*(parameters->hash_size/2));
        if(help_pointer!=NULL){
          parameters->hash_table=help_pointer;
          parameters->hash_size=parameters->hash_size/2;

          for(i=0;i<parameters->hash_size;i++){
            parameters->hash_table[i].number=0;
            parameters->hash_table[i].ptr_to_struct->aem=0;
            parameters->hash_table[i].ptr_to_struct->num_owe_subj=0;
            strcpy(parameters->hash_table[i].ptr_to_struct->name.sirname,"\0");
            strcpy(parameters->hash_table[i].ptr_to_struct->name.first_name,"\0");
            parameters->hash_table[i].ptr_to_struct->next=parameters->hash_table[i].ptr_to_struct;
            parameters->hash_table[i].ptr_to_struct->prev=parameters->hash_table[i].ptr_to_struct;
          }
          make_hash_table(parameters);
        }
      }
      

    if(parameters->num_of_students+parameters->step <= parameters->table_size){
      help_ptr=(credentials **)realloc(parameters->ptr_ptr_table,(parameters->table_size-parameters->step)*sizeof(credentials *));
        parameters->ptr_ptr_table=help_ptr;
        parameters->table_size=parameters->table_size - parameters->step;
    }   
    return 0;
  }
}

/*******************
Allocate init memory
********************/
credentials **get_init_memory(int starting_size){
  credentials **ptr;

  ptr=(credentials **)malloc(starting_size*sizeof(credentials *));

  return (ptr); 
}

/******************************
Turn words' letters to capital
******************************/
void capital(char *name){
  int capital_loop;

  for(capital_loop=0;capital_loop<strlen(name);capital_loop++){
    name[capital_loop]=toupper(name[capital_loop]);
  }
}

/****************************
Find subject in subject list
****************************/
subject_list *isreg(add_param *parameters){
  credentials *find_result;
  subject_list *curr;

  if(parameters->sorted==1){
    find_result=find_sorted(parameters);
  }else{
    find_result=find(parameters);
  }

  if(find_result==NULL){
    return NULL;
  }else{
    if(find_result->head==NULL){
      return NULL;
    }
    if(find_result->head->subj_code>=parameters->subject_code){
      return NULL;
    }
    for(curr=find_result->head; curr->next != NULL;curr=curr->next){
      if(curr->next->subj_code==parameters->subject_code){
        return curr;
      }else{
        if(curr->next->subj_code > parameters->subject_code){
          return curr;
        }
      }
    }
    return curr;
  }
}

/***************************
Add subject in subject list
***************************/
int reg(add_param *parameters){
  credentials *find_result;
  subject_list *isreg_result;
  subject_list *temp;

  if(parameters->sorted==1){
    find_result=find_sorted(parameters);
  }else{
    find_result=find(parameters);
  }

  if(find_result==NULL){
    return -1;
  }else{
    isreg_result=isreg(parameters);
    //
    if(isreg_result==NULL){
      if(find_result->head==NULL){
        find_result->head=(subject_list*)malloc(sizeof(subject_list));
        if(find_result->head==NULL){
          return -2;
        }else{
          find_result->head->next=NULL;
          find_result->head->subj_code=parameters->subject_code;
          return 1;
        }
      }
      if(find_result->head->subj_code==parameters->subject_code){
        return -2;
      }
      if(find_result->head->subj_code>parameters->subject_code){
        temp=find_result->head;
        find_result->head=(subject_list*)malloc(sizeof(subject_list));
        if(find_result->head==NULL){
          return -3;
        }else{
          find_result->head->next=temp;
          find_result->head->subj_code=parameters->subject_code;
          return 1;
        }
      }
    }
    if(isreg_result->next!=NULL && isreg_result->next->subj_code==parameters->subject_code){
      return -2;
    }else{
      //return previous from correct place kombo isreg
      temp=isreg_result->next;
      isreg_result->next=(subject_list*)malloc(sizeof(subject_list));
      if(isreg_result->next==NULL){
        return -3;
      }else{
        isreg_result->next->next=temp;
        isreg_result->next->subj_code=parameters->subject_code;
        return 1;
      }
    }
  }
}

/********************************
Remove subject from subject list
*******************************/
int unreg(add_param *parameters){
  credentials *find_result;
  subject_list *isreg_result,*temp;

  if(parameters->sorted==1){
    find_result=find_sorted(parameters);
  }else{
    find_result=find(parameters);
  }

  if(find_result==NULL){
    return -1;
  }else{
    isreg_result=isreg(parameters);
    if(isreg_result==NULL){
      if(find_result->head==NULL){
        return -2;
      }else if(find_result->head->subj_code==parameters->subject_code){
        temp=find_result->head;
        find_result->head=find_result->head->next;
        free(temp);
        return 1;
      }else{
        return -2;
      }
    }
    if(isreg_result->next != NULL && isreg_result->next->subj_code==parameters->subject_code){
      temp=isreg_result->next;
      isreg_result->next=isreg_result->next->next;
      free(temp);
      return 1;
    }else{
      return -2;
    }
  }
}

/******************
Print subject list
*******************/
int list_courses(add_param *parameters){
  credentials *find_result;
  subject_list *curr;

  if(parameters->sorted==1){
    find_result=find_sorted(parameters);
  }else{
    find_result=find(parameters);
  }

  if(find_result==NULL){
    return -1;
  }

  printf("\n");
  printf("L-OK %s %lu\n", find_result->name.sirname, find_result->aem);
  for(curr=find_result->head; curr!=NULL; curr=curr->next){
    printf("%u\n", curr->subj_code);
  }
  return 1;
}

/*******************
Find student by name
********************/
void find_by_name(add_param *parameters){
  unsigned long int possition;
  int counter=0;//for init message
  long unsigned int comparisons=0;
  credentials *curr;

  possition=hash(parameters->student.name.sirname)%parameters->hash_size;

  for(curr=parameters->hash_table[possition].ptr_to_struct->next;
  curr!=parameters->hash_table[possition].ptr_to_struct;curr=curr->next){
    comparisons++;
    if(strcmp(curr->name.sirname,parameters->student.name.sirname)==0){
      if(counter==0){
        printf("\nN-OK %s\n", parameters->student.name.sirname);
      }
      printf("%s %lu %hd\n", curr->name.first_name, curr->aem, curr->num_owe_subj);
      counter++;
    }else if(counter!=0){
      break;
    }
  }
  if(counter==0){
    printf("\nN-NOK %s\n", parameters->student.name.sirname);
  } 
  fprintf(stderr, "\n@%lu\n", comparisons);
}

/********************************
Print all students by name order,
as they are saved in hash table
*********************************/
void print_by_name(add_param *parameters){
  int largest_bucket=0;
  int i;
  credentials *curr;

  for(i=0;i<parameters->hash_size;i++){
    if(parameters->hash_table[i].number>largest_bucket){
      largest_bucket=parameters->hash_table[i].number;
    }
  }

  printf("\n##\n");
  printf("%d %d %.2lf %d\n", parameters->hash_size, parameters->num_of_students,
  (double)parameters->num_of_students/parameters->hash_size,largest_bucket);

  for(i=0;i<parameters->hash_size;i++){
    printf("%d %d", i, parameters->hash_table[i].number);
    if(parameters->hash_table[i].number!=0){
      for(curr=parameters->hash_table[i].ptr_to_struct->next;
      curr!=parameters->hash_table[i].ptr_to_struct; curr=curr->next){
        printf(" [ %lu %s %s %hd ]", curr->aem, curr->name.first_name, curr->name.sirname, curr->num_owe_subj);
      }
    }
    printf("\n\n");
  }
}

/****
Main
*****/
int main (int argc, char *argv[]) {
    int starting_size,step,i;
    char choice;
    long unsigned int aem;
    int remove_result;
    credentials *find_result;
    char* first;
    char* last;
    short unsigned int courses;
    int mod_result;
    char def_help_char;
    credentials **ptr_ptr_table=NULL;
    credentials **add_result;
    add_param param,*parameters;
    parameters=&param;
    int reg_result, unreg_result, list_courses_res;
    subject_list *isreg_result;
    int hash_init_size;

    
    //terminate if wrong num of args
    if(argc!=4){
        return 42;
    }
    starting_size=atoi(argv[1]);
    step=atoi(argv[2]);
    hash_init_size=atoi(argv[3]);

    ptr_ptr_table=get_init_memory(starting_size);

    if(ptr_ptr_table==NULL){
        return 43;
    }
    //init ptr_table
    for(i=0;i<starting_size;i++){
        ptr_ptr_table[i]=NULL;
    }
    //init hash_table
    parameters->hash_table=(hash_struct*)malloc(sizeof(hash_struct)*hash_init_size);
    if(parameters->hash_table==NULL){
      return 43;
    }
    for(i=0;i<hash_init_size;i++){
      parameters->hash_table[i].ptr_to_struct=(credentials*)malloc(sizeof(credentials)*1);
      if(parameters->hash_table[i].ptr_to_struct==NULL){
        return 43;
      }
      parameters->hash_table[i].number=0;
      parameters->hash_table[i].ptr_to_struct->aem=0;
      parameters->hash_table[i].ptr_to_struct->num_owe_subj=0;
      strcpy(parameters->hash_table[i].ptr_to_struct->name.sirname,"\0");
      strcpy(parameters->hash_table[i].ptr_to_struct->name.first_name,"\0");
      parameters->hash_table[i].ptr_to_struct->next=parameters->hash_table[i].ptr_to_struct;
      parameters->hash_table[i].ptr_to_struct->prev=parameters->hash_table[i].ptr_to_struct;
    }
   
    parameters->min_hash_size=hash_init_size;
    parameters->hash_size=hash_init_size;
    //not sorted
    parameters->sorted=0;

    parameters->num_of_students=0;
    parameters->ptr_ptr_table=ptr_ptr_table;
    parameters->step=step;
    parameters->table_size=starting_size;

    while(1){
        scanf(" %c", &choice);
        switch(choice){
            case 'a':
                scanf("%lu", &aem);
                scanf("%ms", &first);
                scanf("%ms", &last);
                scanf("%hd", &courses);
                if(aem==0||courses==0){
                  printf("\nInvalid input.\n");
                  do{
                    scanf("%c", &def_help_char);
                  }while(def_help_char!='\n');
                  break;
                }
                if((strlen(first)>=NAMESIZE)&&(strlen(last)>=NAMESIZE)){
                  first[NAMESIZE-1]='\0';
                  last[NAMESIZE-1]='\0';
                  printf("\nTrimmed name(s).\n");
                }else if(strlen(last)>=NAMESIZE){
                  last[NAMESIZE-1]='\0';
                  printf("\nTrimmed name(s).\n");
                }else if(strlen(first)>=NAMESIZE){
                  first[NAMESIZE-1]='\0';
                  printf("\nTrimmed name(s).\n");
                }
                //toupper
                capital(first);
                capital(last);
                //success
                strncpy(parameters->student.name.sirname,last,NAMESIZE);
                strncpy(parameters->student.name.first_name,first,NAMESIZE);
                free(first);
                free(last);
                parameters->student.aem=aem;
                parameters->student.courses=courses;
                add_result=add(parameters);
                if(add_result!=NULL){
                    printf("\nA-OK %lu, %d %d\n", aem, parameters->num_of_students, parameters->table_size);
                }else{
                //fail
                printf("\nA-NOK %lu, %d %d\n", aem, parameters->num_of_students, parameters->table_size);
                }
                break;
            case 'r':
                scanf("%lu", &aem);
                if(aem==0){
                  printf("\nInvalid input.\n");
                  do{
                    scanf("%c", &def_help_char);
                  }while(def_help_char!='\n');
                  break;
                }
                parameters->student.aem=aem;
                remove_result=rmv(parameters);
                //success
                if(remove_result==0){
                    printf("\nR-OK %lu, %d %d\n", aem, parameters->num_of_students, parameters->table_size);
                }else{
                //fail
                    printf("\nR-NOK %lu, %d %d\n", aem, parameters->num_of_students, parameters->table_size);
                }
                    break;
            case 'm':
                scanf("%lu", &aem);
                scanf("%hd", &courses);
                if(aem==0||courses==0){
                  printf("\nInvalid input.\n");
                  do{
                    scanf("%c", &def_help_char);
                  }while(def_help_char!='\n');
                  break;
                }
                parameters->student.aem=aem;
                parameters->student.courses=courses;
                mod_result=mod(parameters);
                if(mod_result==0){
                    //success
                    printf("\nM-OK %lu\n", aem);
                }else{
                    //fail
                    printf("\nM-NOK %lu\n", aem);
                }
                break;
            case 's':
                sort(parameters);
                printf("\nS-OK\n");
                fprintf(stderr, "\n$%lu\n", parameters->comparisons);
                break;
            case 'f':
                scanf("%lu", &aem);
                if(aem==0){
                  printf("\nInvalid input.\n");
                  do{
                    scanf("%c", &def_help_char);
                  }while(def_help_char!='\n');
                  break;
                }
                parameters->student.aem=aem;
                //success
                if(parameters->sorted==0){
                  find_result=find(parameters);
                }else{
                  find_result=find_sorted(parameters);
                }
                if(find_result!=NULL){
                  printf("\nF-OK %s %s %d\n",find_result->name.first_name, find_result->name.sirname,find_result->num_owe_subj);
                  fprintf(stderr, "\n$%lu\n", parameters->comparisons);
                  find_result=NULL;
                }else{
                  //fail
                  printf("\nF-NOK %lu\n",aem);
                  fprintf(stderr, "\n$%lu\n", parameters->comparisons);
                }
                break;
            case 'p':
                print(parameters);
                break;
            case 'c':
                clear(parameters);
                printf("\nC-OK\n");
                break;
            case 'q':
                quit(parameters);
                return 0;
                break;
            case 'g':
                scanf("%lu %hd", &parameters->student.aem, &parameters->subject_code);
                if(parameters->student.aem==0||parameters->subject_code==0){
                  printf("\nInvalid input.\n");
                  do{
                    scanf("%c", &def_help_char);
                  }while(def_help_char!='\n');
                  break;
                }
                reg_result=reg(parameters);
                if(reg_result==-1){
                  printf("\nG-NOK %lu\n",parameters->student.aem);
                }else if(reg_result==-2){
                  printf("\nG-NOK %hd\n",parameters->subject_code);
                }else if(reg_result==-3){
                  printf("\nG-NOK MEM");
                }else{
                  printf("\nG-OK %lu %hd\n",parameters->student.aem, parameters->subject_code);
                }
                break;
            case 'u':
                scanf("%lu %hd", &parameters->student.aem, &parameters->subject_code);
                if(parameters->student.aem==0||parameters->subject_code==0){
                  printf("\nInvalid input.\n");
                  do{
                    scanf("%c", &def_help_char);
                  }while(def_help_char!='\n');
                  break;
                }
                unreg_result=unreg(parameters);
                if(unreg_result==-1){
                  printf("\nU-NOK %lu\n",parameters->student.aem);
                }else if(unreg_result==-2){
                  printf("\nU-NOK %hd\n",parameters->subject_code);
                }else{
                  printf("\nU-OK %lu %hd\n",parameters->student.aem, parameters->subject_code);
                }
                break;
            case 'i':
                scanf("%lu %hd", &parameters->student.aem, &parameters->subject_code);
                if(parameters->student.aem==0||parameters->subject_code==0){
                  printf("\nInvalid input.\n");
                  do{
                    scanf("%c", &def_help_char);
                  }while(def_help_char!='\n');
                  break;
                }
                isreg_result=isreg(parameters);
                if(isreg_result==NULL){
                  if(find(parameters)==NULL){
                    printf("\nI-NOK %lu\n", parameters->student.aem);
                  }else if(find(parameters)->head==NULL){
                    printf("\nNO\n");
                  }else if(find(parameters)->head->subj_code==parameters->subject_code){
                    printf("\nYES\n");
                  }else if(find(parameters)->head->subj_code>parameters->subject_code){
                    printf("\nNO\n");
                  }else{
                    printf("\nNO\n");
                  }
                }else if(((isreg_result->next!=NULL) && (isreg_result->next->subj_code > parameters->subject_code))
                        ||isreg_result->next==NULL){
                  printf("\nNO\n");
                }else{
                  printf("\nYES\n");
                }
                break;
            case 'l':
                scanf("%lu", &parameters->student.aem);
                if(parameters->student.aem==0){
                  printf("\nInvalid input.\n");
                  do{
                    scanf("%c", &def_help_char);
                  }while(def_help_char!='\n');
                  break;
                }
                list_courses_res=list_courses(parameters);
                if(list_courses_res==-1){
                  printf("\nL-NOK %lu\n", parameters->student.aem);
                }
                break;
            case 'n':
                scanf("%ms", &last);
                if(strlen(last)>=NAMESIZE){
                  last[NAMESIZE-1]='\0';
                  printf("\nTrimmed name(s).\n");
                }
                capital(last);
                strncpy(parameters->student.name.sirname,last,NAMESIZE);
                free(last);
                find_by_name(parameters);
                break;
            case 't':
                print_by_name(parameters);
                break;
            default:
              printf("\nInvalid command.\n");
                do{
                  scanf("%c", &def_help_char);
                }while(def_help_char!='\n');
                break;
        }
    }
	return 0;
}

