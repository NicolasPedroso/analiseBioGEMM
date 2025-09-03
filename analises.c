#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

#define MAX_ENTRADAS 1000
#define TAM_LINHA 512

typedef struct {
    char codon[4];
    char aminoacido[10];
    int contagem;
} Entrada;

void adicionaOuIncrementa(Entrada *tabela, int *total, const char *codon, const char *aminoacido) {
    for (int i = 0; i < *total; i++) {
        if (strcmp(tabela[i].codon, codon) == 0 && strcmp(tabela[i].aminoacido, aminoacido) == 0) {
            tabela[i].contagem++;
            return;
        }
    }
    if (*total < MAX_ENTRADAS) {
        strncpy(tabela[*total].codon, codon, 3);
        tabela[*total].codon[3] = '\0';
        strncpy(tabela[*total].aminoacido, aminoacido, sizeof(tabela[*total].aminoacido) - 1);
        tabela[*total].aminoacido[sizeof(tabela[*total].aminoacido) - 1] = '\0';
        tabela[*total].contagem = 1;
        (*total)++;
    }
}

void processaArquivoTabular(const char *caminho) {
    FILE *f = fopen(caminho, "r");
    if (!f) {
        perror("Erro ao abrir arquivo .tabular");
        return;
    }

    Entrada tabela[MAX_ENTRADAS];
    int total = 0;
    char linha[TAM_LINHA];

    while (fgets(linha, sizeof(linha), f)) {
        char contig[100];
        int a, b, c, d;
        char aminoacido[10], codon[10];
        int e, fpos;
        float score;

        int n = sscanf(linha, "%s %d %d %d %s %s %d %d %f",
                       contig, &a, &b, &c, aminoacido, codon, &e, &fpos, &score);

        if (n == 9) {
            if (strcmp(aminoacido, "Pseudo") == 0)
                strcpy(aminoacido, "None");
            adicionaOuIncrementa(tabela, &total, codon, aminoacido);
        }
    }

    fclose(f);

    /*Criar nome do arquivo de saida substituindo ".tabular" por ".tabela"*/
    char nomeSaida[512];
    strncpy(nomeSaida, caminho, sizeof(nomeSaida));
    char *p = strstr(nomeSaida, ".tabular");
    if (p) strcpy(p, ".tabela");
    else strcat(nomeSaida, ".tabela");

    FILE *saida = fopen(nomeSaida, "w");
    if (!saida) {
        perror("Erro ao criar arquivo .tabela");
        return;
    }

    /*Agrupar por aminoacido*/
    for (int i = 0; i < total; i++) {
        if (tabela[i].contagem == -1) continue; /*Ja processado*/

        fprintf(saida, "%s |", tabela[i].aminoacido);
        for (int j = i; j < total; j++) {
            if (strcmp(tabela[i].aminoacido, tabela[j].aminoacido) == 0) {
                fprintf(saida, "%s%d", tabela[j].codon, tabela[j].contagem);
                if (j + 1 < total) {
                    /*Verifica se tem mais do mesmo aminoacido*/
                    int temMais = 0;
                    for (int k = j + 1; k < total; k++) {
                        if (strcmp(tabela[i].aminoacido, tabela[k].aminoacido) == 0) {
                            temMais = 1;
                            break;
                        }
                    }
                    if (temMais) fprintf(saida, ",");
                }
                tabela[j].contagem = -1; /*Marca como processado*/
            }
        }
        fprintf(saida, "\n");
    }

    fclose(saida);
    printf("Arquivo '%s' processado.\n", caminho);
}

void percorreDiretorio(const char *caminho) {
    struct dirent *entrada;
    DIR *dir = opendir(caminho);
    if (!dir) {
        perror("Erro ao abrir diretório");
        return;
    }

    while ((entrada = readdir(dir)) != NULL) {
        if (strcmp(entrada->d_name, ".") == 0 || strcmp(entrada->d_name, "..") == 0)
            continue;

        char caminhoCompleto[512];
        snprintf(caminhoCompleto, sizeof(caminhoCompleto), "%s/%s", caminho, entrada->d_name);

        struct stat info;
        if (stat(caminhoCompleto, &info) != 0) continue;

        if (S_ISDIR(info.st_mode)) {
            percorreDiretorio(caminhoCompleto); /*Recursao*/
        } else if (S_ISREG(info.st_mode)) {
            const char *ext = strrchr(caminhoCompleto, '.');
            if (ext && strcmp(ext, ".tabular") == 0) {
                processaArquivoTabular(caminhoCompleto);
            }
        }
    }

    closedir(dir);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <diretorio_raiz>\n", argv[0]);
        return 1;
    }

    percorreDiretorio(argv[1]);
    return 0;
}
