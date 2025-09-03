import matplotlib.pyplot as plt
import os
import glob
import sys
import re

def processar_arquivo(caminho):
    dados = {}
    with open(caminho, "r", encoding="utf-8") as f:
        for linha in f:
            if "|" not in linha:
                continue
            amino, codons = linha.strip().split("|")
            amino = amino.strip()
            codons = codons.strip().split(",")
            total = 0
            for c in codons:
                c = c.strip()
                if not c:
                    continue
                # separa letras e números (ex: "GCC5" -> "GCC", "5")
                m = re.match(r"([A-Z\?]+)(\d+)$", c)
                if m:
                    numero = int(m.group(2))
                    total += numero
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
    if len(sys.argv) != 2:
        print("Uso: python3 graficos.py <caminho_da_pasta>")
        sys.exit(1)

    pasta = sys.argv[1]

    # Cria a pasta "resultados" no diretório atual, se não existir
    pasta_resultados = os.path.join(os.getcwd(), "resultados")
    os.makedirs(pasta_resultados, exist_ok=True)

    todos_dados = {}

    # procura arquivos .tabela recursivamente
    arquivos = glob.glob(os.path.join(pasta, "**", "*.tabela"), recursive=True)

    if not arquivos:
        print("Nenhum arquivo .tabela encontrado.")
        sys.exit(0)

    for arquivo in arquivos:
        dados = processar_arquivo(arquivo)

        # Nome do gráfico individual dentro da pasta "resultados"
        nome_saida = os.path.splitext(os.path.basename(arquivo))[0] + "_grafico.png"
        caminho_saida = os.path.join(pasta_resultados, nome_saida)

        gerar_grafico(dados, f"Frequência em {os.path.basename(arquivo)}", caminho_saida)
        print(f"Gráfico gerado: {caminho_saida}")

        # acumula no geral
        for k, v in dados.items():
            todos_dados[k] = todos_dados.get(k, 0) + v

    # gera gráfico geral
    if todos_dados:
        caminho_saida_geral = os.path.join(pasta_resultados, "geral_grafico.png")
        gerar_grafico(todos_dados, "Gráfico Geral", caminho_saida_geral)
        print(f"Gráfico geral gerado: {caminho_saida_geral}")

if __name__ == "__main__":
    main()
