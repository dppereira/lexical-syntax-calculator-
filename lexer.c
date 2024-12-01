#include <ctype.h>
#include <lexer.h>
#include <stdio.h>

double lexval;
int linenum = 1;
char lexeme[MAXIDLEN + 1];

// ASGN = [:=]
int isASGN(FILE *tape) {
  lexeme[0] = getc(tape);
  if (lexeme[0] == ':') {
    lexeme[1] = getc(tape);
    if (lexeme[1] == '=') {
      lexeme[2] = 0;
      return ASGN;
    }
    ungetc(lexeme[1], tape);
  }
  ungetc(lexeme[0], tape);
  lexeme[0] = 0;
  return 0;
}

// ID = [A-Za-z][A-Za-z0-9]*
// Ex: abacaXI123
int isID(FILE *tape) {
  int i = 0;
  lexeme[i] = getc(tape);

  if (isalpha(lexeme[i])) {
    i++;
    while (isalnum(lexeme[i] = getc(tape)))
      i++;

    ungetc(lexeme[i], tape);
    lexeme[i] = 0;
    return ID;
  }

  ungetc(lexeme[i], tape);
  lexeme[i] = 0;
  return 0;
}

// OCT = 0[0-7]+
// 01 02 03 04 011
int isOCT(FILE *tape){
	int i = 0;
	lexeme[i] = getc(tape);

	if (lexeme[i] == '0'){
		i++;
		while (isdigit(lexeme[i] = getc(tape))){
			if (lexeme[i] < '0' || lexeme[i] > '7'){
				ungetc(lexeme[i], tape);
				lexeme[i] = 0;
				return 0;
			}
			i++;
		};

		ungetc(lexeme[i], tape);
		lexeme[i] = 0;
		return OCT;
	}

	ungetc(lexeme[i], tape);
	lexeme[i] = 0;
	return 0;
}

// HEX = 0[xX][0-9a-fA-F]+
// 0X1 0X2 0x3 0x4 0x11
int isHEX(FILE *tape){
	int i = 0;
 	lexeme[i] = getc(tape);

 	if (lexeme[i] == '0') {
		i++;
 		if (!isspace(lexeme[i] = getc(tape))){
 			if (lexeme[i] == 'x' || lexeme[i] == 'X'){
				i++;
 				while(isxdigit(lexeme[i] = getc(tape))) i++;

				ungetc(lexeme[i], tape);
				lexeme[i] = 0;
				return HEX;
 			}
			ungetc(lexeme[i], tape);
 		}
		i--;
 	}

 	ungetc(lexeme[i], tape);
	lexeme[i] = 0;
 	return 0;
}

int isNUM(FILE *tape) {
  int i = 0, j = 0;
  lexeme[i] = getc(tape);

  // Adiciona '0' se começar com '.'
  if (lexeme[i] == '.') {
    lexeme[i] = '0';
    i++;
    lexeme[i] = '.';
  }

  // Verifica digito
  if (isdigit(lexeme[0])) {
    i++;

    while (isdigit(lexeme[i] = getc(tape))) {
      i++;
    }

    // Verifica Float
    if (lexeme[i] == '.') {
      i++;
      j = i;
      while (isdigit(lexeme[i] = getc(tape))) {
        i++;
      }

      if (j == i) {
        ungetc(lexeme[i], tape);
        lexeme[i] = 0;
        return 0;
      }
    }

    // Verifica Exponencial
    if (lexeme[i] == 'e' || lexeme[i] == 'E') {
      i++;
      lexeme[i] = getc(tape);

      // Verifica sinal
      if (lexeme[i] == '+' || lexeme[i] == '-') {
        i++;
      } else {
        ungetc(lexeme[i], tape);
      }

      j = i;
      while (isdigit(lexeme[i] = getc(tape))) {
        i++;
      }

      if ((j == i || (!isspace(lexeme[i]) && lexeme[i] != '\n')) &&
          lexeme[i] != '+' && lexeme[i] != '-' && lexeme[i] != '*' &&
          lexeme[i] != '/' && lexeme[i] != ')') {
        ungetc(lexeme[i], tape);
        lexeme[i] = 0;
        return 0;
      }
    }
    ungetc(lexeme[i], tape);
    lexeme[i] = 0;
    return NUM;
  }
  ungetc(lexeme[i], tape);
  lexeme[i] = 0;
  return 0;
}

void skipspaces(FILE *tape) {
  int head, prev_head = EOF;

  while (isspace(head = getc(tape))) {
    if (head == '\n') {
      linenum++;
      if (tape == stdin)
        break;
    }
    if (head == prev_head) {
      break;
    }
    prev_head = head;
  }

  if (head != EOF) {
    ungetc(head, tape);
  }
}

int gettoken(FILE *source) {
  int token;

  skipspaces(source);

  if ((token = isASGN(source)))
    return token;
  if ((token = isID(source)))
    return token;
  if ((token = isOCT(source)))
    return 0;
  if ((token = isHEX(source)))
    return 0;
  if ((token = isNUM(source)))
    return token;

  token = getc(source);

  return token;
}