#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include "Paralelism.h"
using namespace std;

void PrintSudoku(char m[9][9])
{
    printf("\t0 1 2 3 4 5 6 7 8  | \n\t------------------ |\n\t");
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            if (m[i][j] == 0)
                printf(". ");  // Imprime '.' para posições vazias
            else
            printf("%d ", m[i][j]);
        }
        printf(" | %d\n\t", i);
    }
}

bool ValidadorDeSudoku(char m[9][9], int linha, int coluna, int num) {
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

void GerarValoresAleatorios(char m[9][9], int quantidade) {
    srand(time(NULL));
    
    for (int k = 0; k < quantidade; k++) {
        int i = rand() % 9;
        int j = rand() % 9;
        int valor = (rand() % 9) + 1;
        
        if (m[i][j] == 0) {
            if (ValidadorDeSudoku(m, i, j, valor)) {
                m[i][j] = valor;
            } else {
                k--; // Tenta novamente se o número não pode ser inserido
            }
        } else {
            k--; // Posição já ocupada, tenta novamente
        }
    }
}

int main() {
    cout << "Voce quer Resolver o Sudoku ou Ver a Resposta Usando Paralelismo?\n";
    cout << "1 - Resolver\n";
    cout << "2 - Ultilizar Paralelismo\n";
    int opcao;
    cin >> opcao;


    char m[9][9] = {
        {0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0},
    };
    cout << "\nGerando Sudoku...\n";
    system("cls");

    GerarValoresAleatorios(m, 20); // Gera 20 números aleatórios
    PrintSudoku(m);
    system("cls");


    if (opcao == 1) {
        while (true) {
            int linha, coluna, valor;
            cout << "Digite a linha (0-8), coluna (0-8) e valor (1-9) para inserir (separados por espaco)\n";
            cin >> linha >> coluna >> valor;

            if (linha < 0 || linha > 8 || coluna < 0 || coluna > 8 || valor < 1 || valor > 9) {
                cout << "Entrada invalida. Tente novamente.\n";
                continue;
            }

            if (ValidadorDeSudoku(m, linha, coluna, valor)) {
                m[linha][coluna] = valor;
                system("cls");
                PrintSudoku(m);
            } else {
                cout << "Valor invalido para a posiçao. Tente novamente.\n";
            }
        }
        return 0;
    } else if (opcao == 2) {
            cout << "\nResolvendo...\n";
        double inicio = omp_get_wtime();
        if (resolverSudokuParalelo(m)) {
            double fim = omp_get_wtime();
            cout << "\nSudoku resolvido:\n";
            PrintSudoku(m);
            cout << "\nTempo de execucao: " << fim - inicio << " segundos\n";
        } else {
            cout << "\nNao foi possivel resolver o Sudoku\n";
        }
         return 0;
    } else {
        cout << "Opcao invalida. Por favor, escolha 1 ou 2.\n";
        return 1;
    }
}