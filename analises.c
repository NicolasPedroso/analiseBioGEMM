#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

#define MAX_ENTRADAS 1000
#define TAM_LINHA 512

typedef struct {
    char codon[4];
    char aminoacido[10];
    int contagem;
} Entrada;

/* Lista fixa de aminoácidos (em ordem alfabética) */
const char *aminoacidos_lista[] = {
    "Ala", "Arg", "Asn", "Asp", "Cys",
    "Gln", "Glu", "Gly", "His", "Ile",
    "Leu", "Lys", "Met", "None", "Phe",
    "Pro", "Ser", "Thr", "Trp", "Tyr",
    "Undet", "Val"
};
const int NUM_AMINO = sizeof(aminoacidos_lista) / sizeof(aminoacidos_lista[0]);

/* Função para garantir que a pasta tabela exista */
void criaPastaTabela() {
    struct stat st = {0};
    if (stat("tabela", &st) == -1) {
        if (mkdir("tabela", 0755) == 0) {
            printf("Pasta 'tabela' criada.\n");
        } else {
            perror("Erro ao criar pasta 'tabela'");
            exit(1);
        }
    }
}

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

    /* Criar nome do arquivo de saída dentro da pasta tabela */
    char nomeSaida[512];
    snprintf(nomeSaida, sizeof(nomeSaida), "tabela/%s", strrchr(caminho, '/') ? strrchr(caminho, '/') + 1 : caminho);

    char *p = strstr(nomeSaida, ".tabular");
    if (p) strcpy(p, ".tabela");
    else strcat(nomeSaida, ".tabela");

    FILE *saida = fopen(nomeSaida, "w");
    if (!saida) {
        perror("Erro ao criar arquivo .tabela");
        return;
    }

    /* Para cada aminoácido da lista fixa */
    for (int idx = 0; idx < NUM_AMINO; idx++) {
        const char *amino = aminoacidos_lista[idx];
        int encontrou = 0;

        fprintf(saida, "%s |", amino);

        /* Percorre a tabela procurando registros desse aminoácido */
        for (int i = 0; i < total; i++) {
            if (strcmp(amino, tabela[i].aminoacido) == 0 && tabela[i].contagem > 0) {
                if (encontrou) fprintf(saida, ",");
                fprintf(saida, "%s%d", tabela[i].codon, tabela[i].contagem);
                encontrou = 1;
            }
        }

        fprintf(saida, "\n");
    }

    fclose(saida);
    printf("Arquivo '%s' processado -> '%s'.\n", caminho, nomeSaida);
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
            percorreDiretorio(caminhoCompleto); /* Recursão */
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

    criaPastaTabela();
    percorreDiretorio(argv[1]);
    return 0;
}
