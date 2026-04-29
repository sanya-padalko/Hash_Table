#include "hash_table.h"
#include <ctype.h>
#include <immintrin.h>
#include <valgrind/callgrind.h>

#ifdef DEBUG
#define print_err(...) fprintf(stderr, __VA_ARGS__)
#else
#define print_err(...) 
#endif

typedef long long ll;

void	Insertion	(Table* table)				;
char*	DictReading	(const char*)				;
char**	SkipLetters	(int word_cnt, char* buf)	;

void	FindTesting	(Table* table, char** words, int word_cnt)	;
ll		GetTicks	(Table* table, char* word)					;


int main() {
	gen_crc32_table();
	Table* table = TableCtor(0, get_hash); // read about caches
	Insertion(table);

	char*	text 	 = DictReading("text/eng_words.txt");
	int	 	word_cnt = 2e5;
	char**	words 	 = SkipLetters(word_cnt, text);

	FindTesting(table, words, word_cnt);

	free(text);
	free(words);
	
	//TablePrint(table);
	TableDtor(table);
}

void FindTesting(Table* table, char** words, int word_cnt) {
	ll sum_time = 0;

	fprintf(stderr, "Finding starting...\n");
	CALLGRIND_TOGGLE_COLLECT;

	for (int i = 0; i < word_cnt * 100; ++i) {
		sum_time += GetTicks(table, words[i % word_cnt]);

		if (i % 10000 == 0)	fprintf(stderr, "\r%8d/%d", i, word_cnt * 100);
	}
	
	CALLGRIND_TOGGLE_COLLECT;
	fprintf(stderr, "\nFinding ended.\n");

	FILE* results = fopen("results/table_results.csv", "a");
	fprintf(results, "%d\n", (int)((double)sum_time / (100 * word_cnt)));
	fclose(results);
}

ll GetTicks(Table* table, char* word) {
	ll start_time	 = __rdtsc();
	volatile int res = TableFind(table, word);
	ll end_time		 = __rdtsc();

	return end_time - start_time;
}

char** SkipLetters(int word_cnt, char* buf) {
	int 	ind	  = 0;
	char**  words = (char**)calloc(word_cnt, sizeof(char*));

	while (ind < word_cnt) {
		while (!isalpha(*buf))
			++buf;

		words[ind] = buf;
		++ind;
		
		while (isalpha(*buf))
			++buf;

		*buf = '\0';
		print_err("\r%6d/%6d", ind, word_cnt);
	}
	print_err("\nReading dictionary ended\n\n");

	return words;
}

void Insertion(Table* table) {
	int	  text_len	  = 253250;
	FILE* peace_n_war = fopen("save_table.txt", "r");

	char* 	text_file = (char*)calloc(text_len + 10, sizeof(char));
	size_t	read_cnt  = fread(text_file, 1, text_len, peace_n_war);
	
	fclose(peace_n_war);

	char word[100];
	int	  sum_len     = 0;
	int	  word_len    = 0;
	char* start_ptr   = text_file;

	print_err("Insertion starting...\n");
	while (sum_len < text_len) {
		sscanf(text_file, " %s%n", word, &word_len);
		TableInsert(table, word);

		text_file += word_len;
		sum_len   += word_len;

		print_err("\r%6d/%6d", sum_len, text_len);
	}
	print_err("\nInsertion ended\n");

	free(start_ptr);
	fprintf(stderr, "Different words: %d\n\n", table->cnt);
}

char* DictReading(const char* file_name) {
	print_err("Reading dictionary starting\n");

	int 	file_len  = (int)(2.4e6);
	FILE*	dict_file = fopen(file_name, "r");
	char*	buf 	  = (char*)calloc(file_len + 10, sizeof(char));
	size_t	read_cnt  = fread(buf, 1, file_len, dict_file);
	fclose(dict_file);

	return buf;
}