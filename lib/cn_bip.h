/**
 * recommandation is carried on between user and item, always two part.
 * maybe a biparte can contain some extra attributes, like score that a user give to a item. but any attribute can be treated as a param.
 * this is more flexible.
 *
 * for bip:
 * 		the origin bipartite is coming from a struct LineFile with using create_Bip(struct LineFile) function.
 * 		(BTW, the origin LineFile is getting from a file which contain at least two int integer in an line)
 *
 * 		struct Bip contains only half information of the bipartite. 
 * 		(I mean one struct Bip can store a LineFile completely, but it doesn't store detail information.)
 * 		you need two struct Bip to store all detail information of a Bipartite.
 * 		one is indexed by i1(user).
 * 		one is indexed by i2(item).
 *
 * struct Metrics_Bip contains the result(all metrics) for all kinds of recommendation algorithm.
 *
 */
#ifndef RSC_BIP_H
#define RSC_BIP_H

#include "io_linefile.h"
#include "cn_iidnet.h"

//this struct is just one way for describing Bipartite.
//other ways like (int ***xx) is also usefully.
//Bipartite contains two parts. e.g. user and item.
//but here, create_Bip only create user Bipartite or item Bipartite.
//if you want both, create two time with different index arg.
#define CA_METRICS_BIP 10 
struct Bip {
	//Basic
	long edgesNum;
	int maxId;
	int minId;
	int idNum;
	int degreeMax;
	int degreeMin;
	int *degree;
	int **edges;

	//additional
	int att1[CA_METRICS_BIP];
	int att2[CA_METRICS_BIP];
	//attI1 & attI2 & attD1's index is same as degree.
	int *attI1;
	int *attI2;
	double *attD1;
	//edgesI & edgesD's index is same as edges.
	int **edgesI;
	double **edgesD;
};

//if index is 1, means the i1 is the index, and i2 is the data saved into (int **edges).
//i1 and i2 is the data in LineFile.
//if index is 2, means i2 is the index.
struct Bip *create_Bip(const struct LineFile * const file, int index);
void free_Bip(struct Bip *bip);
struct Bip * clone_Bip(struct Bip *bip);
void sort_desc_by_edges_Bip(struct Bip *bip);
//make sure there is no duplicate lines.
void verify_Bip(struct Bip *bipi1, struct Bip *bipi2);
void print_Bip(struct Bip *bip, char *filename);
void set_attI_Bip(char *file, struct Bip *bip);

//the dividation will guarantee that: 
//	for each available user(degree of this user is at least one), at least there will be an edge related this user in big part.
//	for each available item(degree of this item is at least one), at least there will be an edge related this item in big part.
//so maybe some users/items are not existed in the small part.
void divide_Bip(struct Bip *bipi1, struct Bip *bipi2, double rate, struct LineFile **small_part, struct LineFile **big_part);

//similarity
//if target == 1, then calculate i1(mostly could be user)'s similarity.
//if target == 2, then calculate i2(mostly could be item)'s similarity.
struct LineFile *cosine_similarity_Bip(struct Bip *bipi1, struct Bip *bipi2, int target);
struct LineFile *pearson_similarity_Bip(struct Bip *bipi1, struct Bip *bipi2, int target);

struct Metrics_Bip {
	double R[CA_METRICS_BIP];
	double RL[CA_METRICS_BIP];
	double PL[CA_METRICS_BIP];
	double HL[CA_METRICS_BIP];
	double IL[CA_METRICS_BIP];
	double NL[CA_METRICS_BIP];
	double COV[CA_METRICS_BIP];

	int *topL;
	int L;
};
struct Metrics_Bip *create_MetricsBip(void);
void clean_MetricsBip(struct Metrics_Bip *m);
void free_MetricsBip(struct Metrics_Bip *m);

//normal
struct Metrics_Bip *mass_Bip(struct Bip *traini1, struct Bip *traini2, struct Bip *testi1, struct Bip *testi2, struct iidNet *trainItemCosinSim, int L);
struct Metrics_Bip *heats_Bip(struct Bip *traini1, struct Bip *traini2, struct Bip *testi1, struct Bip *testi2, struct iidNet *trainItemCosinSim, int L);
struct Metrics_Bip *HNBI_Bip(struct Bip *traini1, struct Bip *traini2, struct Bip *testi1, struct Bip *testi2, struct iidNet *trainItemCosinSim, double HNBI_param, int L);
struct Metrics_Bip *RENBI_Bip(struct Bip *traini1, struct Bip *traini2, struct Bip *testi1, struct Bip *testi2, struct iidNet *trainItemCosinSim, double RENBI_param, int L);
struct Metrics_Bip *hybrid_Bip(struct Bip *traini1, struct Bip *traini2, struct Bip *testi1, struct Bip *testi2, struct iidNet *trainItemCosinSim, double hybrid_param, int L);

//topk and highsimilarity
int *mass_getBK_Bip(struct Bip *traini1, struct Bip *traini2, struct Bip *testi1, struct Bip *testi2, struct iidNet *userSim, double rate);
struct Metrics_Bip *mass_topk_Bip(struct Bip *traini1, struct Bip *traini2, struct Bip *testi1, struct Bip *testi2, struct iidNet *userSim, struct iidNet *itemSim, int topk);
struct Metrics_Bip *mass_hs_Bip(struct Bip *traini1, struct Bip *traini2, struct Bip *testi1, struct Bip *testi2, struct iidNet *userSim, struct iidNet *itemSim, int topk);

//score bip
struct Metrics_Bip *mass_score_Bip(struct Bip *traini1, struct Bip *traini2, struct Bip *testi1, struct Bip *testi2, struct iidNet *itemSim, int maxscore, double mass_score);
struct Metrics_Bip *mass_scoret3step_Bip(struct Bip *traini1, struct Bip *traini2, struct Bip *testi1, struct Bip *testi2, struct iidNet *itemSim, double mass_score);
struct Metrics_Bip *mass_degree_Bip(struct Bip *traini1, struct Bip *traini2, struct Bip *testi1, struct Bip *testi2, struct iidNet *itemSim,  double mass_score);

//rank
double *mass_score_rank_Bip(struct Bip *traini1, struct Bip *traini2, int maxscore, double mass_score);
double *mass_scoret3step_rank_Bip(struct Bip *traini1, struct Bip *traini2, double mass_score);
double *mass_degree_rank_Bip(struct Bip *traini1, struct Bip *traini2, double mass_score);

//CF 
struct Metrics_Bip *UCF_Bip(struct Bip *traini1, struct Bip *traini2, struct Bip *testi1, struct Bip *testi2, struct iidNet *trainItemCosinSim, double *psimM, int L, int K);
struct Metrics_Bip *ICF_Bip(struct Bip *traini1, struct Bip *traini2, struct Bip *testi1, struct Bip *testi2, struct iidNet *trainItemCosinSim, double *psimM, int L, int K);

//SVD
struct Metrics_Bip *SVD_Bip(struct Bip *traini1, struct Bip *traini2, struct Bip *testi1, struct Bip *testi2, struct iidNet *trainItemCosinSim, double *rui, int L);

#endif
