#include <stdio.h>											//include necessary libraries
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <conio.h>
															//initialize global variables							
//check all parentheses if they're closed; character counter; argument counter for p, q, r; column numbers of q and r; row counter															
int parcheck, parsecount, arg, qvalue, rvalue, row, parstore;
int var = 1;												//rows of truth table
char c[2] = "\0";											//placeholder for scanned character
char prevc = '\0';											//placeholder for previous scanned character
char keyword[250] = "";										//placeholder for keyword to be checked
char statement[250] = "";									//placeholder for compressed logical statement
char scanned[250] = "";										//placeholder for the whole logical statement
//boolean switches whether a keyword is currently entered; also flags to indicate whether to continue evaluating or scanning input; arguments to compare
bool ktrue, kfalse, knot, kor, kand, kequal, kimply, p, q, r, argone, argtwo, cont;
bool kpar = true;

void NotLeftPar (char);										//Declaration to prevent warning

char andf(bool arg1, bool arg2){							//and function
	if(arg1 && arg2)
		return 'T';
	return 'F';
}

char orf(bool arg1, bool arg2){								//or function
	if(arg1 || arg2)
		return 'T';
	return 'F';	
}

char implies(bool arg1, bool arg2){							//implies function
	if(!arg1 || arg2)
		return 'T';
	return 'F';
}

char equivalent(bool arg1, bool arg2){						//equivalent function
	if(arg1 == arg2)
		return 'T';
	return 'F';
}

char notf(bool arg1){										//not function
	if(arg1){
		return 'F';
	}
	return 'T';
}

char execute(char copy[], char output[][200][250], int row){
	parsecount = 0;											//reset counter
	//boolean flags to take note of precedence order e.g. if all NOT operators are finished, switch off and compute AND operators etc.
	kpar = true;
	knot = true;											
	kand = true;
	kor = true;
	kimply = true;
	char replace[250] = "";
	while(copy[1] != ';'){									//continue computing until only one logical constant remains
		switch(copy[parsecount]){
			case '(':										//paragraphed expressions are first in precedence
				parcheck++;
				copy[parsecount++] = '\0';					//clear first (
				for(int x = 0; parcheck > 0; x++){			//store enclosed expression in another string and clear the characters of the original string
					replace[x] = copy[parsecount];
					if(copy[parsecount] == '(')
						parcheck++;
					if(copy[parsecount] == ')'){
						parcheck--;
						if(parcheck == 0){
							replace[x] = '\0';
							break;
						}
					}
					copy[parsecount++] = '\0';
				}
				strcat(replace, ";");						//end the enclosed expression with a semi-colon
				parstore = parsecount;						//store the iterated location to place the result of the enclosed expression
				copy[parstore] = execute(replace, output, row);//self iterate the function to find the result of the enclosed expression
				strcpy(replace, "");						//clear string storage
				knot = true;								//reset precendence flags
				kand = true;
				kor = true;
				kimply = true;
				parsecount = 0;								//prepare for rewriting the new expression after computing the enclosed expression
				for(int x = 0; true; x++){					//skip cleared characters (those set to null)
					while(copy[parsecount] == '\0'){
						parsecount++;
					}
					replace[x] = copy[parsecount];			//copy to new string
					if(copy[parsecount] == ';'){
						break;
					}
					parsecount++;
				}
				strcpy(copy, replace);						//write new expression to the old string and continue evaluating if possible
				parsecount = 0;
				break;
			
			case 'E':										//equivalent takes the last precedence
				if(kimply){									//hence requiring the implies flag to go off first
					parsecount++;
					break;
				}
				switch(copy[parsecount - 1]){				//identify if first argument is true or false
					case 'P':
						if(output[row][0][0] == 'T')
							argone = true;
						else
							argone = false;
						break;
					case 'Q':
						if(output[row][qvalue][0] == 'T')
							argone = true;
						else
							argone = false;
						break;
					case 'R':
						if(output[row][rvalue][0] == 'T')
							argone = true;
						else
							argone = false;
						break;
					case 'T':
						argone = true;
						break;
					case 'F':
						argone = false;
						break;
//in case something goes wrong by unforeseen circumstances, for the programmer to quickly identify the problem
					default:								
						printf("FATAL ERROR WHILE CALLING EQUIVALENT");
						exit(0);
				}
				switch(copy[parsecount + 1]){
					case 'P':
						if(output[row][0][0] == 'T')
							argtwo = true;
						else
							argtwo = false;
						break;
					case 'Q':
						if(output[row][qvalue][0] == 'T')
							argtwo = true;
						else
							argtwo = false;
						break;
					case 'R':
						if(output[row][rvalue][0] == 'T')
							argtwo = true;
						else
							argtwo = false;
						break;
					case 'T':
						argtwo = true;
						break;
					case 'F':
						argtwo = false;
						break;
					default:
						printf("FATAL ERROR WHILE CALLING EQUIVALENT");
						exit(0);
				}
				copy[parsecount - 1] = '\0';				//clear the two variables or constants
				copy[parsecount + 1] = '\0';
				if(equivalent(argone, argtwo) == 'T')		//write the answer onto the connective character e.g. TAT => \0T\0
					copy[parsecount] = 'T';
				else
					copy[parsecount] = 'F';
				parsecount = 0;
				for(int x = 0; true; x++){
					while(copy[parsecount] == '\0'){
						parsecount++;
					}
					replace[x] = copy[parsecount];
					if(copy[parsecount] == ';'){
						break;
					}
					parsecount++;
				}
				strcpy(copy, replace);
				parsecount = 0;
				break;
				
			case 'I':										//implies takes the fifth in precedence
				if(kor){									//hence needs the or flag to be off first
					parsecount++;
					break;
				}
				switch(copy[parsecount - 1]){
					case 'P':
						if(output[row][0][0] == 'T')
							argone = true;
						else
							argone = false;
						break;
					case 'Q':
						if(output[row][qvalue][0] == 'T')
							argone = true;
						else
							argone = false;
						break;
					case 'R':
						if(output[row][rvalue][0] == 'T')
							argone = true;
						else
							argone = false;
						break;
					case 'T':
						argone = true;
						break;
					case 'F':
						argone = false;
						break;
					default:
						printf("FATAL ERROR WHILE CALLING IMPLIES");
						exit(0);
				}
				switch(copy[parsecount + 1]){
					case 'P':
						if(output[row][0][0] == 'T')
							argtwo = true;
						else
							argtwo = false;
						break;
					case 'Q':
						if(output[row][qvalue][0] == 'T')
							argtwo = true;
						else
							argtwo = false;
						break;
					case 'R':
						if(output[row][rvalue][0] == 'T')
							argtwo = true;
						else
							argtwo = false;
						break;
					case 'T':
						argtwo = true;
						break;
					case 'F':
						argtwo = false;
						break;
					default:
						printf("FATAL ERROR WHILE CALLING IMPLIES");
						exit(0);
				}
				copy[parsecount - 1] = '\0';
				copy[parsecount + 1] = '\0';
				if(implies(argone, argtwo) == 'T')
					copy[parsecount] = 'T';
				else
					copy[parsecount] = 'F';
				parsecount = 0;
				for(int x = 0; true; x++){
					while(copy[parsecount] == '\0'){
						parsecount++;
					}
					replace[x] = copy[parsecount];
					if(copy[parsecount + 1] == ';'){
						replace[x + 1] = ';';
						break;
					}
					parsecount++;
				}
				strcpy(copy, replace);
				parsecount = 0;
				break;
					
			case 'O':										//or is the fourth when it comes to precendence
				if(kand){									//hence needs the and operators to be resolved first
					parsecount++;
					break;
				}
				switch(copy[parsecount - 1]){
					case 'P':
						if(output[row][0][0] == 'T')
							argone = true;
						else
							argone = false;
						break;
					case 'Q':
						if(output[row][qvalue][0] == 'T')
							argone = true;
						else
							argone = false;
						break;
					case 'R':
						if(output[row][rvalue][0] == 'T')
							argone = true;
						else
							argone = false;
						break;
					case 'T':
						argone = true;
						break;
					case 'F':
						argone = false;
						break;
					default:
						printf("FATAL ERROR WHILE CALLING OR");
						exit(0);
				}
				switch(copy[parsecount + 1]){
					case 'P':
						if(output[row][0][0] == 'T')
							argtwo = true;
						else
							argtwo = false;
						break;
					case 'Q':
						if(output[row][qvalue][0] == 'T')
							argtwo = true;
						else
							argtwo = false;
						break;
					case 'R':
						if(output[row][rvalue][0] == 'T')
							argtwo = true;
						else
							argtwo = false;
						break;
					case 'T':
						argtwo = true;
						break;
					case 'F':
						argtwo = false;
						break;
					default:
						printf("FATAL ERROR WHILE CALLING OR");
						exit(0);
				}
				copy[parsecount - 1] = '\0';
				copy[parsecount + 1] = '\0';
				if(orf(argone, argtwo) == 'T')
					copy[parsecount] = 'T';
				else
					copy[parsecount] = 'F';
				parsecount = 0;
				for(int x = 0; true; x++){
					while(copy[parsecount] == '\0'){
						parsecount++;
					}
					replace[x] = copy[parsecount];
					if(copy[parsecount] == ';'){
						break;
					}
					parsecount++;
				}
				strcpy(copy, replace);
				parsecount = 0;
				break;
			
			case 'A':										//and is the third operator when it comes to precedence
				if(knot){									//we need to resolve not operators first before computing connective operators
					parsecount++;
					break;
				}
				switch(copy[parsecount - 1]){
					case 'P':
						if(output[row][0][0] == 'T')
							argone = true;
						else
							argone = false;
						break;
					case 'Q':
						if(output[row][qvalue][0] == 'T')
							argone = true;
						else
							argone = false;
						break;
					case 'R':
						if(output[row][rvalue][0] == 'T')
							argone = true;
						else
							argone = false;
						break;
					case 'T':
						argone = true;
						break;
					case 'F':
						argone = false;
						break;
					default:
						printf("FATAL ERROR WHILE CALLING AND");
						exit(0);
				}
				switch(copy[parsecount + 1]){
					case 'P':
						if(output[row][0][0] == 'T')
							argtwo = true;
						else
							argtwo = false;
						break;
					case 'Q':
						if(output[row][qvalue][0] == 'T')
							argtwo = true;
						else
							argtwo = false;
						break;
					case 'R':
						if(output[row][rvalue][0] == 'T')
							argtwo = true;
						else
							argtwo = false;
						break;
					case 'T':
						argtwo = true;
						break;
					case 'F':
						argtwo = false;
						break;
					default:
						printf("FATAL ERROR WHILE CALLING AND");
						exit(0);
				}
				copy[parsecount - 1] = '\0';
				copy[parsecount + 1] = '\0';
				if(andf(argone, argtwo) == 'T')
					copy[parsecount] = 'T';
				else
					copy[parsecount] = 'F';
				parsecount = 0;
				for(int x = 0; true; x++){
					while(copy[parsecount] == '\0'){
						parsecount++;
					}
					replace[x] = copy[parsecount];
					if(copy[parsecount] == ';'){
						break;
					}
					parsecount++;
				}
				strcpy(copy, replace);
				parsecount = 0;
				break;
					
			case 'N':										//not takes the second spot in precedence
				if(kpar){									//hence needs the computation of the enclosed expressions first
					parsecount++;
					break;
				}
				switch(copy[parsecount + 1]){
					case 'P':
						if(output[row][0][0] == 'T')
							argtwo = true;
						else
							argtwo = false;
						break;
					case 'Q':
						if(output[row][qvalue][0] == 'T')
							argtwo = true;
						else
							argtwo = false;
						break;
					case 'R':
						if(output[row][rvalue][0] == 'T')
							argtwo = true;
						else
							argtwo = false;
						break;
					case 'T':
						argtwo = true;
						break;
					case 'F':
						argtwo = false;
						break;
					case 'N':
						cont = true;
						break;
					default:
						printf("FATAL ERROR WHILE CALLING NOT");
						exit(0);
				}
				if(cont){
					cont = false;
					parsecount++;
					break;
				}
				copy[parsecount + 1] = '\0';
				if(notf(argtwo) == 'T')
					copy[parsecount] = 'T';
				else
					copy[parsecount] = 'F';
				parsecount = 0;
				for(int x = 0; true; x++){
					while(copy[parsecount] == '\0'){
						parsecount++;
					}
					replace[x] = copy[parsecount];
					if(copy[parsecount] == ';'){
						break;
					}
					parsecount++;
				}
				strcpy(copy, replace);
				parsecount = 0;
				break;
//if the end of the statement is reached put off the flags accordingly, indicating all of this kind of operator has been resolved
			case ';':										
				if(kpar)
					kpar = false;
				else {
					if(knot)
						knot = false;
					else {
						if(kand)
							kand = false;
						else {
							if(kor)
								kor = false;
							else {
								if(kimply)
									kimply = false;
							}
						}
					}
				}
				parsecount = 0;
				break;
			
			default:										//skip atomic characters
				parsecount++;
				break;	
			
		}
	}
	kpar = true;											//reset flags in preparation for the next statement
	knot = false;
	kor = false;
	kand = false;
	kimply = false;
	switch(copy[0]){										//check if final result is true or false; return result as character
		case 'T':
			return 'T';
		
		case 'P':
			if(output[row][0][0] == 'T')
				return 'T';
		
		case 'Q':
			if(output[row][qvalue][0] == 'T')
				return 'T';
		
		case 'R':
			if(output[row][0][0] == 'T')
				return 'T';
	}
	return 'F';
}

void evaluate(){
	int row = 1;
	char copy[250];
	strcpy(copy, statement);								//create a copy for logical evaluation
	arg = 0;
	char output[var + 1][200][250];							//array to be used for output
	if(p)
		strcpy(output[0][arg++], "P");						//enter used logical variables
	if(q){
		qvalue = arg;										//store column number if used
		strcpy(output[0][arg++], "Q");
	}
	if(r){
		rvalue = arg;
		strcpy(output[0][arg++], "R");
	}
	for(int x = arg; x > 0; x--){
		if(x == arg){		
			for(int y = 1; y < var + 1; y += 2){			//place their possible truth value combinations
				strcpy(output[y][x - 1], "True");
				strcpy(output[y + 1][x - 1], "False");
			}
		}
		if(x == arg - 1){
			for(int y = 1; y < var + 1; y += 4){
				strcpy(output[y][x - 1], "True");
				strcpy(output[y + 1][x - 1], "True");
				strcpy(output[y + 2][x - 1], "False");
				strcpy(output[y + 3][x - 1], "False");
			}
		}
		if(x == arg - 2){
			strcpy(output[1][0], "True");
			strcpy(output[2][0], "True");
			strcpy(output[3][0], "True");
			strcpy(output[4][0], "True");
			strcpy(output[5][0], "False");
			strcpy(output[6][0], "False");
			strcpy(output[7][0], "False");
			strcpy(output[8][0], "False");
		}
	}
	strcpy(output[0][arg], statement);
	for(int x = 1; x < var + 1; x++){
		if(execute(copy, output, row) == 'T')
			strcat(output[x][arg], "True");
		else
			strcat(output[x][arg], "False");
		strcpy(copy, statement);
		row++;
	}
	for(int x = 0; x < var + 1; x++){						//print the table
		printf("\n");
		for(int y = 0; y < arg + 1; y++){
			printf("%s\t", output[x][y]);
		}
	}
	printf("\n");
	p = false;												//reset values in preparation for the next logical statement
	q = false;
	r = false;
	for(int x = 0; x < var + 1; x++){
		for(int y = 0; y < 200; y++){
			strcpy(output[x][y], "");
		}
	}
	var = 1;
	strcpy(keyword, "");
}

void ConnectiveSemicolon(char next){
	switch(next){
		case 'A':
		case 'E':
		case 'I':
		case 'O':
			NotLeftPar(statement[++parsecount]);			//check if next input is [Atomic] or [NOT Atomic]
			
		case ';':											//otherwise check if input is [;]
		case '\0':
			break;
		
		default:											//if no condition is met, print a syntax error
			printf("Syntax Error occured 1! Scanned line containing error\n%s", statement);
			break;
	}
}

void RightPar(char next){									
	switch(next){
		case ')':
			RightPar(statement[++parsecount]);				//check if next input is [)]
			break;
		
		default:
			ConnectiveSemicolon(next);						//otherwise check if input is [Connective] or [;]
			break;
	}
}

void Atomic(char next){
	switch(next){
		case 'F':
		case 'P':
		case 'Q':
		case 'R':
		case 'T':
			RightPar(statement[++parsecount]);				//check if next is [Connective] or [)]
			break;
			
		default:											//otherwise print a syntax error
			printf("Syntax Error occured! Scanned line containing error\n%s", scanned);
	}
}

void NotLeftPar(char next){
	switch(next){
		case 'N':
		case '(':
			NotLeftPar(statement[++parsecount]);			//check if next input is another [NOT] or [(] token
			break;
		
		default:
			Atomic(next);									//otherwise check if input is [Atomic]
			break;
	}
}

void parse(char statement[]){								//parser; by this time, input has technically been parsed but to undergo another parsing
	if(p){													//if variables are used, multiply rows needed
		var *= 2;
	}
	if(q){
		var *= 2;
	}
	if(r){
		var *= 2;
	}
	printf("\nLEXEME\t\tTOKEN\n---------------------------\n");//print lexemes used
	for(int i = 0; statement[i] != '\0'; i++){
		switch(statement[i]){
			case 'A':
				printf("AND\t\tCONNECTIVE\n");
				break;
			
			case 'E':
				printf("EQUIVALENT\tCONNECTIVE\n");
				break;
			
			case 'F':
				printf("FALSE\t\tATOMIC_CON\n");
				break;
			
			case 'I':
				printf("IMPLIES\t\tCONNECTIVE\n");
				break;
			
			case 'N':
				printf("NOT\t\tNOT\n");
				break;
			
			case 'O':
				printf("OR\t\tCONNECTIVE\n");
				break;
			
			case 'P':
				printf("P\t\tATOMIC_VAR\n");
				break;
			
			case 'Q':
				printf("Q\t\tATOMIC_VAR\n");
				break;
			
			case 'R':
				printf("R\t\tATOMIC_VAR\n");
				break;
				
			case 'T':
				printf("TRUE\t\tATOMIC_CON\n");
				break;
				
			case ';':
				printf(";\t\tSEMI-COLON\n\n");
				break;
			
			case '(':
				printf("(\t\tLEFT_PAR\n");
				break;
				
			case ')':
				printf(")\t\tRIGHT_PAR\n");
				break;
				
		}
	}
	NotLeftPar(statement[0]);
	evaluate();
}

void uppercase(){
	for (int i = 0; keyword[i] != '\0'; i++){
		if(keyword[i] >= 'a' && keyword[i] <= 'z'){
			keyword[i] = keyword[i] - 32;
		}
	}
}

void scanner(FILE* fp){										//scanner
	while(c[0] != EOF){										//if end of file is not reached conduct a check
		c[0] = getc(fp);
		if(c[1] != '\0')
			c[1] = '\0';
		strcat(scanned, c);
		switch(c[0]){
			case 'a':										//the letter A checks for And, fAlse, and equivAlent
			case 'A':
				if(!kand && prevc != '\0'){
					if(!kequal){
						if(!kfalse){						//the user will probably input the AND keyword
							kand = true;					//switch AND checker on
							strcat(keyword, "A");
							prevc = 'A';					//log current as previous in preparation of the next keyword
							break;
						}
						else if(prevc == 'F'){				//if FALSE checker is on
							strcat(keyword, "A");
							prevc = 'A';
							break;
						}
					}
					else if(prevc == 'V'){					//if EQUIVALENT checker is on
						strcat(keyword, "A");
						prevc = 'A';
						break;
					}
				}
				while(c[0] != ' ' && c[0] != EOF && c[0] != ';'){//if scanner fails to predict a keyword then print an error
					strcat(keyword, c);
					c[0] = getc(fp);
				}
				uppercase();
				if(prevc == '\0' && keyword[0] == 'A' && keyword[1] == 'N' && keyword[2] == 'D')//if an invalid keyword precedes the AND keyword
					printf("Error: Invalid sequence of keywords (no logical constant/statement/variable before AND)");
				else
					printf("Error: No such keyword exists! (%s)", keyword);//if the keyword isn't registered
				printf("\n\nScanned statement containing error:\n%s", scanned);
				exit(0);
				
			case 'd':
			case 'D':										//the letter D checks for anD
				if(prevc == 'N'){							//if AND checker is on; AND is successfully added to the statement to be parsed
					strcpy(keyword, "");
					prevc = '\0';
					strcat(statement, "A");
					kand = false;
					break;
				}
				while(c[0] != ' ' && c[0] != EOF && c[0] != ';'){//invalid since there is no other keyword with the letter D
					strcat(keyword, c);
					c[0] = getc(fp);
				}
				uppercase();
				printf("Error: No such keyword exists! (%s)", keyword);
				printf("\n\nScanned statement containing error:\n%s", scanned);
				exit(0);
				
			case 'e':
			case 'E':										//the letter E checks for EquivalEnt, impliEs, truE, and falsE
				if(!kequal && prevc != '\0'){				
					if(!kimply){
						if(!ktrue){
							if(!kfalse){					//the user will probably input the EQUIVALENT keyword
								kequal = true;
								strcat(keyword, "E");
								prevc = 'E';
								break;
							}
							else if(prevc == 'S'){			//if FALSE checker is on; FALSE is successfully added to the statement to be parsed
								strcpy(keyword, "");
								prevc = ' ';
								strcat(statement, "F");
								kfalse = false;
								break;
							}
						}
						else if(prevc == 'U'){				//if TRUE checker is on; TRUE is successfully added to the statement to be parsed
							strcpy(keyword, "");
							prevc = ' ';
							strcat(statement, "T");
							ktrue = false;
							break;
						}
					}
					else if(prevc == 'I' && keyword[2] != '\0'){//if IMPLIES checker is on; strictly check if keyword is currently I or IMPLI
						strcat(keyword, "E");
						prevc = 'E';
						break;
					}
				}
				else if(prevc == 'L'){						//if EQUIVALENT checker is on
					strcat(keyword, "E");
					prevc = 'E';
					break;
				}
				printf("\n\nScanned statement containing error:\n%s", scanned);
				while(c[0] != ' ' && c[0] != EOF && c[0] != ';'){//if scanner fails to predict a keyword then print an error
					strcat(keyword, c);
					c[0] = getc(fp);
				}
				uppercase();
				//if an invalid keyword precedes the EQUIVALENT keyword
				if(prevc == '\0' && keyword[0] == 'E' && keyword[1] == 'Q' && keyword[2] == 'U' && keyword[3] == 'I' && keyword[4] == 'V' && keyword[5] == 'A' && keyword[6] == 'L' && keyword[7] == 'E' && keyword[8] == 'N' && keyword[9] == 'T')
					printf("\n\nError: Invalid sequence of keywords (no logical constant/statement/variable before EQUIVALENT)");
				else
					printf("\n\nError: No such keyword exists! (%s)", keyword);//if the keyword isn't registered
				exit(0);
			
			case 'f':
			case 'F':										//the letter F checks for False
				if(!kfalse && prevc == '\0'){				//the user will probably input the FALSE keyword
					kfalse = true;
					strcat(keyword, "F");
					prevc = 'F';
					break;
				}
				while(c[0] != ' ' && c[0] != EOF && c[0] != ';'){//invalid since there is no other keyword with the letter F
					strcat(keyword, c);
					c[0] = getc(fp);
				}
				uppercase();
				//if an invalid keyword precedes the FALSE keyword
				if(prevc == ' ' && keyword[0] == 'F' && keyword[1] == 'A' && keyword[2] == 'L' && keyword[3] == 'S' && keyword[4] == 'E')
					printf("Error: Invalid sequence of keywords (unexpected constant/variable before FALSE)");
				else
					printf("Error: No such keyword exists! (%s)", keyword);//if the keyword isn't registered
				printf("\n\nScanned statement containing error:\n%s", scanned);
				exit(0);
			
			case 'i':
			case 'I':										//the letter I checks for ImplIes and equIvalent
				if(!kimply && prevc != '\0'){
					if(!kequal){							//the user will probably input the IMPLIES keyword
						kimply = true;
						strcat(keyword, "I");
						prevc = 'I';
						break;
					}
					else if(prevc == 'U'){					//if EQUIVALENT checker is on
						strcat(keyword, "I");
						prevc = 'I';
						break;
					}
				}
				else if(prevc == 'L'){						//if IMPLIES checker is on
					strcat(keyword, "I");
					prevc = 'I';
					break;	
				}
				printf("Scanned statement containing error:\n%s", scanned);
				while(c[0] != ' ' && c[0] != EOF && c[0] != ';'){//if scanner fails to predict a keyword then print an error
					strcat(keyword, c);
					c[0] = getc(fp);
				}
				uppercase();
				//if an invalid keyword precedes the IMPLIES keyword
				if(prevc == '\0' && keyword[0] == 'I' && keyword[1] == 'M' && keyword[2] == 'P' && keyword[3] == 'L' && keyword[4] == 'I' && keyword[5] == 'E' && keyword[6] == 'S')
					printf("\n\nError: Invalid sequence of keywords (no logical constant/statement/variable before IMPLIES)");
				else
					printf("\n\nError: No such keyword exists! (%s)", keyword);//if the keyword isn't registered
				exit(0);
			
			case 'l':
			case 'L':										//the letter L checks for impLies, equivaLent, and faLse
				if(kimply && prevc == 'P'){					//if IMPLIES checker is on
					strcat(keyword, "L");
					prevc = 'L';
					break;
				}
				else if(prevc == 'A' && (kequal || kfalse)){//if FALSE or EQUIVALENT checker is on
					strcat(keyword, "L");
					prevc = 'L';
					break;
				}
				while(c[0] != ' ' && c[0] != 'L' && c[0] != ';'){//if scanner fails to predict a keyword then print an error
					strcat(keyword, c);
					c[0] = getc(fp);
				}
				uppercase();
				printf("Error: No such keyword exists! (%s)", keyword);
				printf("\n\nScanned statement containing error:\n%s", scanned);
				exit(0);
			
			case 'm':
			case 'M':										//the letter M checks for iMplies
				if(kimply && prevc == 'I' && keyword[1] == '\0'){//if IMPLIES checker is on; strictly check if keyword is currently I or IMPLI
					strcat(keyword, "M");
					prevc = 'M';
					break;
				}
				while(c[0] != ' ' && c[0] != EOF && c[0] != ';'){//invalid since there is no other keyword with the letter M
					strcat(keyword, c);
					c[0] = getc(fp);
				}
				uppercase();
				printf("Error: No such keyword exists! (%s)", keyword);//if the keyword isn't registered
				printf("\n\nScanned statement containing error:\n%s", scanned);
				exit(0);
				
			case 'n':
			case 'N':										//the letter N checks for aNd and equivaleNt
				if(kand && prevc == 'A'){					//if AND checker is on
					strcat(keyword, "N");
					prevc = 'N';
					break;
				}
				else if(kequal && prevc == 'E' && keyword[2] != '\0'){//if EQUIVALENT checker is on
					strcat(keyword, "N");
					prevc = 'N';
					break;
				}
				else if(!knot && prevc == '\0'){			//the user will probably input the NOT keyword
					strcat(keyword, "N");
					prevc = 'N';
					knot = true;
					break;
				}
				while(c[0] != ' ' && c[0] != EOF && c[0] != ';'){//if scanner fails to predict a keyword then print an error
					strcat(keyword, c);
					c[0] = getc(fp);
				}
				uppercase();
				if(prevc == ' ' && keyword[0] == 'N' && keyword[1] == 'O' && keyword[2] == 'T')//if an invalid keyword precedes the NOT keyword
					printf("Error: Invalid sequence of keywords (unexpected constant/variable before NOT)");
				else
					printf("Error: No such keyword exists! (%s)", keyword);//if the keyword isn't registered
				printf("\n\nScanned statement containing error:\n%s", scanned);
				exit(0);
			
			case 'o':
			case 'O':										//the letter O checks for Or
				if(!kor && prevc != '\0' && prevc != 'N'){	//the user will probably input the OR keyword
					kor = true;
					strcat(keyword, "O");
					prevc = 'O';
					break;
				}
				else if(knot && prevc == 'N'){				//if NOT checker is on
					strcat(keyword, "O");
					prevc = 'O';
					break;
				}
				while(c[0] != ' ' && c[0] != EOF && c[0] != ';'){//if scanner fails to predict a keyword then print an error
					strcat(keyword, c);
					c[0] = getc(fp);
				}
				uppercase();
				if(prevc == '\0' && keyword[0] == 'O' && keyword[1] == 'R')//if an invalid keyword precedes the OR keyword
					printf("Error: Invalid sequence of keywords (no logical constant/statement/variable before OR)");
				else
					printf("Error: No such keyword exists! (%s)", keyword);//if the keyword isn't registered
				printf("\n\nScanned statement containing error:\n%s", scanned);
				exit(0);
			
			case 'p':
			case 'P':										//the letter P checks for the variable P and imPlies
				//strictly check if IMPLIES checker is off; if off, P is successfully added to the statement to be parsed
				if(prevc == '\0' && !kimply){				
					strcat(statement, "P");
					prevc = ' ';
					p = true;
					break;
				}
				else if(kimply && prevc == 'M'){			//if IMPLIES checker is on
					strcat(keyword, "P");
					prevc = 'P';
					break;
				}
				while(c[0] != ' ' && c[0] != EOF && c[0] != ';'){//if scanner fails to predict a keyword then print an error
					strcat(keyword, c);
					c[0] = getc(fp);
				}
				uppercase();
				if(prevc == ' ' && keyword[0] == 'P')//if an invalid keyword precedes the variable P
					printf("Error: Invalid sequence of keywords (unexpected constant/variable before P)");
				else
					printf("Error: No such keyword exists! (%s)", keyword);//if the keyword isn't registered
				printf("\n\nScanned statement containing error:\n%s", scanned);
				exit(0);
				
			case 'q':
			case 'Q':										//the letter Q checks for the variable Q and eQuivalent
				//strictly check if EQUIVALENT checker is off; if off, Q is successfully added to the statement to be parsed
				if(prevc == '\0' && !kequal){				
					strcat(statement, "Q");
					prevc = ' ';
					q = true;
					break;
				}
				else if(kequal && prevc == 'E' && keyword[1] == '\0'){//if EQUIVALENT checker is on; strictly check if keyword is currently E or EQUIVALE
					strcat(keyword, "Q");
					prevc = 'Q';
					break;
				}
				while(c[0] != ' ' && c[0] != EOF && c[0] != ';'){//if scanner fails to predict a keyword then print an error
					strcat(keyword, c);
					c[0] = getc(fp);
				}
				uppercase();
				if(prevc == ' ' && keyword[0] == 'Q')//if an invalid keyword precedes the variable Q
					printf("Error: Invalid sequence of keywords (unexpected constant/variable before Q)");
				else
					printf("Error: No such keyword exists! (%s)", keyword);//if the keyword isn't registered
				printf("\n\nScanned statement containing error:\n%s", scanned);
				exit(0);
				
			case 'r':
			case 'R':										//the letter R checks for the variable R, oR, and tRue
				//strictly check if both OR and TRUE checkers are off; if off, R is successfully added to the statement to be parsed
				if(!kor && !ktrue && prevc == '\0'){	
					strcat(statement, "R");
					prevc = ' ';
					r = true;
					break;
				}
				else if(kor && prevc == 'O'){				//if OR checker is on; OR is successfully added to the statement to be parsed
					strcpy(keyword, "");
					strcat(statement, "O");
					prevc = '\0';
					kor = false;
					break;
				}
				else if(ktrue && prevc == 'T'){				//if TRUE checker is on
					strcat(keyword, "R");
					prevc = 'R';
					break;
				}
				while(c[0] != ' ' && c[0] != EOF && c[0] != ';'){//if scanner fails to predict a keyword then print an error
					strcat(keyword, c);
					c[0] = getc(fp);
				}
				uppercase();
				if(prevc == ' ' && keyword[0] == 'R')//if an invalid keyword precedes the variable R
					printf("Error: Invalid sequence of keywords (unexpected constant/variable before R)");
				else
					printf("Error: No such keyword exists! (%s)", keyword);//if the keyword isn't registered
				printf("\n\nScanned statement containing error:\n%s", scanned);
				exit(0);
				
			case 's':
			case 'S':										//the letter S checks for implieS and falSe
				if(kimply && prevc == 'E'){					//if IMPLIES checker is on; IMPLIES is successfully added to the statement to be parsed
					strcpy(keyword, "");
					strcat(statement, "I");
					prevc = '\0';
					kimply = false;
					break;
				}
				else if(kfalse && prevc == 'L'){			//if FALSE checker is on
					strcat(keyword, "S");
					prevc = 'S';
					break;
				}
				while(c[0] != ' ' && c[0] != EOF && c[0] != ';'){//if scanner fails to predict a keyword then print an error
					strcat(keyword, c);
					c[0] = getc(fp);
				}
				uppercase();
				printf("Error: No such keyword exists! (%s)", keyword);//if the keyword isn't registered
				printf("\n\nScanned statement containing error:\n%s", scanned);
				exit(0);
			
			case 't':
			case 'T':										//the letter T checks for True and equivalenT
				if(!ktrue && prevc == '\0'){				//the user will probably input the TRUE keyword
					strcat(keyword, "T");
					prevc = 'T';
					ktrue = true;
					break;
				}
				else if(kequal && prevc == 'N'){			//if EQUIVALENT is on; EQUIVALENT is successfully added to the statement to be parsed
					strcpy(keyword, "");
					strcat(statement, "E");
					prevc = '\0';
					kequal = false;
					break;
				}
				else if(knot && prevc == 'O'){				//if NOT is on; NOT is successfully added to the statement to be parsed
					strcpy(keyword, "");
					strcat(statement, "N");
					prevc = '\0';
					knot = false;
					break;
				}
				printf("Scanned statement containing error:\n%s", scanned);
				while(c[0] != ' ' && c[0] != EOF && c[0] != ';'){//if scanner fails to predict a keyword then print an error
					strcat(keyword, c);
					c[0] = getc(fp);
				}
				uppercase();
				//if an invalid keyword precedes the TRUE keyword
				if(prevc == ' ' && keyword[0] == 'T' && keyword[1] == 'R' && keyword[2] == 'U' && keyword[3] == 'E')
					printf("\n\nError: Invalid sequence of keywords (unexpected constant/variable before TRUE)");
				else
					printf("\n\nError: No such keyword exists! (%s)", keyword);//if the keyword isn't registered
				exit(0);
			
			case 'u':
			case 'U':										//the letter U also checks for trUe and eqUivalent
				if(ktrue && prevc == 'R'){					//if TRUE checker is on
					strcat(keyword, "U");
					prevc = 'U';
					break;
				}
				else if(kequal && prevc == 'Q'){			//if EQUIVALENT checker is on
					strcat(keyword, "U");
					prevc = 'U';
					break;
				}
				while(c[0] != ' ' && c[0] != EOF && c[0] != ';'){//if scanner fails to predict a keyword then print an error
					strcat(keyword, c);
					c[0] = getc(fp);
				}
				uppercase();
				printf("Error: No such keyword exists! (%s)", keyword);//if the keyword isn't registered
				printf("\n\nScanned statement containing error:\n%s", scanned);
				exit(0);
			
			case 'v':
			case 'V':										//the letter V checks for equiValent
				if(kequal && prevc == 'I'){					//if EQUIVALENT checker is on
					strcat(keyword, "V");
					prevc = 'V';
					break;
				}
				while(c[0] != ' ' && c[0] != EOF && c[0] != ';'){//invalid since there is no other keyword with the letter V
					strcat(keyword, c);
					c[0] = getc(fp);
				}
				uppercase();
				printf("Error: No such keyword exists! (%s)", keyword);//if the keyword isn't registered
				printf("\n\nScanned statement containing error:\n%s", scanned);
				exit(0);
			
			case '(':										//checks if left parenthesis is inputted correctly
				if(prevc == '\0'){
					strcat(statement, "(");
					prevc = '\0';
					parcheck++;
					break;
				}
				printf("Error: Unexpected ( after operator!");
				printf("\n\nScanned statement containing error:\n%s", scanned);
				exit(0);
			
			case ')':										//checks if right parenthesis is inputted correctly
				if(prevc == ' '){
					strcat(statement, ")");
					prevc = ' ';
					parcheck--;
					break;
				}
				printf("Error: Unexpected ) after operator!");
				printf("\n\nScanned statement containing error:\n%s", scanned);
				exit(0);
				
			case ';':										//execute parser if no error
				strcpy(keyword, "");
				strcat(statement, ";");
				if(parcheck != 0){
					printf("Error: Either there are unclosed left parentheses or too many right parentheses");
					printf("\n\nScanned statement containing error:\n%s", scanned);
					exit(0);
				}
				else if(prevc != ' '){
					printf("Error: Unexpected ; or EOF after operator!");
				printf("\n\nScanned statement containing error:\n%s", scanned);
				exit(0);
				}
				prevc = '\0';
				ktrue = false;								//reset flags in preparation for next expression
				kor = false;
				kand = false;
				kimply = false;
				kequal = false;
				kfalse = false;
				knot = false;
				parse(statement);
				strcpy(scanned, "");						//reset strings in preparation for next expression
				strcpy(statement, "");
			
			default:										//remove added character if foreign aside from accepted syntax
				scanned[strlen(scanned) - 1] = '\0';
				break;
		}
	}
}

int main(){	
	FILE *fp;							//declare file pointer
	fp = fopen("sentence.pl", "r");		//open sentence.pl (which contains the input for this program)
	if(fp == NULL)						//if file can't be opened due to numerous reasons notify the user
		perror("Error opening file");
	else {
		scanner(fp);					//call scanner function if file is successfully opened
	}
	getch();
}
