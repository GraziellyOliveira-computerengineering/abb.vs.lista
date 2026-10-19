# =====================================================================
#  contagem.r
#  Compila e executa o programa em C (abbxlista.c), lê o
#  resultados.csv e gera o gráfico comparando Lista e ABB.
#
#  ANTES DE RODAR
#  --------------
#  1) Tenha o gcc instalado (compilador de C).
#  2) Deixe abbxlista.c e este arquivo na MESMA pasta.
#  3) No R, aponte para essa pasta com:
#         setwd("caminho/da/pasta")
#     e depois rode:
#         source("contagem.r")
#
#  SÍMBOLOS DO R QUE APARECEM AQUI
#  -------------------------------
#  #     começa um comentário (o R ignora o resto da linha)
#  <-    atribuição: "guarda o valor da direita na variável da esquerda"
#  c()   cria um vetor, juntando valores: c("a", "b") é o vetor com
#        os dois textos
#  $     acessa uma parte de um objeto: dados$n é a coluna "n" da tabela
#  [ , ] seleciona linhas e colunas de uma tabela: tabela[linhas, colunas]
#        (se deixar um lado vazio, pega todas)
#  16:17 sequência de números de 16 até 17, ou seja, c(16, 17)
# =====================================================================


# ---------------------------------------------------------------------
# Nome do executável.
# No Windows o programa compilado termina em ".exe". No Linux e no Mac
# não tem extensão, e o "./" no começo significa "nesta pasta".
#
# ".Platform$OS.type" informa o sistema operacional ("windows" ou "unix").
# "==" compara se são iguais; "if ... else ..." escolhe um dos dois
# textos conforme o resultado.
# ---------------------------------------------------------------------
exe <- if (.Platform$OS.type == "windows") "abbxlista.exe" else "./abbxlista"


# ---------------------------------------------------------------------
# PASSO 1: compilar o C
# system() executa um comando como se você o digitasse no terminal.
# "gcc arquivo.c -o nome" compila arquivo.c e gera o programa "nome".
# Se você já compilou à mão, pode apagar esta linha.
# ---------------------------------------------------------------------
system("gcc abbxlista.c -o abbxlista")


# ---------------------------------------------------------------------
# PASSO 2: executar o programa em C
# Ele faz todos os sorteios e cria o arquivo resultados.csv.
# ---------------------------------------------------------------------
system(exe)


# ---------------------------------------------------------------------
# PASSO 3: ler os resultados
# read.csv() lê o CSV e devolve uma "tabela" (data frame) com as
# colunas n, comps_lista e comps_abb, e uma linha por tamanho.
# ---------------------------------------------------------------------
dados <- read.csv("resultados.csv")

# mostra a tabela no console, para você conferir os 10 valores
print(dados)


# ---------------------------------------------------------------------
# FUNÇÃO QUE DESENHA O GRÁFICO
# Em vez de escrever os comandos duas vezes (uma para a tela, outra
# para salvar a imagem), colocamos tudo numa função chamada "plotar".
# "function() { ... }" cria uma função; tudo que está entre as chaves
# roda toda vez que chamamos plotar().
# ---------------------------------------------------------------------
plotar <- function() {

  # par(mfrow = c(1, 2)) divide a área de desenho em
  # 1 linha e 2 colunas, para caberem dois gráficos lado a lado.
  par(mfrow = c(1, 2))


  # -------------------------------------------------------------------
  # GRÁFICO DA ESQUERDA: escala normal (linear)
  # matplot() desenha várias linhas de uma vez. Argumentos:
  # -------------------------------------------------------------------
  matplot(
    # eixo x: a coluna "n" (os tamanhos)
    dados$n,

    # eixo y: duas colunas de uma vez, cada uma vira uma linha.
    # dados[ , c(...)] = todas as linhas, só as colunas citadas.
    dados[, c("comps_lista", "comps_abb")],

    # type = "b" (both): desenha linha E pontos
    type = "b",

    # pch = formato do ponto. 16 = círculo cheio, 17 = triângulo cheio.
    # Um para cada linha, para dar para distinguir mesmo em preto e branco.
    pch = 16:17,

    # lty = tipo da linha. 1 = contínua.
    lty = 1,

    # cores das linhas, na mesma ordem das colunas: lista, ABB
    col = c("red", "blue"),

    # textos dos eixos e título
    xlab = "Número de dados (n)",
    ylab = "Média de comparações",
    main = "Lista vs ABB"
  )

  # Legenda no canto superior esquerdo, repetindo cores e símbolos.
  legend("topleft",
         legend = c("Lista", "ABB"),
         col = c("red", "blue"),
         pch = 16:17,
         lty = 1)


  # -------------------------------------------------------------------
  # GRÁFICO DA DIREITA: mesmos dados, eixo y em escala logarítmica
  #
  # Por que fazer isso? No gráfico linear, a lista chega a milhares de
  # comparações e a ABB fica perto de 20, então a linha da ABB parece
  # colada no zero e não dá para ver como ela se comporta. Na escala
  # log, cada marca do eixo vale 10 vezes a anterior (10, 100, 1000...),
  # e as duas linhas ficam visíveis ao mesmo tempo.
  # -------------------------------------------------------------------
  matplot(
    dados$n,
    dados[, c("comps_lista", "comps_abb")],
    type = "b",
    pch = 16:17,
    lty = 1,
    col = c("red", "blue"),

    # log = "y" liga a escala logarítmica só no eixo y
    log = "y",

    xlab = "Número de dados (n)",
    ylab = "Média de comparações (escala log)",
    main = "Escala logarítmica"
  )

  legend("topleft",
         legend = c("Lista", "ABB"),
         col = c("red", "blue"),
         pch = 16:17,
         lty = 1)
}


# ---------------------------------------------------------------------
# PASSO 4: mostrar o gráfico na tela
# ---------------------------------------------------------------------
plotar()


# ---------------------------------------------------------------------
# PASSO 5: salvar o mesmo gráfico como imagem PNG
# png() abre um arquivo de imagem (1000 x 450 pixels). A partir daí,
# tudo que for desenhado vai para o arquivo em vez da tela.
# dev.off() fecha o arquivo e grava de fato no disco.
# O arquivo grafico.png aparece na mesma pasta do script.
# ---------------------------------------------------------------------
png("grafico.png", width = 1000, height = 450)
plotar()
dev.off()