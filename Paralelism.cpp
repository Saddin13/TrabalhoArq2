#include <omp.h>
#include <iostream>
#include "sudokuPURO.h"
#include "Paralelism.h"
using namespace std;

Posicao encontrarVazia(char tabuleiro[9][9]) {
    Posicao pos = {-1, -1};
    #pragma omp parallel for collapse(2)
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (tabuleiro[i][j] == 0) {
                #pragma omp critical
                {
                    if (pos.linha == -1) {
                        pos.linha = i;
                        pos.coluna = j;
                    }
                }
            }
        }
    }
    return pos;
}

bool resolverSudokuParalelo(char tabuleiro[9][9]) {
    Posicao vazia = encontrarVazia(tabuleiro);
    if (vazia.linha == -1) return true; // Sudoku completo

    bool resolvido = false;
    #pragma omp parallel for
    for (int num = 1; num <= 9; num++) {
        if (!resolvido) {
            if (ValidadorDeSudoku(tabuleiro, vazia.linha, vazia.coluna, num)) {
                #pragma omp critical
                {
                    if (!resolvido) {
                        tabuleiro[vazia.linha][vazia.coluna] = num;
                        if (resolverSudokuParalelo(tabuleiro)) {
                            resolvido = true;
                        } else {
                            tabuleiro[vazia.linha][vazia.coluna] = 0;
                        }
                    }
                }
            }
        }
    }
    return resolvido;
}