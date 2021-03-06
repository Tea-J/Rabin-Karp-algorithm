//Rabin-Karp algorithm in C

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <errno.h>

#define N 80

int detectedHash = 0;
int correctHash = 0;


int Ascii(char c){
	return c;
}

int Check(char *A, char *B, int lenght){
	int i;
	for (i = 0; i < lenght; ++i){
		if(*(A+i) != *(B+i))
			return 0;	//Strings are not equal
	}
	return 1;	//Strings are equal
}


int main(int argc, char *argv[]){
	FILE *genome; FILE *input; FILE *output;
	int i, j, no_results, lpattern;
	int num_patterns = 10;
	int *result;
	char oldbyte, newbyte;
	char *buffer, *patterns, *string, *size;
	long input_file_size, hsubstring, sub_sum;
	long *hpattern;
	float efficiency;

	time_t start, end;
	double dif;
	
	if (argc != 3){
		fprintf(stderr, "Usage: RabinKarp [genome.fa] [patterns.fa]\n");
		getchar();
		return 0;
	}
	
	genome = fopen(argv[1], "r");	//genome
	input = fopen(argv[2], "r");	//patterns
	output = fopen("output.txt", "w");	//output

	if (input == NULL || genome == NULL || output == NULL){
		fprintf(stderr, "Error opening file.\n");
		getchar();
		return 0;
	}

	start = clock();

	fseek(genome, 0, SEEK_END);
	input_file_size = ftell(genome);
	rewind(genome);

	size = (char *)malloc(N*sizeof(char));
	memset(size, '\0', N*sizeof(char));

	i = 0;
	while ((j = fgetc(input)) != '\n'){
        	if (j == ','){
			fgetc(input);	//Read space
			while ((j = fgetc(input)) != ' '){
				*(size+i) = j;
				i++;
			}
			break;
		}
	}

	rewind(input);
	lpattern = atoi(size);
	
	patterns = (char *)malloc(num_patterns*(lpattern + 1)*sizeof(char));
	buffer = (char *)malloc((lpattern + N)*sizeof(char));
	memset(buffer, '\0', (lpattern + N)*sizeof(char));

	for (i = 0; i < num_patterns; i++){
		fgets(buffer, (lpattern + N)*sizeof(char), input);	//Reads comment line
		memset(buffer, '\0', (lpattern + N)*sizeof(char));
		fgets(buffer, (lpattern + N)*sizeof(char), input);	//Reads pattern
		strncpy(patterns + i*lpattern, buffer, lpattern);	//Saves pattern
	}

	fclose(input);

	end = clock();
	dif = ((double)(end - start)) / CLOCKS_PER_SEC;

	fprintf(output, "Processing input data took %.2lf ms.\n\n", dif * 1000);

	result = (int *)malloc(num_patterns*sizeof(int));
	hpattern = (long *)malloc(num_patterns*sizeof(long));

	hsubstring = 0;
	sub_sum = 0;
	no_results = 1;
	string = (char*)malloc(input_file_size*sizeof(char));
	memset(string, '\0', input_file_size);
	fread(string, sizeof(char), input_file_size, genome);


	start = clock();

	//Calculating hash values for patterns
	for (i = 0; i < num_patterns; ++i) {
		*(result + i) = 0;
		*(hpattern + i) = 0;
		for (j = 0; j < lpattern; ++j){
			*(hpattern + i) += *(patterns + i*lpattern + j) * (lpattern - j);
		}
	}

	//Searchig for patterns in input data
	for (i = 0; i < input_file_size - lpattern + 1; ++i){
		
		//Calculating hash value for a substring of input data
		if (i == 0){
			for (j = 0; j < lpattern; ++j){
				hsubstring += *(string + i + j) * (lpattern - j);
				sub_sum += Ascii(*(string + i + j));
			}
		}
		else{
			newbyte = *(string + i + lpattern - 1);
			sub_sum = sub_sum - Ascii(oldbyte) + Ascii(newbyte);
			hsubstring = hsubstring + sub_sum - lpattern*Ascii(oldbyte);
		}
		oldbyte = *(string + i);

		//Comparing hash value of a substring with hash values of patterns
		//If we find same hash values, we still have compare substring with pattern
		for (j = 0; j < num_patterns; ++j){
			if (hsubstring == *(hpattern + j)){
				detectedHash++;
				*(result + j) = Check(string+i, patterns + j*lpattern, lpattern);
			}

			if (*(result + j)){
				correctHash++;
				fprintf(output, "%d. pattern found at index %d.\n", j, i);
				*(result + j) = 0;

				if (no_results)
					no_results = 0;
			}
		}
	}

	if (no_results)
		fprintf(output, "No pattern was found.\n");

	end = clock();
	dif = ((double)(end - start)) / CLOCKS_PER_SEC;

	if (detectedHash)
		efficiency = ((float)correctHash / detectedHash) * 100;
	else
		efficiency = 0;

	fprintf(output, "\nDetected: %d\nCorrect: %d\nEfficiency: %.2f %%\n", detectedHash, correctHash, efficiency);
	fprintf(output, "Calculations took %.2lf ms.\n", dif * 1000);

	fclose(genome);
	fclose(output);
	free(result);
	free(buffer);
	free(patterns);
	free(hpattern);

	return 0;
}