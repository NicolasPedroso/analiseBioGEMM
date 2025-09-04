import matplotlib.pyplot as plt
import os
import glob
import sys
import re
from collections import defaultdict

def processar_arquivo(caminho):
    dados_totais = {}         # soma de todos os códons por aminoácido (para gráfico de barras)
    dados_distrib = defaultdict(list)  # lista de valores de cada códon (para boxplot)

    with open(caminho, "r", encoding="utf-8") as f:
        for linha in f:
            if "|" not in linha:
                continue
            amino, codons = linha.strip().split("|")
            amino = amino.strip()
            codons = codons.strip().split(",")

            for c in codons:
                c = c.strip()
                if not c:
                    continue
                # separa letras e números (ex: "GCC12" -> "GCC", "12")
                m = re.match(r"([A-Z\?]+)(\d+)$", c)
                if m:
                    numero = int(m.group(2))
                    dados_totais[amino] = dados_totais.get(amino, 0) + numero
                    dados_distrib[amino].append(numero)

    return dados_totais, dados_distrib

def gerar_grafico_barras(dados, titulo, saida):
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

def gerar_grafico_boxplot(dados_distrib, titulo, saida):
    labels = list(dados_distrib.keys())
    valores = [dados_distrib[amino] for amino in labels]

    plt.figure(figsize=(12,6))
    plt.boxplot(valores, labels=labels, patch_artist=True)
    plt.xlabel("Aminoácidos")
    plt.ylabel("Distribuição dos códons")
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

    # Cria as pastas de saída
    pasta_resultados = os.path.join(os.getcwd(), "resultados")
    pasta_barras = os.path.join(pasta_resultados, "graficos_barras")
    pasta_boxplot = os.path.join(pasta_resultados, "graficos_boxplot")
    os.makedirs(pasta_barras, exist_ok=True)
    os.makedirs(pasta_boxplot, exist_ok=True)

    todos_dados_totais = {}
    todos_dados_distrib = defaultdict(list)

    arquivos = glob.glob(os.path.join(pasta, "**", "*.tabela"), recursive=True)

    if not arquivos:
        print("Nenhum arquivo .tabela encontrado.")
        sys.exit(0)

    for arquivo in arquivos:
        dados_totais, dados_distrib = processar_arquivo(arquivo)
        nome_base = os.path.splitext(os.path.basename(arquivo))[0]

        # gráfico de barras
        caminho_saida_barras = os.path.join(pasta_barras, nome_base + "_barras.png")
        gerar_grafico_barras(dados_totais, f"Frequência em {os.path.basename(arquivo)}", caminho_saida_barras)
        print(f"Gráfico de barras gerado: {caminho_saida_barras}")

        # gráfico boxplot
        caminho_saida_box = os.path.join(pasta_boxplot, nome_base + "_boxplot.png")
        gerar_grafico_boxplot(dados_distrib, f"Distribuição em {os.path.basename(arquivo)}", caminho_saida_box)
        print(f"Gráfico boxplot gerado: {caminho_saida_box}")

        # acumula nos totais e distribuições gerais
        for k, v in dados_totais.items():
            todos_dados_totais[k] = todos_dados_totais.get(k, 0) + v
        for k, lista in dados_distrib.items():
            todos_dados_distrib[k].extend(lista)

    # gráficos gerais
    if todos_dados_totais:
        caminho_saida_geral_barras = os.path.join(pasta_barras, "geral_barras.png")
        gerar_grafico_barras(todos_dados_totais, "Gráfico Geral (Barras)", caminho_saida_geral_barras)
        print(f"Gráfico geral de barras gerado: {caminho_saida_geral_barras}")

    if todos_dados_distrib:
        caminho_saida_geral_box = os.path.join(pasta_boxplot, "geral_boxplot.png")
        gerar_grafico_boxplot(todos_dados_distrib, "Gráfico Geral (Boxplot)", caminho_saida_geral_box)
        print(f"Gráfico geral boxplot gerado: {caminho_saida_geral_box}")

if __name__ == "__main__":
    main()

