#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>

struct OptionArgs {
	int calculate_mass;
	int calculate_heat;
	int calculate_hybrid;
	char *total_filename;
	char *train_filename;
	char *test_filename;

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
	puts("-?: help information");
	exit(EXIT_FAILURE);
}

int main(int argc, char **argv) {
	struct OptionArgs OptionArgs;
	OptionArgs.calculate_mass = 0;
	OptionArgs.calculate_heat = 0;
	OptionArgs.calculate_hybrid = 0;
	OptionArgs.total_filename = NULL;
	OptionArgs.train_filename = NULL;
	OptionArgs.test_filename = NULL;
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
				printf("t\n");
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

	return 0;
}
