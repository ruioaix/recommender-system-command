#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <string.h>

#include "linefile.h"
//#include "bip.h"

struct OptionArgs {
	int calculate_mass;
	int calculate_heat;
	int calculate_hybrid;
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
	OptionArgs.total_filename = NULL;
	OptionArgs.train_filename = NULL;
	OptionArgs.test_filename = NULL;
	OptionArgs.dataset_divide_rate = 0.1;
	static const char *optString = "mhHi:T:t:?";

	struct option longOpts[] = {
		{"mass", no_argument, NULL, 'm'},
		{"heat", no_argument, NULL, 'h'},
		{"hybrid", no_argument, NULL, 'H'},
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
				printf("m\n");
				break;
			case 'h':
				OptionArgs.calculate_heat = 1;
				printf("h\n");
				break;
			case 'H':
				OptionArgs.calculate_hybrid = 1;
				printf("H\n");
				break;
			case 'i':
				OptionArgs.total_filename = optarg;
				printf("dataset: %s\n", optarg);
				break;
			case 'T':
				OptionArgs.train_filename = optarg;
				printf("trainset: %s\n", optarg);
				break;
			case 't':
				OptionArgs.test_filename = optarg;
				printf("testset: %s\n", optarg);
				break;
			case 'd':
				OptionArgs.dataset_divide_rate = 0.1;
				printf("d\n");
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
	printf("%d\n", OptionArgs.random);

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

static void do_work(struct OptionArgs *oa) {
	struct LineFile *lf = create_LineFile(oa->total_filename, 1, 1, -1);
	free_LineFile(lf);
	/*
	struct Bip *ds1, *tr1, *te1;
	struct Bip *ds2, *tr2, *te2;
	if (oa->total_filename != NULL) {
		struct LineFile *lf = create_LineFile(oa->total_filename, 1, 1, -1);
		ds1 = create_Bip(lf, 1);
		ds2 = create_Bip(lf, 2);
		struct LineFile *smlp, *bigp;
		divide_Bip(ds1, ds2, oa->dataset_divide_rate, &smlp, &bigp);
		tr1 = create_Bip(smlp, 1);
		tr2 = create_Bip(smlp, 2);
		te1 = create_Bip(bigp, 1);
		te2 = create_Bip(bigp, 2);
		free_LineFile(lf);
		free_LineFile(smlp);
		free_LineFile(bigp);
	}
	else {
		struct LineFile *trlf = create_LineFile(oa->train_filename, 1, 1, -1);
		struct LineFile *telf = create_LineFile(oa->test_filename, 1, 1, -1);
		struct LineFile *lf = add_LineFile(trlf, telf);
		ds1 = create_Bip(lf, 1);
		ds2 = create_Bip(lf, 2);
		tr1 = create_Bip(trlf, 1);
		tr2 = create_Bip(trlf, 2);
		te1 = create_Bip(telf, 1);
		te2 = create_Bip(telf, 2);
		free_LineFile(trlf);
		free_LineFile(telf);
		free_LineFile(lf);
	}
	free_Bip(ds1);
	free_Bip(ds2);
	free_Bip(tr1);
	free_Bip(tr2);
	free_Bip(te1);
	free_Bip(te2);
	*/
}
