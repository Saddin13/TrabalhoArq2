#ifndef SUDOKU_PURO_H
#define SUDOKU_PURO_H

void PrintSudoku(char m[9][9]);
bool ValidadorDeSudoku(char m[9][9], int linha, int coluna, int num);
void GerarValoresAleatorios(char m[9][9], int quantidade);

#endif