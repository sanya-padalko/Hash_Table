#include "hash_table.h"
#include <immintrin.h>
#include <valgrind/callgrind.h>

void	Insertion	(Table* table)	;
char*	Reading		(const char*)	;

int main() {
	Table* table = TableCtor(0);
	
	Insertion(table);
	char* buf = Reading("text/eng_words.txt");
	int word_cnt = 2e5;

	int real_cnt = 0;
	char** word = (char**)calloc(word_cnt, sizeof(char*));
	char* start_buf = buf;

	while (real_cnt < word_cnt) {
		while (*buf < 'a' || *buf > 'z')
			++buf;

		word[real_cnt] = buf;
		++real_cnt;
		
		while ('a' <= *buf && *buf <= 'z')
			++buf;

		*buf = '\0';
		printf("\r%6d/%6d", real_cnt, word_cnt);
	}
	printf("\nReal count: %d\n", real_cnt);

	long long sum_time = 0;
	CALLGRIND_TOGGLE_COLLECT;
	for (int i = 0; i < word_cnt * 100; ++i) {
		long long start_time = __rdtsc();
		volatile int res = TableFind(table, word[i % word_cnt]);
		long long end_time = __rdtsc();

		sum_time += (end_time - start_time);

		if (i % 10000 == 0) {
			fprintf(stderr, "\r%10d", i);
		}
	}
	CALLGRIND_TOGGLE_COLLECT;

	printf("\n");
	printf("Sum: %-12lld \nAvg: %lg\n", sum_time, (double)sum_time / word_cnt);
	free(word);
	free(start_buf);
	
	//TablePrint(table);
	TableDtor(table);
}

void Insertion(Table* table) {
	int text_len = 253250;
	FILE* peace_n_war = fopen("table_view.txt", "r");
	char* text_file = (char*)calloc(text_len + 10, sizeof(char));
	if (fread(text_file, 1, text_len, peace_n_war) != text_len)
		printf("Real file size < %d\n", text_len);

	fclose(peace_n_war);

	char word[100] = {};
	int sum_len = 0, word_len = 0;
	char* start_ptr = text_file;
	while (sum_len < text_len) {
		sscanf(text_file, " %s%n", word, &word_len);
		TableInsert(table, word);

		text_file += word_len;
		sum_len   += word_len;

		fprintf(stderr, "\r%6d/%6d", sum_len, text_len);
	}
	printf("\n");

	free(start_ptr);
	fprintf(stderr, "Peace and war readed\n");
	fprintf(stderr, "Different words: %d\n", table->cnt);
}

char* Reading(const char* file_name) {
	fprintf(stderr, "Reading starting\n");
	int file_len = (int)(2.4e6);
	FILE* dict_file = fopen(file_name, "r");
	char* buf = (char*)calloc(file_len + 10, sizeof(char));
	if (fread(buf, 1, file_len, dict_file) != file_len)
		printf("Real file size < %d\n", file_len);
	fclose(dict_file);

	fprintf(stderr, "Reading ended\n");
	return buf;
}