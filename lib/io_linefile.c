#include "sc_verbose.h"
#include "io_linefile.h"
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>


#define LINES_STEP 1000000
#define LINE_LENGTH 2000
#define LINES_READIN 1000

//put i/d/sx 's address into i/d/slist
static void set_list_LineFile(struct LineFile *lf) {
	lf->ilist[0] = &(lf->i1);
	lf->ilist[1] = &(lf->i2);
	lf->ilist[2] = &(lf->i3);
	lf->ilist[3] = &(lf->i4);
	lf->ilist[4] = &(lf->i5);
	lf->ilist[5] = &(lf->i6);
	lf->ilist[6] = &(lf->i7);
	lf->ilist[7] = &(lf->i8);
	lf->ilist[8] = &(lf->i9);

	lf->dlist[0] = &(lf->d1);
	lf->dlist[1] = &(lf->d2);
	lf->dlist[2] = &(lf->d3);
	lf->dlist[3] = &(lf->d4);
	lf->dlist[4] = &(lf->d5);
	lf->dlist[5] = &(lf->d6);
	lf->dlist[6] = &(lf->d7);
	lf->dlist[7] = &(lf->d8);
	lf->dlist[8] = &(lf->d9);

	lf->slist[0] = &(lf->s1);
	lf->slist[1] = &(lf->s2);
	lf->slist[2] = &(lf->s3);
	lf->slist[3] = &(lf->s4);
	lf->slist[4] = &(lf->s5);
	lf->slist[5] = &(lf->s6);
	lf->slist[6] = &(lf->s7);
	lf->slist[7] = &(lf->s8);
	lf->slist[8] = &(lf->s9);
	printsf("fill ilist&dlist&slist with the address of ix&dx&sx.");
}

void set_filename_LineFile(struct LineFile *lf, char *filename) {
	int len = strlen(filename)+1;
	char *fn = smalloc(len);
	memcpy(fn, filename, len);
	free(lf->filename);
	lf->filename = fn;
}

//create an empty but completive LineFile.
static struct LineFile *init_LineFile(void) {
	struct LineFile *lf = smalloc(sizeof(struct LineFile));
	lf->linesNum = 0;
	lf->memNum = 0;
	lf->filename = NULL;
	set_filename_LineFile(lf, "free-valid");

	lf->iNum = 9;
	lf->dNum = 9;
	lf->sNum = 9;
	lf->ilist = smalloc(lf->iNum*sizeof(int **));
	lf->dlist = smalloc(lf->dNum*sizeof(double **));
	lf->slist = smalloc(lf->sNum*sizeof(char ***));

	set_list_LineFile(lf);

	int i;
	for (i = 0; i < lf->iNum; ++i) {
		*(lf->ilist[i]) = NULL;
	}
	for (i = 0; i < lf->dNum; ++i) {
		*(lf->dlist[i]) = NULL;
	}
	for (i = 0; i < lf->sNum; ++i) {
		*(lf->slist[i]) = NULL;
	}
	printsf("generate a empty free-valid LineFile struct and return.");
	return lf;
}
//alloc memory according to typelist.
static void init_memory_LineFile(struct LineFile *lf, int vn, int *typelist) {
	int ii = 0;
	int di = 0;
	int si = 0;
	int ***ilist = lf->ilist;
	double ***dlist = lf->dlist;
	char ****slist = lf->slist;
	int i;
	for (i = 0; i < vn; ++i) {
		int type = typelist[i];
		switch(type) {
			case 1:
				if (ii < lf->iNum) {
					*(ilist[ii++]) = smalloc(LINES_STEP * sizeof(int));
				}
				else {
					isError("%s =>> ilimit too small.", __func__);
				}
				break;
			case 2:
				if (di < lf->dNum) {
					*(dlist[di++]) = smalloc(LINES_STEP * sizeof(double));
				}
				else {
					isError("%s =>> dlimit too small.", __func__);
				}
				break;
			case 3:
				if (si < lf->sNum) {
					*(slist[si++]) = smalloc(LINES_STEP * sizeof(char *));
				}
				else {
					isError("%s =>> slimit too small.", __func__);
				}
				break;
			default:
				isError("%s =>> wrong type.");
				break;
		}
	}
	lf->memNum += LINES_STEP;
	printsf("first time allocate memory according to typelist. now memNum is %ld, lf->linesNum is %ld.", lf->memNum, lf->linesNum);
}
//increase memory, not need typelist anymore, just check whether point is NULL or not.
static void add_memory_LineFile(struct LineFile *lf) {
	int ***ilist = lf->ilist;
	double ***dlist = lf->dlist;
	char ****slist = lf->slist;
	int i;
	for (i=0; i<lf->iNum; ++i) {
		if (*(ilist[i]) != NULL) {
			*(ilist[i]) = srealloc(*(ilist[i]), (size_t)(lf->memNum + LINES_STEP)*sizeof(int));
		}
	}
	for (i=0; i<lf->dNum; ++i) {
		if (*(dlist[i]) != NULL) {
			*(dlist[i]) = srealloc(*(dlist[i]), (size_t)(lf->memNum + LINES_STEP)*sizeof(double));
		}
	}
	for (i=0; i<lf->sNum; ++i) {
		if (*(slist[i]) != NULL) {
			*(slist[i]) = srealloc(*(slist[i]), (size_t)(lf->memNum + LINES_STEP)*sizeof(char *));
		}
	}
	lf->memNum += LINES_STEP;
	printsf("increase memory, now memNum is %ld, linesNum is %ld.", lf->memNum, lf->linesNum);
}
static void set_buffer_LineFile(FILE *fp, char *buffer, int *lread) {
	char *line = buffer;
	*lread = 0;
	while((*lread) != LINES_READIN && fgets(line, LINE_LENGTH, fp)) {
		line += LINE_LENGTH;
		++(*lread);
	}
}
static void set_allparts_LineFile(char *buffer, char **allparts, int vn, int lread) {
	int i,j;
	char *line = buffer;
	char *delimiter = "\t ,:\n";
	for (i = 0; i < lread; ++i) {
		allparts[i] =strtok(line, delimiter);
		for (j=1; j<vn; ++j) {
			allparts[i + j * LINES_READIN] = strtok(NULL, delimiter);
		}
		line += LINE_LENGTH;
	}
}
static void set_lf_LineFile(struct LineFile *lf, char **allparts, int *typelist, int lread, int vn, char *isok) {
	int ***ilist = lf->ilist;
	double ***dlist = lf->dlist;
	char ****slist = lf->slist;
	int IL = 0, DL = 0, SL = 0;
	int *ip;
	double *dp;
	char **sp;

	int i,j;
	char *pend;
	for (i = 0; i < vn; ++i) {
		int type = typelist[i];
		char **p = allparts + i*LINES_READIN;
		switch(type) {
			case 1:
				ip = *(ilist[IL++]);
				for (j = 0; j < lread; ++j) {
					if (p[j] != NULL) {
						ip[j+lf->linesNum] = strtol(p[j], &pend, 10);
						if (pend[0]!='\0') {
							printer("wrong line: \"%s\" file, line: %ld, i%d part.\n", lf->filename, j+lf->linesNum, IL);
							*isok = 0;
						}
					}
					else {
						ip[j+lf->linesNum] = -1;
					}
				}
				break;
			case 2:
				dp = *(dlist[DL++]);
				for (j = 0; j < lread; ++j) {
					if (p[j] != NULL) {
						dp[j+lf->linesNum] = strtod(p[j], &pend);
						if (pend[0]!='\0') {
							printer("wrong line: \"%s\" file, line: %ld, d%d part.\n", lf->filename, j+lf->linesNum, DL);
							*isok = 0;
						}
					}
					else {
						dp[j+lf->linesNum] = -1;
					}
				}
				break;
			case 3:
				sp = *(slist[SL++]);
				for (j = 0; j < lread; ++j) {
					if (p[j] != NULL) {
						int size = strlen(p[j]) + 1;
						sp[j+lf->linesNum] = smalloc(size*sizeof(char));
						memcpy(sp[j+lf->linesNum], p[j], size);
					}
					else {
						sp[j+lf->linesNum] = NULL;
					}
				}
				break;
			default:
				isError("%s =>> wrong type.", __func__);
		}
	}
	lf->linesNum += lread;
}

#if VERBOSE_LEVEL >= 10
static char *typetype[] = {"int", "double", "c-string"};
#endif

struct LineFile *create_LineFile(char *filename, ...) {
	//the return lf.
	struct LineFile *lf = init_LineFile();

	if (NULL == filename) {
		printgf("filename == NULL, return an empty free-valid linefile.");
		return lf;
	}

	//get typelist.
	int argMax = lf->iNum + lf->dNum + lf->sNum;
	int *typelist = smalloc(argMax*sizeof(int));
	va_list vl;
	va_start(vl, filename);
	int vn = 0, type = -2;
	printgf("detected line style: ");
	while (1 == (type = va_arg(vl, int)) || 2 == type || 3 == type) {
		if (vn < argMax) {
			typelist[vn++] = type;
			printgf("\ttype of column%d is \"%s\"", vn, typetype[type - 1]);
		}
		else {
			isError("%s =>> too much args.", __func__);
		}
	}
	va_end(vl);

	if (0 == vn || type != -1) {
		free(typelist);
		printgf("not valid types, return an empty free-valid linefile.");
		return lf;
	}
	set_filename_LineFile(lf, filename);

	//check filename.
	FILE *fp = sfopen(filename, "r");

	printgf("open \"%s\" successfully.", filename);

	//set lf memory with typelist.
	init_memory_LineFile(lf, vn, typelist);

	//buffer used to read file.
	char isok = 1;
	char *buffer = smalloc(LINE_LENGTH * LINES_READIN * sizeof(char));
	char **allparts = smalloc(vn * LINES_READIN * sizeof(char *));
	int lread = LINES_READIN;
	while (lread == LINES_READIN) {
		set_buffer_LineFile(fp, buffer, &lread);
		set_allparts_LineFile(buffer, allparts, vn, lread);
		while (lf->linesNum + lread > lf->memNum) {
			add_memory_LineFile(lf);
		}
		set_lf_LineFile(lf, allparts, typelist, lread, vn, &isok);
	}
	printgf("totally read in %ld lines.", lf->linesNum);
	free(typelist);
	fclose(fp);
	free(buffer);
	free(allparts);

	if (isok == 0 ) {
		isError("file \"%s\" has some non-valid lines.", filename);
	} 

	return lf;
}

void free_LineFile(struct LineFile *lf) {
	printgf("free a struct LineFile named \"%s\".", lf->filename);
	int i;
	long j;
	for (i = 0; i < lf->iNum; ++i) {
		free(*(lf->ilist[i]));
	}
	for (i = 0; i < lf->dNum; ++i) {
		free(*(lf->dlist[i]));
	}
	for (i = 0; i < lf->sNum; ++i) {
		if (*(lf->slist[i]) != NULL) {
			for (j = 0; j < lf->linesNum; ++j) {
				free((*(lf->slist[i]))[j]);
			}
		}
		free(*(lf->slist[i]));
	}
	free(lf->ilist);
	free(lf->dlist);
	free(lf->slist);
	free(lf->filename);
	free(lf);
}

void print_LineFile(struct LineFile *lf, char *filename) {
	if (NULL == lf) {
		printgf("lf == NULL, print nothing.\n");
		return;
	}
	FILE *fp = sfopen(filename, "w");
	int ***ilist = lf->ilist;
	double ***dlist = lf->dlist;
	char ****slist = lf->slist;
	int i;
	long j;
	for (j = 0; j < lf->linesNum; ++j) {
		for (i=0; i<lf->iNum; ++i) {
			if (*(ilist[i]) != NULL) {
				fprintf(fp, "%d\t", (*(ilist[i]))[j]);
			}
		}
		for (i=0; i<lf->dNum; ++i) {
			if (*(dlist[i]) != NULL) {
				fprintf(fp, "%f\t", (*(dlist[i]))[j]);
			}
		}
		for (i=0; i<lf->sNum; ++i) {
			if (*(slist[i]) != NULL) {
				fprintf(fp, "%s\t", (*(slist[i]))[j]);
			}
		}
		fprintf(fp, "\n");
	}
	fclose(fp);
	printgf("print a struct LineFile named \"%s\" into \"%s\".\n", lf->filename, filename);
}

struct LineFile *add_LineFile(struct LineFile *lf1, struct LineFile *lf2) {
	if (lf1 == NULL || lf2 == NULL) {
		printgf("lf1 or lf2 is NULL, return NULL.\n");
		return NULL;
	}

	struct LineFile *lf = init_LineFile();

	int ***ilist = lf->ilist;
	double ***dlist = lf->dlist;
	char ****slist = lf->slist;
	int ***ilist1 = lf1->ilist;
	double ***dlist1 = lf1->dlist;
	char ****slist1 = lf1->slist;
	int ***ilist2 = lf2->ilist;
	double ***dlist2 = lf2->dlist;
	char ****slist2 = lf2->slist;

	lf->linesNum = lf1->linesNum + lf2->linesNum;

	int i;
	long j;
	for (i=0; i<lf->iNum; ++i) {
		if (*(ilist1[i]) != NULL && *(ilist2[i]) != NULL) {
			*(ilist[i]) = smalloc((lf->linesNum)*sizeof(int));
			for (j = 0; j < lf1->linesNum; ++j) {
				(*(ilist[i]))[j] = (*(ilist1[i]))[j];
			}
			for (j = 0; j < lf2->linesNum; ++j) {
				(*(ilist[i]))[j+lf1->linesNum] = (*(ilist2[i]))[j];
			}
		}
		else if (*(ilist1[i]) == NULL && *(ilist2[i]) == NULL) {
		}
		else {
			isError("%s =>> lf1 and lf2 have different int Structures, can not add lf1 with lf2.\n", __func__);
		}
	}
	for (i=0; i<lf->dNum; ++i) {
		if (*(dlist1[i]) != NULL && *(dlist2[i]) != NULL) {
			*(dlist[i]) = smalloc((lf1->linesNum + lf2->linesNum)*sizeof(double));
			for (j = 0; j < lf1->linesNum; ++j) {
				(*(dlist[i]))[j] = (*(dlist1[i]))[j];
			}
			for (j = 0; j < lf2->linesNum; ++j) {
				(*(dlist[i]))[j+lf1->linesNum] = (*(dlist2[i]))[j];
			}
		}
		else if (*(dlist1[i]) == NULL && *(dlist2[i]) == NULL) {
		}
		else {
			isError("%s =>> lf1 and lf2 have different double Structures, can not add lf1 with lf2.\n", __func__);
		}
	}
	for (i=0; i<lf->sNum; ++i) {
		if (*(slist1[i]) != NULL && *(slist2[i]) != NULL) {
			*(slist[i]) = smalloc((lf->linesNum)*sizeof(char *));
			for (j = 0; j < lf1->linesNum; ++j) {
				int size = strlen((*(slist1[i]))[j]) + 1;
				(*(slist[i]))[j] = smalloc(size*sizeof(char));
				memcpy((*(slist[i]))[j], (*(slist1[i]))[j], size*sizeof(char));
			}
			for (j = 0; j < lf2->linesNum; ++j) {
				int size = strlen((*(slist2[i]))[j]) + 1;
				(*(slist[i]))[j+lf1->linesNum] = smalloc(size*sizeof(char));
				memcpy((*(slist[i]))[j+lf1->linesNum], (*(slist2[i]))[j], size*sizeof(char));
			}
		}
		else if (*(slist1[i]) == NULL && *(slist2[i]) == NULL) {
		}
		else {
			isError("%s =>> lf1 and lf2 have different c-string Structures, can not add lf1 with lf2.\n", __func__);
		}
	}

	lf->memNum = lf->linesNum;
	char addfn[1000];
	sprintf(addfn, "%s+%s", lf1->filename, lf2->filename);
	set_filename_LineFile(lf, addfn);
	printgf("add LF \"%s\" and LF \"%s\", get the new LF \"%s\"", lf1->filename, lf2->filename, lf->filename);
	printgf("linesNum of LF \"%s\" is %ld and linesNum of LF \"%s\" is %ld, linesNum of the new LF \"%s\" is %ld", lf1->filename, lf1->linesNum, lf2->filename, lf2->linesNum, lf->filename, lf->linesNum);
	return lf;
}

struct LineFile *clone_LineFile(struct LineFile *lf) {
	if (lf == NULL) {
		printgf("source LineFile is NULL, return NULL.\n");
		return NULL;
	}

	int ***ilist 	= 	lf->ilist;
	double ***dlist = 	lf->dlist;
	char ****slist = 	lf->slist;

	struct LineFile *newlf = init_LineFile();
	int ***ilist_n = 	newlf->ilist;
	double ***dlist_n = 	newlf->dlist;
	char ****slist_n = 	newlf->slist;

	newlf->linesNum = lf->linesNum;

	int i;
	long j;
	for (i=0; i<lf->iNum; ++i) {
		if (*(ilist[i]) != NULL) {
			*(ilist_n[i]) = smalloc((lf->linesNum)*sizeof(int));
			for (j = 0; j < lf->linesNum; ++j) {
				(*(ilist_n[i]))[j] = (*(ilist[i]))[j];
			}
		}
	}
	for (i=0; i<lf->dNum; ++i) {
		if (*(dlist[i]) != NULL) {
			*(dlist_n[i]) = smalloc((lf->linesNum)*sizeof(double));
			for (j = 0; j < lf->linesNum; ++j) {
				(*(dlist_n[i]))[j] = (*(dlist[i]))[j];
			}
		}
	}
	for (i=0; i<lf->sNum; ++i) {
		if (*(slist[i]) != NULL) {
			*(slist_n[i]) = smalloc((lf->linesNum)*sizeof(char *));
			for (j = 0; j < lf->linesNum; ++j) {
				int size = strlen((*(slist[i]))[j]) + 1;
				(*(slist_n[i]))[j] = smalloc(size*sizeof(char));
				memcpy((*(slist_n[i]))[j], (*(slist[i]))[j], size*sizeof(char));
			}
		}
	}

	newlf->memNum = newlf->linesNum;
	char addfn[1000];
	sprintf(addfn, "clone_%s", lf->filename);
	set_filename_LineFile(newlf, addfn);
	printgf("clone LF \"%s\", get new LF \"%s\"; linesNum in \"%s\" is %ld, linesNum in \"%s\" is %ld.", lf->filename, newlf->filename, lf->filename, lf->linesNum, newlf->filename, newlf->linesNum);
	return newlf;
}
