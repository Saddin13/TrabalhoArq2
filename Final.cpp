#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <cstring>
#include <vector>
using namespace std;

struct Posicao {
    int linha;
    int coluna;
};

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

// Função sequencial para o worker
bool resolverSudoku(int tabuleiro[9][9]) {
    Posicao vazia = encontrarVazia(tabuleiro);
    if (vazia.linha == -1) return true;
    for (int num = 1; num <= 9; num++) {
        if (ValidadorDeSudoku(tabuleiro, vazia.linha, vazia.coluna, num)) {
            tabuleiro[vazia.linha][vazia.coluna] = num;
            if (resolverSudoku(tabuleiro)) return true;
            tabuleiro[vazia.linha][vazia.coluna] = 0;
        }
    }
    return false;
}

// Serializa/deserializa o tabuleiro para pipe
void write_board(int fd, int m[9][9]) {
    write(fd, m, sizeof(int) * 81);
}
void read_board(int fd, int m[9][9]) {
    read(fd, m, sizeof(int) * 81);
}

int main() {
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

    PrintSudoku(m);

    // Pipes para cada worker
    int to_worker[4][2], from_worker[4][2];
    pid_t pids[4];

    for (int i = 0; i < 4; i++) {
        pipe(to_worker[i]);
        pipe(from_worker[i]);
        pids[i] = fork();
        if (pids[i] == 0) {
            // Worker
            close(to_worker[i][1]);
            close(from_worker[i][0]);
            while (true) {
                int board[9][9];
                int task;
                if (read(to_worker[i][0], &task, sizeof(int)) != sizeof(int)) break;
                read_board(to_worker[i][0], board);
                bool solved = resolverSudoku(board);
                write(from_worker[i][1], &solved, sizeof(int));
                if (solved) write_board(from_worker[i][1], board);
            }
            exit(0);
        }
        close(to_worker[i][0]);
        close(from_worker[i][1]);
    }

    // Gerente: distribui as primeiras 4 possibilidades para os workers
    Posicao vazia = encontrarVazia(m);
    int worker_idx = 0;
    vector<int> nums;
    for (int num = 1; num <= 9; num++) {
        if (ValidadorDeSudoku(m, vazia.linha, vazia.coluna, num)) {
            nums.push_back(num);
        }
    }

    int solved = 0;
    int solved_board[9][9];

    for (size_t i = 0; i < nums.size() && i < 4; i++) {
        int board[9][9];
        memcpy(board, m, sizeof(m));
        board[vazia.linha][vazia.coluna] = nums[i];
        int task = 1;
        write(to_worker[i][1], &task, sizeof(int));
        write_board(to_worker[i][1], board);
    }

    // Espera resposta dos workers
    for (size_t i = 0; i < nums.size() && i < 4; i++) {
        int ok = 0;
        read(from_worker[i][0], &ok, sizeof(int));
        if (ok && !solved) {
            read_board(from_worker[i][0], solved_board);
            solved = 1;
        }
    }

    // Fecha pipes e espera workers
    for (int i = 0; i < 4; i++) {
        close(to_worker[i][1]);
        close(from_worker[i][0]);
        waitpid(pids[i], NULL, 0);
    }

    if (solved) {
        cout << "\nSudoku resolvido:\n";
        PrintSudoku(solved_board);
    } else {
        cout << "\nNao foi possivel resolver o Sudoku\n";
    }
    return 0;
}