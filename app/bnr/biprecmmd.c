#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <getopt.h>
#include <string.h>

#include "sc_mtprand.h"
#include "io_linefile.h"
#include "cn_bip.h"
#include "sc_verbose.h"

struct OptionArgs {
	int calculate_mass;
	int calculate_heat;
	int calculate_hybrid;
	int calculate_HNBI;
	int calculate_RENBI;
	int calculate_UCF;
	int calculate_ICF;
	int calculate_SVD;
	char *user_extra_att;
	char *total_filename;
	char *train_filename;
	char *test_filename;

	int loopNum;
	double dataset_divide_rate;
	int random_seed;
	int L;
	int K;
	int F;

	int random;
};

static void display_usage(void) {
	puts("biprecmmd - bipart network recommemder system.");
	puts("-m: using mass");
	puts("-h: using heat");
	puts("-H: using hybrid");
	puts("-N: using HNBI");
	puts("-E: using RENBI");
	puts("-C: using UCF");
	puts("-F: using ICF");
	puts("-S: using SVD");
	puts("-u: users extra attribute");
	puts("-i: full dataset filename");
	puts("-T: train dataset filename");
	puts("-t: test dataset filename");

	puts("-l: loop Number");
	puts("-d: dataset divide rate(double)");
	puts("-s: random seed");
	puts("-L: L");
	puts("-K: K");

	puts("-?: help information");
	exit(EXIT_FAILURE);
}

static void verify_OptionArgs(struct OptionArgs *OptionArgs);
static void do_work(struct OptionArgs *oa);

int main(int argc, char **argv) {
	struct OptionArgs OptionArgs;

	OptionArgs.calculate_mass = 0;
	OptionArgs.calculate_heat = 0;
	OptionArgs.calculate_hybrid = 0;
	OptionArgs.calculate_HNBI = 0;
	OptionArgs.calculate_RENBI = 0;
	OptionArgs.calculate_UCF = 0;
	OptionArgs.calculate_ICF = 0;
	OptionArgs.calculate_SVD = 0;

	OptionArgs.user_extra_att = NULL;
	OptionArgs.total_filename = NULL;
	OptionArgs.train_filename = NULL;
	OptionArgs.test_filename = NULL;

	OptionArgs.loopNum = 1;
	OptionArgs.dataset_divide_rate = 0.1;
	OptionArgs.random_seed = 1;
	OptionArgs.L = 50;
	OptionArgs.K = 50;
	OptionArgs.F = 100;

	OptionArgs.random = 0;

	static const char *optString = "mhHNEi:T:t:l:u:d:s:L:CFK:f:?";

	struct option longOpts[] = {
		{"mass", no_argument, NULL, 'm'},
		{"heat", no_argument, NULL, 'h'},
		{"hybrid", no_argument, NULL, 'H'},
		{"HNBI", no_argument, NULL, 'N'},
		{"RENBI", no_argument, NULL, 'E'},
		{"UCF", no_argument, NULL, 'C'},
		{"ICF", no_argument, NULL, 'F'},
		{"SVD", no_argument, NULL, 'S'},

		{"usersExtraAtt", required_argument, NULL, 'u'},
		{"dataset", required_argument, NULL, 'i'},
		{"trainset", required_argument, NULL, 'T'},
		{"testset", required_argument, NULL, 't'},

		{"loopNum", required_argument, NULL, 'l'},
		{"dividerate", required_argument, NULL, 'd'},
		{"random-seed", required_argument, NULL, 's'},
		{"recmlistLength", required_argument, NULL, 'L'},
		{"CFuserSimlistLength", required_argument, NULL, 'K'},
		{"SVD-F", required_argument, NULL, 'f'},
		

		{"help", no_argument, NULL, '?'}
	};
	int longIndex = 0;
	int opt = getopt_long(argc, argv, optString, longOpts, &longIndex);
	while (opt != -1) {
		switch (opt) {
			//sign
			case 'm':
				OptionArgs.calculate_mass = 1;
				//printf("m\n");
				break;
			case 'h':
				OptionArgs.calculate_heat = 1;
				//printf("h\n");
				break;
			case 'H':
				OptionArgs.calculate_hybrid = 1;
				//printf("H\n");
				break;
			case 'N':
				OptionArgs.calculate_HNBI = 1;
				break;
			case 'E':
				OptionArgs.calculate_RENBI = 1;
				break;
			case 'C':
				OptionArgs.calculate_UCF = 1;
				break;
			case 'F':
				OptionArgs.calculate_ICF = 1;
				break;
			case 'S':
				OptionArgs.calculate_SVD = 1;
				break;

			//file
			case 'u':
				OptionArgs.user_extra_att = optarg;
				//printf("testset: %s\n", optarg);
				break;
			case 'i':
				OptionArgs.total_filename = optarg;
				//printf("dataset: %s\n", optarg);
				break;
			case 'T':
				OptionArgs.train_filename = optarg;
				//printf("trainset: %s\n", optarg);
				break;
			case 't':
				OptionArgs.test_filename = optarg;
				//printf("testset: %s\n", optarg);
				break;

			//arguments
			case 'd':
				OptionArgs.dataset_divide_rate = strtod(optarg, NULL);
				//printf("d\n");
				break;
			case 'l':
				OptionArgs.loopNum = strtol(optarg, NULL, 10);
				break;
			case 's':
				OptionArgs.random_seed = strtol(optarg, NULL, 10);
				break;
			case 'L':
				OptionArgs.L = strtol(optarg, NULL, 10);
				break;
			case 'K':
				OptionArgs.K = strtol(optarg, NULL, 10);
				break;
			case 'f':
				OptionArgs.F = strtol(optarg, NULL, 10);
				break;

			//help
			case '?':
				display_usage();
				break;
			case '0':
				if (strcmp("randomize", longOpts[longIndex].name) == 0) {
					OptionArgs.random = 1;
				}
				break;
			default:
				break;
		}
		opt = getopt_long(argc, argv, optString, longOpts, &longIndex);
	}
	//printf("%d\n", OptionArgs.random);

	set_seed_MTPR(OptionArgs.random_seed);
	verify_OptionArgs(&OptionArgs);
	do_work(&OptionArgs);
	return 0;
}

static void verify_OptionArgs(struct OptionArgs *OptionArgs) {
	if (OptionArgs->total_filename == NULL && \
			(OptionArgs->train_filename == NULL || OptionArgs->test_filename == NULL)) {
		printer("dataset not enough.\n");
		exit(EXIT_FAILURE);
	}
	else if (OptionArgs->total_filename != NULL && \
			(OptionArgs->train_filename != NULL || OptionArgs->test_filename != NULL)) {
		printer("dataset too much.\n");
		exit(EXIT_FAILURE);
	}
	if (OptionArgs->user_extra_att == NULL) {
		printer("user extra att file not specify.\n");
		exit(EXIT_FAILURE);
	}
}

static void do_work_divide_noscore(struct OptionArgs *oa);
static void do_work_divide_score(struct OptionArgs *oa);
static void do_work_merge(void);

static void do_work(struct OptionArgs *oa) {
	if (oa->total_filename != NULL) {
		if (oa->calculate_UCF || oa->calculate_ICF || oa->calculate_SVD) {
			do_work_divide_score(oa);
		}
		else {
			do_work_divide_noscore(oa);
		}
	}
	else {
		do_work_merge();
	}
}

static void do_work_divide_noscore_mass(struct Bip *tr1, struct Bip *tr2, struct Bip *te1, struct Bip *te2, struct iidNet *trsim, struct Metrics_Bip *result, struct User_ATT *ua, int L);
static void do_work_divide_noscore_heat(struct Bip *tr1, struct Bip *tr2, struct Bip *te1, struct Bip *te2, struct iidNet *trsim, struct Metrics_Bip *result, struct User_ATT *ua, int L);
static void do_work_divide_noscore_hybrid(struct Bip *tr1, struct Bip *tr2, struct Bip *te1, struct Bip *te2, struct iidNet *trsim, double hybrid_param, struct Metrics_Bip *result, struct User_ATT *ua, int L);
static void do_work_divide_noscore_HNBI(struct Bip *tr1, struct Bip *tr2, struct Bip *te1, struct Bip *te2, struct iidNet *trsim, double HNBI_param, struct Metrics_Bip *result, struct User_ATT *ua, int L);
static void do_work_divide_noscore_RENBI(struct Bip *tr1, struct Bip *tr2, struct Bip *te1, struct Bip *te2, struct iidNet *trsim, double RENBI_param, struct Metrics_Bip *result, struct User_ATT *ua, int L);

static void do_work_divide_noscore(struct OptionArgs *oa) {
	struct Bip *ds1, *ds2, *tr1, *tr2, *te1, *te2;
	struct LineFile *smlp, *bigp;
	
	struct LineFile *lf = create_LineFile(oa->total_filename, 1, 1, -1);
	ds1 = create_Bip(lf, 1);
	ds2 = create_Bip(lf, 2);

	struct LineFile *ulf = create_LineFile(oa->user_extra_att, 1, 1, 1, -1);
	int *user_gender = malloc((ds1->maxId + 1) * sizeof(int));
	int *user_age = malloc((ds1->maxId + 1) * sizeof(int));
	int i;
	for (i = 0; i < ds1->maxId + 1; ++i) {
		user_gender[i] = -1;
		user_age[i] = -1;
	}
	for (i = 0; i < ulf->linesNum; ++i) {
		user_gender[ulf->i1[i]] = ulf->i2[i];
		user_age[ulf->i1[i]] = ulf->i3[i];
	}
	free_LineFile(ulf);

	struct User_ATT ua;
	ua.gender = user_gender;
	ua.age = user_age;

	struct Metrics_Bip *mass_result = create_MetricsBip();
	struct Metrics_Bip *heats_result = create_MetricsBip();
	struct Metrics_Bip *hybrid_result = create_MetricsBip();
	struct Metrics_Bip *HNBI_result = create_MetricsBip();
	struct Metrics_Bip *RENBI_result = create_MetricsBip();

	for (i = 0; i < oa->loopNum; ++i) {
		divide_Bip(ds1, ds2, oa->dataset_divide_rate, &smlp, &bigp);
		te1 = create_Bip(smlp, 1);
		te2 = create_Bip(smlp, 2);
		tr1 = create_Bip(bigp, 1);
		tr2 = create_Bip(bigp, 2);

		for (i = 0; i < CA_METRICS_BIP; ++i) {
			ua.testset_node_num[i] = 0;	
			ua.testset_edge_num[i] = 0;	
		}

		for (i = 0; i < te1->maxId + 1; ++i) {
			if (te1->degree[i]) {
				(ua.testset_node_num[user_gender[i]]) ++;
				ua.testset_edge_num[user_gender[i]] += te1->degree[i];
				(ua.testset_node_num[user_age[i]]) ++;
				ua.testset_edge_num[user_age[i]] += te1->degree[i];
			}
		}
		ua.testset_node_num[0] = te1->idNum;
		ua.testset_edge_num[0] = te1->edgesNum;
		
		//for (i = 0; i < CA_METRICS_BIP; ++i) {
		//	printf("%d, %d, %d\n", i, ua.testset_node_num[i], ua.testset_edge_num[i]);
		//}

		struct LineFile *simf = cosine_similarity_Bip(tr1, tr2, 2);
		struct iidNet *trsim = create_iidNet(simf);
		free_LineFile(simf);

		if (oa->calculate_mass == 1) {
			do_work_divide_noscore_mass(tr1, tr2, te1, te2, trsim, mass_result, &ua, oa->L);
		}
		if (oa->calculate_heat == 1) {
			do_work_divide_noscore_heat(tr1, tr2, te1, te2, trsim, heats_result, &ua, oa->L);
		}
		if (oa->calculate_hybrid == 1) {
			do_work_divide_noscore_hybrid(tr1, tr2, te1, te2, trsim, 0.2, hybrid_result, &ua, oa->L);
		}
		if (oa->calculate_HNBI == 1) {
			do_work_divide_noscore_HNBI(tr1, tr2, te1, te2, trsim, -0.8, HNBI_result, &ua, oa->L);
		}
		if (oa->calculate_RENBI == 1) {
			do_work_divide_noscore_RENBI(tr1, tr2, te1, te2, trsim, -0.75, RENBI_result, &ua, oa->L);
		}

		free_LineFile(smlp); 
		free_LineFile(bigp);
		free_Bip(tr1); free_Bip(tr2);
		free_Bip(te1); free_Bip(te2);
		free_iidNet(trsim);
	}

	int loopNum = oa->loopNum;
	if (oa->calculate_mass == 1) {
		for (i = 0; i < CA_METRICS_BIP; ++i) {
			printf("\tmass_%d\tloopNum: %d, R: %.17f, RL: %.17f, PL: %.17f, IL: %.17f, HL: %.17f, NL: %.17f, COV: %.17f\n", i, loopNum, mass_result->R[i]/loopNum, mass_result->RL[i]/loopNum, mass_result->PL[i]/loopNum, mass_result->IL[i]/loopNum, mass_result->HL[i]/loopNum, mass_result->NL[i]/loopNum, mass_result->COV[i]/loopNum);
		}
	}
	if (oa->calculate_heat == 1) {
		for (i = 0; i < CA_METRICS_BIP; ++i) {
			printf("\theats_%d\tloopNum: %d, R: %.17f, RL: %.17f, PL: %.17f, IL: %.17f, HL: %.17f, NL: %.17f, COV: %.17f\n", i, loopNum, heats_result->R[i]/loopNum, heats_result->RL[i]/loopNum, heats_result->PL[i]/loopNum, heats_result->IL[i]/loopNum, heats_result->HL[i]/loopNum, heats_result->NL[i]/loopNum, heats_result->COV[i]/loopNum);
		}
	}
	if (oa->calculate_hybrid == 1) {
		for (i = 0; i < CA_METRICS_BIP; ++i) {
			printf("\thybrid_%d\tloopNum: %d, R: %.17f, RL: %.17f, PL: %.17f, IL: %.17f, HL: %.17f, NL: %.17f, COV: %.17f\n", i, loopNum, hybrid_result->R[i]/loopNum, hybrid_result->RL[i]/loopNum, hybrid_result->PL[i]/loopNum, hybrid_result->IL[i]/loopNum, hybrid_result->HL[i]/loopNum, hybrid_result->NL[i]/loopNum, hybrid_result->COV[i]/loopNum);
		}
	}
	if (oa->calculate_HNBI == 1) {
		for (i = 0; i < CA_METRICS_BIP; ++i) {
			printf("\tHNBI_%d\tloopNum: %d, R: %.17f, RL: %.17f, PL: %.17f, IL: %.17f, HL: %.17f, NL: %.17f, COV: %.17f\n", i, loopNum, HNBI_result->R[i]/loopNum, HNBI_result->RL[i]/loopNum, HNBI_result->PL[i]/loopNum, HNBI_result->IL[i]/loopNum, HNBI_result->HL[i]/loopNum, HNBI_result->NL[i]/loopNum, HNBI_result->COV[i]/loopNum);
		}
	}
	if (oa->calculate_RENBI == 1) {
		for (i = 0; i < CA_METRICS_BIP; ++i) {
			printf("\tRENBI_%d\tloopNum: %d, R: %.17f, RL: %.17f, PL: %.17f, IL: %.17f, HL: %.17f, NL: %.17f, COV: %.17f\n", i, loopNum, RENBI_result->R[i]/loopNum, RENBI_result->RL[i]/loopNum, RENBI_result->PL[i]/loopNum, RENBI_result->IL[i]/loopNum, RENBI_result->HL[i]/loopNum, RENBI_result->NL[i]/loopNum, RENBI_result->COV[i]/loopNum);
		}
	}

	free(user_gender); free(user_age);
	free_MetricsBip(mass_result);
	free_MetricsBip(heats_result);
	free_MetricsBip(hybrid_result);
	free_MetricsBip(HNBI_result);
	free_MetricsBip(RENBI_result);
	free_Bip(ds1); free_Bip(ds2);
	free_LineFile(lf); 
}

static void do_work_merge(void) {
}

static void metrics_add_add(struct Metrics_Bip *sum, struct Metrics_Bip *single) {
	int i;
	for (i = 0; i < CA_METRICS_BIP; ++i) {
		sum->R[i] +=  single->R[i];
		sum->RL[i] += single->RL[i];
		sum->PL[i] += single->PL[i];
		sum->HL[i] += single->HL[i];
		sum->IL[i] += single->IL[i];
		sum->NL[i] += single->NL[i];
		sum->COV[i] += single->COV[i];
	}
}

static void do_work_divide_noscore_mass(struct Bip *tr1, struct Bip *tr2, struct Bip *te1, struct Bip *te2, struct iidNet *trsim, struct Metrics_Bip *result, struct User_ATT *ua, int L) {
	struct Metrics_Bip *mass_result = mass_Bip(tr1, tr2, te1, te2, trsim, ua, L);
	metrics_add_add(result, mass_result);
	free_MetricsBip(mass_result);
}

static void do_work_divide_noscore_heat(struct Bip *tr1, struct Bip *tr2, struct Bip *te1, struct Bip *te2, struct iidNet *trsim, struct Metrics_Bip *result, struct User_ATT *ua, int L) {
	struct Metrics_Bip *heats_result = heats_Bip(tr1, tr2, te1, te2, trsim, ua, L);
	metrics_add_add(result, heats_result);
	free_MetricsBip(heats_result);
}

static void do_work_divide_noscore_hybrid(struct Bip *tr1, struct Bip *tr2, struct Bip *te1, struct Bip *te2, struct iidNet *trsim, double hybrid_param, struct Metrics_Bip *result, struct User_ATT *ua, int L) {
	struct Metrics_Bip *hybrid_result = hybrid_Bip(tr1, tr2, te1, te2, trsim, hybrid_param, ua, L);
	metrics_add_add(result, hybrid_result);
	free_MetricsBip(hybrid_result);
}

static void do_work_divide_noscore_HNBI(struct Bip *tr1, struct Bip *tr2, struct Bip *te1, struct Bip *te2, struct iidNet *trsim, double HNBI_param, struct Metrics_Bip *result, struct User_ATT *ua, int L) {
	struct Metrics_Bip *HNBI_result = HNBI_Bip(tr1, tr2, te1, te2, trsim, HNBI_param, ua, L);
	metrics_add_add(result, HNBI_result);
	free_MetricsBip(HNBI_result);
}

static void do_work_divide_noscore_RENBI(struct Bip *tr1, struct Bip *tr2, struct Bip *te1, struct Bip *te2, struct iidNet *trsim, double RENBI_param, struct Metrics_Bip *result, struct User_ATT *ua, int L) {
	struct Metrics_Bip *RENBI_result = RENBI_Bip(tr1, tr2, te1, te2, trsim, RENBI_param, ua, L);
	metrics_add_add(result, RENBI_result);
	free_MetricsBip(RENBI_result);
}


static void do_work_divide_score_UCF(struct Bip *tr1, struct Bip *tr2, struct Bip *te1, struct Bip *te2, struct iidNet *trainItemSim, double *psimM, struct Metrics_Bip *result, struct User_ATT *ua, int L, int K);
static void do_work_divide_score_ICF(struct Bip *tr1, struct Bip *tr2, struct Bip *te1, struct Bip *te2, struct iidNet *trainItemSim, double *psimM, struct Metrics_Bip *result, struct User_ATT *ua, int L, int K);
static void do_work_divide_score_SVD(struct Bip *tr1, struct Bip *tr2, struct Bip *te1, struct Bip *te2, struct iidNet *trainItemSim, struct Metrics_Bip *result, struct User_ATT *ua, int L, int F);

static double *create_psimM(struct LineFile *simf, int maxId) {
	double *psimM = smalloc((maxId + 1)*(maxId + 1) * sizeof(double));
	int i;
	for (i = 0; i <(maxId + 1) * (maxId + 1); ++i) {
		psimM[i] = 0;	
	}
	for (i = 0; i < simf->linesNum; ++i) {
		psimM[simf->i1[i] * (maxId + 1) + simf->i2[i]] = simf->d1[i];
		psimM[simf->i2[i] * (maxId + 1) + simf->i1[i]] = simf->d1[i];
	}
	return psimM;
}

static void do_work_divide_score(struct OptionArgs *oa) {
	struct Bip *ds1, *ds2, *tr1, *tr2, *te1, *te2;
	struct LineFile *smlp, *bigp;
	
	struct LineFile *lf = create_LineFile(oa->total_filename, 1, 1, 1, -1);
	ds1 = create_Bip(lf, 1);
	ds2 = create_Bip(lf, 2);

	struct LineFile *ulf = create_LineFile(oa->user_extra_att, 1, 1, 1, -1);
	int *user_gender = malloc((ds1->maxId + 1) * sizeof(int));
	int *user_age = malloc((ds1->maxId + 1) * sizeof(int));
	int i;
	for (i = 0; i < ds1->maxId + 1; ++i) {
		user_gender[i] = -1;
		user_age[i] = -1;
	}
	for (i = 0; i < ulf->linesNum; ++i) {
		user_gender[ulf->i1[i]] = ulf->i2[i];
		user_age[ulf->i1[i]] = ulf->i3[i];
	}
	free_LineFile(ulf);

	struct User_ATT ua;
	ua.gender = user_gender;
	ua.age = user_age;

	struct Metrics_Bip *UCF_result = create_MetricsBip();
	struct Metrics_Bip *ICF_result = create_MetricsBip();
	struct Metrics_Bip *SVD_result = create_MetricsBip();

	for (i = 0; i < oa->loopNum; ++i) {
		divide_Bip(ds1, ds2, oa->dataset_divide_rate, &smlp, &bigp);
		te1 = create_Bip(smlp, 1);
		te2 = create_Bip(smlp, 2);
		tr1 = create_Bip(bigp, 1);
		tr2 = create_Bip(bigp, 2);

		for (i = 0; i < CA_METRICS_BIP; ++i) {
			ua.testset_node_num[i] = 0;	
			ua.testset_edge_num[i] = 0;	
		}

		for (i = 0; i < te1->maxId + 1; ++i) {
			if (te1->degree[i]) {
				(ua.testset_node_num[user_gender[i]]) ++;
				ua.testset_edge_num[user_gender[i]] += te1->degree[i];
				(ua.testset_node_num[user_age[i]]) ++;
				ua.testset_edge_num[user_age[i]] += te1->degree[i];
			}
		}
		ua.testset_node_num[0] = te1->idNum;
		ua.testset_edge_num[0] = te1->edgesNum;
		
		for (i = 0; i < CA_METRICS_BIP; ++i) {
			printf("%d, %d, %d\n", i, ua.testset_node_num[i], ua.testset_edge_num[i]);
		}

		struct LineFile *simf = cosine_similarity_Bip(tr1, tr2, 2);
		struct iidNet *trsim = create_iidNet(simf);
		free_LineFile(simf);


		if (oa->calculate_UCF == 1) {
			simf = pearson_similarity_Bip(tr1, tr2, 1);
			double *psimM = create_psimM(simf, tr1->maxId);
			free_LineFile(simf);
			do_work_divide_score_UCF(tr1, tr2, te1, te2, trsim, psimM, UCF_result, &ua, oa->L, oa->K);
			free(psimM);
		}
		if (oa->calculate_ICF == 1) {
			simf = pearson_similarity_Bip(tr1, tr2, 2);
			double *psimM = create_psimM(simf, tr2->maxId);
			free_LineFile(simf);
			do_work_divide_score_ICF(tr1, tr2, te1, te2, trsim, psimM, ICF_result, &ua, oa->L, oa->K);
			free(psimM);
		}
		if (oa->calculate_SVD == 1) {
			do_work_divide_score_SVD(tr1, tr2, te1, te2, trsim, SVD_result, &ua, oa->L, oa->F);
		}

		free_LineFile(smlp); 
		free_LineFile(bigp);
		free_Bip(tr1); free_Bip(tr2);
		free_Bip(te1); free_Bip(te2);
		free_iidNet(trsim);
	}

	int loopNum = oa->loopNum;
	if (oa->calculate_UCF == 1) {
		for (i = 0; i < CA_METRICS_BIP; ++i) {
			printf("\tUCF_%d\tloopNum: %d, R: %.17f, RL: %.17f, PL: %.17f, IL: %.17f, HL: %.17f, NL: %.17f, COV: %.17f\n", i, loopNum, UCF_result->R[i]/loopNum, UCF_result->RL[i]/loopNum, UCF_result->PL[i]/loopNum, UCF_result->IL[i]/loopNum, UCF_result->HL[i]/loopNum, UCF_result->NL[i]/loopNum, UCF_result->COV[i]/loopNum);
		}
	}
	if (oa->calculate_ICF == 1) {
		for (i = 0; i < CA_METRICS_BIP; ++i) {
			printf("\tICF_%d\tloopNum: %d, R: %.17f, RL: %.17f, PL: %.17f, IL: %.17f, HL: %.17f, NL: %.17f, COV: %.17f\n", i, loopNum, ICF_result->R[i]/loopNum, ICF_result->RL[i]/loopNum, ICF_result->PL[i]/loopNum, ICF_result->IL[i]/loopNum, ICF_result->HL[i]/loopNum, ICF_result->NL[i]/loopNum, ICF_result->COV[i]/loopNum);
		}
	}

	free(user_gender); free(user_age);
	free_MetricsBip(UCF_result);
	free_MetricsBip(ICF_result);
	free_MetricsBip(SVD_result);
	free_Bip(ds1); free_Bip(ds2);
	free_LineFile(lf); 
}

static void do_work_divide_score_UCF(struct Bip *tr1, struct Bip *tr2, struct Bip *te1, struct Bip *te2, struct iidNet *trsim, double *psimM, struct Metrics_Bip *result, struct User_ATT *ua, int L, int K) {
	struct Metrics_Bip *UCF_result = UCF_Bip(tr1, tr2, te1, te2, trsim, psimM, ua, L, K);
	metrics_add_add(result, UCF_result);
	free_MetricsBip(UCF_result);
}

static void do_work_divide_score_ICF(struct Bip *tr1, struct Bip *tr2, struct Bip *te1, struct Bip *te2, struct iidNet *trsim, double *psimM, struct Metrics_Bip *result, struct User_ATT *ua, int L, int K) {
	struct Metrics_Bip *ICF_result = ICF_Bip(tr1, tr2, te1, te2, trsim, psimM, ua, L, K);
	metrics_add_add(result, ICF_result);
	free_MetricsBip(ICF_result);
}

static void do_work_divide_score_SVD(struct Bip *tr1, struct Bip *tr2, struct Bip *te1, struct Bip *te2, struct iidNet *trsim, struct Metrics_Bip *result, struct User_ATT *ua, int L, int F) {
	//memory allocate
	double *rui = smalloc((tr1->maxId + 1) * (tr2->maxId + 1) * sizeof(double));
	double *rui_s = smalloc((tr1->maxId + 1) * (tr2->maxId + 1) * sizeof(double)); 
	double *eui = smalloc((tr1->maxId + 1) * (tr2->maxId + 1) * sizeof(double)); 
	//all score avarage 
	long allscore = 0;
	int i,j,k;
	for (j = 0; j < tr1->maxId + 1; ++j) {
		if (tr1->degree[j]) {	
			for (k = 0; k < tr1->degree[j]; ++k) {
				allscore+=tr1->score[j][k];	
			}
		}
	}
	double avascore = (double)allscore/tr1->edgesNum;

	//bi
	double *itemscore = smalloc((tr2->maxId + 1)*sizeof(double));
	double SVD_l2 = 25;
	for (j = 0; j < tr2->maxId + 1; ++j) {
		itemscore[j] = 0;
		for (k = 0; k < tr2->degree[j]; ++k) {
			itemscore[j] += tr2->score[j][k] - avascore;	
		}
		itemscore[j] /= SVD_l2 + tr2->degree[j];
	}

	//bu
	double *userscore = smalloc((tr1->maxId + 1) * sizeof(double));
	double SVD_l3 = 25;
	for (j = 0; j < tr1->maxId + 1; ++j) {
		userscore[j] = 0;
		for (k = 0; k < tr1->degree[j]; ++k) {
			userscore[j] += tr1->score[j][k] - avascore - itemscore[tr1->edges[j][k]];
		}
		userscore[j] /= SVD_l3 + tr1->degree[j];
	}

	//p, q
	double *p = smalloc((tr1->maxId + 1)*F*sizeof(double));
	double *q = smalloc((tr2->maxId + 1)*F*sizeof(double));
	double sqrtF = sqrt(F);
	for (i = 0; i < (tr1->maxId + 1)*F; ++i) {
		p[i] = 0.1*get_d_MTPR()/sqrtF;
	}
	for (i = 0; i < (tr2->maxId + 1)*F; ++i) {
		q[i] = 0.1*get_d_MTPR()/sqrtF;	
	}

	//loop, 
	//1 rui, 
	//2 eui,
	//3 bu,
	//4 bi,
	//5 p,
	//6 q,
	double SVD_l = 0.3, SVD_g = 0.02;
	int Tstep = 100;
	double slowRate = 0.99;
	for (i = 0; i < tr1->maxId + 1; ++i) {
		for (j = 0; j < tr2->maxId + 1; ++j) {
			rui[i * (tr1->maxId + 1) + j] = avascore + userscore[i] + itemscore[j];
			for (k = 0; k < F; ++k) {
				rui[i * (tr1->maxId + 1) + j] += p[i * (tr1->maxId + 1) + k] * q[j * (tr2->maxId + 1) + k];
			}
		}
	}
	for (i = 0; i < tr1->maxId + 1; ++i) {
		for (j = 0; j < tr1->degree[i]; ++j) {
			eui[i * (tr1->maxId + 1) + tr1->edges[i][j]] = tr1->score[i][j] - rui[i * (tr1->maxId + 1) + tr1->edges[i][j]];
		}
	}
	for (i = 0; i < tr1->maxId + 1; ++i) {
		double usi = userscore[i];
		for (j = 0; j < tr1->degree[i]; ++j) {
			userscore[i] = usi + SVD_g * (eui[i * (tr1->maxId + 1) + tr1->edges[i][j]] - SVD_l * usi);
		}
	}
	for (i = 0; i < tr2->maxId + 1; ++i) {
		double isi = itemscore[i];
		for (j = 0; j < tr2->degree[i]; ++j) {
			itemscore[i] = isi + SVD_g * (eui[tr2->edges[i][j] * (tr1->maxId + 1) + i] - SVD_l * isi);	
		}
	}
	for (i = 0; i < tr1->maxId + 1; ++i) {
		for (k = 0; k < F; ++k) {
			double pik = p[i * (tr1->maxId + 1) + k];
			for (j = 0; j < tr1->degree[i]; ++j) {
				p[j * (tr1->maxId + 1) + k] = pik + SVD_g * (eui[i * (tr1->maxId + 1) + tr1->edges[i][j]] * q[tr1->edges[j][i] * (tr2->maxId + 1) + k] - SVD_l * pik);
			}
		}
	}
	for (i = 0; i < tr2->maxId + 1; ++i) {
		for (k = 0; k < F; ++k) {
			double qik = q[i * (tr2->maxId + 1) + k];
			for (j = 0; j < tr2->degree[j]; ++j) {
				q[i * (tr2->maxId + 1) + k] = qik + SVD_g * (eui[tr2->edges[i][j] * (tr1->maxId + 1) + i] * p[tr2->edges[i][j] * (tr1->maxId + 1) + k] - SVD_l * qik);
			}
		}
	}

	double RMSE_T = 0;
	for (i = 0; i < tr1->maxId + 1; ++i) {
		for (j = 0; j < tr1->degree[i]; ++j) {
			RMSE_T += pow(eui[i * (tr1->maxId + 1) + tr1->edges[i][j]], 2);
		}
	}
	RMSE_T = sqrt(RMSE_T/tr1->edgesNum);
	memcpy(rui_s, rui, sizeof(double) * (tr1->maxId + 1) * (tr2->maxId + 1));

	do {
		for (i = 0; i < tr1->maxId + 1; ++i) {
			for (j = 0; j < tr2->maxId + 1; ++j) {
				rui[i * (tr1->maxId + 1) + j] = avascore + userscore[i] + itemscore[j];
				for (k = 0; k < F; ++k) {
					rui[i * (tr1->maxId + 1) + j] += p[i * (tr1->maxId + 1) + k] * q[j * (tr2->maxId + 1) + k];
				}
			}
		}
		for (i = 0; i < tr1->maxId + 1; ++i) {
			for (j = 0; j < tr1->degree[i]; ++j) {
				eui[i * (tr1->maxId + 1) + tr1->edges[i][j]] = tr1->score[i][j] - rui[i * (tr1->maxId + 1) + tr1->edges[i][j]];
			}
		}
		for (i = 0; i < tr1->maxId + 1; ++i) {
			double usi = userscore[i];
			for (j = 0; j < tr1->degree[i]; ++j) {
				userscore[i] = usi + SVD_g * (eui[i * (tr1->maxId + 1) + tr1->edges[i][j]] - SVD_l * usi);
			}
		}
		for (i = 0; i < tr2->maxId + 1; ++i) {
			double isi = itemscore[i];
			for (j = 0; j < tr2->degree[i]; ++j) {
				itemscore[i] = isi + SVD_g * (eui[tr2->edges[i][j] * (tr1->maxId + 1) + i] - SVD_l * isi);	
			}
		}
		for (i = 0; i < tr1->maxId + 1; ++i) {
			for (k = 0; k < F; ++k) {
				double pik = p[i * (tr1->maxId + 1) + k];
				for (j = 0; j < tr1->degree[i]; ++j) {
					p[j * (tr1->maxId + 1) + k] = pik + SVD_g * (eui[i * (tr1->maxId + 1) + tr1->edges[i][j]] * q[tr1->edges[j][i] * (tr2->maxId + 1) + k] - SVD_l * pik);
				}
			}
		}
		for (i = 0; i < tr2->maxId + 1; ++i) {
			for (k = 0; k < F; ++k) {
				double qik = q[i * (tr2->maxId + 1) + k];
				for (j = 0; j < tr2->degree[j]; ++j) {
					q[i * (tr2->maxId + 1) + k] = qik + SVD_g * (eui[tr2->edges[i][j] * (tr1->maxId + 1) + i] * p[tr2->edges[i][j] * (tr1->maxId + 1) + k] - SVD_l * qik);
				}
			}
		}
		double RMSE = 0;
		for (i = 0; i < tr1->maxId + 1; ++i) {
			for (j = 0; j < tr1->degree[i]; ++j) {
				RMSE += pow(eui[i * (tr1->maxId + 1) + tr1->edges[i][j]], 2);
			}
		}
		RMSE = sqrt(RMSE/tr1->edgesNum); 

		if (RMSE < RMSE_T) {
			memcpy(rui_s, rui, sizeof(double) * (tr1->maxId + 1) * (tr2->maxId + 1));
		}

		SVD_g *= slowRate;
	} while (Tstep--);
	free(p); free(q);
	free(userscore); free(itemscore);
	free(rui); free(eui);

	struct Metrics_Bip *SVD_result = SVD_Bip(tr1, tr2, te1, te2, trsim, rui_s, ua, L);
	metrics_add_add(result, SVD_result);
	free_MetricsBip(SVD_result);
}
