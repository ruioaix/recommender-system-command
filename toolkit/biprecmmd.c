#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <string.h>

#include "linefile.h"
#include "bip.h"
#include "base.h"

struct OptionArgs {
	int calculate_mass;
	int calculate_heat;
	int calculate_hybrid;
	int calculate_HNBI;
	int calculate_RENBI;
	int loopNum;
	int calculate_user_gender;
	char *total_filename;
	char *train_filename;
	char *test_filename;
	double dataset_divide_rate;

	int random;
};

static void display_usage(void) {
	puts("biprecmmd - bipart network recommemder system.");
	puts("-m: using mass");
	puts("-h: using heat");
	puts("-H: using hybrid");
	puts("-N: using HNBI");
	puts("-E: using RENBI");
	puts("-l: loop Number");
	puts("-i: full dataset filename");
	puts("-T: train dataset filename");
	puts("-t: test dataset filename");
	puts("-d: dataset divide rate(double)");
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
	OptionArgs.loopNum = 1;
	OptionArgs.total_filename = NULL;
	OptionArgs.train_filename = NULL;
	OptionArgs.test_filename = NULL;
	OptionArgs.dataset_divide_rate = 0.1;
	OptionArgs.random = 0;
	static const char *optString = "mhHNEi:T:t:l:?";

	struct option longOpts[] = {
		{"mass", no_argument, NULL, 'm'},
		{"heat", no_argument, NULL, 'h'},
		{"hybrid", no_argument, NULL, 'H'},
		{"HNBI", no_argument, NULL, 'N'},
		{"RENBI", no_argument, NULL, 'E'},
		{"loopNum", no_argument, NULL, 'l'},
		{"dataset", required_argument, NULL, 'i'},
		{"trainset", required_argument, NULL, 'T'},
		{"testset", required_argument, NULL, 't'},
		{"help", no_argument, NULL, '?'}
	};
	int longIndex = 0;
	int opt = getopt_long(argc, argv, optString, longOpts, &longIndex);
	while (opt != -1) {
		switch (opt) {
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
			case 'l':
				OptionArgs.loopNum = strtol(optarg, NULL, 10);
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
			case 'd':
				OptionArgs.dataset_divide_rate = 0.1;
				//printf("d\n");
				break;
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

	verify_OptionArgs(&OptionArgs);
	do_work(&OptionArgs);
	return 0;
}

static void verify_OptionArgs(struct OptionArgs *OptionArgs) {
	if (OptionArgs->total_filename == NULL && \
			(OptionArgs->train_filename == NULL || OptionArgs->test_filename == NULL)) {
		printf("dataset not enough.\n");
	}
	else if (OptionArgs->total_filename != NULL && \
			(OptionArgs->train_filename != NULL || OptionArgs->test_filename != NULL)) {
		printf("dataset too much.\n");
	}
}

static void do_work_divide(struct OptionArgs *oa);
static void do_work_merge(void);

static void do_work(struct OptionArgs *oa) {
	if (oa->total_filename != NULL) {
		do_work_divide(oa);
	}
	else {
		do_work_merge();
	}
}

static void do_work_divide_mass(struct Bip *tr1, struct Bip *tr2, struct Bip *te1, struct Bip *te2, struct iidNet *trsim, struct Metrics_Bip *result);
static void do_work_divide_heat(struct Bip *tr1, struct Bip *tr2, struct Bip *te1, struct Bip *te2, struct iidNet *trsim, struct Metrics_Bip *result);
static void do_work_divide_hybrid(struct Bip *tr1, struct Bip *tr2, struct Bip *te1, struct Bip *te2, struct iidNet *trsim, double hybrid_param, struct Metrics_Bip *result);
static void do_work_divide_HNBI(struct Bip *tr1, struct Bip *tr2, struct Bip *te1, struct Bip *te2, struct iidNet *trsim, double HNBI_param, struct Metrics_Bip *result);
static void do_work_divide_RENBI(struct Bip *tr1, struct Bip *tr2, struct Bip *te1, struct Bip *te2, struct iidNet *trsim, double RENBI_param, struct Metrics_Bip *result);

static void do_work_divide(struct OptionArgs *oa) {
	struct Bip *ds1, *ds2, *tr1, *tr2, *te1, *te2;
	struct LineFile *smlp, *bigp;
	
	struct LineFile *lf = create_LineFile(oa->total_filename, 1, 1, -1);
	ds1 = create_Bip(lf, 1);
	ds2 = create_Bip(lf, 2);

	struct Metrics_Bip *mass_result = create_MetricsBip();
	struct Metrics_Bip *heats_result = create_MetricsBip();
	struct Metrics_Bip *hybrid_result = create_MetricsBip();
	struct Metrics_Bip *HNBI_result = create_MetricsBip();
	struct Metrics_Bip *RENBI_result = create_MetricsBip();

	int i;
	for (i = 0; i < oa->loopNum; ++i) {
		divide_Bip(ds1, ds2, oa->dataset_divide_rate, &smlp, &bigp);
		te1 = create_Bip(smlp, 1);
		te2 = create_Bip(smlp, 2);
		tr1 = create_Bip(bigp, 1);
		tr2 = create_Bip(bigp, 2);

		struct LineFile *simf = similarity_Bip(tr1, tr2, 2);
		struct iidNet *trsim = create_iidNet(simf);
		free_LineFile(simf);

		if (oa->calculate_mass == 1) {
			do_work_divide_mass(tr1, tr2, te1, te2, trsim, mass_result);
		}
		if (oa->calculate_heat == 1) {
			do_work_divide_heat(tr1, tr2, te1, te2, trsim, heats_result);
		}
		if (oa->calculate_hybrid == 1) {
			do_work_divide_hybrid(tr1, tr2, te1, te2, trsim, 0.2, hybrid_result);
		}
		if (oa->calculate_HNBI == 1) {
			do_work_divide_HNBI(tr1, tr2, te1, te2, trsim, -0.8, HNBI_result);
		}
		if (oa->calculate_RENBI == 1) {
			do_work_divide_RENBI(tr1, tr2, te1, te2, trsim, -0.75, RENBI_result);
		}

		free_LineFile(smlp); 
		free_LineFile(bigp);
		free_Bip(tr1); free_Bip(tr2);
		free_Bip(te1); free_Bip(te2);
		free_iidNet(trsim);
	}
	int loopNum = oa->loopNum;
	if (oa->calculate_mass == 1) {
		printf("\tmass\tloopNum: %d, R: %f, RL: %f, PL: %f, IL: %f, HL: %f, NL: %f, COV: %f\n", loopNum, mass_result->R/loopNum, mass_result->RL/loopNum, mass_result->PL/loopNum, mass_result->IL/loopNum, mass_result->HL/loopNum, mass_result->NL/loopNum, mass_result->COV/loopNum);
	}
	if (oa->calculate_heat == 1) {
		printf("\theats\tloopNum: %d, R: %f, RL: %f, PL: %f, IL: %f, HL: %f, NL: %f, COV: %f\n", loopNum, heats_result->R/loopNum, heats_result->RL/loopNum, heats_result->PL/loopNum, heats_result->IL/loopNum, heats_result->HL/loopNum, heats_result->NL/loopNum, heats_result->COV/loopNum);
	}
	if (oa->calculate_hybrid == 1) {
		printf("\thybrid\tloopNum: %d, R: %f, RL: %f, PL: %f, IL: %f, HL: %f, NL: %f, COV: %f\n", loopNum, hybrid_result->R/loopNum, hybrid_result->RL/loopNum, hybrid_result->PL/loopNum, hybrid_result->IL/loopNum, hybrid_result->HL/loopNum, hybrid_result->NL/loopNum, hybrid_result->COV/loopNum);
	}
	if (oa->calculate_HNBI == 1) {
		printf("\tHNBI\tloopNum: %d, R: %f, RL: %f, PL: %f, IL: %f, HL: %f, NL: %f, COV: %f\n", loopNum, HNBI_result->R/loopNum, HNBI_result->RL/loopNum, HNBI_result->PL/loopNum, HNBI_result->IL/loopNum, HNBI_result->HL/loopNum, HNBI_result->NL/loopNum, HNBI_result->COV/loopNum);
	}
	if (oa->calculate_RENBI == 1) {
		printf("\tRENBI\tloopNum: %d, R: %f, RL: %f, PL: %f, IL: %f, HL: %f, NL: %f, COV: %f\n", loopNum, RENBI_result->R/loopNum, RENBI_result->RL/loopNum, RENBI_result->PL/loopNum, RENBI_result->IL/loopNum, RENBI_result->HL/loopNum, RENBI_result->NL/loopNum, RENBI_result->COV/loopNum);
	}

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
	sum->R +=  single->R;
	sum->RL += single->RL;
	sum->PL += single->PL;
	sum->HL += single->HL;
	sum->IL += single->IL;
	sum->NL += single->NL;
	sum->COV += single->COV;
}

static void do_work_divide_mass(struct Bip *tr1, struct Bip *tr2, struct Bip *te1, struct Bip *te2, struct iidNet *trsim, struct Metrics_Bip *result) {
	struct Metrics_Bip *mass_result = mass_Bip(tr1, tr2, te1, te2, trsim);
	metrics_add_add(result, mass_result);
	free_MetricsBip(mass_result);
}

static void do_work_divide_heat(struct Bip *tr1, struct Bip *tr2, struct Bip *te1, struct Bip *te2, struct iidNet *trsim, struct Metrics_Bip *result) {
	struct Metrics_Bip *heats_result = heats_Bip(tr1, tr2, te1, te2, trsim);
	metrics_add_add(result, heats_result);
	free_MetricsBip(heats_result);
}

static void do_work_divide_hybrid(struct Bip *tr1, struct Bip *tr2, struct Bip *te1, struct Bip *te2, struct iidNet *trsim, double hybrid_param, struct Metrics_Bip *result) {
	struct Metrics_Bip *hybrid_result = hybrid_Bip(tr1, tr2, te1, te2, trsim, hybrid_param);
	metrics_add_add(result, hybrid_result);
	free_MetricsBip(hybrid_result);
}

static void do_work_divide_HNBI(struct Bip *tr1, struct Bip *tr2, struct Bip *te1, struct Bip *te2, struct iidNet *trsim, double HNBI_param, struct Metrics_Bip *result) {
	struct Metrics_Bip *HNBI_result = HNBI_Bip(tr1, tr2, te1, te2, trsim, HNBI_param);
	metrics_add_add(result, HNBI_result);
	free_MetricsBip(HNBI_result);
}

static void do_work_divide_RENBI(struct Bip *tr1, struct Bip *tr2, struct Bip *te1, struct Bip *te2, struct iidNet *trsim, double RENBI_param, struct Metrics_Bip *result) {
	struct Metrics_Bip *RENBI_result = RENBI_Bip(tr1, tr2, te1, te2, trsim, RENBI_param);
	metrics_add_add(result, RENBI_result);
	free_MetricsBip(RENBI_result);
}
