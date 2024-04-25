#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <dirent.h>
#include <signal.h>
#include <time.h>
#include "ssu_score.h"
#include "blank.h"

extern struct ssu_scoreTable score_table[QNUM];
extern char id_table[SNUM][10];

struct ssu_scoreTable score_table[QNUM];
char id_table[SNUM][10];

char stuDir[BUFLEN];
char ansDir[BUFLEN];
char errorDir[BUFLEN];
// -n option 인자
char csv[BUFLEN] = "score.csv"; // 기본 파일 명 score.csv
char csvPath[BUFLEN]; // .csv 파일 저장 경로

char threadFiles[ARGNUM][FILELEN];
char iIDs[ARGNUM][FILELEN];
// -c, -p option 인자
char studentIDs[ARGNUM][FILELEN]; // student ID arguments
char blankIDs[SNUM][FILELEN]; // 

// -s option 인자
char sortargs[BUFLEN]; // first argument of -s

// option 여부 확인
int eOption = false;
int tOption = false;
int t_optind = 0; // number of arguments for t
int mOption = false;
int iOption = false;
// -c option check
int cOption = false; // number of arguments for c
int noptind = 0; // number of arguments for both c and p (when c and p are used at the same time
// -p option check
int pOption = false; // number of arguments for p
int STUNUM = 0;
// -n option check
int nOption = false;
// -s option check
int sOption = false;
char sorttype[BUFLEN]; // second argument of -s

void ssu_score(int argc, char *argv[])
{
	char saved_path[BUFLEN];
	int i;

	// -h option entered
	for(i = 0; i < argc; i++){
		if(!strcmp(argv[i], "-h")){
			print_usage();
			return;
		}
	}

	memset(saved_path, 0, BUFLEN);
	if(argc >= 3 && strcmp(argv[1], "-i") != 0){
		strcpy(stuDir, argv[1]); // student directory
		strcpy(ansDir, argv[2]); // answer directory
	}
	
	// -n option : go get the location where .csv will be saved
	strcpy(csvPath, ansDir);

	if(!check_option(argc, argv)) // options check
		exit(1);

	if(!mOption && !eOption && !tOption && iOption 
			&& !strcmp(stuDir, "") && !strcmp(ansDir, "")){
		do_iOption(iIDs);
		return;
	}

	getcwd(saved_path, BUFLEN);

	if(chdir(stuDir) < 0){
		fprintf(stderr, "%s doesn't exist\n", stuDir);
		return;
	}
	getcwd(stuDir, BUFLEN);

	chdir(saved_path);
	if(chdir(ansDir) < 0){
		fprintf(stderr, "%s doesn't exist\n", ansDir);
		return;
	}
	getcwd(ansDir, BUFLEN);

	chdir(saved_path);

	set_scoreTable(ansDir);
	set_idTable(stuDir);

	// when user input -n, change the .csv filename and location
	if(mOption)
		do_mOption();

	// -t option first argument checking
	if(tOption){
		int errorcheck = 0;
		for(int j =0; j < sizeof(score_table) / sizeof(score_table[0]); j++)
      if(strstr(score_table[j].qname, threadFiles[0]) != NULL){
				errorcheck++;
				break;
			}
		if(errorcheck == 0){
				fprintf(stderr, "first argument for -t option is not available!\n");
				exit(1);
		}
	}

	printf("grading student's test papers..\n");

	// -n option : changed .csv filename and location
	score_students(csvPath, csv);

	if(iOption)
		do_iOption(iIDs);

	// -s option : changing .csv file order
	if(sOption)
		do_sOption(csvPath, csv);

	return;
}

int check_option(int argc, char *argv[])
{
	int i, j, k;
	int c, c_optind = 0, p_optind = 0;
	int exist = 0;
	int idx;

	while((c = getopt(argc, argv, "e:n:pscthmi1")) != -1) // n, m, e, p, c, h, s etc.
	{
		switch(c){
			case '1': // blank case for -s
			case 's':
				sOption = true; // option checking on
				if(!strcmp(argv[argc-1], "1")) // checking second argument
					i = optind;
				else if(!strcmp(argv[argc-1], "-1"))
					i = optind+1;
				strcpy(sortargs, argv[argc-2]); // first argu
				strcpy(sorttype, argv[argc-1]);// second argu
				if(strcmp(sortargs, "stdid") != 0 && strcmp(sortargs, "score") != 0){ // checking first argument
						fprintf(stderr, "<CATEROGY> should be either \"stdid\" or \"score\"!\n");
						exit(1);
				}
				if(strcmp(sorttype, "1") != 0 && strcmp(sorttype, "-1") != 0){ // checking second argu
						fprintf(stderr, "<1|-1> should be either \"1\" or \"-1\"!\n");
						exit(1);
				}
				
				break;
			case 'n':
				nOption = true; // n option on
				strcpy(csvPath, optarg); // copy argu which is the path of new .csv
				if(strstr(optarg, ".csv") == NULL){ // error checking
					fprintf(stderr, "%s is not .csv file!\n", optarg);
					exit(1);
				}
				for(idx = strlen(csvPath)-1; csvPath[idx] != '/'; idx--);
				if(idx <= 0){
					idx = 0;
					strcpy(csv, csvPath+idx); // taking filename from path
				}
				else strcpy(csv, csvPath+idx+1);
				if(idx != 0)
					csvPath[idx] = '\0';
				else
					getcwd(csvPath, BUFLEN); // checking current wd when new path is not given
				break;
			case 'e':
				eOption = true; // e option on
				strcpy(errorDir, optarg); // error dir path

				if(access(errorDir, F_OK) < 0) // path checking
					mkdir(errorDir, 0755);
				else{
					rmdirs(errorDir);
					mkdir(errorDir, 0755);
				}
				break;
			case 'c':
				cOption = true; // c option on
				i = optind;
        j = 0;

        while(i < argc && argv[i][0] != '-'){
 
          if(j >= ARGNUM)
            printf("Maximum Number of Argument Exceeded. :: %s\n", argv[i]);
          else{
            strcpy(studentIDs[j], argv[i]); // copying argus to new array
          }
          i++;
          j++;
        }
				c_optind = argc - optind; // number of argus
				noptind = c_optind;
        break;
			case 'p':
				pOption = true; // p option on
				i = optind;
        j = 0;

        while(i < argc && argv[i][0] != '-'){

					if(j >= ARGNUM)
           printf("Maximum Number of Argument Exceeded. :: %s\n", argv[i]);
         else{
           strcpy(studentIDs[j], argv[i]); // copying argus to new array
         }
				 i++;
         j++;
        }
       p_optind = argc - optind; // number of argus
			 noptind = p_optind;
       break;
			case 't':
				tOption = true; // t option on
				i = optind;
				j = 0;

				while(i < argc && argv[i][0] != '-'){

					if(j >= ARGNUM)
						printf("Maximum Number of Argument Exceeded.  :: %s\n", argv[i]);
					else{
						strcpy(threadFiles[j], argv[i]); // copying argus to new thread array
					}
					i++; 
					j++;
				}
				t_optind = argc - optind;
				break;
			case 'm':
				mOption = true; // m option on
				break;
			case 'i':
				iOption = true; // professor's code
				i = optind;
				j = 0;

				while(i < argc && argv[i][0] != '-'){
					if(j >= ARGNUM)
						printf("Maximum Number of Argument Exceeded. :: %s\n", argv[i]);
					else
						strcpy(iIDs[j], argv[i]);
					i++;
					j++;
				}
				break;
			case '?': // else
				printf("Unkown option %c\n", optopt);
				return false;
		}
	}
	// -c와 -p 동시 사용 시
	if(cOption && pOption){
		// -c -p 인자
		if(c_optind == p_optind+1 && p_optind != 0)
			noptind = p_optind;
		// -c 인자 -p
		else if(c_optind > 1 && p_optind == 0)
			noptind = c_optind-1;
		// -c -p
		else if(c_optind == 1 && p_optind == 0)
			noptind = 0;
		// -p -c 인자
		else if(p_optind == c_optind+1 && c_optind != 0)
			noptind = c_optind;
		// -p 인자 -c
		else if(p_optind > 1 && c_optind == 0)
			noptind = p_optind-1;
		// -p -c
		else if(p_optind == 1 && c_optind == 0)
			noptind = 0;
		// else
		else{
				fprintf(stderr, "Input arguments group once\n");
				exit(1);
		}
	}
	// -c와 -t 동시 사용 시
  if(cOption && tOption){
    // -c -t 인자
    if(c_optind == t_optind+1 && t_optind != 0)
      noptind = 0;
    // -c 인자 -t
    else if(c_optind > 1 && t_optind == 0)
      noptind = c_optind-1;
    // -c -t
    else if(c_optind == 1 && t_optind == 0)
      noptind = 0;
    // -t -c 인자
    else if(t_optind == c_optind+1 && c_optind != 0)
      noptind = c_optind;
    // -t 인자 -c
    else if(t_optind > 1 && c_optind == 0)
      noptind = 0;
    // -t -c
    else if(t_optind == 1 && c_optind == 0)
      noptind = 0;
    // -t 인자 -c 인자 / -c 인자 -t 인자
    else
			noptind = c_optind;
	}

	return true;
}

//-p option
void do_pOption(char *pIDs)
{
	FILE *fp1;
	char tmp[BUFLEN];
	char qname[QNUM][FILELEN];
	char *p, *id;
	int i, j, NUM;
	char first, exist;
	char filename[FILELEN];
	wrongq* head = NULL; // linked list struct for p option

	sprintf(filename, "%s/%s", csvPath, csv); 

  if((fp1 = fopen(filename, "r")) == NULL){ // open .csv file
    fprintf(stderr, "%s file doesn't exist\n", csv);
    return;
  }

  // get all qnames
  i = 0;     
  fscanf(fp1, "%s\n", tmp);
  strcpy(qname[i++], strtok(tmp, ","));
  
  while((p = strtok(NULL, ",")) != NULL)
    strcpy(qname[i++], p);

  // print result which are wrong 
  i = 0;
	if(noptind == 0) // no argument for c or p 
			NUM = STUNUM;
	else
			NUM = ARGNUM; // arguments available for c or p
  while(i++ <= NUM - 1)
  {
    exist = 0;
    fseek(fp1, 0, SEEK_SET);
    fscanf(fp1, "%s\n", tmp);

    while(fscanf(fp1, "%s\n", tmp) != EOF){

      id = strtok(tmp, ","); // get ids from .csv file

      if(!strcmp(pIDs, id)){
        exist = 1;
        j = 0;
        first = 0;
        while((p = strtok(NULL, ",")) != NULL){
					// check qnames which are wrong
          if(atof(p) == 0){
            if(!first){
              printf("wrong answer : "); // print result
							first = 1;
            }
            if(strcmp(qname[j], "sum")){
							if(strstr(qname[j], ".txt"))
									qname[j][strlen(qname[j])-4] = '\0';
							else if(strstr(qname[j], ".c"))
									qname[j][strlen(qname[j])-2] = '\0';
							add_pNode(&head, qname[j], score_table[j].score); // put qname and score into linked list
						}
					}
					j++;
				}
				print_plist(head); // print linked list
				wrongq* current = head;
				while(current != NULL){ // free memory
						wrongq* next = current->next;
						free(current);
						current = next;
				}
      }
    }
		break;
  }

  fclose(fp1);
}

// creating new node for p
wrongq* create_pNode(char* qname, double score)
{
	wrongq* newNode = (wrongq*)malloc(sizeof(wrongq));
	newNode->qname = qname;
	newNode->score = score;
	newNode->next = NULL;
	return newNode;
}

// connecting new node
void add_pNode(wrongq** head, char* qname, double score)
{
	wrongq* newNode = create_pNode(qname, score); // creating new node using function

	if(*head == NULL){
			*head = newNode;
			return;
	}

	wrongq* current = *head;
	while(current->next != NULL){
			current = current->next; // move to next
	}
	current->next = newNode;
}

//printing linked list
void print_plist(wrongq* head)
{
	wrongq* current = head;
	while(current != NULL){
			printf("%s(%.2f)", current->qname, current->score); // print linked list data
			if(current->next != NULL) // ', ' between qnames
					printf(", ");
			current = current->next; // move to next
	}
}

void do_iOption(char (*ids)[FILELEN])
{
	FILE *fp;
	char tmp[BUFLEN];
	char qname[QNUM][FILELEN];
	char *p, *id;
	int i, j;
	char first, exist;
	char filename[FILELEN];

	sprintf(filename, "%s/%s", csvPath, csv);

	if((fp = fopen(filename, "r")) == NULL){
		fprintf(stderr, "%s file doesn't exist\n", csv);
		return;
	}

	// get qnames
	i = 0;
	fscanf(fp, "%s\n", tmp);
	strcpy(qname[i++], strtok(tmp, ","));
	
	while((p = strtok(NULL, ",")) != NULL)
		strcpy(qname[i++], p);

	// print result
	i = 0;
	while(i++ <= ARGNUM - 1)
	{
		exist = 0;
		fseek(fp, 0, SEEK_SET);
		fscanf(fp, "%s\n", tmp);

		while(fscanf(fp, "%s\n", tmp) != EOF){
			id = strtok(tmp, ",");

			if(!strcmp(ids[i - 1], id)){
				exist = 1;
				j = 0;
				first = 0;
				while((p = strtok(NULL, ",")) != NULL){
					if(atof(p) == 0){
						if(!first){
							printf("%s's wrong answer : ", id);
							first = 1;
						}
						if(strcmp(qname[j], "sum"))
							printf("%s    ", qname[j]);
					}
					j++;
				}
				printf("\n");
			}
		}

		if(!exist)
			printf("%s doesn't exist!\n", ids[i - 1]);
	}

	fclose(fp);
}

// m option function by professor
void do_mOption(char *ansDir)
{
	double newScore;
	char modiName[FILELEN];
	char filename[FILELEN];
	char *ptr;
	int i;

	ptr = malloc(sizeof(char) * FILELEN);

	while(1){

		printf("Input question's number to modify >> ");
		scanf("%s", modiName);

		if(strcmp(modiName, "no") == 0)
			break;

		for(i=0; i < sizeof(score_table) / sizeof(score_table[0]); i++){
			strcpy(ptr, score_table[i].qname);
			ptr = strtok(ptr, ".");
			if(!strcmp(ptr, modiName)){
				printf("Current score : %.2f\n", score_table[i].score);
				printf("New score : ");
				scanf("%lf", &newScore);
				getchar();
				score_table[i].score = newScore;
				break;
			}
		}
	}

	sprintf(filename, "./%s", "score_table.csv"); // print result
	write_scoreTable(filename);
	free(ptr);

}

// s option function
void do_sOption(char *andDir, char *csv)
{
	// filename for score.csv location
  char filename[FILELEN];
  int size = sizeof(id_table) / sizeof(id_table[0]);

  // score.csv를 ansDir로 받은 directory에 저장
  sprintf(filename, "%s/%s", ansDir, csv);

	Node *head = NULL; // linked list head
	FILE *fp = fopen(filename, "r");
	char line[BUFLEN];
	char qnum[BUFLEN];

	// taking first line which is qnum
	fgets(qnum, BUFLEN, fp);

	// read line by line & put in list
	while(fgets(line, BUFLEN, fp)){
			student_csv *student = read_student_scores(line);
			insert_node2(&head, student);
	}
	fclose(fp);
	// 학번 기준 오름차순
	if(!strcmp(sortargs, "stdid") && !strcmp(sorttype, "1"))
		asort_by_id(&head);
	// 학번 기준 내림차순
	if(!strcmp(sortargs, "stdid") && !strcmp(sorttype, "-1"))
		dsort_by_id(&head);
	// 총점 기준 오름차순
	if(!strcmp(sortargs, "score") && !strcmp(sorttype, "1"))
		asort_by_sum(&head);
	// 총점 기준 내림차순
	if(!strcmp(sortargs, "score") && !strcmp(sorttype, "-1"))
		dsort_by_sum(&head);
	// csv 파일 다시 저장
	fp = fopen(filename, "w");
	fprintf(fp, "%s", qnum);

	// get number of questions 
	int qcount = 0;
	for(int i = 0; i < QNUM; i++){
			if(strstr(score_table[i].qname, ".") != NULL)
					qcount++;
	}

	// save .csv in new order
	Node* current = head;
	while(current != NULL){
			fprintf(fp, "%d, ", current->data->id); // id
			int i;
			for(i = 0; i < qcount; i++){
					fprintf(fp, "%.2f, ", current->data->scores[i]); // scores
			}
			fprintf(fp, "%.2f\n", current->data->scores[i]); // sum
			current = current->next; // next id
	}

	fclose(fp);
}

// 학번 기준 오름차순
void asort_by_id(Node** head)
{
		Node *current = *head;
		Node *sorted_head = NULL;

		while(current != NULL){
				Node *next = current->next;
				
				if(sorted_head == NULL || current->data->id < sorted_head->data->id){
						current->next = sorted_head; // compare ids
						sorted_head = current;
				}
				else{
						Node *prev = sorted_head;

						while(prev->next != NULL && current->data->id >= prev->next->data->id){
								prev = prev->next; // compare ids
						}

						current->next = prev->next;
						prev->next = current;
				}
				current = next;
		}
		*head = sorted_head;
}

// 학번 기준 내림차순
void dsort_by_id(Node** head)
{
		Node* current = *head;
		Node* sorted_head = NULL;

		while(current != NULL){
				Node* next = current->next;

				if(sorted_head == NULL || current->data->id > sorted_head->data->id){
						current->next = sorted_head; // compare ids
            sorted_head = current;
        }
        else{
            Node *prev = sorted_head;
            while(prev->next != NULL && current->data->id <= prev->next->data->id){
                prev = prev->next; // compare ids
            }
            current->next = prev->next;
            prev->next = current;
        }
        current = next;
    }
    *head = sorted_head;
}

// 총점 기준 오름차순
void asort_by_sum(Node** head)
{
    Node *current = *head;
    Node *sorted_head = NULL;
 
 while(current != NULL){
        Node *next = current->next;
        if(sorted_head == NULL || current->data->sum < sorted_head->data->sum){
            current->next = sorted_head; // compare sum
            sorted_head = current;
        }
        else{
            Node *prev = sorted_head;
            while(prev->next != NULL && current->data->sum >= prev->next->data->sum){
                prev = prev->next; // compare sum
            }
            current->next = prev->next;
            prev->next = current;
        }
        current = next;
    }
    *head = sorted_head;
}

// 총점 기준 내림차순
void dsort_by_sum(Node** head)
{
		Node *current = *head;
    Node *sorted_head = NULL;
		
		while(current != NULL){
        Node *next = current->next;
        if(sorted_head == NULL || current->data->sum > sorted_head->data->sum){
            current->next = sorted_head; // compare sum
            sorted_head = current;
        }
        else{
            Node *prev = sorted_head;
            while(prev->next != NULL && current->data->sum <= prev->next->data->sum){
                prev = prev->next; // compare sum
            }
            current->next = prev->next;
            prev->next = current;
        }
        current = next;
    }
    *head = sorted_head;
}

// insert node for s
void insert_node2(Node** head, student_csv *student)
{
		Node *new_node = (Node*)malloc(sizeof(Node));
		new_node->data = student;
		new_node->next = NULL;

		// if empty list
		if(*head == NULL)
				*head = new_node;
		// insert in the middle of list
		else{
				Node *current = *head;
				Node *prev = NULL;
				while(current != NULL && current->data->id < student->id){
						prev = current;
						current = current->next;
				}

				if(prev == NULL){
						new_node->next = *head;
						*head = new_node;
				}
				else{
						prev->next = new_node;
						new_node->next = current;
				}		
		}
}

// get scores, put into linked list for s
student_csv* read_student_scores(char *line)
{
	student_csv *student = (student_csv*)malloc(sizeof(student_csv));
	
	int num_problems;
	char *token = strtok(line, ","); // 쉽표 기준 문자열 분리
	student->id = atoi(token); // id

	num_problems = sizeof(score_table) / sizeof(score_table[0]);
	student->scores = (double*)malloc(num_problems*sizeof(double));
	student->sum = 0; 
    
	num_problems = 0;
	while(token != NULL){
			token = strtok(NULL, ",");
			if(token != NULL){
					if(strcmp(token, " ") == 0)
							student->scores[num_problems] = 0; // error check
					else
							student->scores[num_problems] = atof(token); // scores for each q
					student->sum += student->scores[num_problems]; // sum
					num_problems++; // number of q
			}
	}


	return student;
}

int is_exist(char (*src)[FILELEN], char *target)
{
	int i = 0;

	while(1)
	{
		if(i >= ARGNUM)
			return false;
		else if(!strcmp(src[i], ""))
			return false;
		else if(!strcmp(src[i++], target))
			return true;
	}
	return false;
}

void set_scoreTable(char *ansDir)
{
	char filename[FILELEN];

	// score_table 기본 저장 경로 수정 
	sprintf(filename, "%s/%s", ansDir, "score_table.csv");

	// check exist
	if(access(filename, F_OK) == 0)
		read_scoreTable(filename);
	else{
		make_scoreTable(ansDir);
		write_scoreTable(filename);
	}
}

void read_scoreTable(char *path)
{
	FILE *fp;
	char qname[FILELEN];
	char score[BUFLEN];
	int idx = 0;

	if((fp = fopen(path, "r")) == NULL){
		fprintf(stderr, "file open error for %s\n", path);
		return ;
	}

	while(fscanf(fp, "%[^,],%s\n", qname, score) != EOF){
		strcpy(score_table[idx].qname, qname);
		score_table[idx++].score = atof(score);
	}

	fclose(fp);
}

void make_scoreTable(char *ansDir)
{
	int type, num;
	double score, bscore, pscore;
	struct dirent *dirp, *c_dirp;
	DIR *dp, *c_dp;
	char *tmp;
	int idx = 0;
	int i;

	num = get_create_type();

	if(num == 1)
	{
		printf("Input value of blank question : ");
		scanf("%lf", &bscore);
		printf("Input value of program question : ");
		scanf("%lf", &pscore);
	}

	if((dp = opendir(ansDir)) == NULL){
		fprintf(stderr, "open dir error for %s\n", ansDir);
		return;
	}

	while((dirp = readdir(dp)) != NULL){

		if(!strcmp(dirp->d_name, ".") || !strcmp(dirp->d_name, ".."))
			continue;

		if((type = get_file_type(dirp->d_name)) < 0)
			continue;

		strcpy(score_table[idx].qname, dirp->d_name);

		idx++;
	}

	closedir(dp);
	sort_scoreTable(idx);

	for(i = 0; i < idx; i++)
	{
		type = get_file_type(score_table[i].qname);

		if(num == 1)
		{
			if(type == TEXTFILE)
				score = bscore;
			else if(type == CFILE)
				score = pscore;
		}
		else if(num == 2)
		{
			printf("Input of %s: ", score_table[i].qname);
			scanf("%lf", &score);
		}

		score_table[i].score = score;
	}
}

void write_scoreTable(char *filename)
{
	int fd;
	char tmp[BUFLEN];
	int i;
	int num = sizeof(score_table) / sizeof(score_table[0]);

	if((fd = creat(filename, 0666)) < 0){
		fprintf(stderr, "creat error for %s\n", filename);
		return;
	}

	for(i = 0; i < num; i++)
	{
		if(score_table[i].score == 0)
			break;

		sprintf(tmp, "%s,%.2f\n", score_table[i].qname, score_table[i].score);
		write(fd, tmp, strlen(tmp));
	}

	close(fd);
}


void set_idTable(char *stuDir)
{
	struct stat statbuf;
	struct dirent *dirp;
	DIR *dp;
	char tmp[BUFLEN];
	int num = 0;

	if((dp = opendir(stuDir)) == NULL){
		fprintf(stderr, "opendir error for %s\n", stuDir);
		exit(1);
	}

	while((dirp = readdir(dp)) != NULL){
		if(!strcmp(dirp->d_name, ".") || !strcmp(dirp->d_name, ".."))
			continue;

		sprintf(tmp, "%s/%s", stuDir, dirp->d_name);
		stat(tmp, &statbuf);

		if(S_ISDIR(statbuf.st_mode)){
			strcpy(blankIDs[STUNUM++], dirp->d_name);
			strcpy(id_table[num++], dirp->d_name);
		}
		else
			continue;
	}
	closedir(dp);

	sort_idTable(num);
}

void sort_idTable(int size)
{
	int i, j;
	char tmp[10];

	for(i = 0; i < size - 1; i++){
		for(j = 0; j < size - 1 -i; j++){
			if(strcmp(id_table[j], id_table[j+1]) > 0){
				strcpy(tmp, id_table[j]);
				strcpy(id_table[j], id_table[j+1]);
				strcpy(id_table[j+1], tmp);
			}
		}
	}
}

void sort_scoreTable(int size)
{
	int i, j;
	struct ssu_scoreTable tmp;
	int num1_1, num1_2;
	int num2_1, num2_2;

	for(i = 0; i < size - 1; i++){
		for(j = 0; j < size - 1 - i; j++){

			get_qname_number(score_table[j].qname, &num1_1, &num1_2);
			get_qname_number(score_table[j+1].qname, &num2_1, &num2_2);

			if((num1_1 > num2_1) || ((num1_1 == num2_1) && (num1_2 > num2_2))){

				memcpy(&tmp, &score_table[j], sizeof(score_table[0]));
				memcpy(&score_table[j], &score_table[j+1], sizeof(score_table[0]));
				memcpy(&score_table[j+1], &tmp, sizeof(score_table[0]));
			}
		}
	}
}

void get_qname_number(char *qname, int *num1, int *num2)
{
	char *p;
	char dup[FILELEN];

	strncpy(dup, qname, strlen(qname));
	*num1 = atoi(strtok(dup, "-."));
	
	p = strtok(NULL, "-.");
	if(p == NULL)
		*num2 = 0;
	else
		*num2 = atoi(p);
}

int get_create_type()
{
	int num;

	// 예외 값(글자) 입력 시 무한루프 수정
	while(1)
	{
		printf("score_table.csv file doesn't exist in %s!\n", ansDir);
		printf("1. input blank question and program question's score. ex) 0.5 1\n");
		printf("2. input all question's score. ex) Input value of 1-1: 0.1\n");
		printf("select type >> ");
		scanf("%d", &num);
	
		// 1 or 2 가 아닌 다른 모든 값 입력 시 버퍼를 비워 무한루프 수정
		if(num != 1 && num != 2){
			printf("not correct number!\n");
			while(getchar() != '\n');
		}
		else
			break;
	}

	return num;
}

// [option]을 위한 인자 수정
void score_students(char *ansDir, char *csv)
{
	double score = 0;
	int num = 0, i;
	int fd;
	char tmp[BUFLEN];

	// filename for score.csv location
	char filename[FILELEN];
	char errorname[FILELEN];
	char buf[BUFLEN];
	char path[BUFLEN];
	int size = sizeof(id_table) / sizeof(id_table[0]);

	// score.csv를 ansDir로 받은 directory에 저장
	sprintf(filename, "%s/%s", ansDir, csv);
	sprintf(errorname, "%s/%s", getcwd(buf, BUFLEN), errorDir);
	
	if((fd = creat(filename, 0666)) < 0){ // create new csv
		fprintf(stderr, "creat error for score.csv\n");
		return;
	}
	write_first_row(fd); // write q nums
	
	// -c option 에러처리 수정
	if((cOption && (noptind != 0)) || (pOption && (noptind != 0))){
		int errorcheck = 0;
		for(num = 0; num < size; num++){
			if(!strcmp(studentIDs[0], id_table[num]))
					errorcheck++;
		}
		if(errorcheck == 0){
			fprintf(stderr, "argument error for %s not in <STD_DIR>\n", studentIDs[0]);
			exit(1);
		}
	}
						
	for(num = 0; num < size; num++)
	{
		if(!strcmp(id_table[num], ""))
			break;

		sprintf(tmp, "%s,", id_table[num]);
		write(fd, tmp, strlen(tmp)); 
		
		score += score_student(fd, id_table[num]); // score for each question
	}

	// require changes for -c option
	if(cOption){
		if(noptind != 0)
			printf("Total average : %.2f\n", score / noptind);
		else
			printf("Total average : %.2f\n", score / num);
	}

	// output "result saved" for score.csv
	printf("result saved.. (%s)\n", realpath(filename, path));
	if(eOption)
			printf("error saved.. (%s)\n", realpath(errorname, path));

	close(fd);
}

double score_student(int fd, char *id)
{
	int type;
	double result;
	double score = 0;
	int i, j;
	char tmp[BUFLEN];
	int size = sizeof(score_table) / sizeof(score_table[0]);

	for(i = 0; i < size ; i++)
	{
		if(score_table[i].score == 0)
			break;

		sprintf(tmp, "%s/%s/%s", stuDir, id, score_table[i].qname);

		if(access(tmp, F_OK) < 0)
			result = false;
		else
		{
			if((type = get_file_type(score_table[i].qname)) < 0)
				continue;
			
			if(type == TEXTFILE)
				result = score_blank(id, score_table[i].qname);
			else if(type == CFILE)
				result = score_program(id, score_table[i].qname);
		}

		if(result == false)
			write(fd, "0,", 2);
		else{
			if(result == true){
				score += score_table[i].score;
				sprintf(tmp, "%.2f,", score_table[i].score);
			}
			else if(result < 0){
				score = score + score_table[i].score + result;
				sprintf(tmp, "%.2f,", score_table[i].score + result);
			}
			write(fd, tmp, strlen(tmp));
		}
	}

	sprintf(tmp, "%.2f\n", score);
	write(fd, tmp, strlen(tmp));

	// -c option으로 수정
	if(cOption){
		if(noptind != 0){
			printf("%s is finished.. ", id);
			for(j = 0; j < noptind; j++){
				if(!strcmp(studentIDs[j], id)){
						printf("score : %.2f", score);
						// -c & -p 동시 사용 시 인자 주어졌을 때
						if(pOption){
								printf(", ");
								do_pOption(studentIDs[j]); // call p function
						}
						printf("\n");
						return score;
				}
			}
			printf("\n");
			return 0;
		}
		else{
			printf("%s is finished.. score : %.2f", id, score);
			// -c & -p 동시 사용 시 인자 없을 때
			if(pOption){
				printf(", ");
				for(j = 0; j < STUNUM; j++){
					if(!strcmp(blankIDs[j], id))
						do_pOption(blankIDs[j]); // call p fuction
				}
			}
			printf("\n");
			return score;
		}
	}
	else if(pOption){
			printf("%s is finished.. ", id);
			// -p option만 사용됐을 때
			if(noptind != 0){
				for(j = 0; j < noptind; j++){
					if(pOption && (!strcmp(studentIDs[j], id)))
						do_pOption(studentIDs[j]); // call p
				}
			}
			else{
				for(j = 0; j < STUNUM; j++){
					if(!strcmp(blankIDs[j], id))
						do_pOption(blankIDs[j]); // call p
				}
			}
			printf("\n");
			return score;
	}
	else{
		printf("%s is finished.. \n", id);
		return score;
	}
}

void write_first_row(int fd)
{
	int i;
	char tmp[BUFLEN];
	int size = sizeof(score_table) / sizeof(score_table[0]);

	write(fd, ",", 1);

	for(i = 0; i < size; i++){
		if(score_table[i].score == 0)
			break;
		
		sprintf(tmp, "%s,", score_table[i].qname);
		write(fd, tmp, strlen(tmp));
	}
	write(fd, "sum\n", 4);
}

char *get_answer(int fd, char *result)
{
	char c;
	int idx = 0;

	memset(result, 0, BUFLEN);
	while(read(fd, &c, 1) > 0)
	{
		if(c == ':')
			break;
		
		result[idx++] = c;
	}
	if(result[strlen(result) - 1] == '\n')
		result[strlen(result) - 1] = '\0';

	return result;
}

int score_blank(char *id, char *filename)
{
	char tokens[TOKEN_CNT][MINLEN];
	node *std_root = NULL, *ans_root = NULL;
	int idx, start;
	char tmp[BUFLEN];
	char s_answer[BUFLEN], a_answer[BUFLEN];
	char qname[FILELEN];
	int fd_std, fd_ans;
	int result = true;
	int has_semicolon = false;

	memset(qname, 0, sizeof(qname));
	memcpy(qname, filename, strlen(filename) - strlen(strrchr(filename, '.')));

	sprintf(tmp, "%s/%s/%s", stuDir, id, filename);
	fd_std = open(tmp, O_RDONLY);
	strcpy(s_answer, get_answer(fd_std, s_answer));

	if(!strcmp(s_answer, "")){
		close(fd_std);
		return false;
	}

	if(!check_brackets(s_answer)){
		close(fd_std);
		return false;
	}

	strcpy(s_answer, ltrim(rtrim(s_answer)));

	if(s_answer[strlen(s_answer) - 1] == ';'){
		has_semicolon = true;
		s_answer[strlen(s_answer) - 1] = '\0';
	}

	if(!make_tokens(s_answer, tokens)){
		close(fd_std);
		return false;
	}

	idx = 0;
	std_root = make_tree(std_root, tokens, &idx, 0);

	sprintf(tmp, "%s/%s", ansDir, filename);
	fd_ans = open(tmp, O_RDONLY);

	while(1)
	{
		ans_root = NULL;
		result = true;

		for(idx = 0; idx < TOKEN_CNT; idx++)
			memset(tokens[idx], 0, sizeof(tokens[idx]));

		strcpy(a_answer, get_answer(fd_ans, a_answer));

		if(!strcmp(a_answer, ""))
			break;

		strcpy(a_answer, ltrim(rtrim(a_answer)));

		if(has_semicolon == false){
			if(a_answer[strlen(a_answer) -1] == ';')
				continue;
		}

		else if(has_semicolon == true)
		{
			if(a_answer[strlen(a_answer) - 1] != ';')
				continue;
			else
				a_answer[strlen(a_answer) - 1] = '\0';
		}

		if(!make_tokens(a_answer, tokens))
			continue;

		idx = 0;
		ans_root = make_tree(ans_root, tokens, &idx, 0);

		compare_tree(std_root, ans_root, &result);

		if(result == true){
			close(fd_std);
			close(fd_ans);

			if(std_root != NULL)
				free_node(std_root);
			if(ans_root != NULL)
				free_node(ans_root);
			return true;

		}
	}
	
	close(fd_std);
	close(fd_ans);

	if(std_root != NULL)
		free_node(std_root);
	if(ans_root != NULL)
		free_node(ans_root);

	return false;
}

double score_program(char *id, char *filename)
{
	double compile;
	int result;

	compile = compile_program(id, filename);

	if(compile == ERROR || compile == false)
		return false;
	
	result = execute_program(id, filename);

	if(!result)
		return false;

	if(compile < 0)
		return compile;

	return true;
}

int is_thread(char *qname)
{
	int i;
	int size = sizeof(threadFiles) / sizeof(threadFiles[0]);

	for(i = 0; i < size; i++){
		if(!strcmp(threadFiles[i], qname))
			return true;
	}
	return false;
}

double compile_program(char *id, char *filename)
{
	int fd;
	char tmp_f[BUFLEN], tmp_e[BUFLEN];
	char command[BUFLEN];
	char qname[FILELEN];
	int isthread;
	off_t size;
	double result;

	memset(qname, 0, sizeof(qname));
	memcpy(qname, filename, strlen(filename) - strlen(strrchr(filename, '.')));

	isthread = is_thread(qname);

	sprintf(tmp_f, "%s/%s", ansDir, filename);
	sprintf(tmp_e, "%s/%s.exe", ansDir, qname);

	if(tOption && isthread)
		sprintf(command, "gcc -o %s %s -lpthread", tmp_e, tmp_f);
	else
		sprintf(command, "gcc -o %s %s", tmp_e, tmp_f);
	
	sprintf(tmp_e, "%s/%s_error.txt", ansDir, qname);
	fd = creat(tmp_e, 0666);

	redirection(command, fd, STDERR);
	size = lseek(fd, 0, SEEK_END);
	close(fd);
	unlink(tmp_e);

	if(size > 0)
		return false;

	sprintf(tmp_f, "%s/%s/%s", stuDir, id, filename);
	sprintf(tmp_e, "%s/%s/%s.stdexe", stuDir, id, qname);

	if(tOption && isthread)
		sprintf(command, "gcc -o %s %s -lpthread", tmp_e, tmp_f);
	else
		sprintf(command, "gcc -o %s %s", tmp_e, tmp_f);

	sprintf(tmp_f, "%s/%s/%s_error.txt", stuDir, id, qname);
	fd = creat(tmp_f, 0666);

	redirection(command, fd, STDERR);
	size = lseek(fd, 0, SEEK_END);
	close(fd);

	if(size > 0){
		if(eOption)
		{
			sprintf(tmp_e, "%s/%s", errorDir, id);
			if(access(tmp_e, F_OK) < 0)
				mkdir(tmp_e, 0755);

			sprintf(tmp_e, "%s/%s/%s_error.txt", errorDir, id, qname);
			rename(tmp_f, tmp_e);

			result = check_error_warning(tmp_e);
		}
		else{ 
			result = check_error_warning(tmp_f);
			unlink(tmp_f);
		}

		return result;
	}

	unlink(tmp_f);
	return true;
}

double check_error_warning(char *filename)
{
	FILE *fp;
	char tmp[BUFLEN];
	double warning = 0;

	if((fp = fopen(filename, "r")) == NULL){
		fprintf(stderr, "fopen error for %s\n", filename);
		return false;
	}

	while(fscanf(fp, "%s", tmp) > 0){
		if(!strcmp(tmp, "error:"))
			return ERROR;
		else if(!strcmp(tmp, "warning:"))
			warning += WARNING;
	}

	return warning;
}

int execute_program(char *id, char *filename)
{
	char std_fname[BUFLEN], ans_fname[BUFLEN];
	char tmp[BUFLEN];
	char qname[FILELEN];
	time_t start, end;
	pid_t pid;
	int fd;

	memset(qname, 0, sizeof(qname));
	memcpy(qname, filename, strlen(filename) - strlen(strrchr(filename, '.')));

	sprintf(ans_fname, "%s/%s.stdout", ansDir, qname);
	fd = creat(ans_fname, 0666);

	sprintf(tmp, "%s/%s.exe", ansDir, qname);
	redirection(tmp, fd, STDOUT);
	close(fd);

	sprintf(std_fname, "%s/%s/%s.stdout", stuDir, id, qname);
	fd = creat(std_fname, 0666);

	sprintf(tmp, "%s/%s/%s.stdexe &", stuDir, id, qname);

	start = time(NULL);
	redirection(tmp, fd, STDOUT);
	
	sprintf(tmp, "%s.stdexe", qname);
	while((pid = inBackground(tmp)) > 0){
		end = time(NULL);

		if(difftime(end, start) > OVER){
			kill(pid, SIGKILL);
			close(fd);
			return false;
		}
	}

	close(fd);

	return compare_resultfile(std_fname, ans_fname);
}

pid_t inBackground(char *name)
{
	pid_t pid;
	char command[64];
	char tmp[64];
	int fd;
	off_t size;
	
	memset(tmp, 0, sizeof(tmp));
	fd = open("background.txt", O_RDWR | O_CREAT | O_TRUNC, 0666);

	sprintf(command, "ps | grep %s", name);
	redirection(command, fd, STDOUT);

	lseek(fd, 0, SEEK_SET);
	read(fd, tmp, sizeof(tmp));

	if(!strcmp(tmp, "")){
		unlink("background.txt");
		close(fd);
		return 0;
	}

	pid = atoi(strtok(tmp, " "));
	close(fd);

	unlink("background.txt");
	return pid;
}

int compare_resultfile(char *file1, char *file2)
{
	int fd1, fd2;
	char c1, c2;
	int len1, len2;

	fd1 = open(file1, O_RDONLY);
	fd2 = open(file2, O_RDONLY);

	while(1)
	{
		while((len1 = read(fd1, &c1, 1)) > 0){
			if(c1 == ' ') 
				continue;
			else 
				break;
		}
		while((len2 = read(fd2, &c2, 1)) > 0){
			if(c2 == ' ') 
				continue;
			else 
				break;
		}
		
		if(len1 == 0 && len2 == 0)
			break;

		to_lower_case(&c1);
		to_lower_case(&c2);

		if(c1 != c2){
			close(fd1);
			close(fd2);
			return false;
		}
	}
	close(fd1);
	close(fd2);
	return true;
}

void redirection(char *command, int new, int old)
{
	int saved;

	saved = dup(old);
	dup2(new, old);
	
	system(command);
	
	dup2(saved, old);
	close(saved);
}

int get_file_type(char *filename)
{
	char *extension = strrchr(filename, '.');

	if(!strcmp(extension, ".txt"))
		return TEXTFILE;
	else if (!strcmp(extension, ".c"))
		return CFILE;
	else
		return -1;
}

void rmdirs(const char *path)
{
	struct dirent *dirp;
	struct stat statbuf;
	DIR *dp;
	char tmp[50];
	
	if((dp = opendir(path)) == NULL)
		return;

	while((dirp = readdir(dp)) != NULL)
	{
		if(!strcmp(dirp->d_name, ".") || !strcmp(dirp->d_name, ".."))
			continue;

		sprintf(tmp, "%s/%s", path, dirp->d_name);

		if(lstat(tmp, &statbuf) == -1)
			continue;

		if(S_ISDIR(statbuf.st_mode))
			rmdirs(tmp);
		else
			unlink(tmp);
	}

	closedir(dp);
	rmdir(path);
}

void to_lower_case(char *c)
{
	if(*c >= 'A' && *c <= 'Z')
		*c = *c + 32;
}

// h option function
void print_usage()
{
	printf("Usage : ssu_score <STUDENTDIR> <TRUEDIR> [OPTION]\n");
	printf("Option : \n");
	printf(" -n <CSVFILENAME>\n");
	printf(" -m\n");
	printf(" -c [STUDENTIDS ...]\n");
	printf(" -p [STUDENTIDS ...]\n");
	printf(" -t [QNAMES ...]\n");
	printf(" -s <CATEGORY> <1|-1>\n");
	printf(" -e <DIRNAME>\n");
	printf(" -h\n");
}
