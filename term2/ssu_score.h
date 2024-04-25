#include <sys/types.h>

#ifndef MAIN_H_
#define MAIN_H_

#ifndef true
	#define true 1
#endif
#ifndef false
	#define false 0
#endif
#ifndef STDOUT
	#define STDOUT 1
#endif
#ifndef STDERR
	#define STDERR 2
#endif
#ifndef TEXTFILE
	#define TEXTFILE 3
#endif
#ifndef CFILE
	#define CFILE 4
#endif
#ifndef OVER
	#define OVER 5
#endif
#ifndef WARNING
	#define WARNING -0.1
#endif
#ifndef ERROR
	#define ERROR 0
#endif

#define FILELEN 128
#define BUFLEN 1024
#define SNUM 100
#define QNUM 100
#define ARGNUM 5

// student score struct
typedef struct student_csv {
		int id; // student id
		double *scores; // score for q
		double sum; // sum of all scores
} student_csv;

// s option linked list
typedef struct node2 {
		student_csv* data; // linked-linked 
		struct node2 *next;
} Node;

// p option linked list
typedef struct wrongq {
		char *qname; // qnames
		double score; // score
		struct wrongq *next; // linked
} wrongq;

struct ssu_scoreTable{
	char qname[FILELEN];
	double score;
};

void ssu_score(int argc, char *argv[]); // main program
int check_option(int argc, char *argv[]); // option chekcing
void print_usage(); // -h option

void score_students(char *ansDir, char *csv); // scoring function && creating new csv file
double score_student(int fd, char *id); // scoring students
void write_first_row(int fd); // qnames into csv

char *get_answer(int fd, char *result); // answer from ANSDIR
int score_blank(char *id, char *filename); // blank qs
double score_program(char *id, char *filename); // c qs
double compile_program(char *id, char *filename); // compile c
int execute_program(char *id, char *filname); // execute c
pid_t inBackground(char *name);
double check_error_warning(char *filename); // error checking
int compare_resultfile(char *file1, char *file2); // compare STUDIR and ANSDIR

void do_pOption(char *pIDs); // p function
void do_iOption(char (*ids)[FILELEN]); // i function
void do_mOption(); // m function 
void do_sOption(char *andsDir, char *csv); // s function
int is_exist(char (*src)[FILELEN], char *target); // check existance

wrongq* create_pNode(char* qname, double score); // node for p
void add_pNode(wrongq** head, char* qname, double score); // linked list for p
void print_plist(wrongq* head); // print result of p

void insert_node2(Node** head, student_csv* data); // node for s
void asort_by_id(Node** head); // ascend by id
void dsort_by_id(Node** head); // descend by id
void asort_by_sum(Node** head);// ascend by sum
void dsort_by_sum(Node** head);// descend by sum
student_csv* read_student_scores(char* line); // taking line by line from old csv

int is_thread(char *qname); // thread checking
void redirection(char *command, int newfd, int oldfd); // command
int get_file_type(char *filename); // check file type
void rmdirs(const char *path); // remove directory
void to_lower_case(char *c); // case checking

void set_scoreTable(char *ansDir); // set score table
void read_scoreTable(char *path); // read score table
void make_scoreTable(char *ansDir); // make score table
void write_scoreTable(char *filename); // write score table
void set_idTable(char *stuDir); // set id table
int get_create_type(); // get types

void sort_idTable(int size); // sort idtable
void sort_scoreTable(int size); // sort scoretable
void get_qname_number(char *qname, int *num1, int *num2); // get qname

#endif
