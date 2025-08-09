#!/bin/bash

# Arquivos de entrada
T1="87-tRNA_prediction_on_data_PCitricarpa_USA110.tabela"
T2="testeSlow_101-tRNA_prediction_on_data_PCitricarpa_USA110.tabela"
T3="testeVerySlow_101-tRNA_prediction_on_data_PCitricarpa_USA110.tabela"

# Arquivo de saída
OUT="comparacao.csv"

# Cabeçalho
echo "Codon,Tabela1,Tabela2,Tabela3" > "$OUT"

# Extrair todos os códons únicos das três tabelas
cut -d'|' -f1 "$T1" "$T2" "$T3" | tr -d ' ' | sort | uniq > todos_codons.tmp

# Função para buscar valor por códon
buscar_valor() {
    local codon="$1"
    local arquivo="$2"
    grep -E "^$codon[[:space:]]*\|" "$arquivo" | cut -d'|' -f2 | tr -d ' ' || echo "-"
}

# Para cada códon, busca nas três tabelas
while read -r codon; do
    v1=$(buscar_valor "$codon" "$T1")
    v2=$(buscar_valor "$codon" "$T2")
    v3=$(buscar_valor "$codon" "$T3")
    echo "$codon,$v1,$v2,$v3" >> "$OUT"
done < todos_codons.tmp

# Limpeza
rm todos_codons.tmp

echo "Comparação salva em: $OUT"
