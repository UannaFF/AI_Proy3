#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstring>
using namespace std;

string buffer;

void to_char(int c) {
	int i;
	int len;
	char s[64];

	sprintf(s, "%d", c);
	len = strlen(s);
	for (i = 0; i < len; i++) {
		buffer.push_back(s[i]);
	}
}

void agregar_clausula(int *c, int n) {
	int i;
	for (i = 0; i < n; i++) {
		to_char(c[i]);
		buffer.push_back(' ');
	}
	to_char(0);
	buffer.push_back('\n');
}

void desigualdad(int pos, int val) {
	int c[4];
	pos = pos * 4;
	pos++;
	c[0] = (val & 1)? -pos : pos;
	pos++;
	c[1] = (val & 2)? -pos : pos;
	pos++;
	c[2] = (val & 4)? -pos : pos;
	pos++;
	c[3] = (val & 8)? -pos : pos;
	agregar_clausula(c, 4);
}

void igualdad(int pos, int val) {
	int c;
	pos =  pos * 4;
	pos++;
	c = (val & 1)? pos : -pos;
	agregar_clausula(&c, 1);
	pos++;
	c = (val & 2)? pos : -pos;
	agregar_clausula(&c, 1);
	pos++;
	c = (val & 4)? pos : -pos;
	agregar_clausula(&c, 1);
	pos++;
	c = (val & 8)? pos : -pos;
	agregar_clausula(&c, 1);
}

void diferencia(int i, int j) {
	int c[8];
	int l;
	i *= 4;
	j *= 4;

	i++;
	j++;
	for (l = 0; l < 16; l++) {
		int ii = i;
		int jj = j;
		int *cc = c;

		*cc++ = (l&1) ? ii : -ii; ii++;
		*cc++ = (l&2) ? ii : -ii; ii++;
		*cc++ = (l&4) ? ii : -ii; ii++;
		*cc++ = (l&8) ? ii : -ii;

		*cc++ = (l&1) ? jj : -jj; jj++;
		*cc++ = (l&2) ? jj : -jj; jj++;
		*cc++ = (l&4) ? jj : -jj; jj++;
		*cc++ = (l&8) ? jj : -jj;

		agregar_clausula(c, 8);
	}
}

void rango_valor(int pos) {
	desigualdad(pos, 0);
	desigualdad(pos, 15);
	desigualdad(pos, 14);
	desigualdad(pos, 13);
	desigualdad(pos, 12);
	desigualdad(pos, 11);
	desigualdad(pos, 10);
}


int coord_var(int c, int v) {
	int x_cube = c % 3;
	int y_cube = c / 3;
	int top_left = y_cube * 9 * 3 + x_cube * 3;
	int x_v = v % 3;
	int y_v = v / 3;
	return top_left + y_v * 9 + x_v;
}

void dif_cubo(int c, int i, int j){
	int x = coord_var(c, i);
	int y = coord_var(c, j);
	diferencia(x, y);
}


int main(int argc, char const *argv[]) {
	int i, j, c, f;
	ofstream output;
	ifstream file;
	string line;

	buffer.reserve(469000);
	file.open("sudoku_10k.txt");
	output.open("sudoku.cnf");

	// Se construyen las clausulas generales para cualquier sudoku
	// Comprobación de valor entre 1 y 9
	for (i = 0; i < 9*9; i++) {
    	rango_valor(i);
	}
    // Comprobación de filas
    for (f = 0; f < 9; f++) {
    	for (i = 0; i < 9; i++) {
    		for (j = i+1; j < 9; j++) {
    			diferencia(i+f*9, j+f*9);
    		}
    	}
    }
    // Comprobacioón de columnas
    for (c = 0; c < 9; c++) {
    	for (i = 0; i < 9; i++) {
    		for (j = i+1; j < 9; j++) {
    			diferencia(i*9+c, j*9+c);
    		}
    	}
    }
    // Comprobación para sub cubos
	for (c = 0; c < 9; c++) {
		for (i = 0; i < 9; i++) {
			for (j = i+1; j < 9; j++) {
				if (i/3 == j/3)
					continue;
				if (i%3 == j%3)
					continue;
				dif_cubo(c, i, j);
			}
		}
	}
	// Comprobación para los números fijos
	getline(file, line);
	stringstream str(line);
	i = 0;
	for (char c; str >> c;){
		if(c != '.'){
			cout << c << endl;
			igualdad(i, c);
		}
		i++;
	}

	buffer.shrink_to_fit();
    output << buffer;
    output.close();	
	return 0;
}