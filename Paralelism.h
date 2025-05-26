#ifndef PARALELISM_H
#define PARALELISM_H

struct Posicao {
    int linha;
    int coluna;
};

Posicao encontrarVazia(char tabuleiro[9][9]);
bool resolverSudokuParalelo(char tabuleiro[9][9]);

#endif