#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char *output_name = NULL;
FILE *output = NULL;
char *source = NULL;
int pos = 0;
int line = 1;
int test_flag = 0;
char *token = NULL;

void skip_whitespace(void)
{
	while (source[pos] == '\x20' || source[pos] == '\t' ||
			source[pos] == '\r' || source[pos] == '\n') {
		// increment line counter when new line reached
		if (source[pos] == '\n') {
			line++;
		}
		pos++;
	}
}

void make_token(int start_pos)
{
	int length = pos - start_pos;
	free(token);
	token = malloc(length + 1);
	token[length] = '\0';
	memcpy(token, &source[start_pos], length);
}

// emits the currently recognized token
void emit_token(void)
{
	int i;
	// strings are converted to C format
	if (token[0] == '\'') {
		fprintf(output, "\"");
		for (i = 1; token[i] != '\0' && token[i] != '\''; i++) {
			switch (token[i]) {
			case '\"':
				fprintf(output, "\\\"");
				break;
			case '\\':
				fprintf(output, "\\\\");
				break;
			default:
				fprintf(output, "%c", token[i]);
				break;
			}
		}
		fprintf(output, "\"");
		return;
	}
	// if token is not a string, emit as-is
	fprintf(output, "%s", token);
}

void emit(const char *str)
{
	fprintf(output, "%s", str);
}

void emit_nl(void)
{
	fprintf(output, "\n");
}

void read_literal(const char *literal)
{
	int entry_pos;
	int i;

	skip_whitespace();
	// compare source with the literal
	entry_pos = pos;
	i = 0;
	while (source[pos] != '\0' && literal[i] != '\0' &&
			source[pos] == literal[i]) {
		pos++;
		i++;
	}
	// if the end of the literal has been reached, comparison successful
	if (literal[i] == '\0') {
		test_flag = 1;
		make_token(entry_pos);
	} else {
		// reset position
		pos = entry_pos;
		test_flag = 0;
	}
}

void read_id(void)
{
	int entry_pos;

	skip_whitespace();
	// recognize initial alphabetic character
	entry_pos = pos;
	if (('A' <= source[pos] && source[pos] <= 'Z') ||
			('a' <= source[pos] && source[pos] <= 'z')) {
		pos++;
		test_flag = 1;
	} else {
		test_flag = 0;
		return;
	}
	// recognize alphanumeric characters
	while (('A' <= source[pos] && source[pos] <= 'Z') ||
			('a' <= source[pos] && source[pos] <= 'z') ||
			('0' <= source[pos] && source[pos] <= '9')) {
		pos++;
	}
	// recognition successful, copy into token
	make_token(entry_pos);
}

void read_number(void)
{
	int entry_pos;

	skip_whitespace();
	// recognize optional negative sign
	entry_pos = pos;
	if (source[pos] == '-') {
		pos++;
	}
	// recognize initial numeric character
	if ('0' <= source[pos] && source[pos] <= '9') {
		pos++;
		test_flag = 1;
	} else {
		test_flag = 0;
		return;
	}
	// recognize subsequent numeric characters
	while ('0' <= source[pos] && source[pos] <= '9') {
		pos++;
	}
	// recognition successful, copy into token
	make_token(entry_pos);
}

void read_string(void)
{
	int entry_pos;

	skip_whitespace();
	// recognize initial single quote
	entry_pos = pos;
	if (source[pos] == '\'') {
		pos++;
		// test_flag is not set as recognition can still fail
	} else {
		test_flag = 0;
		return;
	}

	// recognize contents
	while (source[pos] != '\0' && source[pos] != '\'') {
		// increment line counter when new line reached
		if (source[pos] == '\n') {
			line++;
		}
		pos++;
	}

	// recognize final single quote
	if (source[pos] == '\'') {
		pos++;
		test_flag = 1;
		make_token(entry_pos);
	} else if (source[pos] == '\0') {
		// reset position
		pos = entry_pos;
		test_flag = 0;
	}
}

void error_if_false(void)
{
	if (!test_flag) {
		fprintf(stderr, "error in line %i at token %s\n", line, token);
		fclose(output);
		// delete the output file
		remove(output_name);
		free(source);
		free(token);
		exit(1);
	}
}

void meta_program(void);
void meta_exp1(void);

int main(int argc, char *argv[])
{
	FILE *input;
	int length;

	if (argc < 3) {
		fprintf(stderr, "usage: meta <input> <output>\n");
		exit(1);
	}

	// open input and output
	input = fopen(argv[1], "r");
	if (input == NULL) {
		fprintf(stderr, "invalid input file\n");
		exit(1);
	}
	output_name = argv[2];
	output = fopen(output_name, "w");
	if (output == NULL) {
		fprintf(stderr, "invalid output file\n");
		exit(1);
	}
	// read entire input into source
	fseek(input, 0, SEEK_END);
	length = (int)ftell(input);
	fseek(input, 0, SEEK_SET);
	source = malloc(length + 1);
	fread(source, 1, length, input);
	source[length] = '\0';
	fclose(input);

	// initially we have empty token; token is never NULL
	token = malloc(1);
	token[0] = '\0';

	// run meta
	meta_program();

	fprintf(stderr, "%d %d\n", pos, strlen(source));

	fclose(output);
	free(source);
	free(token);
	return 0;
}

/* end */
