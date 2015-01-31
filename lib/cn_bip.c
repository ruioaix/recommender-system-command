#include "cn_bip.h"
#include "sc_verbose.h"
#include "sc_mtprand.h"
#include "sc_sort.h"
#include <math.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>

/*********************************************************************/
/***operation on struct Bip*******************************************/
/*********************************************************************/
static struct Bip *init_Bip(void) {
	struct Bip *bip = smalloc(sizeof(struct Bip));
	//baisc
	bip->edgesNum = 0;
	bip->maxId = 0;
	bip->minId = 0;
	bip->idNum = 0;
	bip->degreeMax = 0;
	bip->degreeMin = 0;

	bip->degree = NULL;
	bip->attI1 = NULL;
	bip->attI2 = NULL;
	bip->attD1 = NULL;

	bip->edges = NULL;
	bip->edgesI = NULL;
	bip->edgesD = NULL;

#if CA_METRICS_BIP > 1
	//additional
	//just for init.
	int i;
	for (i = 0; i < CA_METRICS_BIP; ++i) {
		bip->att1[i] = 0;
		bip->att2[i] = 0;
	}
#endif

	return bip;
}

static void set_maxId_minId_create_Bip(int *i1, int *i2, long edgesNum, int index, int *maxId, int *minId) {
	//set minId&maxId.
	long i;
	if (1 == index) {
		*minId = *maxId = i1[0];
		for(i=0; i<edgesNum; ++i) {
			*maxId = *maxId > i1[i] ? *maxId : i1[i];
			*minId = *minId < i1[i] ? *minId : i1[i];
		}
	}
	else {
		*minId = *maxId = i2[0];
		for(i=0; i<edgesNum; ++i) {
			*maxId = *maxId > i2[i] ? *maxId : i2[i];
			*minId = *minId < i2[i] ? *minId : i2[i];
		}
	}
}

static int *set_degree_create_Bip(int *i1, int *i2, int maxId, long edgesNum, int index) {
	int *degree = scalloc(maxId+1, sizeof(int));
	int i;
	if (1 == index) {
		for(i=0; i<edgesNum; ++i) {
			++degree[i1[i]];
		}
	}
	else {
		for(i=0; i<edgesNum; ++i) {
			++degree[i2[i]];
		}
	}
	return degree;
}

static void prepare_edges_attr_and_set_degreeMaxMin_idNum_create_Bip(int *i3, double *d1, int maxId, int *degree, int *idNum_retn, int *degreeMax_retn, int *degreeMin_retn, int ***edges_retn, int ***edgesI_retn, double ***edgesD_retn) {
	int **edges = smalloc((maxId+1)*sizeof(void *));
	int **edgesI = NULL;
	double **edgesD = NULL;
	int degreeMax = -1;
	int degreeMin = INT_MAX;
	int idNum = 0;
	
	if (i3 != NULL) {
		edgesI = smalloc((maxId + 1) * sizeof(int *));
	}
	if (d1 != NULL) {
		edgesD = smalloc((maxId + 1) * sizeof(double *));
	}
	int j;
	for(j=0; j<maxId+1; ++j) {
		if (degree[j]>0) {
			degreeMax = degreeMax>degree[j]?degreeMax:degree[j];
			degreeMin = degreeMin<degree[j]?degreeMin:degree[j];
			++idNum;
			edges[j]=smalloc(degree[j]*sizeof(int));
		}
		else {
			edges[j] = NULL;
		}
	}
	if (i3 != NULL) {
		for(j=0; j<maxId+1; ++j) {
			if (degree[j]>0) {
				edgesI[j]=smalloc(degree[j]*sizeof(int));
			}
			else {
				edgesI[j] = NULL;
			}
		}
	}
	if (d1 != NULL) {
		for (j = 0; j < maxId + 1; ++j) {
			if (degree[j] > 0) {
				edgesD[j] = smalloc(degree[j] * sizeof(double));	
			}
			else {
				edgesD[j] = NULL;
			}
		}
	}
	*degreeMax_retn = degreeMax;
	*degreeMin_retn = degreeMin;
	*edges_retn = edges;
	*edgesI_retn = edgesI;
	*edgesD_retn = edgesD;
	*idNum_retn = idNum;
}

static void set_edges_attr_create_Bip(int *i1, int *i2, int *i3, double *d1, int maxId, int index, long edgesNum, int ***edges, int ***edgesI, double ***edgesD) {
	//fill edges
	long i;
	int *temp = scalloc(maxId+1, sizeof(int));
	if (1 == index) {
		if (i3 != NULL && d1 != NULL) {
			for(i=0; i<edgesNum; ++i) {
				int ii1 =i1[i];
				(*edges)[ii1][temp[ii1]]=i2[i];
				(*edgesI)[ii1][temp[ii1]] = i3[i];
				(*edgesD)[ii1][temp[ii1]] = d1[i];
				++temp[ii1];
			}
		}
		else if (i3 != NULL) {
			for(i=0; i<edgesNum; ++i) {
				int ii1 =i1[i];
				(*edges)[ii1][temp[ii1]]=i2[i];
				(*edgesI)[ii1][temp[ii1]] = i3[i];
				++temp[ii1];
			}
		}
		else if (d1 != NULL) {
			for(i=0; i<edgesNum; ++i) {
				int ii1 =i1[i];
				(*edges)[ii1][temp[ii1]]=i2[i];
				(*edgesD)[ii1][temp[ii1]] = d1[i];
				++temp[ii1];
			}
		}
		else {
			for(i=0; i<edgesNum; ++i) {
				int ii1 =i1[i];
				(*edges)[ii1][temp[ii1]]=i2[i];
				++temp[ii1];
			}
		}
	}
	else {
		if (i3 != NULL && d1 != NULL) {
			for(i=0; i<edgesNum; ++i) {
				int ii2 =i2[i];
				(*edges)[ii2][temp[ii2]]=i1[i];
				(*edgesI)[ii2][temp[ii2]] = i3[i];
				(*edgesD)[ii2][temp[ii2]] = d1[i];
				++temp[ii2];
			}
		}
		else if (i3 != NULL) {
			for(i=0; i<edgesNum; ++i) {
				int ii2 =i2[i];
				(*edges)[ii2][temp[ii2]]=i1[i];
				(*edgesI)[ii2][temp[ii2]] = i3[i];
				++temp[ii2];
			}
		}
		else if (d1 != NULL) {
			for(i=0; i<edgesNum; ++i) {
				int ii2 =i2[i];
				(*edges)[ii2][temp[ii2]]=i1[i];
				(*edgesD)[ii2][temp[ii2]] = d1[i];
				++temp[ii2];
			}
		}
		else {
			for(i=0; i<edgesNum; ++i) {
				int ii2 =i2[i];
				(*edges)[ii2][temp[ii2]]=i1[i];
				++temp[ii2];
			}
		}
	}
	free(temp);
}

static void set_attID_Bip(const struct LineFile * const af, struct Bip *bip) {
	if (af == NULL) return;

	if (af->i2 != NULL) {
		bip->attI1 = scalloc((bip->maxId + 1), sizeof(int));
	}
	if (af->i3 != NULL) {
		bip->attI2 = scalloc((bip->maxId + 1), sizeof(int));
	}
	if (af->d1 != NULL) {
		bip->attD1 = scalloc((bip->maxId + 1), sizeof(double));
	}
	
	long j;
	for (j = 0; j < af->linesNum; ++j) {
		if (af->i1[j] > bip->maxId) isError("attribute file has a id %d larger than maxId.", af->i1[j]);
		if (bip->attI1 != NULL) bip->attI1[j] = af->i2[af->i1[j]];
		if (bip->attI2 != NULL) bip->attI2[j] = af->i3[af->i1[j]];
		if (bip->attD1 != NULL) bip->attD1[j] = af->d1[af->i1[j]];
	}
}

//kind of simple, just create struct Bip.
//index is 1, i1 of struct LineFile will be the index.
//index is 2, i2 of struct LineFile will be the index.
struct Bip *create_Bip(const struct LineFile * const lf, const struct LineFile * const af, int index) {
	if (lf == NULL || lf->i1 == NULL || lf->i2 == NULL || lf->linesNum < 1) isError("create_Bip lf: %p, lf->i1: %p, lf->i2: %p, lf->linesNum: %ld", lf, lf->i1, lf->i2, lf->linesNum);
	if (index != 1 && index != 2) isError("create_Bip index: %d", index);

	int *i1 = lf->i1;
	int *i2 = lf->i2;

	struct Bip *Bip = init_Bip();
	//set edgesNum
	Bip->edgesNum = lf->linesNum;
	//set maxId minId
	set_maxId_minId_create_Bip(i1, i2, lf->linesNum, index, &(Bip->maxId), &(Bip->minId));
	//set degree
	Bip->degree = set_degree_create_Bip(i1, i2, Bip->maxId, lf->linesNum, index);
	//set idNum, degreeMax, degreeMin
	prepare_edges_attr_and_set_degreeMaxMin_idNum_create_Bip(lf->i3, lf->d1, Bip->maxId, Bip->degree, &(Bip->idNum), &(Bip->degreeMax), &(Bip->degreeMin), &(Bip->edges), &(Bip->edgesI), &(Bip->edgesD));
	//set edges, edgesI, edgesD
	set_edges_attr_create_Bip(i1, i2, lf->i3, lf->d1, Bip->maxId, index, lf->linesNum, &(Bip->edges), &(Bip->edgesI), &(Bip->edgesD));	

	//set attI1, attI2, attD1
	set_attID_Bip(af, Bip);

	printgf("file:%s, index:%d, Max: %5d, Min: %5d, Num: %5d, degreeMax: %5d, degreeMin: %5d, edgesNum: %5ld", \
			lf->filename, index, Bip->maxId, Bip->minId, Bip->idNum, Bip->degreeMax, Bip->degreeMin, Bip->edgesNum);

	return Bip;
}

void sort_desc_by_edges_Bip(struct Bip *bip) {
	int i;
	if (bip->edgesI != NULL && bip->edgesD != NULL) {
		for (i = 0; i < bip->maxId + 1; ++i) {
			if (bip->degree[i]) {
				qsort_iid_desc(bip->edges[i], 0, bip->degree[i] - 1, bip->edgesI[i], bip->edgesD[i]);
			}
		}
	}
	else if (bip->edgesI != NULL) {
		for (i = 0; i < bip->maxId + 1; ++i) {
			if (bip->degree[i]) {
				qsort_ii_desc(bip->edges[i], 0, bip->degree[i] - 1, bip->edgesI[i]);
			}
		}
	}
	else if (bip->edgesD != NULL) {
		for (i = 0; i < bip->maxId + 1; ++i) {
			if (bip->degree[i]) {
				qsort_id_desc(bip->edges[i], 0, bip->degree[i] - 1, bip->edgesD[i]);
			}
		}
	}
	else {
		for (i = 0; i < bip->maxId + 1; ++i) {
			if (bip->degree[i]) {
				qsort_i_desc(bip->edges[i], 0, bip->degree[i] - 1);
			}
		}
	}
	printgf("done.");
}

void free_Bip(struct Bip *bip) {
	int i=0;
	for(i=0; i<bip->maxId+1; ++i) {
		free(bip->edges[i]);
		if (bip->edgesI != NULL) {
			free(bip->edgesI[i]);
		}
		if (bip->edgesD != NULL) {
			free(bip->edgesD[i]);
		}
	}
	free(bip->degree);
	free(bip->attI1);
	free(bip->attI2);
	free(bip->attD1);
	free(bip->edges);
	free(bip->edgesI);
	free(bip->edgesD);
	free(bip);
	printgf("done.");
}

struct Bip * clone_Bip(struct Bip *bip) {
	struct Bip *new = smalloc(sizeof(struct Bip));
	new->degree = smalloc((bip->maxId + 1) * sizeof(int));
	memcpy(new->degree, bip->degree, (bip->maxId + 1) * sizeof(int));

	if (bip->attI1 != NULL) {
		memcpy(new->attI1, bip->attI1, (bip->maxId + 1) * sizeof(int));
	}
	if (bip->attI2 != NULL) {
		memcpy(new->attI2, bip->attI2, (bip->maxId + 1) * sizeof(int));
	}
	if (bip->attD1 != NULL) {
		memcpy(new->attD1, bip->attD1, (bip->maxId + 1) * sizeof(double));
	}

	int i;
#if CA_METRICS_BIP > 1
	for (i = 0; i < CA_METRICS_BIP; ++i) {
		new->att1[i] = bip->att1[i];
		new->att2[i] = bip->att2[i];
	}
#endif

	new->edges = smalloc((bip->maxId + 1) * sizeof(int *));
	if (bip->edgesI != NULL && bip->edgesD != NULL) {
		new->edgesI = smalloc((bip->maxId + 1)*sizeof(int *));
		new->edgesD = smalloc((bip->maxId + 1)*sizeof(double *));
		for (i = 0; i < bip->maxId + 1; ++i) {
			if (new->degree[i]) {
				new->edges[i] = smalloc(new->degree[i]*sizeof(int));
				memcpy(new->edges[i], bip->edges[i], new->degree[i]*sizeof(int));
				new->edgesI[i] = smalloc(new->degree[i]*sizeof(int));
				memcpy(new->edgesI[i], bip->edgesI[i], new->degree[i]*sizeof(int));
				new->edgesD[i] = smalloc(new->degree[i]*sizeof(double));
				memcpy(new->edgesD[i], bip->edgesD[i], new->degree[i]*sizeof(double));
			}
			else {
				new->edges[i] = NULL;
				new->edgesI[i] = NULL;
				new->edgesD[i] = NULL;
			}
		}
	}
	else if (bip->edgesI != NULL) {
		new->edgesI = smalloc((bip->maxId + 1)*sizeof(int *));
		new->edgesD = NULL;
		for (i = 0; i < bip->maxId + 1; ++i) {
			if (new->degree[i]) {
				new->edges[i] = smalloc(new->degree[i]*sizeof(int));
				memcpy(new->edges[i], bip->edges[i], new->degree[i]*sizeof(int));
				new->edgesI[i] = smalloc(new->degree[i]*sizeof(int));
				memcpy(new->edgesI[i], bip->edgesI[i], new->degree[i]*sizeof(int));
			}
			else {
				new->edges[i] = NULL;
				new->edgesI[i] = NULL;
			}
		}
	}
	else if (bip->edgesD != NULL) {
		new->edgesI = NULL;
		new->edgesD = smalloc((bip->maxId + 1)*sizeof(double *));
		for (i = 0; i < bip->maxId + 1; ++i) {
			if (new->degree[i]) {
				new->edges[i] = smalloc(new->degree[i]*sizeof(int));
				memcpy(new->edges[i], bip->edges[i], new->degree[i]*sizeof(int));
				new->edgesD[i] = smalloc(new->degree[i]*sizeof(double));
				memcpy(new->edgesD[i], bip->edgesD[i], new->degree[i]*sizeof(double));
			}
			else {
				new->edges[i] = NULL;
				new->edgesD[i] = NULL;
			}
		}
	}
	else {
		new->edgesI = NULL;
		new->edgesD = NULL;
		for (i=0; i<bip->maxId +1 ; ++i) {
			if (new->degree[i]) {
				new->edges[i] = smalloc(new->degree[i]*sizeof(int));
				memcpy(new->edges[i], bip->edges[i], new->degree[i]*sizeof(int));
			}
			else {
				new->edges[i] = NULL;
			}
		}
	}

	new->maxId = bip->maxId;
	new->minId = bip->minId;
	new->idNum = bip->idNum;
	new->edgesNum = bip->edgesNum;
	new->degreeMax = bip->degreeMax;
	new->degreeMin = bip->degreeMin;
	printgf("done.");

	return new;
}

void verify_Bip(struct Bip *bipi1, struct Bip *bipi2) {
	int i;
	int j,k;
	int *place = smalloc((bipi2->maxId+1)*sizeof(int));
	FILE *fp = sfopen("data/duplicatePairsinNet", "w");
	FILE *fp2 = sfopen("data/NoDuplicatePairsNetFile", "w");
	fprintf(fp, "the following pairs are duplicate in the net file\n");
	int sign=0;
	for (j=0; j<bipi1->maxId+1; ++j) {
		if (bipi1->degree[j]>0) {
			for (k=0; k<bipi2->maxId + 1; ++k) {
				place[k] = -1;
			}
			for (i=0; i<bipi1->degree[j]; ++i) {
				int origin = bipi1->edges[j][i];
				int next = place[origin];
				if (next == -1) {
					place[origin]=origin;
					fprintf(fp2, "%d\t%d\n", j, origin);
				}
				else {
					fprintf(fp, "%d\t%d\n", j, next);
					sign=1;
				}
			}
		}
	}
	free(place);
	fclose(fp);
	fclose(fp2);
	if (sign == 1) {
		isError("the file has duplicate pairs, you can check data/duplicatePairsinNet.\nwe generate a net file named data/NoDuplicatePairsNetFile which doesn't contain any duplicate pairsr.\nyou should use this file instead the origin wrong one.\n");
	}
	printgf("perfect network.\n");
}

void print_Bip(struct Bip *bip, char *filename) {
	FILE *fp = sfopen(filename, "w");
	int i;
	int j;
	for (i = 0; i < bip->maxId + 1; ++i) {
		for (j = 0; j < bip->degree[i]; ++j) {
			fprintf(fp, "%d\t%d", i, bip->edges[i][j]);
			if (bip->edgesI != NULL) {
				fprintf(fp, "\t%d", bip->edgesI[i][j]);
			}
			if (bip->edgesD != NULL) {
				fprintf(fp, "\t%f", bip->edgesD[i][j]);
			}
			fprintf(fp, "\n");
		}
	}
	fclose(fp);
	printgf("print %s done.\n", filename);
}

//divide Bip into two parts.
//return two struct LineFile. 
//the first one is always the small one.
//the second is always the large one.
void divide_Bip(struct Bip *bipi1, struct Bip *bipi2, double rate, struct LineFile **small, struct LineFile **big) {
	if (rate <=0 || rate >= 1) {
		isError("divide_Bip error: wrong rate.\n");
	}
	rate = rate>0.5?1-rate:rate;

	long l1, l2;
	if (bipi1->edgesNum > 100000) {
		l1 = (long)(bipi1->edgesNum*(rate+0.1));
		l2 = (long)(bipi1->edgesNum*(1-rate+0.1));
	}
	else {
		l2 = l1 = bipi1->edgesNum;
	}

	*small = create_LineFile(NULL);
	*big = create_LineFile(NULL);

	(*small)->i1 = smalloc(l1*sizeof(int));
	(*small)->i2 = smalloc(l1*sizeof(int));
	(*big)->i1 = smalloc(l2*sizeof(int));
	(*big)->i2 = smalloc(l2*sizeof(int));
	
	if (bipi1->edgesI != NULL) {
		(*small)->i3 = smalloc(l1*sizeof(int));
		(*big)->i3 = smalloc(l2*sizeof(int));
	}
	if (bipi1->edgesD != NULL) {
		(*small)->d1 = smalloc(l1 * sizeof(double));
		(*big)->d1 = smalloc(l2 * sizeof(double));
	}

	long line1=0, line2=0;

	int *i1sign = scalloc(bipi1->maxId + 1, sizeof(int));
	int *i2sign = scalloc(bipi2->maxId + 1, sizeof(int));

	int *degreei1 = smalloc((bipi1->maxId + 1)*sizeof(int));
	memcpy(degreei1, bipi1->degree, (bipi1->maxId + 1)*sizeof(int));
	int *degreei2 = smalloc((bipi2->maxId + 1)*sizeof(int));
	memcpy(degreei2, bipi2->degree, (bipi2->maxId + 1)*sizeof(int));

	int i, j, neigh;
	for (i=0; i<bipi1->maxId + 1; ++i) {
		for (j=0; j<bipi1->degree[i]; ++j) {
			neigh = bipi1->edges[i][j];
			if (get_d01_MTPR() < rate) {
				if ((degreei1[i] == 1 && i1sign[i] == 0) || (degreei2[neigh] == 1 && i2sign[neigh] == 0)) {
					(*big)->i1[line2] = i;	
					(*big)->i2[line2] = neigh;	
					if (bipi1->edgesI != NULL) {
						(*big)->i3[line2] = bipi1->edgesI[i][j];
					}
					if (bipi1->edgesD != NULL) {
						(*big)->d1[line2] = bipi1->edgesD[i][j];
					}
					--degreei1[i];
					--degreei2[neigh];
					i1sign[i] = 1;
					i2sign[neigh] = 1;
					++line2;
					continue;
				}
				(*small)->i1[line1] = i;	
				(*small)->i2[line1] = neigh;	
				if (bipi1->edgesI != NULL) {
					(*small)->i3[line1] = bipi1->edgesI[i][j];
				}
				if (bipi1->edgesD != NULL) {
					(*small)->d1[line1] = bipi1->edgesD[i][j];
				}
				--degreei1[i];
				--degreei2[neigh];
				++line1;
			}
			else {
				(*big)->i1[line2] = i;	
				(*big)->i2[line2] = neigh;	
				if (bipi1->edgesI != NULL) {
					(*big)->i3[line2] = bipi1->edgesI[i][j];
				}
				if (bipi1->edgesD != NULL) {
					(*big)->d1[line2] = bipi1->edgesD[i][j];
				}
				i1sign[i] = 1;
				i2sign[neigh] = 1;
				--degreei1[i];
				--degreei2[neigh];
				++line2;
			}
		}
	}
	if ((line1 > l1) || (line2 > l2)) {
		isError("l1 and l2 two small.\n");
	}

	free(i1sign);
	free(i2sign);
	free(degreei1);
	free(degreei2);

	(*small)->linesNum = line1;
	(*big)->linesNum = line2;

	set_filename_LineFile(*big, "divided-trainset");
	set_filename_LineFile(*small, "divided-testset");

	printgf("rate: %.3f, big file's linesNum: %ld, small file's linesNum: %ld.", rate, line2, line1);
}

/*********************************************************************/
/**all kinds of similarity********************************************/
/*********************************************************************/
#define SIMFILESTEP 1000000
//if target == 1, then calculate i1(mostly user)'s similarity.
//if target == 2, then calculate i2(mostly item)'s similarity.
struct LineFile *cosine_similarity_Bip(struct Bip *bipi1, struct Bip *bipi2, int target) {
	if (target != 1 && target != 2) isError("target should be 1 or 2");
	int idmax, idmax2;
	int *degree;
	int **edges;
	if (target == 1) {
		idmax = bipi1->maxId;
		idmax2 = bipi2->maxId;
		degree = bipi1->degree;
		edges = bipi1->edges;
	}
	else {
		idmax = bipi2->maxId;
		idmax2 = bipi1->maxId;
		degree = bipi2->degree;
		edges = bipi2->edges;
	}

	int *sign = scalloc((idmax2 + 1),sizeof(int));

	struct LineFile *simfile = create_LineFile(NULL);

	int con = SIMFILESTEP;
	int *i1 = smalloc(con*sizeof(int));
	int *i2 = smalloc(con*sizeof(int));
	double *d1 = smalloc(con*sizeof(double));

	long linesNum = 0;

	int k;
	int Sij;
	double soij;
	int i,j;
	for (i=0; i<idmax; ++i) {
		if (degree[i]) {
			memset(sign, 0, (idmax2 + 1)*sizeof(int));
			for (k=0; k<degree[i]; ++k) {
				sign[edges[i][k]] = 1;
			}
			for (j = i+1; j<idmax + 1; ++j) {
				if (degree[j]) {
					Sij = 0;
					for (k=0; k<degree[j]; ++k) {
						Sij += sign[edges[j][k]];
					}
					if (Sij) {
						soij = (double)Sij/sqrt(degree[i] * degree[j]);
						//fprintf(fp, "%d, %d, %.17f\n", i, j, soij);
						i1[linesNum] = i;
						i2[linesNum] = j;
						d1[linesNum] = soij;
						++linesNum;
						if (linesNum == con) {
							con += SIMFILESTEP;
							i1 = srealloc(i1, con*sizeof(int));
							i2 = srealloc(i2, con*sizeof(int));
							d1 = srealloc(d1, con*sizeof(double));
						}
					}
				}
			}
		}
	}
	free(sign);

	simfile->linesNum = linesNum;
	simfile->i1 = i1;
	simfile->i2 = i2;
	simfile->d1 = d1;
	char simfilename[100];
	sprintf(simfilename, "cosine_similarity_i%d", target);
	set_filename_LineFile(simfile, simfilename);

	printgf("calculate %s done, linesNum: %ld.", simfile->filename, linesNum);
	return simfile;
}

struct LineFile *pearson_similarity_Bip(struct Bip *bipi1, struct Bip *bipi2, int target) {
	if (target != 1 && target != 2) isError("target should be 1 or 2");
	if (bipi1->edgesI == NULL) isError("pearson similarity need edgesI");
	int idmax, idmax2;
	int *degree;
	int **edges;
	int **edgesI;
	if (target == 1) {
		idmax = bipi1->maxId;
		idmax2 = bipi2->maxId;
		degree = bipi1->degree;
		edges = bipi1->edges;
		edgesI = bipi1->edgesI;
	}
	else {
		idmax = bipi2->maxId;
		idmax2 = bipi1->maxId;
		degree = bipi2->degree;
		edges = bipi2->edges;
		edgesI = bipi2->edgesI;
	}

	int *sign = scalloc((idmax2 + 1),sizeof(int));

	struct LineFile *simfile = create_LineFile(NULL);

	int con = SIMFILESTEP;
	int *i1 = smalloc(con*sizeof(int));
	int *i2 = smalloc(con*sizeof(int));
	double *d1 = smalloc(con*sizeof(double));

	long linesNum = 0;

	double soij;
	int i ,j, k;
	for (i=0; i<idmax; ++i) {
		if (degree[i]) {
			memset(sign, 0, (idmax2 + 1)*sizeof(int));
			for (k=0; k<degree[i]; ++k) {
				sign[edges[i][k]] = edgesI[i][k];
			}
			for (j = i+1; j<idmax + 1; ++j) {
				if (degree[j]) {
					//user i and user j.
					int sumx = 0, sumxsp = 0;
					int sumy = 0, sumysp = 0;
					int sumxy = 0;
					int com = 0;
					for (k=0; k<degree[j]; ++k) {
						int scorei = sign[edges[j][k]];
						int scorej = edgesI[j][k];
						if (scorei && scorej) {
							com++;
							sumx += scorei;
							sumy += scorej;
							sumxsp += scorei * scorei;
							sumysp += scorej * scorej;
							sumxy += scorei * scorej;
						}
					}
					if (com) {
						double fenzi = sumxy * com - sumx * sumy;
						double fenmu1 = sumxsp * com - sumx * sumx;
						double fenmu2 = sumysp * com - sumy * sumy;;
						if (fenzi < 1E-17) {
							soij = 0;
							//not need to do anything
						}
						else if (fenmu1 < 1E-17 || fenmu2 < 1E-17) {
							isError("fezi: %.17f, fenmu1: %.17f fenmu2: %.17f", fenzi, fenmu1, fenmu2);
						}
						else {
							double fenmu = sqrt(fenmu1) * sqrt(fenmu2);
							soij = fenzi/fenmu;
							//printf("%d, %d, %.17f\n", i, j, soij);
							i1[linesNum] = i;
							i2[linesNum] = j;
							d1[linesNum] = soij;
							++linesNum;
							if (linesNum == con) {
								con += 1000000;
								i1 = srealloc(i1, con*sizeof(int));
								i2 = srealloc(i2, con*sizeof(int));
								d1 = srealloc(d1, con*sizeof(double));
							}
						}
					}
				}
			}
		}
	}
	free(sign);

	simfile->linesNum = linesNum;
	simfile->i1 = i1;
	simfile->i2 = i2;
	simfile->d1 = d1;
	char simfilename[100];
	sprintf(simfilename, "pearson_similarity_i%d", target);
	set_filename_LineFile(simfile, simfilename);

	printgf("calculate %s done, linesNum: %ld.", simfile->filename, linesNum);
	return simfile;
}

/*********************************************************************/
/***Operation on struct Metrics_Bip***********************************/
/*********************************************************************/
struct Metrics_Bip *create_MetricsBip(void) {
	struct Metrics_Bip *lp = smalloc(sizeof(struct Metrics_Bip));
	lp->R = lp->RL = lp->PL = 0.0;
	lp->HL = lp->IL = lp->NL = lp->COV = 0.0;
	lp->L = 0;
	lp->topL = NULL;

#if CA_METRICS_BIP > 1
	int i;
	for (i = 0; i < CA_METRICS_BIP; ++i) {
		lp->RA[i] = 0;
		lp->RLA[i] = 0;
		lp->PLA[i] = 0;
		lp->HLA[i] = 0;
		lp->ILA[i] = 0;
		lp->NLA[i] = 0;
		lp->COVA[i] = 0;
	}
#endif

	return lp;
}

//free(NULL) is ok.
void clean_MetricsBip(struct Metrics_Bip *lp) {
	lp->R = lp->RL = lp->PL = 0.0;
	lp->HL = lp->IL = lp->NL = lp->COV = 0.0;
	lp->L = 0;
	free(lp->topL);
	lp->topL = NULL;

#if CA_METRICS_BIP > 1
	int i;
	for (i = 0; i < CA_METRICS_BIP; ++i) {
		lp->RA[i] = 0;
		lp->RLA[i] = 0;
		lp->PLA[i] = 0;
		lp->HLA[i] = 0;
		lp->ILA[i] = 0;
		lp->NLA[i] = 0;
		lp->COVA[i] = 0;
	}
#endif
}

void free_MetricsBip(struct Metrics_Bip *lp) {
	free(lp->topL);
	free(lp);
}

/*********************************************************************/
/**the special struct only used in recommend_Bip**********************/
/*********************************************************************/
struct Bip_recommend_param{
	int i1;

	int mass_topk;

	double HNBI_param;

	double RENBI_param;

	double hybrid_param;

	struct iidNet *userSim;
	struct iidNet *itemSim;

	struct Bip *traini1;
	struct Bip *traini2;
	struct Bip *testi1;
	struct Bip *testi2;

	double *i1source;
	double *i2source;
	double *i1sourceA;
	double *i2sourceA;
	int *i1id;
	int *i2id;

	double mass_score;
	int maxscore;

	int L;
	int K;
	double *psimM;
};

/*********************************************************************/
/**metrics calculation functions**************************************/
/*********************************************************************/
//following is for recommendation.
//R is rankscore.
//PL is precision
//Warning: about unselected_list_length, I use bipii->idNum, not bipii->maxId. 
//	but I believe in linyuan's paper, she use the bipii->maxId. 
//	I think bipii->idNum make more sence.
static void metrics_R_RL_PL_Bip(int i1, int *i1degree, int i2idNum, struct Bip *testi1, int L, int *rank, int *user_gender, int *user_age, double *R, double *RL, double *PL) {
	if (i1<testi1->maxId + 1 &&  testi1->degree[i1]) {
		int unselected_list_length = i2idNum - i1degree[i1];
		int rank_i1_j = 0;
		int DiL = 0;
		int j, id;
		for (j=0; j<testi1->degree[i1]; ++j) {
			id = testi1->edges[i1][j];
			rank_i1_j += rank[id];
			if (rank[id] <= L) {
				++DiL;
			}
		}

		R[0] += (double)rank_i1_j/(double)unselected_list_length;
		RL[0] += (double)DiL/(double)(testi1->degree[i1]);
		PL[0] += (double)DiL/(double)L;

		R[user_gender[i1]] += (double)rank_i1_j/(double)unselected_list_length;
		RL[user_gender[i1]] += (double)DiL/(double)(testi1->degree[i1]);
		PL[user_gender[i1]] += (double)DiL/(double)L;

		R[user_age[i1]] += (double)rank_i1_j/(double)unselected_list_length;
		RL[user_age[i1]] += (double)DiL/(double)(testi1->degree[i1]);
		PL[user_age[i1]] += (double)DiL/(double)L;
	}
}
//IL is intrasimilarity
static void metrics_IL_Bip(int i1maxId, int *i1degree, int i1idNum, int i2maxId, int L, int *Hij, struct iidNet *sim, int *user_gender, int *user_age, double *IL) {
	if (!sim) return;
	double *sign = calloc((i2maxId + 1), sizeof(double));
	assert(sign != NULL);
	int i, j;
	int k;
	for (i=0; i<i1maxId + 1; ++i) {
		if (i1degree[i]) {
			int *tmp = Hij + i*L;
			for (j=0; j<L; ++j) {
				int id = tmp[j];
				memset(sign, 0, (i2maxId + 1)*sizeof(double));
				for (k=0; k<sim->count[id]; ++k) {
					sign[sim->edges[id][k]] = sim->d[id][k];
				}
				for (k=j+1; k<L; ++k) {
					id = tmp[k];
					IL[0] += sign[id];
					IL[user_gender[i]] += sign[id];
					IL[user_age[i]] += sign[id];
				}
			}
		}
	}
	free(sign);
	for (i = 0; i < CA_METRICS_BIP; ++i) {
		IL[i] /= L*(L-1)/2;
	}
}

//HL is hamming distance.
static void metrics_HL_COV_Bip(int i1maxId, int *i1degree, int i2maxId, int L, int *Hij, int *user_gender, int *user_age, double *HL, double *COV) {
	int *sign = calloc((i2maxId + 1), sizeof(int));
	int *sign1 = calloc((i2maxId + 1), sizeof(int));
	int *sign2 = calloc((i2maxId + 1)*CA_METRICS_BIP, sizeof(int));
	assert(sign != NULL);
	int i, j;
	int k;
	int cou[CA_METRICS_BIP];
	for (i = 0; i < CA_METRICS_BIP; ++i) {
		cou[i] = 0;
	}
	int Cij = 0;
	for (i=0; i<i1maxId + 1; ++i) {
		if (i1degree[i]) {
			memset(sign, 0, (i2maxId + 1)*sizeof(int));
			for (k=i*L; k<i*L+L; ++k) {
				sign[Hij[k]] = 1;
				sign1[Hij[k]] = 1;
				sign2[user_gender[i]*(i2maxId+1) + Hij[k]] = 1;
				sign2[user_age[i]*(i2maxId+1) + Hij[k]] = 1;
			}
			for (j=0; j<i1maxId + 1; ++j) {
				if ( i != j && i1degree[j]) {
					Cij = 0;
					for (k=j*L; k<j*L+L; ++k) {
						if (sign[Hij[k]]) {
							++Cij;
						}
					}
					HL[0] += 1 - ((double)Cij)/(double)L;
					cou[0] ++;
					HL[user_gender[i]] += 1 - ((double)Cij)/(double)L;
					cou[user_gender[i]]++;
					HL[user_age[i]] += 1 - ((double)Cij)/(double)L;
					cou[user_age[i]]++;
				}
			}
		}
	}
	int cov[CA_METRICS_BIP];
	for (j = 0; j < CA_METRICS_BIP; ++j) {
		cov[j] = 0;	
	}
	for (i = 0; i < i2maxId + 1; ++i) {
		if (sign1[i]) {
			cov[0]++;
		}	
	}
	COV[0] = (double)(cov[0])/(i2maxId + 1);
	HL[0] /= cou[0];
	for (j = 1; j < CA_METRICS_BIP; ++j) {
		cov[j] = 0;
		for (i = j*(i2maxId + 1); i < (j+1) * (i2maxId + 1); ++i) {
			if (sign2[i]) {
				cov[j]++;
			}	
		}
		COV[j] = (double)(cov[j])/(i2maxId+1);
		HL[j] /= cou[j];
	}
	free(sign);
	free(sign1);
	free(sign2);
}

//NL is popularity.
static void metrics_NL_Bip(int i1maxId, int *i1degree, int i1idNum, int *i2degree, int L, int *Hij, int *user_gender, int *user_age, double *NL) {
	int i,j;
	for (i=0; i<i1maxId + 1; ++i) {
		if (i1degree[i]) {
			int *tmp = Hij + i*L;
			for (j=0; j<L; ++j) {
				NL[0] += i2degree[tmp[j]];
				NL[user_gender[i]] += i2degree[tmp[j]];
				NL[user_age[i]] += i2degree[tmp[j]];
			}
		}
	}
	for (i = 0; i < CA_METRICS_BIP; ++i) {
		NL[i] /= L;
	}
}

/*********************************************************************/
/**after recommend_core, do this common function.*********************/
/*********************************************************************/
static inline void Bip_core_common_part(struct Bip_recommend_param *args, int *i2id, int *rank, int *topL_i1L, int L) {
	
	int uid = args->i1;
	int uiddegree = args->traini1->degree[uid];
	int *uidId = args->traini1->edges[uid];
	int i2maxId = args->traini2->maxId;
	int *i2degree = args->traini2->degree;
	double *i2source = args->i2source;

	int i;
	//set selected item's source to -1
	for (i=0; i<uiddegree; ++i) {
		i2source[uidId[i]] = -1;
		//i2source[uidId[i]] = 0;
	}
	//set i2id and rank.
	int j=0;
	double dtmp;
	for (i=0; i<i2maxId + 1; ++i) {
		i2id[i] = i;
		//set unexisted item's source to -2.
		if (!i2degree[i]) {
			i2source[i] = -2;
		}

		if (i2source[i] > 0 ) {
			//valid i2source and i2id will be in the front i positions of the array.
			dtmp = i2source[j];
			i2source[j] = i2source[i];
			i2source[i] = dtmp;

			i2id[i] = i2id[j];
			i2id[j] = i;

			++j;
		}
	}
	//to this step, i2source contains four parts: 
	//1, nomral i2source[x] , which (0, 1].
	//2, i2source[x] = 0, which x isn't selected by anyone has similarity.
	//3, i2source[x] = -1, which x has been selected by i1.
	//4, i2source[x] = -2, which x is the hole, x isn't selected by anyone.
	//
	//after qsort_di_desc, the id of the item with most source will be in i2id[0];
	//qsort_di_desc(i2source, 0, i2maxId, i2id);
	//printf("%d\t%d\n", j, i2maxId);
	qsort_di_desc(i2source, 0, j-1, i2id);
	//copy the top L itemid into topL.
	memcpy(topL_i1L, i2id, L*sizeof(int));
	//get rank;
	for (i=0; i<i2maxId + 1; ++i) {
		rank[i2id[i]] = i+1;
	}
}


/*********************************************************************/
/***all kinds of recommend_core functions.****************************/
/*********************************************************************/
//three-step random walk of Probs
//return i2source
static void mass_recommend_Bip(struct Bip_recommend_param *args) {

	int i1 = args->i1;
	double * i1source = args->i1source;
	double *i2source = args->i2source;
	int **i1ids = args->traini1->edges;
	int **i2ids = args->traini2->edges; 
	int i1maxId = args->traini1->maxId;
	int i2maxId = args->traini2->maxId;
	int *i1degree = args->traini1->degree;
	int *i2degree = args->traini2->degree;

	int i, j, neigh;
	int degree;
	double source;
	//one 
	memset(i2source, 0, (i2maxId+1)*sizeof(double));
	for (j=0; j<i1degree[i1]; ++j) {
		neigh = i1ids[i1][j];
		i2source[neigh] = 1.0;
	}
	//two
	memset(i1source, 0, (i1maxId+1)*sizeof(double));
	for (i=0; i<i2maxId + 1; ++i) {
		if (i2source[i]) {
			degree = i2degree[i];
			source = i2source[i]/(double)degree;
			for (j=0; j<degree; ++j) {
				neigh = i2ids[i][j];
				i1source[neigh] += source;
			}
		}
	}
	
	//three
	memset(i2source, 0, (i2maxId+1)*sizeof(double));
	for (i=0; i<i1maxId + 1; ++i) {
		if (i1source[i]) {
			degree = i1degree[i];
			source = (double)i1source[i]/(double)degree;
			for (j=0; j<degree; ++j) {
				neigh = i1ids[i][j];
				i2source[neigh] += source;
			}
		}
	}
}

//three-step random walk of heats
//return i2source
static void heats_recommend_Bip(struct Bip_recommend_param *args) {
	int i1 = args->i1;
	double * i1source = args->i1source;
	double *i2source = args->i2source;
	int **i1ids = args->traini1->edges;
	int **i2ids = args->traini2->edges; 
	int i1maxId = args->traini1->maxId;
	int i2maxId = args->traini2->maxId;
	int *i1degree = args->traini1->degree;
	int *i2degree = args->traini2->degree;

	int neigh, i;
	int j;
	memset(i2source, 0, (i2maxId+1)*sizeof(double));
	for (j=0; j<i1degree[i1]; ++j) {
		neigh = i1ids[i1][j];
		i2source[neigh] = 1;
	}
	memset(i1source, 0, (i1maxId+1)*sizeof(double));
	for (i=0; i<i1maxId + 1; ++i) {
		if (i1degree[i]) {
			for (j=0; j<i1degree[i]; ++j) {
				neigh = i1ids[i][j];
				i1source[i] += i2source[neigh];
			}
			i1source[i] /= i1degree[i];
		}
	}
	memset(i2source, 0, (i2maxId+1)*sizeof(double));
	for (i=0; i<i2maxId + 1; ++i) {
		if (i2degree[i]) {
			for (j=0; j<i2degree[i]; ++j) {
				neigh = i2ids[i][j];
				i2source[i] += i1source[neigh];
			}
			i2source[i] /= i2degree[i];
		}
	}
}

//three-step random walk of HNBI
//return i2source
static void HNBI_recommend_Bip(struct Bip_recommend_param *args) {
	int i1 = args->i1;
	double HNBI_param = args->HNBI_param;

	double * i1source = args->i1source;
	double *i2source = args->i2source;
	int **i1ids = args->traini1->edges;
	int **i2ids = args->traini2->edges; 
	int i1maxId = args->traini1->maxId;
	int i2maxId = args->traini2->maxId;
	int *i1degree = args->traini1->degree;
	int *i2degree = args->traini2->degree;

	int i, j, neigh;
	int degree;
	double source;
	memset(i2source, 0, (i2maxId+1)*sizeof(double));
	for (j=0; j<i1degree[i1]; ++j) {
		neigh = i1ids[i1][j];
		i2source[neigh] = 1.0*pow(i2degree[neigh], HNBI_param);
	}
	memset(i1source, 0, (i1maxId+1)*sizeof(double));
	for (i=0; i<i2maxId + 1; ++i) {
		if (i2source[i]) {
			degree = i2degree[i];
			source = i2source[i]/(double)degree;
			for (j=0; j<degree; ++j) {
				neigh = i2ids[i][j];
				i1source[neigh] += source;
			}
		}
	}
	memset(i2source, 0, (i2maxId+1)*sizeof(double));
	for (i=0; i<i1maxId + 1; ++i) {
		if (i1source[i]) {
			degree = i1degree[i];
			source = (double)i1source[i]/(double)degree;
			for (j=0; j<degree; ++j) {
				neigh = i1ids[i][j];
				i2source[neigh] += source;
			}
		}
	}
}

//five-step random walk of RENBI
//return i2source
static void RENBI_recommend_Bip(struct Bip_recommend_param *args) {
	int i1 = args->i1;
	double RENBI_param = args->RENBI_param;
	double *i2sourceA = args->i2sourceA;

	double * i1source = args->i1source;
	double *i2source = args->i2source;
	int **i1ids = args->traini1->edges;
	int **i2ids = args->traini2->edges; 
	int i1maxId = args->traini1->maxId;
	int i2maxId = args->traini2->maxId;
	int *i1degree = args->traini1->degree;
	int *i2degree = args->traini2->degree;

	int i, j, neigh;
	int degree;
	double source;
	memset(i2source, 0, (i2maxId+1)*sizeof(double));
	//one
	for (j=0; j<i1degree[i1]; ++j) {
		neigh = i1ids[i1][j];
		i2source[neigh] = 1.0;
	}
	//two
	memset(i1source, 0, (i1maxId+1)*sizeof(double));
	for (i=0; i<i2maxId + 1; ++i) {
		if (i2source[i]) {
			degree = i2degree[i];
			source = i2source[i]/(double)degree;
			for (j=0; j<degree; ++j) {
				neigh = i2ids[i][j];
				i1source[neigh] += source;
			}
		}
	}
	//three
	memset(i2source, 0, (i2maxId+1)*sizeof(double));
	for (i=0; i<i1maxId + 1; ++i) {
		if (i1source[i]) {
			degree = i1degree[i];
			source = (double)i1source[i]/(double)degree;
			for (j=0; j<degree; ++j) {
				neigh = i1ids[i][j];
				i2source[neigh] += source;
			}
		}
	}
	//save three steps result	
	memcpy(i2sourceA, i2source, (i2maxId + 1)*sizeof(double));
	//four
	memset(i1source, 0, (i1maxId+1)*sizeof(double));
	for (i=0; i<i2maxId + 1; ++i) {
		if (i2source[i]) {
			degree = i2degree[i];
			source = i2source[i]/(double)degree;
			for (j=0; j<degree; ++j) {
				neigh = i2ids[i][j];
				i1source[neigh] += source;
			}
		}
	}
	//five
	memset(i2source, 0, (i2maxId+1)*sizeof(double));
	for (i=0; i<i1maxId + 1; ++i) {
		if (i1source[i]) {
			degree = i1degree[i];
			source = (double)i1source[i]/(double)degree;
			for (j=0; j<degree; ++j) {
				neigh = i1ids[i][j];
				i2source[neigh] += source;
			}
		}
	}
	//now, i2source save w*w result.
	//add: w+RENBI_param*w*w.
	for (i=0; i<i2maxId + 1; ++i) {
		i2sourceA[i] += RENBI_param*i2source[i];
	}

	memcpy(i2source, i2sourceA, (i2maxId + 1)*sizeof(double));
}

//three-step random walk of hybrid
//return i2source
static void hybrid_recommend_Bip(struct Bip_recommend_param *args) {
	int i1 = args->i1;
	double hybrid_param = args->hybrid_param;

	double * i1source = args->i1source;
	double *i2source = args->i2source;
	int **i1ids = args->traini1->edges;
	int **i2ids = args->traini2->edges; 
	int i1maxId = args->traini1->maxId;
	int i2maxId = args->traini2->maxId;
	int *i1degree = args->traini1->degree;
	int *i2degree = args->traini2->degree;

	int neigh, i;
	//double source;
	int j;
	//one
	memset(i2source, 0, (i2maxId+1)*sizeof(double));
	for (j=0; j<i1degree[i1]; ++j) {
		neigh = i1ids[i1][j];
		i2source[neigh] = 1;
	}
	//two
	memset(i1source, 0, (i1maxId+1)*sizeof(double));
	for (i=0; i<i2maxId + 1; ++i) {
		if (i2source[i]) {
			double powl = pow(i2degree[i], hybrid_param);
			for (j=0; j<i2degree[i]; ++j) {
				neigh = i2ids[i][j];
				i1source[neigh] += i2source[i]/powl;
			}
		}
	}
	//three
	memset(i2source, 0, (i2maxId+1)*sizeof(double));
	for (i=0; i<i2maxId + 1; ++i) {
		if (i2degree[i]) {
			double powl = pow(i2degree[i], 1-hybrid_param);
			for (j=0; j<i2degree[i]; ++j) {
				neigh = i2ids[i][j];
				i2source[i] += i1source[neigh]/i1degree[neigh];
			}
			i2source[i] /= powl;
		}
	}
}

//CF
//return i2source
static void UCF_recommend_Bip(struct Bip_recommend_param *args) {

	//variables get from args.
	int i1 = args->i1;
	double * i1source = args->i1source;
	double *i2source = args->i2source;
	int **i1ids = args->traini1->edges;
	int **i2ids = args->traini2->edges;
	int i1maxId = args->traini1->maxId;
	int i2maxId = args->traini2->maxId;
	int *i1degree = args->traini1->degree;
	int *i2degree = args->traini2->degree;
	int **i2score = args->traini2->edgesI;
	int *i1id = args->i1id;
	int *i2id = args->i2id;
	double *psimM = args->psimM;

	printf("%d\t", i1); fflush(stdout);

	int i, j, neigh;

	for (j = 0; j < i2maxId + 1; ++j) {
		i2id[j] = 1;
		i2source[j] = 0;
	}
	for (j = 0; j < i1degree[i1]; ++j) {
		i2id[i1ids[i1][j]] = 0;
	}
	
	int k=0;
	for (j=0; j<i2maxId+1; ++j) {
		//j is item
		if (i2id[j] == 0) continue;
		//now j is the item which user i1 hasn't choosen.
		k=0;
		for (i = 0; i < i2degree[j]; ++i) {
			//neigh is user
			neigh = i2ids[j][i];
			//neigh give score to j.
			int score = i2score[j][i];
			i1source[k] = psimM[i1 * (i1maxId + 1) + neigh];
			i1id[k] = score;
			k++;
		}
		if (k) {
			//printf("%dxx%d\t", k, args->K);
			qsort_di_desc(i1source, 0, k-1, i1id);
			for (i = 0; i < k && i < args->K; ++i) {
				i2source[j] += i1source[i] * i1id[i];	
			}
			//for (i = 0; i < k && i < args->K; ++i) {
			//	double most = -1;
			//	int index = -1;
			//	int s;
			//	for (s = 0; s < k; ++s) {
			//		if (most < i1source[s]) {
			//			most = i1source[s];
			//			index = s;
			//		}
			//	}
			//	i2source[j] += i1source[index] * i1id[index];	
			//	i1source[index] = -2;
			//}
		}
	}
	print_time();
}

static void ICF_recommend_Bip(struct Bip_recommend_param *args) {

	//variables get from args.
	int i1 = args->i1;
	double *i2source = args->i2source;
	int **i1ids = args->traini1->edges;
	int i2maxId = args->traini2->maxId;
	int *i1degree = args->traini1->degree;
	int *i2id = args->i2id;
	double *psimM = args->psimM;

	printf("%d\t", i1); fflush(stdout);

	int i, j, neigh;

	for (j = 0; j < i2maxId + 1; ++j) {
		i2id[j] = 1;
		i2source[j] = 0;
	}
	for (j = 0; j < i1degree[i1]; ++j) {
		i2id[i1ids[i1][j]] = 0;
	}
	
	for (j=0; j<i2maxId+1; ++j) {
		//j is item
		if (i2id[j] == 0) continue;
		//now j is the item which user i1 hasn't choosen.
		for (i = 0; i < i1degree[i1]; ++i) {
			//neigh is one selected item.
			neigh = i1ids[i1][i];
			i2source[j] += psimM[j * (i2maxId + 1) + neigh];
		}
	}
	print_time();
}

//mass topk
//return i2source
static void mass_topk_recommend_Bip(struct Bip_recommend_param *args) {

	int i1 = args->i1;
    struct iidNet *userSim = args->userSim;
	int topk = args->mass_topk;

	double * i1source = args->i1source;
	double *i2source = args->i2source;

	int **i1ids = args->traini1->edges;
	int **i2ids = args->traini2->edges; 
	int i1maxId = args->traini1->maxId;
	int i2maxId = args->traini2->maxId;
	int *i1degree = args->traini1->degree;
	int *i2degree = args->traini2->degree;

	int i, j, neigh;
	int degree;
	double source;
	//one 
	memset(i2source, 0, (i2maxId+1)*sizeof(double));
	for (j=0; j<i1degree[i1]; ++j) {
		neigh = i1ids[i1][j];
		i2source[neigh] = 1.0;
	}
	//two
	memset(i1source, 0, (i1maxId+1)*sizeof(double));
	for (i=0; i<i2maxId + 1; ++i) {
		if (i2source[i]) {
			degree = i2degree[i];
			source = i2source[i]/(double)degree;
			for (j=0; j<degree; ++j) {
				neigh = i2ids[i][j];
				i1source[neigh] += source;
			}
		}
	}
	//three
	memset(i2source, 0, (i2maxId+1)*sizeof(double));
	int k;
	for (k=0; k<userSim->count[i1]; ++k) {
		i = userSim->edges[i1][k];
		if (k < topk) {
			degree = i1degree[i];
			source = (double)i1source[i]/(double)degree;
			for (j=0; j<degree; ++j) {
				neigh = i1ids[i][j];
				i2source[neigh] += source;
			}
		}
	}
}

//mass hs
//return i2source
static void mass_hs_recommend_Bip(struct Bip_recommend_param *args) {

	int i1 = args->i1;
    struct iidNet *userSim = args->userSim;
	int topk = args->mass_topk;

	double * i1source = args->i1source;
	double *i2source = args->i2source;

	int **i1ids = args->traini1->edges;
	int **i2ids = args->traini2->edges; 
	int i1maxId = args->traini1->maxId;
	int i2maxId = args->traini2->maxId;
	int *i1degree = args->traini1->degree;
	int *i2degree = args->traini2->degree;

	int i, j, neigh;
	int degree;
	double source;
	//one 
	memset(i2source, 0, (i2maxId+1)*sizeof(double));
	for (j=0; j<i1degree[i1]; ++j) {
		neigh = i1ids[i1][j];
		i2source[neigh] = 1.0;
	}
	//two
	memset(i1source, 0, (i1maxId+1)*sizeof(double));
	for (i=0; i<i2maxId + 1; ++i) {
		if (i2source[i]) {
			degree = i2degree[i];
			source = i2source[i]/(double)degree;
			for (j=0; j<degree; ++j) {
				neigh = i2ids[i][j];
				i1source[neigh] += source;
			}
		}
	}
	//three
	memset(i2source, 0, (i2maxId+1)*sizeof(double));
	int k;
	for (k=0; k<userSim->count[i1]; ++k) {
		i = userSim->edges[i1][k];
		if (k < topk) {
			degree = i1degree[i];
			source = (double)i1source[i]/((double)degree*k);
			for (j=0; j<degree; ++j) {
				neigh = i1ids[i][j];
				i2source[neigh] += source;
			}
		}
		else {
			break;
		}
	}
}

//mass score
//return i2source
static void mass_score_recommend_Bip(struct Bip_recommend_param *args) {
	int i1 = args->i1;
	double *i1source = args->i1source;
	double *i2source = args->i2source;
	int **i1ids = args->traini1->edges;
	int **i2ids = args->traini2->edges; 
	int i1maxId = args->traini1->maxId;
	int i2maxId = args->traini2->maxId;
	int *i1degree = args->traini1->degree;
	int *i2degree = args->traini2->degree;

	int maxscore = args->maxscore;
	double theta = args->mass_score;

	double *i1sourceA = args->i1sourceA;
	double *i2sourceA = args->i2sourceA;
	
	int **i1score = args->traini1->edgesI;
	int **i2score = args->traini2->edgesI;
	if (i1score == NULL || i2source == NULL) isError("mass_score_recommend_Bip");

	int *i2id = args->i2id;

	int i, j, neigh;
	int degree;
	double source;
	double totalsource = 0;
	//one 
	memset(i2id, 0, (i2maxId +1)*sizeof(int));
	memset(i2source, 0, (i2maxId+1)*sizeof(double));
	for (j=0; j<i1degree[i1]; ++j) {
		neigh = i1ids[i1][j];
		i2id[neigh] = i1score[i1][j];
		i2source[neigh] = pow(i1score[i1][j], theta);
		totalsource += i2source[neigh];	
	}
	for (j=0; j<i1degree[i1]; ++j) {
		i2source[neigh] = i2source[neigh]*i1degree[i1]/totalsource;
	}
	//two
	memset(i1source, 0, (i1maxId+1)*sizeof(double));
	for (i=0; i<i2maxId + 1; ++i) {
		if (i2source[i]) {
			degree = i2degree[i];
			source = i2source[i];
			totalsource = 0;
			for (j=0; j<degree; ++j) {
				neigh = i2ids[i][j];
				//i1sourceA[neigh] = pow(maxscore - fabs(0-i2id[i]), theta); 
				i1sourceA[neigh] = pow(maxscore - fabs(i2score[i][j]-i2id[i]), theta); 
				totalsource += i1sourceA[neigh];
			}
			for (j=0; j<degree; ++j) {
				neigh = i2ids[i][j];
				i1source[neigh] += source*i1sourceA[neigh]/totalsource;
			}
		}
	}
	//printf("xxxo\n");fflush(stdout);
	
	//three
	memset(i2source, 0, (i2maxId+1)*sizeof(double));
	for (i=0; i<i1maxId + 1; ++i) {
		if (i1source[i]) {
			totalsource = 0;
			degree = i1degree[i];
			source = i1source[i];
			for (j=0; j<degree; ++j) {
				neigh = i1ids[i][j];
				i2sourceA[neigh] = pow((double)i1score[i][j]/(double)i2degree[neigh], theta);
				totalsource += i2sourceA[neigh];
			}
			for (j=0; j<degree; ++j) {
				neigh = i1ids[i][j];
				i2source[neigh] += source*i2sourceA[neigh]/totalsource;
			}
		}
	}
}

//mass score 3 step
//return i2source
static void mass_scoret3step_recommend_Bip(struct Bip_recommend_param *args) {
	int i1 = args->i1;
	double * i1source = args->i1source;
	double *i2source = args->i2source;
	int **i1ids = args->traini1->edges;
	int **i2ids = args->traini2->edges; 
	int i1maxId = args->traini1->maxId;
	int i2maxId = args->traini2->maxId;
	int *i1degree = args->traini1->degree;
	int *i2degree = args->traini2->degree;

	double theta = args->mass_score;

	double *i2sourceA = args->i2sourceA;
	
	int **i1score = args->traini1->edgesI;
	if (i1score == NULL) isError("mass_scoret3step_recommend_Bip");


	int i, j, neigh;
	int degree;
	double source;
	double totalsource = 0;
	//one 
	memset(i2source, 0, (i2maxId+1)*sizeof(double));
	for (j=0; j<i1degree[i1]; ++j) {
		neigh = i1ids[i1][j];
		i2source[neigh] = 1.0;
	}
	//two
	memset(i1source, 0, (i1maxId+1)*sizeof(double));
	for (i=0; i<i2maxId + 1; ++i) {
		if (i2source[i]) {
			degree = i2degree[i];
			source = i2source[i]/(double)degree;
			for (j=0; j<degree; ++j) {
				neigh = i2ids[i][j];
				i1source[neigh] += source;
			}
		}
	}
	//three
	memset(i2source, 0, (i2maxId+1)*sizeof(double));
	for (i=0; i<i1maxId + 1; ++i) {
		if (i1source[i]) {
			totalsource = 0;
			degree = i1degree[i];
			source = i1source[i];
			for (j=0; j<degree; ++j) {
				neigh = i1ids[i][j];
				i2sourceA[neigh] = pow((double)i1score[i][j]/(double)i2degree[neigh], theta);
				totalsource += i2sourceA[neigh];
			}
			for (j=0; j<degree; ++j) {
				neigh = i1ids[i][j];
				i2source[neigh] += source*i2sourceA[neigh]/totalsource;
			}
		}
	}
}

//mass degree
//return i2source
static void mass_degree_recommend_Bip(struct Bip_recommend_param *args) {
	int i1 = args->i1;
	double * i1source = args->i1source;
	double *i2source = args->i2source;
	int **i1ids = args->traini1->edges;
	int **i2ids = args->traini2->edges; 
	int i1maxId = args->traini1->maxId;
	int i2maxId = args->traini2->maxId;
	int *i1degree = args->traini1->degree;
	int *i2degree = args->traini2->degree;

	double theta = args->mass_score;

	double *i2sourceA = args->i2sourceA;
	
	int **score = args->traini1->edgesI;
	if (score == NULL) isError("mass_degree_recommend_Bip");


	int i, j, neigh;
	int degree;
	double source;
	double totalsource = 0;
	//one 
	memset(i2source, 0, (i2maxId+1)*sizeof(double));
	for (j=0; j<i1degree[i1]; ++j) {
		neigh = i1ids[i1][j];
		i2source[neigh] = 1.0;
	}
	//two
	memset(i1source, 0, (i1maxId+1)*sizeof(double));
	for (i=0; i<i2maxId + 1; ++i) {
		if (i2source[i]) {
			degree = i2degree[i];
			source = i2source[i]/(double)degree;
			for (j=0; j<degree; ++j) {
				neigh = i2ids[i][j];
				i1source[neigh] += source;
			}
		}
	}
	//three
	memset(i2source, 0, (i2maxId+1)*sizeof(double));
	for (i=0; i<i1maxId + 1; ++i) {
		if (i1source[i]) {
			totalsource = 0;
			degree = i1degree[i];
			source = i1source[i];
			for (j=0; j<degree; ++j) {
				neigh = i1ids[i][j];
				i2sourceA[neigh] = pow(1.0/(double)i2degree[neigh], theta);
				totalsource += i2sourceA[neigh];
			}
			for (j=0; j<degree; ++j) {
				neigh = i1ids[i][j];
				i2source[neigh] += source*i2sourceA[neigh]/totalsource;
			}
		}
	}
}

/** 
 * core function of recommendation.
 * type :
 *   -- mass
 *   -- heats
 *   -- HNBI
 *   -- RENBI
 *   -- hybrid
 *   -- CF
 *   -- mass topk 
 *   -- mass hs
 *   -- mass score
 *   -- mass score 3 step
 *   -- mass degree
 */
static struct Metrics_Bip *recommend_Bip(void (*recommend_core)(struct Bip_recommend_param *), struct Bip_recommend_param *args) {
	//param from args.
	int i1maxId      = args->traini1->maxId;
	int i2maxId      = args->traini2->maxId;
	int i1idNum      = args->traini1->idNum;
	int *i1degree    = args->traini1->degree;
	int *i2degree    = args->traini2->degree;
	int *user_gender = args->traini1->attI1;
	int *user_age = args->traini1->attI2;
	struct iidNet *itemSim = args->itemSim;
	int L = args->L;

	//param from here, will be given to args.
	double *i1source = smalloc((i1maxId + 1)*sizeof(double));
	double *i2source = smalloc((i2maxId + 1)*sizeof(double));
	double *i1sourceA = smalloc((i1maxId + 1)*sizeof(double));
	double *i2sourceA = smalloc((i2maxId + 1)*sizeof(double));
	int *i1id = smalloc((i1maxId + 1)*sizeof(int));
	int *i2id = smalloc((i2maxId + 1)*sizeof(int));
	args->i1source = i1source;
	args->i2source = i2source;
	args->i1sourceA = i1sourceA;
	args->i2sourceA = i2sourceA;
	args->i1id = i1id;
	args->i2id = i2id;

	//the result
	struct Metrics_Bip *retn = create_MetricsBip();
	retn->L = L;
	int *topL = scalloc(L*(i1maxId + 1), sizeof(int));
	retn->topL = topL;
	double *R, *RL, *PL, *HL, *IL, *NL, *COV;
	R=retn->RA;
	RL=retn->RLA;
	PL=retn->PLA;
	HL=retn->HLA;
	IL=retn->ILA;
	NL=retn->NLA;
	COV=retn->COVA;

	//only use in this function.
	int *rank = smalloc((i2maxId + 1)*sizeof(int));

	int i;
	for (i = 0; i<i1maxId + 1; ++i) {
		if (i1degree[i]) {//each valid user in trainset.
			args->i1 = i;
			//get i2source
			recommend_core(args);
			//use i2source, get i2id & rank & topL
			Bip_core_common_part(args, i2id, rank, topL + i*L, L);
			metrics_R_RL_PL_Bip(i, i1degree, i2maxId/*i2idNum*/, args->testi1, L, rank, user_gender, user_age, R, RL, PL);
		}
	}

	metrics_HL_COV_Bip(i1maxId, i1degree, i2maxId, L, topL, user_gender, user_age, HL, COV);
	metrics_IL_Bip(i1maxId, i1degree, i1idNum, i2maxId, L, topL, itemSim, user_gender, user_age, IL);
	metrics_NL_Bip(i1maxId, i1degree, i1idNum, i2degree, L, topL, user_gender, user_age, NL);

	for (i = 0; i < CA_METRICS_BIP; ++i) {
		retn->RA[i] = R[i] / args->testi1->att2[i];
		retn->RLA[i] = RL[i] / args->testi1->att1[i];
		retn->PLA[i] = PL[i] / args->testi1->att1[i];
		retn->HLA[i] = HL[i];
		retn->ILA[i] = IL[i] / args->testi1->att1[i];
		retn->NLA[i] = NL[i] / args->testi1->att1[i];
		retn->COVA[i] = COV[i];
	}

	free(i1source); free(i2source);
	free(i1sourceA); free(i2sourceA);
	free(i1id); free(i2id);
	free(rank);
	return retn;
}


struct Metrics_Bip *mass_Bip(struct Bip *traini1, struct Bip *traini2, struct Bip *testi1, struct Bip *testi2, struct iidNet *itemSim, int L) {
	struct Bip_recommend_param param;
	param.itemSim = itemSim;

	param.traini1 = traini1;
	param.traini2 = traini2;
	param.testi1 = testi1;
	param.L = L;

	return recommend_Bip(mass_recommend_Bip, &param);
}

struct Metrics_Bip *heats_Bip(struct Bip *traini1, struct Bip *traini2, struct Bip *testi1, struct Bip *testi2, struct iidNet *itemSim, int L) {
	struct Bip_recommend_param param;
	param.itemSim = itemSim;

	param.traini1 = traini1;
	param.traini2 = traini2;
	param.testi1 = testi1;

	param.L = L;

	return recommend_Bip(heats_recommend_Bip, &param);
}

struct Metrics_Bip *HNBI_Bip(struct Bip *traini1, struct Bip *traini2, struct Bip *testi1, struct Bip *testi2, struct iidNet *itemSim, double HNBI_param, int L) {
	struct Bip_recommend_param param;
	param.itemSim = itemSim;
	param.HNBI_param = HNBI_param;

	param.traini1 = traini1;
	param.traini2 = traini2;
	param.testi1 = testi1;

	param.L = L;
	return recommend_Bip(HNBI_recommend_Bip, &param);
}

struct Metrics_Bip *RENBI_Bip(struct Bip *traini1, struct Bip *traini2, struct Bip *testi1, struct Bip *testi2, struct iidNet *itemSim, double RENBI_param, int L) {
	struct Bip_recommend_param param;
	param.itemSim = itemSim;
	param.RENBI_param = RENBI_param;

	param.traini1 = traini1;
	param.traini2 = traini2;
	param.testi1 = testi1;

	param.L = L;
	return recommend_Bip(RENBI_recommend_Bip, &param);
}

struct Metrics_Bip *hybrid_Bip(struct Bip *traini1, struct Bip *traini2, struct Bip *testi1, struct Bip *testi2, struct iidNet *itemSim, double hybrid_param, int L) {
	struct Bip_recommend_param param;
	param.itemSim = itemSim;
	param.hybrid_param = hybrid_param;

	param.traini1 = traini1;
	param.traini2 = traini2;
	param.testi1 = testi1;


	param.L = L;
	return recommend_Bip(hybrid_recommend_Bip, &param);
}


int *mass_getBK_Bip(struct Bip *traini1, struct Bip *traini2, struct Bip *testi1, struct Bip *testi2, struct iidNet *userSim, double rate) {
	printf("calculate BK begin =>> ");
	print_time();

	double *i1source = malloc((traini1->maxId + 1)*sizeof(double));
	double *i2source = malloc((traini2->maxId + 1)*sizeof(double));
	int *rank = malloc((traini2->maxId + 1)*sizeof(int));
	int *i2id =  malloc((traini2->maxId + 1)*sizeof(int));

	double R, PL;

	struct Bip_recommend_param args;
	args.traini1 = traini1;
	args.traini2 = traini2;
	args.testi1 = testi1;
	args.i1source = i1source;
	args.i2source = i2source;
	args.userSim = userSim;

	int i;
    int j;
	int L = 50;
	int *topL = calloc(L*(traini1->maxId + 1), sizeof(int));
	double bR, bPL;
	int bi;
	int *best = malloc((traini1->maxId + 1)*sizeof(int));
	double total_bR = 0, total_R = 0;
	double total_bPL = 0, total_PL = 0;

	for (i = 0; i<traini1->maxId + 1; ++i) { //each user
		if (i<testi1->maxId + 1 && testi1->degree[i]) {
			//just to make sure bestR is enough big.
			bR = 10000; //big enough.
			bPL = -1;
			bi = -1;
			//to make realR2 is same with mass.
			for (j=1; j<= userSim->count[i] || (j ==1 && userSim->count[i] == 0); ++j) {

				//probs_knn_Bip_core(i, &args, userSim, j);
				args.i1 = i;
				args.mass_topk = j;
				mass_topk_recommend_Bip(&args);

				
				Bip_core_common_part(&args, i2id, rank, topL + i*L, L);

				R=PL=0;
				double RL = 0;
				metrics_R_RL_PL_Bip(i, traini1->degree, traini2->idNum, testi1, L, rank, NULL, NULL, &R, &RL, &PL);
				//R will never be 0, because i is in testi1.
				if (bR > R) {
					bR = R;
					bi = j;
				}
				if (bPL < PL) {
					bPL = PL;
				}
			}
			
			total_bR += bR;
			total_R += R;
			total_bPL += bPL;
			total_PL += PL;

			best[i] = bi;

		}
		else {
			best[i] = -1;
		}
	}
	printf("BR result =>> Rate: %f, Best R: %f, normal R: %f, Best PL: %f, normal PL: %f\n", rate, total_bR/testi1->edgesNum, total_R/testi1->edgesNum, total_bPL/testi1->edgesNum, total_PL/testi1->edgesNum);

	free(i1source);
	free(i2source);
	free(rank);
	free(i2id);
	free(topL);
	printf("calculate BK done =>> ");
	print_time();
	return best;
}

struct Metrics_Bip *mass_topk_Bip(struct Bip *traini1, struct Bip *traini2, struct Bip *testi1, struct Bip *testi2, struct iidNet *userSim, struct iidNet *itemSim, int topk) {
	struct Bip_recommend_param param;
	param.userSim = userSim;
	param.itemSim = itemSim;
	param.mass_topk = topk;

	param.traini1 = traini1;
	param.traini2 = traini2;
	param.testi1 = testi1;

	return recommend_Bip(mass_topk_recommend_Bip, &param);
}

struct Metrics_Bip *mass_hs_Bip(struct Bip *traini1, struct Bip *traini2, struct Bip *testi1, struct Bip *testi2, struct iidNet *userSim, struct iidNet *itemSim, int topk) {
	struct Bip_recommend_param param;
	param.userSim = userSim;
	param.itemSim = itemSim;
	param.mass_topk = topk;

	param.traini1 = traini1;
	param.traini2 = traini2;
	param.testi1 = testi1;

	return recommend_Bip(mass_hs_recommend_Bip, &param);
}

struct Metrics_Bip *mass_score_Bip(struct Bip *traini1, struct Bip *traini2, struct Bip *testi1, struct Bip *testi2, struct iidNet *itemSim, int maxscore, double mass_score) {
	struct Bip_recommend_param param;
	param.itemSim = itemSim;
	param.mass_score = mass_score;
	param.maxscore = maxscore;

	param.traini1 = traini1;
	param.traini2 = traini2;
	param.testi1 = testi1;

	//return recommend_Bip(mass_recommend_Bip, &param);
	return recommend_Bip(mass_score_recommend_Bip, &param);
}

struct Metrics_Bip *mass_scoret3step_Bip(struct Bip *traini1, struct Bip *traini2, struct Bip *testi1, struct Bip *testi2, struct iidNet *itemSim, double mass_score) {
	struct Bip_recommend_param param;
	param.itemSim = itemSim;
	param.mass_score = mass_score;

	param.traini1 = traini1;
	param.traini2 = traini2;
	param.testi1 = testi1;

	return recommend_Bip(mass_scoret3step_recommend_Bip, &param);
}

struct Metrics_Bip *mass_degree_Bip(struct Bip *traini1, struct Bip *traini2, struct Bip *testi1, struct Bip *testi2, struct iidNet *itemSim, double mass_score) {
	struct Bip_recommend_param param;
	param.itemSim = itemSim;
	param.mass_score = mass_score;

	param.traini1 = traini1;
	param.traini2 = traini2;
	param.testi1 = testi1;

	return recommend_Bip(mass_degree_recommend_Bip, &param);
}

//just for rank
static double *rank_Bip(void (*recommend_core)(struct Bip_recommend_param *), struct Bip_recommend_param *args) {

	int i1maxId      = args->traini1->maxId;
	int i2maxId      = args->traini2->maxId;
	int i1idNum      = args->traini1->idNum;
	int i2idNum      = args->traini2->idNum;
	int *i1degree    = args->traini1->degree;
 	// all L is from this function. if you want to change, change the L below.
	int L = 50;

	double *i1source = malloc((i1maxId + 1)*sizeof(double));
	assert(i1source != NULL);
	double *i2source = malloc((i2maxId + 1)*sizeof(double));
	assert(i2source != NULL);
	args->i1source = i1source;
	args->i2source = i2source;

	double *i2sourceA = malloc((i2maxId + 1)*sizeof(double));
	assert(i2sourceA != NULL);
	args->i2sourceA = i2sourceA;
	double *i1sourceA = malloc((i1maxId + 1)*sizeof(double));
	assert(i1sourceA != NULL);
	args->i1sourceA = i1sourceA;

	int *rank = malloc((i2maxId + 1)*sizeof(int));
	assert(rank != NULL);
	int *i1id =  malloc((i1maxId + 1)*sizeof(int));
	assert(i1id != NULL);
	int *i2id =  malloc((i2maxId + 1)*sizeof(int));
	assert(i2id != NULL);
	args->i1id = i1id;
	args->i2id = i2id;

	int i, j;
	int *topL = calloc(L*(i1maxId + 1), sizeof(int));
	assert(topL != NULL);

	double *rankA = calloc((i2maxId + 1), sizeof(double));
	assert(rankA != NULL);

	for (i = 0; i<i1maxId + 1; ++i) { //each user
		//if (i%1000 ==0) {printf("%d\n", i);fflush(stdout);}
		if (i1degree[i]) {
			//get rank
			args->i1 = i;
			recommend_core(args);
			Bip_core_common_part(args, i2id, rank, topL + i*L, L);
			for (j=0; j<i2maxId + 1; ++j) {
				rankA[j] += (double)rank[j]/((double)i2idNum*i1idNum);
			}
		}
	}

	free(i1source);
	free(i2source);
	free(i1sourceA);
	free(i2sourceA);
	free(i1id);
	free(i2id);
	free(rank);
	return rankA;
}

double *mass_score_rank_Bip(struct Bip *traini1, struct Bip *traini2, int maxscore, double mass_score) {
	struct Bip_recommend_param param;
	param.mass_score = mass_score;
	param.maxscore = maxscore;

	param.traini1 = traini1;
	param.traini2 = traini2;

	//return recommend_Bip(mass_recommend_Bip, &param);
	return rank_Bip(mass_score_recommend_Bip, &param);
}

double *mass_scoret3step_rank_Bip(struct Bip *traini1, struct Bip *traini2, double mass_score) {
	struct Bip_recommend_param param;
	//param.itemSim = itemSim;
	param.mass_score = mass_score;

	param.traini1 = traini1;
	param.traini2 = traini2;

	return rank_Bip(mass_scoret3step_recommend_Bip, &param);
}

double *mass_degree_rank_Bip(struct Bip *traini1, struct Bip *traini2, double mass_score) {
	struct Bip_recommend_param param;
	param.mass_score = mass_score;

	param.traini1 = traini1;
	param.traini2 = traini2;

	return rank_Bip(mass_degree_recommend_Bip, &param);
}

struct Metrics_Bip *UCF_Bip(struct Bip *traini1, struct Bip *traini2, struct Bip *testi1, struct Bip *testi2, struct iidNet *trainItemSim, double *psimM, int L, int K) {
	print_time();
	struct Bip_recommend_param param;
	param.itemSim = trainItemSim;

	param.traini1 = traini1;
	param.traini2 = traini2;
	param.testi1 = testi1;

	param.L = L;
	param.K = K;

	param.psimM = psimM;

	return recommend_Bip(UCF_recommend_Bip, &param);
}

struct Metrics_Bip *ICF_Bip(struct Bip *traini1, struct Bip *traini2, struct Bip *testi1, struct Bip *testi2, struct iidNet *trainItemSim, double *psimM, int L, int K) {
	print_time();
	struct Bip_recommend_param param;
	param.itemSim = trainItemSim;

	param.traini1 = traini1;
	param.traini2 = traini2;
	param.testi1 = testi1;

	param.L = L;
	param.K = K;

	param.psimM = psimM;

	return recommend_Bip(ICF_recommend_Bip, &param);
}

struct Metrics_Bip *SVD_Bip(struct Bip *traini1, struct Bip *traini2, struct Bip *testi1, struct Bip *testi2, struct iidNet *trainItemSim, double *rui, int L) {
	return NULL;
}
