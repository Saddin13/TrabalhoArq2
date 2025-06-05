#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <string.h>
using namespace std;

struct Posicao {
    int linha;
    int coluna;
};

// Protótipos das funções
void PrintSudoku(int m[9][9]);
bool ValidadorDeSudoku(int m[9][9], int linha, int coluna, int num);
Posicao encontrarVazia(int tabuleiro[9][9]);
bool resolverSudokuParalelo(int tabuleiro[9][9], int nivel);

void PrintSudoku(int m[9][9]) {
    printf("\t0 1 2 3 4 5 6 7 8  | \n\t------------------ |\n\t");
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (m[i][j] == 0)
                printf(". ");
            else
                printf("%d ", m[i][j]);
        }
        printf(" | %d\n\t", i);
    }
}

bool ValidadorDeSudoku(int m[9][9], int linha, int coluna, int num) {
    for (int j = 0; j < 9; j++) {
        if (m[linha][j] == num) return false;
    }
    for (int i = 0; i < 9; i++) {
        if (m[i][coluna] == num) return false;
    }
    int quadLinha = linha - (linha % 3);
    int quadColuna = coluna - (coluna % 3);
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (m[quadLinha + i][quadColuna + j] == num) return false;
        }
    }
    return true;
}

int main() {
    omp_set_num_threads(8); // Define o número de threads a serem usadas

    //O MAIS DIFICIL DA HISTORIA DE ACORDO COM O GOOGLE
    int m[9][9] = {
        {8,0,0,0,0,0,0,0,0},
        {0,0,3,6,0,0,0,0,0},
        {0,7,0,0,9,0,2,0,0},
        {0,5,0,0,0,7,0,0,0},
        {0,0,0,0,4,5,7,0,0},
        {0,0,0,1,0,0,0,3,0},
        {0,0,1,0,0,0,0,6,8},
        {0,0,8,5,0,0,0,1,0},
        {0,9,0,0,0,0,4,0,0},
    };

    /*int m[9][9] = {
    {5,3,0,0,7,0,0,0,0},
    {6,0,0,1,9,5,0,0,0},
    {0,9,8,0,0,0,0,6,0},
    {8,0,0,0,6,0,0,0,3},
    {4,0,0,8,0,3,0,0,1},
    {7,0,0,0,2,0,0,0,6},
    {0,6,0,0,0,0,2,8,0},
    {0,0,0,4,1,9,0,0,5},
    {0,0,0,0,8,0,0,7,9}
    };*/

    system("cls");

    PrintSudoku(m);
    system("cls");
    cout << "\nResolvendo...\n";
    double inicio = omp_get_wtime();
    if (resolverSudokuParalelo(m, 0)) {
        double fim = omp_get_wtime();
        cout << "\nSudoku resolvido:\n";
        PrintSudoku(m);
        cout << "\nTempo de execucao: " << fim - inicio << " segundos\n";
    } else {
        cout << "\nNao foi possivel resolver o Sudoku\n";
    }
    return 0;
}

Posicao encontrarVazia(int tabuleiro[9][9]) {
    Posicao pos = {-1, -1};
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (tabuleiro[i][j] == 0) {
                pos.linha = i;
                pos.coluna = j;
                return pos;
            }
        }
    }
    return pos;
}

bool resolverSudokuParalelo(int tabuleiro[9][9], int nivel) {
    Posicao vazia = encontrarVazia(tabuleiro);
    if (vazia.linha == -1) return true;

    bool resolvido = false;

    if (nivel == 0) {
        #pragma omp parallel
        {
            #pragma omp single
            {
                for (int num = 1; num <= 9 && !resolvido; num++) {
                    if (ValidadorDeSudoku(tabuleiro, vazia.linha, vazia.coluna, num)) {
                        #pragma omp task shared(resolvido)
                        {
                            #pragma omp critical
                            {
                                cout << "[DEBUG] Thread " << omp_get_thread_num()
                                     << " tentando num=" << num
                                     << " na pos (" << vazia.linha << "," << vazia.coluna << ")"
                                     << " nivel=" << nivel << endl;
                            }
                            int copia_tabuleiro[9][9];
                            memcpy(copia_tabuleiro, tabuleiro, sizeof(int) * 9 * 9);
                            copia_tabuleiro[vazia.linha][vazia.coluna] = num;

                            if (resolverSudokuParalelo(copia_tabuleiro, nivel + 1)) {
                                #pragma omp critical
                                {
                                    if (!resolvido) {
                                        memcpy(tabuleiro, copia_tabuleiro, sizeof(int) * 9 * 9);
                                        resolvido = true;
                                    }
                                }
                            }
                        }
                    }
                }
                #pragma omp taskwait
            }
        }
    } else {
        for (int num = 1; num <= 9 && !resolvido; num++) {
            if (ValidadorDeSudoku(tabuleiro, vazia.linha, vazia.coluna, num)) {
                #pragma omp critical
                {
                    cout << "[DEBUG] Thread " << omp_get_thread_num()
                         << " tentando num=" << num
                         << " na pos (" << vazia.linha << "," << vazia.coluna << ")"
                         << " nivel=" << nivel << endl;
                }
                int copia_tabuleiro[9][9];
                memcpy(copia_tabuleiro, tabuleiro, sizeof(int) * 9 * 9);
                copia_tabuleiro[vazia.linha][vazia.coluna] = num;

                if (resolverSudokuParalelo(copia_tabuleiro, nivel + 1)) {
                    memcpy(tabuleiro, copia_tabuleiro, sizeof(int) * 9 * 9);
                    resolvido = true;
                    break;
                }
            }
        }
    }
    return resolvido;
}