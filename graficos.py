import matplotlib.pyplot as plt
import os
import glob
import sys  # <--- necessário para ler argumentos

def processar_arquivo(caminho):
    dados = {}
    with open(caminho, "r", encoding="utf-8") as f:
        for linha in f:
            if "|" not in linha:
                continue
            amino, codons = linha.strip().split("|")
            amino = amino.strip()
            codons = codons.strip().split(",")
            total = sum(int(c[-1]) for c in codons if c[-1].isdigit())
            dados[amino] = dados.get(amino, 0) + total
    return dados

def gerar_grafico(dados, titulo, saida):
    labels = list(dados.keys())
    valores = list(dados.values())

    plt.figure(figsize=(12,6))
    plt.bar(labels, valores)
    plt.xlabel("Aminoácidos")
    plt.ylabel("Quantidade total")
    plt.title(titulo)
    plt.xticks(rotation=45)
    plt.tight_layout()
    plt.savefig(saida)
    plt.close()

def main():
    #Pega o caminho da pasta passado pelo usuario
    if len(sys.argv) != 2:
        print("Uso: python3 grafico.py <caminho_da_pasta>")
        sys.exit(1)

    pasta = sys.argv[1]  #Pasta passada pelo usuario
    todos_dados = {}

    #Procurar todos os arquivos .tabela dentro da pasta e subpastas
    arquivos = glob.glob(os.path.join(pasta, "**", "*.tabela"), recursive=True)

    for arquivo in arquivos:
        dados = processar_arquivo(arquivo)
        
        #Nome do grafico individual
        nome_saida = os.path.splitext(os.path.basename(arquivo))[0] + "_grafico.png"
        gerar_grafico(dados, f"Frequência em {os.path.basename(arquivo)}", nome_saida)
        print(f"Gráfico gerado: {nome_saida}")

        #Acumular no geral
        for k, v in dados.items():
            todos_dados[k] = todos_dados.get(k, 0) + v

    #Grafico geral
    if todos_dados:
        gerar_grafico(todos_dados, "Gráfico Geral", "geral_grafico.png")
        print("Gráfico geral gerado: geral_grafico.png")

if __name__ == "__main__":
    main()
    