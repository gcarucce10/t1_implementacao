#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdbool.h>

#define MAX_VERTICES 200

int tempo = 0;

struct aresta {
    int destino;
    int peso;
    struct aresta * proxima;
};

struct vertice {
    int valor;
    struct aresta * lista_adj;
};

struct retorno {
    int ** matriz_adj;
    int ** matriz_contador_pesos;
    int ** quais_pesos;
    int numero_vertices;
    int numero_arestas;
    char tipo;
    int valorado;
};

struct retorno_lista {
    struct vertice * grafo;
    int ** matriz_adj;
    int numero_vertices;
    int numero_arestas;
    char tipo;
    int valorado;
};

struct pesos {
    int ** matriz_contador;
};

struct print {
    int * pai;
};

struct aresta_dfs {
    int origem;
    int destino;
};

struct aresta_dfs * floresta_dfs[MAX_VERTICES];
int contador_aresta_dfs = 0;

void adicionar_aresta_dfs(int origem, int destino) {
    struct aresta_dfs * nova_aresta_dfs = (struct aresta_dfs*)malloc(sizeof(struct aresta_dfs));
    nova_aresta_dfs->origem = origem;
    nova_aresta_dfs->destino = destino;
    floresta_dfs[contador_aresta_dfs] = nova_aresta_dfs;
    contador_aresta_dfs++;
}

void criar_vertice(struct vertice * grafo, int valor) {
    grafo->valor = valor;
    grafo->lista_adj = NULL;
}

void adicionar_aresta(struct vertice * grafo, int destino, int peso) {
    struct aresta * nova_aresta = (struct aresta *)malloc(sizeof(struct aresta));
    nova_aresta->destino = destino;
    nova_aresta->peso = peso;
    nova_aresta->proxima = grafo->lista_adj;
    grafo->lista_adj = nova_aresta;
}

void imprimir_lista_adjacencias(struct vertice * grafo, int num_vertices, int valorado) {

        if(valorado == 0) {
            for (int i = 0; i < num_vertices; i++) {
                printf("%d: ", (grafo[i].valor + 1));
                struct aresta * aresta = grafo[i].lista_adj;
                while (aresta != NULL) {
                    printf("%d ", (aresta->destino + 1));
                    aresta = aresta->proxima;
                    if (aresta != NULL) printf("--> "); 
                }
                putchar('\n');
            }
        } else if(valorado == 1) {
            for (int i = 0; i < num_vertices; i++) {
                printf("%d: ", (grafo[i].valor + 1));
                struct aresta * aresta = grafo[i].lista_adj;
                while (aresta != NULL) {
                    printf("%d ", aresta->peso);
                    aresta = aresta->proxima;
                    if (aresta != NULL) printf("--> "); 
                }
                putchar('\n');
            }
        }
}

struct retorno gerar_matriz_adjacencias(FILE * fp) {

    rewind(fp);

    int num_vertices, num_arestas;
    char tipo;
    int valorado;
    int opcao, contador_loop = 0, contador_loop2 = 0;

    fscanf(fp, "%d %d %c %d", &num_vertices, &num_arestas, &tipo, &valorado);

    int ** matriz_adj = (int**)malloc(num_vertices * sizeof(int *));

    for (int i = 0; i < num_vertices; i++) {
        matriz_adj[i] = (int *)malloc(num_vertices * sizeof(int));
    }

    int contador[num_vertices][num_vertices];

    for(int i = 0; i < num_vertices; i++) {
        for(int j = 0; j < num_vertices; j++) {
            contador[i][j] = 0;
        }
    }

    int contador2[num_vertices][num_vertices];

    for(int i = 0; i < num_vertices; i++) {
        for(int j = 0; j < num_vertices; j++) {
            contador2[i][j] = 0;
        }
    }

    int ** contador_vezes_pesos = (int**)malloc(num_vertices * sizeof(int *));

    for (int i = 0; i < num_vertices; i++) {
        contador_vezes_pesos[i] = (int *)malloc(num_vertices * sizeof(int));
    }

    for(int i = 0; i < num_vertices; i++) {
        for(int j = 0; j < num_vertices; j++) {
            contador_vezes_pesos[i][j] = 0;
        }
    }

    int ** quais_pesos = (int**)malloc(num_arestas * sizeof(int *));

    for (int i = 0; i < num_arestas; i++) {
        quais_pesos[i] = (int *)malloc(num_arestas * sizeof(int));
    }

    for(int i = 0; i < num_arestas; i++) {
        for(int j = 0; j < num_arestas; j++) {
            quais_pesos[i][j] = 0;
        }
    }

    if(valorado == 0 && tipo == 'g') {

        while(contador_loop < num_arestas) {
            int v1, v2;
            fscanf(fp, "%d %d", &v1, &v2);
            contador2[v1 - 1][v2 - 1] += 1;
            if(v1 != v2) { 
                contador2[v2 - 1][v1 - 1] += 1;
                matriz_adj[v1 - 1][v2 - 1] = contador2[v1 - 1][v2 - 1];
                matriz_adj[v2 - 1][v1 - 1] = contador2[v2 - 1][v1 - 1];
            } else {
                contador2[v2 - 1][v1 - 1] += 0;
                matriz_adj[v1 - 1][v2 - 1] = contador2[v1 - 1][v2 - 1];
                matriz_adj[v2 - 1][v1 - 1] = contador2[v1 - 1][v2 - 1];
            }
            contador_loop++;
        }
    } else if(valorado == 1 && tipo == 'g') {
        
        while(contador_loop < num_arestas) {
            int v1, v2, peso;
            fscanf(fp, "%d %d %d", &v1, &v2, &peso);
            contador2[v1 - 1][v2 - 1] += peso;
            contador_vezes_pesos[v1 - 1][v2 - 1] += 1;
            if(v1 != v2) {
                contador2[v2 - 1][v1 - 1] += peso;
                matriz_adj[v1 - 1][v2 - 1] = contador2[v1 - 1][v2 - 1];
                matriz_adj[v2 - 1][v1 - 1] = contador2[v2 - 1][v1 - 1];
                contador_vezes_pesos[v2 - 1][v1 - 1] += 1;
            } else {
                contador2[v2 - 1][v1 - 1] += 0;
                matriz_adj[v1 - 1][v2 - 1] = contador2[v1 - 1][v2 - 1];
                matriz_adj[v2 - 1][v1 - 1] = contador2[v1 - 1][v2 - 1];
                contador_vezes_pesos[v2 - 1][v1 - 1] += 0;
            }
            contador_loop++;
        }

        rewind(fp);

        int k2 = 0;
        int numero_vertices_aux, numero_arestas_aux, valorado_aux;
        char tipo_aux;
        fscanf(fp, "%d %d %c %d", &numero_vertices_aux, &numero_arestas_aux, &tipo_aux, &valorado_aux);
        while(contador_loop2 < num_arestas) {
            int v1, v2, peso;
            fscanf(fp, "%d %d %d", &v1, &v2, &peso);
            if(v1 != v2) {
                quais_pesos[v1 - 1][k2] = peso;
                quais_pesos[v2 - 1][k2] = peso;
            } else {
                quais_pesos[v1 - 1][k2] = peso;
            }
            contador_loop2++;
            k2++;
        }
    } else if(valorado == 0 && tipo == 'd') {

        while(contador_loop < num_arestas) {
            int v1, v2;
            fscanf(fp, "%d %d", &v1, &v2);
            contador[v1 - 1][v2 - 1] += 1;
            matriz_adj[v1 - 1][v2 - 1] = contador[v1 - 1][v2 - 1];
            contador_loop++;
        }
    } else if(valorado == 1 && tipo == 'd') {
        while(contador_loop < num_arestas) {
            int v1, v2, peso;
            fscanf(fp, "%d %d %d", &v1, &v2, &peso);
            contador2[v1 - 1][v2 - 1] += peso;
            matriz_adj[v1 - 1][v2 - 1] = contador2[v1 - 1][v2 - 1];
            contador_vezes_pesos[v1 - 1][v2 - 1] += 1;
            contador_loop++;
        }

        rewind(fp);

        int k2 = 0;
        int numero_vertices_aux, numero_arestas_aux, valorado_aux;
        char tipo_aux;
        fscanf(fp, "%d %d %c %d", &numero_vertices_aux, &numero_arestas_aux, &tipo_aux, &valorado_aux);

        while(contador_loop2 < num_arestas) {
            int v1, v2, peso;
            fscanf(fp, "%d %d %d", &v1, &v2, &peso);
            quais_pesos[v1 - 1][k2] = peso;
            contador_loop2++;
            k2++;
        }
    }

    struct retorno ret;

    ret.matriz_adj = matriz_adj;
    ret.matriz_contador_pesos = contador_vezes_pesos;
    ret.quais_pesos = quais_pesos;
    ret.numero_vertices = num_vertices;
    ret.numero_arestas = num_arestas;
    ret.tipo = tipo;
    ret.valorado = valorado;
        
    return ret;
}

struct retorno_lista gerar_lista_adjacencias(FILE * fp) {

    rewind(fp);
    
    int num_vertices, num_arestas;
    char tipo;
    int valorado;
    int opcao, contador_loop = 0;

    fscanf(fp, "%d %d %c %d", &num_vertices, &num_arestas, &tipo, &valorado);

    int ** matriz_adj2 = (int**)malloc(num_vertices * sizeof(int *));

    for (int i = 0; i < num_vertices; i++) 
        matriz_adj2[i] = (int *)malloc(num_vertices * sizeof(int));

    int contador_aux1[num_vertices][num_vertices];

    for(int i = 0; i < num_vertices; i++) {
        for(int j = 0; j < num_vertices; j++) {
            contador_aux1[i][j] = 0;
        }
    }

    int contador_aux2[num_vertices][num_vertices];

    for(int i = 0; i < num_vertices; i++) {
        for(int j = 0; j < num_vertices; j++) {
            contador_aux2[i][j] = 0;
        }
    }

    struct vertice * grafo = (struct vertice*)malloc(MAX_VERTICES * sizeof(struct vertice));
    for (int i = 0; i < num_vertices; i++) {
        criar_vertice(&grafo[i], i);
    }

    if(valorado == 0 && tipo == 'd') {
        while(contador_loop < num_arestas) {
            int origem, destino;
            fscanf(fp, "%d %d", &origem, &destino);
            adicionar_aresta(&grafo[origem - 1], destino - 1, -1);
            contador_aux1[origem - 1][destino - 1] += 1;
            matriz_adj2[origem - 1][destino - 1] = contador_aux1[origem - 1][destino - 1];
            contador_loop++;
        }
    } else if(valorado == 0 && tipo == 'g') {
        while(contador_loop < num_arestas) {
            int origem, destino;
            fscanf(fp, "%d %d", &origem, &destino);
            adicionar_aresta(&grafo[origem - 1], destino - 1, -1);
            adicionar_aresta(&grafo[destino - 1], origem - 1, -1);
            contador_aux1[origem - 1][destino - 1] += 1;
            contador_aux1[destino - 1][origem - 1] += 1;
            matriz_adj2[origem - 1][destino - 1] = contador_aux1[origem - 1][destino - 1];
            matriz_adj2[destino - 1][origem - 1] = contador_aux1[destino - 1][origem - 1];
            contador_loop++;
        }
    } else if(valorado == 1 && tipo == 'd') {
        while(contador_loop < num_arestas) {
            int origem, destino, peso;
            fscanf(fp, "%d %d %d", &origem, &destino, &peso);
            adicionar_aresta(&grafo[origem - 1], destino - 1, peso);
            contador_aux2[origem - 1][destino - 1] += peso;
            matriz_adj2[origem - 1][destino - 1] = contador_aux2[origem - 1][destino - 1];
            contador_loop++;
            }
    } else if(valorado == 1 && tipo == 'g') {
        while(contador_loop < num_arestas) {
            int origem, destino, peso;
            fscanf(fp, "%d %d %d", &origem, &destino, &peso);
            adicionar_aresta(&grafo[origem - 1], destino - 1, peso);
            adicionar_aresta(&grafo[destino - 1], origem - 1, peso);
            contador_aux2[origem - 1][destino - 1] += peso;
            contador_aux2[destino - 1][origem - 1] += peso;
            matriz_adj2[origem - 1][destino - 1] = contador_aux2[origem - 1][destino - 1];
            matriz_adj2[destino - 1][origem - 1] = contador_aux2[destino - 1][origem - 1];
            contador_loop++;
        }
    }

    struct retorno_lista ret;
    ret.matriz_adj = matriz_adj2;
    ret.grafo = grafo;
    ret.numero_vertices = num_vertices;
    ret.numero_arestas = num_arestas;
    ret.tipo = tipo;
    ret.valorado = valorado;
    return ret;

}

void gerar_arquivo(struct retorno ret) {

    printf("\nDigite o nome do arquivo de saída (sem extensão):\n");

    char nome_arquivo[30];
    scanf("%s", nome_arquivo);
    strcat(nome_arquivo, ".txt");
    

    FILE * fp2 = fopen(nome_arquivo, "w");

    fprintf(fp2, "%d %d %c %d\n", ret.numero_vertices, ret.numero_arestas, ret.tipo, ret.valorado);

    if(ret.tipo == 'g' && ret.valorado == 0) {

        for(int i = 0; i < ret.numero_vertices; i++) {
            for(int j = 0; j < ret.numero_vertices; j++) {
                if(ret.matriz_adj[i][j] != 0) {
                    ret.matriz_adj[j][i] = 0;
                    fprintf(fp2, "%d %d\n", i + 1, j + 1);
                }
            }
        }
    } else if(ret.tipo == 'd' && ret.valorado == 0) {

        for(int i = 0; i < ret.numero_vertices; i++) {
            for(int j = 0; j < ret.numero_vertices; j++) {
                if(ret.matriz_adj[i][j] != 0) {
                    fprintf(fp2, "%d %d\n", i + 1, j + 1);
                }
            }
        }
    } else if(ret.tipo == 'g' && ret.valorado == 1) {
        for(int i = 0; i < ret.numero_vertices; i++) {
            for(int j = 0; j < ret.numero_vertices; j++) {
                if(ret.matriz_adj[i][j] != 0) {
                    ret.matriz_adj[j][i] = 0;
                    fprintf(fp2, "%d %d %d\n", i + 1, j + 1, ret.matriz_adj[i][j]);
                }
            }
        }
    } else if(ret.tipo == 'd' && ret.valorado == 1) {
        for(int i = 0; i < ret.numero_vertices; i++) {
            for(int j = 0; j < ret.numero_vertices; j++) {
                if(ret.matriz_adj[i][j] != 0) {
                    fprintf(fp2, "%d %d %d\n", i + 1, j + 1, ret.matriz_adj[i][j]);
                }
            }
        }
    }

    printf("\n%s foi criado!\n\n", nome_arquivo);

    fclose(fp2);
}

void gerar_arquivo_lista(struct retorno_lista ret) {

    printf("\nDigite o nome do arquivo de saída (sem extensão):\n");

    char nome_arquivo[30];
    scanf("%s", nome_arquivo);
    strcat(nome_arquivo, ".txt");
    

    FILE * fp3 = fopen(nome_arquivo, "w");

    fprintf(fp3, "%d %d %c %d\n", ret.numero_vertices, ret.numero_arestas, ret.tipo, ret.valorado);

    if(ret.tipo == 'g' && ret.valorado == 0) {

        for(int i = 0; i < ret.numero_vertices; i++) {
            for(int j = 0; j < ret.numero_vertices; j++) {
                if(ret.matriz_adj[i][j] != 0) {
                    ret.matriz_adj[j][i] = 0;
                    fprintf(fp3, "%d %d\n", i + 1, j + 1);
                }
            }
        }
    } else if(ret.tipo == 'd' && ret.valorado == 0) {

        for(int i = 0; i < ret.numero_vertices; i++) {
            for(int j = 0; j < ret.numero_vertices; j++) {
                if(ret.matriz_adj[i][j] != 0) {
                    fprintf(fp3, "%d %d\n", i + 1, j + 1);
                }
            }
        }
    } else if(ret.tipo == 'g' && ret.valorado == 1) {
        for(int i = 0; i < ret.numero_vertices; i++) {
            for(int j = 0; j < ret.numero_vertices; j++) {
                if(ret.matriz_adj[i][j] != 0) {
                    ret.matriz_adj[j][i] = 0;
                    fprintf(fp3, "%d %d %d\n", i + 1, j + 1, ret.matriz_adj[i][j]);
                }
            }
        }
    } else if(ret.tipo == 'd' && ret.valorado == 1) {
        for(int i = 0; i < ret.numero_vertices; i++) {
            for(int j = 0; j < ret.numero_vertices; j++) {
                if(ret.matriz_adj[i][j] != 0) {
                    fprintf(fp3, "%d %d %d\n", i + 1, j + 1, ret.matriz_adj[i][j]);
                }
            }
        }
    }

    printf("\n%s foi criado!\n\n", nome_arquivo);

    fclose(fp3);
}

struct retorno converter_lista_matriz(struct retorno_lista retorno) {

    int ** matriz_adj2 = (int**)malloc(retorno.numero_vertices * sizeof(int *));

    for (int i = 0; i < retorno.numero_vertices; i++) 
        matriz_adj2[i] = (int *)malloc(retorno.numero_vertices * sizeof(int));

    int contador_aux1[retorno.numero_vertices][retorno.numero_vertices];

    for(int i = 0; i < retorno.numero_vertices; i++) {
        for(int j = 0; j < retorno.numero_vertices; j++) {
            contador_aux1[i][j] = 0;
        }
    }
    int contador_aux2[retorno.numero_vertices][retorno.numero_vertices];

    for(int i = 0; i < retorno.numero_vertices; i++) {
        for(int j = 0; j < retorno.numero_vertices; j++) {
            contador_aux2[i][j] = 0;
        }
    }

    int contador_vezes_pesos[retorno.numero_vertices][retorno.numero_vertices];

    for(int i = 0; i < retorno.numero_vertices; i++) {
        for(int j = 0; j < retorno.numero_vertices; j++) {
            contador_vezes_pesos[i][j] = 0;
        }
    }

    int flag[retorno.numero_vertices][retorno.numero_vertices];

    for(int i = 0; i < retorno.numero_vertices; i++) {
        for(int j = 0; j < retorno.numero_vertices; j++) {
            flag[i][j] = i;
        }
    }

    if(retorno.valorado == 0 && retorno.tipo == 'd') {
            for (int i = 0; i < retorno.numero_vertices; i++) {
            struct aresta * aresta = retorno.grafo[i].lista_adj;
                while (aresta != NULL) {
                    contador_aux1[i][aresta->destino] += 1;
                    matriz_adj2[i][aresta->destino] = contador_aux1[i][aresta->destino];
                    aresta = aresta->proxima;
                }
            }
    } else if(retorno.valorado == 0 && retorno.tipo == 'g') {
            for (int i = 0; i < retorno.numero_vertices; i++) {
            struct aresta * aresta = retorno.grafo[i].lista_adj;
                while (aresta != NULL) {
                    if(flag[i][aresta->destino] != i) {
                        aresta = aresta->proxima;
                        continue;
                    } else {
                        contador_aux2[i][aresta->destino] += 1;
                        if(i != aresta->destino) { 
                            matriz_adj2[i][aresta->destino] = contador_aux2[i][aresta->destino];
                            matriz_adj2[aresta->destino][i] = contador_aux2[aresta->destino][i];
                        } else {
                            contador_aux2[aresta->destino][i] += 0;
                            matriz_adj2[i][aresta->destino] = contador_aux2[i][aresta->destino];
                            matriz_adj2[aresta->destino][i] = contador_aux2[i][aresta->destino];
                        }
                    aresta = aresta->proxima;
                    }
                }
            }
    } else if(retorno.valorado == 1 && retorno.tipo == 'd') {
        for (int i = 0; i < retorno.numero_vertices; i++) {
            struct aresta * aresta = retorno.grafo[i].lista_adj;
                while (aresta != NULL) {
                    contador_aux2[i][aresta->destino] += aresta->peso;
                    matriz_adj2[i][aresta->destino] = contador_aux2[i][aresta->destino];
                    contador_vezes_pesos[i][aresta->destino] += 1;
                    aresta = aresta->proxima;
                }
            }
    } else if(retorno.valorado == 1 && retorno.tipo == 'g') {
        for (int i = 0; i < retorno.numero_vertices; i++) {
            struct aresta * aresta = retorno.grafo[i].lista_adj;
                while (aresta != NULL) {
                    if(flag[i][aresta->destino] != i) {
                        aresta = aresta->proxima;
                        continue;
                    } else {
                        contador_aux2[i][aresta->destino] += aresta->peso;
                        contador_vezes_pesos[i][aresta->destino] += 1;
                        if(i != aresta->destino) { 
                            matriz_adj2[i][aresta->destino] = contador_aux2[i][aresta->destino];
                            matriz_adj2[aresta->destino][i] = contador_aux2[aresta->destino][i];
                            contador_vezes_pesos[aresta->destino][i] += 1;
                        } else {
                            contador_aux2[aresta->destino][i] += 0;
                            matriz_adj2[i][aresta->destino] = contador_aux2[i][aresta->destino];
                            matriz_adj2[aresta->destino][i] = contador_aux2[i][aresta->destino];
                            contador_vezes_pesos[aresta->destino][i] += 0;
                        }
                    aresta = aresta->proxima;
                    }
                }
            }
    }

    struct retorno ret;
    ret.matriz_adj = matriz_adj2;
    ret.numero_vertices = retorno.numero_vertices;
    ret.numero_arestas = retorno.numero_arestas;
    ret.tipo = retorno.tipo;
    ret.valorado = retorno.valorado;
    
    return ret;
}

struct retorno_lista converter_matriz_lista(struct retorno retorno) {

    struct vertice * grafo_aux = (struct vertice*)malloc(MAX_VERTICES * sizeof(struct vertice));

    for (int i = 0; i < retorno.numero_vertices; i++) {
        criar_vertice(&grafo_aux[i], i);
    }
    
    if(retorno.valorado == 0) {
        for(int i = 0; i < retorno.numero_vertices; i++) {
            for(int j = 0; j < retorno.numero_vertices; j++) {
                if(retorno.matriz_adj[i][j] != 0) {
                    if(retorno.matriz_adj[i][j] != 1) {
                        for(int k = 0; k < retorno.matriz_adj[i][j]; k++) {
                            adicionar_aresta(&grafo_aux[i], j, -1);
                        }
                        continue;
                    }
                    adicionar_aresta(&grafo_aux[i], j, -1);
                }
            }
        }
    } else if(retorno.valorado == 1) {
        for(int i = 0; i < retorno.numero_arestas; i++) {
            for(int j = 0; j < retorno.numero_arestas; j++) {
                if(retorno.quais_pesos[i][j] != 0) adicionar_aresta(&grafo_aux[i], j, retorno.quais_pesos[i][j]);
            }
        }
    }

    struct retorno_lista ret;

    ret.grafo = grafo_aux;
    ret.numero_vertices = retorno.numero_vertices;
    ret.numero_arestas = retorno.numero_arestas;
    ret.tipo = retorno.tipo;
    ret.valorado = retorno.valorado;

    return ret;
    
}

void calcular_graus(struct retorno ret, struct pesos contador_pesos) {

    if(ret.tipo == 'g' && ret.valorado == 0) {
        for(int i = 0; i < ret.numero_vertices; i++) {
            int grau = 0;
            for(int j = 0; j < ret.numero_vertices; j++) {
                if(ret.matriz_adj[i][j] != 0) {
                    grau += ret.matriz_adj[i][j];
                }
            }
            printf("O vértice %d tem grau %d.\n", i + 1, grau);
        }
        putchar('\n');
        putchar('\n');
    } else if(ret.tipo == 'd' && ret.valorado == 0) {
        for(int i = 0; i < ret.numero_vertices; i++) {
            int grau_saida = 0;
            for(int j = 0; j < ret.numero_vertices; j++) {
                if(ret.matriz_adj[i][j] != 0) {
                    grau_saida += ret.matriz_adj[i][j];
                }
            }
            printf("O vértice %d tem grau de saída %d.\n", i + 1, grau_saida);
        }
        putchar('\n');
        for(int i = 0; i < ret.numero_vertices; i++) {
            int grau_entrada = 0;
            for(int j = 0; j < ret.numero_vertices; j++) {
                if(ret.matriz_adj[j][i] != 0) {
                    grau_entrada += ret.matriz_adj[j][i];
                }
            }
            printf("O vértice %d tem grau de entrada %d.\n", i + 1, grau_entrada);
        }
        putchar('\n');
        putchar('\n');
    } else if(ret.tipo == 'g' && ret.valorado == 1) {
        for(int i = 0; i < ret.numero_vertices; i++) {
            int grau = 0;
            for(int j = 0; j < ret.numero_vertices; j++) {
                if(contador_pesos.matriz_contador[i][j] != 0) {
                    grau += contador_pesos.matriz_contador[i][j];
                }
            }
            printf("O vértice %d tem grau %d.\n", i + 1, grau);
        }
        putchar('\n');
        putchar('\n');
    } else if(ret.tipo == 'd' && ret.valorado == 1) {
        for(int i = 0; i < ret.numero_vertices; i++) {
            int grau_saida_ponderado = 0;
            for(int j = 0; j < ret.numero_vertices; j++) {
                if(contador_pesos.matriz_contador[i][j] != 0) {
                    grau_saida_ponderado += contador_pesos.matriz_contador[i][j];
                }
            }
            printf("O vértice %d tem grau de saída %d.\n", i + 1, grau_saida_ponderado);
        }
        putchar('\n');
        for(int i = 0; i < ret.numero_vertices; i++) {
            int grau_entrada_ponderado = 0;
            for(int j = 0; j < ret.numero_vertices; j++) {
                if(contador_pesos.matriz_contador[j][i] != 0) {
                    grau_entrada_ponderado += contador_pesos.matriz_contador[j][i];
                }
            }
            printf("O vértice %d tem grau de entrada %d.\n", i + 1, grau_entrada_ponderado);
        }
        putchar('\n');
    }
}

struct pesos contador_pesos(FILE * fp) {
    
    rewind(fp);

    int num_vertices, num_arestas;
    char tipo;
    int valorado;
    int contador_loop = 0;

    fscanf(fp, "%d %d %c %d", &num_vertices, &num_arestas, &tipo, &valorado);
    int ** contador_pesos = (int**)malloc(num_vertices * sizeof(int *));

    for (int i = 0; i < num_vertices; i++) {
        contador_pesos[i] = (int *)malloc(num_vertices * sizeof(int));
    }

    if(valorado == 1 && tipo == 'g') {

        while(contador_loop < num_arestas) {
            int v1, v2, peso;
            fscanf(fp, "%d %d %d", &v1, &v2, &peso);
            contador_pesos[v1 - 1][v2 - 1] += 1;
            if(v1 != v2) { 
                contador_pesos[v2 - 1][v1 - 1] += 1;
            } else {
                contador_pesos[v2 - 1][v1 - 1] += 0;
            }
            contador_loop++;
        }
    } else if(valorado == 1 && tipo == 'd') {

        while(contador_loop < num_arestas) {
            int v1, v2, peso;
            fscanf(fp, "%d %d %d", &v1, &v2, &peso);
            contador_pesos[v1 - 1][v2 - 1] += 1;
            contador_loop++;
        }
    }

    struct pesos ret;

    ret.matriz_contador = contador_pesos;

    return ret;

}

void algoritmo_prim(struct vertice * grafo, int num_vertices, char tipo, int valorado) {
    
    FILE * fp_aux = fopen("arvore_geradora_minima.txt", "w");

    if(fp_aux == NULL) {
        printf("Falha ao abrir o arquivo de árvores geradoras mínimas.\n");
        exit(1);
    }

    int chave[num_vertices];
    int conjunto_arvore[num_vertices];
    int pai[num_vertices];

    for (int i = 0; i < num_vertices; i++) {
        chave[i] = INT_MAX;
        conjunto_arvore[i] = 0;
    }

    chave[0] = 0;
    pai[0] = -1;

    for (int contador = 0; contador < num_vertices - 1; contador++) {

        int u, v, peso_minimo = INT_MAX;

        for (int i = 0; i < num_vertices; i++) {
            if (!conjunto_arvore[i] && chave[i] < peso_minimo) {
                u = i;
                peso_minimo = chave[i];
            }
        }

        conjunto_arvore[u] = 1;

        struct aresta * aresta = grafo[u].lista_adj;
        while (aresta != NULL) {
            v = aresta->destino;
            int peso = aresta->peso;
            
            if (!conjunto_arvore[v] && peso < chave[v]) {
                pai[v] = u;
                chave[v] = peso;
            }
            
            aresta = aresta->proxima;
        }
    }

    int contador_arestas = 0;

    for (int i = 1; i < num_vertices; i++) contador_arestas++;

    fprintf(fp_aux, "%d %d %c %d\n", num_vertices, contador_arestas, tipo, valorado);

    for (int i = 1; i < num_vertices; i++) fprintf(fp_aux, "%d %d %d\n", pai[i] + 1, i + 1, chave[i]);

    fclose(fp_aux);

}

struct retorno_lista gerar_arvore_minima(struct retorno_lista ret) {

    if(ret.tipo == 'g' && ret.valorado == 1) {
        algoritmo_prim(ret.grafo, ret.numero_vertices, ret.tipo, ret.valorado);
        FILE * fp_aux = fopen("arvore_geradora_minima.txt", "r");
        rewind(fp_aux);
        struct retorno_lista auxiliar2 = gerar_lista_adjacencias(fp_aux);
        return auxiliar2;
    } else if(ret.tipo == 'd' && ret.valorado == 1) {
        printf("O grafo fornecido é direcionado e valorado, não é possível gerar uma árvore geradora mínima nessa implementação!\n");
    } else {
        printf("O grafo fornecido não é valorado!\n\n");
        exit(1);
    }
}

int distancia_menor(int * distancia, int * menor_caminho, int num_vertices) {
    int minimo = INT_MAX, indice_minimo = -1;

    for (int i = 0; i < num_vertices; i++) {
        if (!menor_caminho[i] && distancia[i] <= minimo) {
            minimo = distancia[i];
            indice_minimo = i;
        }
    }

    return indice_minimo;
}

void algoritmo_dijkstra(int ** grafo, int origem, int num_vertices, char tipo, int valorado) {

    FILE * fp_aux = fopen("caminho_mais_curto.txt", "w");

    if(fp_aux == NULL) {
        printf("Falha ao abrir o arquivo de caminhos mais curtos.\n");
        exit(1);
    }

    FILE * fp_aux2 = fopen("distancia_origem.txt", "w");

    if(fp_aux2 == NULL) {
        printf("Falha ao abrir o arquivo de distâncias da origem.\n");
        exit(1);
    }

    int * distancia = (int *)malloc(num_vertices * sizeof(int));
    int * menor_caminho = (int *)malloc(num_vertices * sizeof(int));
    int * pai = (int *)malloc(num_vertices * sizeof(int));

    for (int i = 0; i < num_vertices; i++) {
        distancia[i] = INT_MAX;
        menor_caminho[i] = 0;
    }

    distancia[origem] = 0, pai[origem] = -1;

    for (int contador = 0; contador < num_vertices - 1; contador++) {
        int u = distancia_menor(distancia, menor_caminho, num_vertices);
        menor_caminho[u] = 1;

        for (int i = 0; i < num_vertices; i++) {
            if (!menor_caminho[i] && grafo[u][i] && distancia[u] != INT_MAX && distancia[u] + grafo[u][i] < distancia[i]) {
                distancia[i] = distancia[u] + grafo[u][i];
                pai[i] = u;
            }
        }
    }
    
    int contador_arestas_caminhos_minimos = 0;

    for(int i = 1; i < num_vertices; i++) contador_arestas_caminhos_minimos++;

    fprintf(fp_aux, "%d %d %c %d\n", num_vertices, contador_arestas_caminhos_minimos, tipo, valorado);

    if(tipo == 'g') {
        for (int i = 0; i < num_vertices; i++) {
            if(pai[i] == -1) continue;
            else fprintf(fp_aux, "%d %d %d\n", pai[i] + 1, i + 1, grafo[i][pai[i]]);
        } 
    } else if(tipo == 'd') {
        for (int i = 0; i < num_vertices; i++) {
            if(pai[i] == -1) continue;
            else fprintf(fp_aux, "%d %d %d\n", pai[i] + 1, i + 1, grafo[pai[i]][i]); 
        } 
    }

    fprintf(fp_aux2, "Vértice\t Distância da fonte %d\n", origem + 1);

    for (int i = 0; i < num_vertices; i++) fprintf(fp_aux2, "%d \t\t %d\n", i + 1, distancia[i]);

    fclose(fp_aux);
    fclose(fp_aux2);

}

struct retorno_lista caminho_mais_curto(struct retorno_lista ret, int origem) {

    if(ret.tipo == 'g' && ret.valorado == 1) {
        algoritmo_dijkstra(ret.matriz_adj, origem - 1, ret.numero_vertices, ret.tipo, ret.valorado);
        FILE * fp_aux = fopen("caminho_mais_curto.txt", "r");
        rewind(fp_aux);
        struct retorno_lista auxiliar2 = gerar_lista_adjacencias(fp_aux);
        return auxiliar2;
    } else if(ret.tipo == 'd' && ret.valorado == 1) {
        algoritmo_dijkstra(ret.matriz_adj, origem - 1, ret.numero_vertices, ret.tipo, ret.valorado);
        FILE * fp_aux = fopen("caminho_mais_curto.txt", "r");
        rewind(fp_aux);
        struct retorno_lista auxiliar2 = gerar_lista_adjacencias(fp_aux);
        return auxiliar2;
    } else {
        printf("O grafo fornecido não é valorado!\n\n");
        exit(1);
    }
}

void print_caminho_bfs(int origem, int destino, int * pai) {

    if(origem == destino) printf("%d ", origem + 1);
    else {
        if(pai[destino] == -1) {
            printf("Não há caminho entre a origem e o vértice destino em questão!\n");
            return;
        } else {
            print_caminho_bfs(origem, pai[destino], pai);
            printf("%d ", destino + 1);
        }
    }
    
}

void algoritmo_bfs(int ** matriz, int numero_vertices, int origem, bool * visitado, int * distancia, int * pai) {

    int inicio_fila = 0, fim_fila = 0;
    visitado[origem] = true;
    distancia[origem] = 0;
    pai[origem] = -1;

    int * fila_bfs = (int *)malloc(MAX_VERTICES * sizeof(int));
    fila_bfs[fim_fila++] = origem;

    while(inicio_fila != fim_fila) {
        int atual = fila_bfs[inicio_fila++];
        for(int i = 0; i < numero_vertices; i++) {
            if(matriz[atual][i] != 0 && !visitado[i]) {
                visitado[i] = true;
                pai[i] = atual;
                distancia[i] = distancia[atual] + 1;
                fila_bfs[fim_fila++] = i;
            }
        }
    }

}

struct print bfs_floresta(struct retorno_lista ret, int origem) {

    FILE * fp_aux = fopen("busca_largura.txt", "w");

    if(fp_aux == NULL) {
        printf("Falha ao abrir o arquivo de busca em largura.\n");
        exit(1);
    }

    bool * visitado = (bool *)malloc(ret.numero_vertices * sizeof(bool));
    int * distancia = (int *)malloc(ret.numero_vertices * sizeof(int));
    int * pai = (int *)malloc(ret.numero_vertices * sizeof(int));
    int inicio_fila = 0, fim_fila = 0;

    for(int i = 0; i < ret.numero_vertices; i++) {
        visitado[i] = false;
        pai[i] = -1;
        distancia[i] = INT_MAX;
    }

    for(int i = 0; i < ret.numero_vertices; i++) {
        if(visitado[i] == false) algoritmo_bfs(ret.matriz_adj, ret.numero_vertices, i, visitado, distancia, pai);
    }

    int contador_arestas = 0;

    for (int i = 0; i < ret.numero_vertices; i++) {
        for (int k = 0; k < ret.numero_vertices; k++) {
            if (pai[k] == i) {
                contador_arestas++;
            }
        }
    }

    fprintf(fp_aux, "%d %d %c %d\n", ret.numero_vertices, contador_arestas, ret.tipo, ret.valorado);

    for (int i = 0; i < ret.numero_vertices; i++) {
        for (int j = 0; j < ret.numero_vertices; j++) {
            if (pai[j] == i) {
                fprintf(fp_aux, "%d %d\n", pai[j] + 1, j + 1);
            }
        }
    }

    fclose(fp_aux);

    struct print aux;
    aux.pai = pai;

    return aux;

}

struct retorno_lista busca_largura(struct retorno_lista ret, int origem) {

    if(ret.tipo == 'g' && ret.valorado == 0) {
        struct print auxiliar3 = bfs_floresta(ret, origem - 1);
        FILE * fp_aux = fopen("busca_largura.txt", "r");
        rewind(fp_aux);
        struct retorno_lista auxiliar2 = gerar_lista_adjacencias(fp_aux);
        printf("\nLista de Adjacências da Árvore BFS:\n\n");
        imprimir_lista_adjacencias(auxiliar2.grafo, auxiliar2.numero_vertices, auxiliar2.valorado);
        putchar('\n');
        int imprimir;
        printf("Deseja imprimir o menor caminho entre a origem e um vértice?\n");
        scanf("%d", &imprimir);
        if(imprimir == 1) {
            int destino;
            printf("Informe o vértice destino.\n");
            scanf("%d", &destino);
            print_caminho_bfs(origem - 1, destino - 1, auxiliar3.pai);
            putchar('\n');
        }
        return auxiliar2;
    } else if(ret.tipo == 'd' && ret.valorado == 0) {
        struct print auxiliar3 = bfs_floresta(ret, origem - 1);
        FILE * fp_aux = fopen("busca_largura.txt", "r");
        rewind(fp_aux);
        struct retorno_lista auxiliar2 = gerar_lista_adjacencias(fp_aux);
        printf("\nLista de Adjacências da Árvore BFS:\n\n");
        imprimir_lista_adjacencias(auxiliar2.grafo, auxiliar2.numero_vertices, auxiliar2.valorado);
        putchar('\n');
        int imprimir;
        printf("Deseja imprimir o menor caminho entre a origem e um vértice?\n");
        scanf("%d", &imprimir);
        if(imprimir == 1) {
            int destino;
            printf("Informe o vértice destino.\n");
            scanf("%d", &destino);
            putchar('\n');
            print_caminho_bfs(origem - 1, destino - 1, auxiliar3.pai);
            putchar('\n');
        }
        return auxiliar2;
    } else {
        printf("O grafo fornecido não pode ser valarado!\n\n");
        exit(1);
    }
}

typedef enum {BRANCO, CINZA, PRETO} COR;

void escrever_arestas_dfs_arquivo(struct retorno_lista ret) {

    FILE* fp_aux = fopen("busca_profundidade.txt", "w");

    if (fp_aux == NULL) {
        printf("Erro ao abrir o arquivo de busca em profundidade!\n");
        exit(1);
    }

    fprintf(fp_aux, "%d %d %c %d\n", ret.numero_vertices, contador_aresta_dfs, ret.tipo, ret.valorado);

    for (int i = 0; i < contador_aresta_dfs; i++) fprintf(fp_aux, "%d %d\n", floresta_dfs[i]->origem, floresta_dfs[i]->destino);

    fclose(fp_aux);
}

void dfs_formar_arvore(struct vertice * grafo, int u, COR * cor, int * descoberta, int * finalizacao, int * pi) {
    
    cor[u] = CINZA; 
    tempo = tempo + 1;
    descoberta[u] = tempo;

    struct aresta * aresta = grafo[u].lista_adj;
    while(aresta != NULL) {
        int i = aresta->destino;
        if(cor[i] == BRANCO) {
            pi[i] = u;
            adicionar_aresta_dfs(u + 1, i + 1);
            dfs_formar_arvore(grafo, i, cor, descoberta, finalizacao, pi);
        }
        aresta = aresta->proxima;
    }

    cor[u] = PRETO; 
    tempo = tempo + 1;
    finalizacao[u] = tempo;

}

void algoritmo_dfs(struct retorno_lista ret) {
    
    COR * cor = (COR*)malloc(ret.numero_vertices * sizeof(COR));
    int * descoberta = (int*)malloc(ret.numero_vertices * sizeof(int));
    int * finalizacao = (int*)malloc(ret.numero_vertices * sizeof(int));
    int * pi = (int*)malloc(ret.numero_vertices * sizeof(int));

    for (int i = 0; i < ret.numero_vertices; i++) {
        cor[i] = BRANCO;
        pi[i] = -1;
    }

    tempo = 0;

    for (int u = 0; u < ret.numero_vertices; u++) {
        if (cor[u] == BRANCO) dfs_formar_arvore(ret.grafo, u, cor, descoberta, finalizacao, pi);
    }

    escrever_arestas_dfs_arquivo(ret);

}

struct retorno_lista busca_profundidade(struct retorno_lista ret) {

    if(ret.tipo == 'g' && ret.valorado == 0) {
        algoritmo_dfs(ret);
        FILE * fp_aux = fopen("busca_profundidade.txt", "r");
        rewind(fp_aux);
        struct retorno_lista auxiliar2 = gerar_lista_adjacencias(fp_aux);
        printf("\nLista de Adjacências da Floresta DFS:\n\n");
        imprimir_lista_adjacencias(auxiliar2.grafo, auxiliar2.numero_vertices, auxiliar2.valorado);
        putchar('\n');
        return auxiliar2;
    } else if(ret.tipo == 'd' && ret.valorado == 0) {
        algoritmo_dfs(ret);
        FILE * fp_aux = fopen("busca_profundidade.txt", "r");
        rewind(fp_aux);
        struct retorno_lista auxiliar2 = gerar_lista_adjacencias(fp_aux);
        printf("\nLista de Adjacências da Floresta DFS:\n\n");
        imprimir_lista_adjacencias(auxiliar2.grafo, auxiliar2.numero_vertices, auxiliar2.valorado);
        putchar('\n');
        return auxiliar2;
    } else {
        printf("O grafo fornecido não pode ser valarado!\n\n");
        exit(1);
    }
}

int main() {

    int funcao, contador = 0, flag3 = 0, flag4 = 0;
    char flag = 'z';

    FILE * fp = fopen("teste_g_0.txt", "r");

    if(fp == NULL) {
        printf("Falha ao abrir o arquivo principal.\n");
        exit(1);
    }

    struct pesos retorno_pesos = contador_pesos(fp);

    struct retorno ret;
    struct retorno_lista retorno;
    
    if(funcao == 3) {
        printf("Primeiro deve-se gerar um grafo em memória!\n");
        exit(1);
    }

    while(1) {

        printf("Escolha a função desejada\n");

        printf("1 - Gerar Matriz de Adjacencias\n2 - Gerar Lista de Adjacencias\n3 - Gerar arquivo baseado no grafo\n4 - Converção LA->MA\n5 - Converção MA->LA\n6 - Calcular os Graus de cada vértice\n7 - Encontrar uma árvore geradora mínima\n8 - Encontrar um caminho mais curto usando Algoritmo de Dijkstra\n9 - Busca em Largura (BFS)\n10 - Busca em Profundidade (DFS)\n");

        scanf("%d", &funcao);
        if(funcao == 3 && contador < 1) {
            printf("\nPrimeiro deve-se gerar um grafo em memória!\n");
            exit(1);
        }

        if(funcao == 0) break;
        else if(funcao == 1) {
            ret = gerar_matriz_adjacencias(fp);
            printf("\nMatriz de Adjacências do grafo fornecido:\n");
            for (int i = 0; i < ret.numero_vertices; i++) {
                for (int j = 0; j < ret.numero_vertices; j++) {
                    printf("| %2d |", ret.matriz_adj[i][j]); 
                }
            printf("\n");
            }
            putchar('\n');
            printf("Atenção: Agora o grafo em memória é a Matriz de Adjacências do arquivo principal!\n\n");
            flag = 'a';
        } else if(funcao == 2) {
            printf("\nLista de Adjacências do grafo fornecido:\n");
            retorno = gerar_lista_adjacencias(fp);
            imprimir_lista_adjacencias(retorno.grafo, retorno.numero_vertices, retorno.valorado);
            putchar('\n');
            printf("Atenção: Agora o grafo em memória é a Lista de Adjacências do arquivo principal!\n\n");
            flag = 'b';
        } else if(funcao == 3) {
            if(flag3 == 1) {
                gerar_arquivo_lista(retorno);
                flag3 = 0;
                continue;
            }
            if(flag4 == 1) {
                gerar_arquivo_lista(retorno);
                flag4 = 0;
                continue;
            }
            if(flag == 'a' || flag == 'c') gerar_arquivo(ret);
            else if(flag == 'b' || flag == 'd') gerar_arquivo_lista(retorno);
            else printf("\nPrimeiro deve-se ter um grafo em memória!\n\n");
        } else if(funcao == 4) {
            if(flag == 'b' || flag == 'd') {
                printf("\nMatriz de Adjacências correspondente à lista:\n");
                ret = converter_lista_matriz(retorno);
                for(int i = 0; i < ret.numero_vertices; i++) {
                    for(int j = 0; j < ret.numero_vertices; j++) {
                        printf("| %2d |", ret.matriz_adj[i][j]);
                    }
                    putchar('\n');
                }
                flag = 'c';
                putchar('\n');
            } else printf("\nPrimeiro deve-se ter uma LA em memória!\n");
        } else if(funcao == 5) {
            if(flag == 'a' || flag == 'c') {
                printf("\nLista de Adjacências correspondente à matriz:\n");
                retorno = converter_matriz_lista(ret);
                imprimir_lista_adjacencias(retorno.grafo, retorno.numero_vertices, retorno.valorado);
                putchar('\n');
                flag = 'd';
            }
            else printf("\nPrimeiro deve-se gerar uma MA em memória.\n");
        } else if(funcao == 6) {
            if(flag == 'a' || flag == 'c') calcular_graus(ret, retorno_pesos);
            else if(flag == 'b' || flag == 'd') {
              ret = converter_lista_matriz(retorno);  
              calcular_graus(ret, retorno_pesos);  
            }
            else printf("\nPrimeiro deve-se gerar um grafo em memória!\n\n");
        } else if(funcao == 7) {
            if(flag == 'b') {
                retorno = gerar_arvore_minima(retorno);
                printf("\nLista de Adjacências da árvore geradora mínima:\n\n");
                imprimir_lista_adjacencias(retorno.grafo, retorno.numero_vertices, retorno.valorado);
                putchar('\n');
                flag3 = 1;
                printf("\nAtenção: Agora o grafo em memória é a Árvore geradora mínima!\n");
                remove("arvore_geradora_minima.txt");
            } else {
                retorno = gerar_lista_adjacencias(fp);
                retorno = gerar_arvore_minima(retorno);
                printf("\nLista de Adjacências da árvore geradora mínima:\n\n");
                imprimir_lista_adjacencias(retorno.grafo, retorno.numero_vertices, retorno.valorado);
                putchar('\n');
                flag = 'b';
                flag3 = 1;
                printf("\nAtenção: Agora o grafo em memória é a Árvore geradora mínima!\n\n");
                remove("arvore_geradora_minima.txt");
            }
        } else if(funcao == 8) {
            if(flag == 'b') {
                int origem;
                printf("\nDigite a origem do caminho mais curto:\n");
                scanf("%d", &origem);
                retorno = caminho_mais_curto(retorno, origem);
                printf("\nLista de Adjacências do caminho mais curto:\n\n");
                imprimir_lista_adjacencias(retorno.grafo, retorno.numero_vertices, retorno.valorado);
                putchar('\n');
                flag4 = 1;
                printf("\nAtenção: Agora o grafo em memória é a Árvore de Caminho Mínimo!\n\n");
                remove("caminho_mais_curto.txt");
            } else {
                int origem;
                printf("\nDigite a origem do caminho mais curto:\n");
                scanf("%d", &origem);
                retorno = gerar_lista_adjacencias(fp);
                retorno = caminho_mais_curto(retorno, origem);
                printf("\nLista de Adjacências do caminho mais curto:\n\n");
                imprimir_lista_adjacencias(retorno.grafo, retorno.numero_vertices, retorno.valorado);
                putchar('\n');
                flag4 = 1;
                flag = 'b';
                printf("\nAtenção: Agora o grafo em memória é a Árvore de Caminho Mínimo!\n\n");
                remove("caminho_mais_curto.txt");
            }
        } else if(funcao == 9) {
            int origem;
            printf("\nDigite o vértice de origem da busca em largura:\n");
            scanf("%d", &origem);
            putchar('\n');
            if(flag == 'b') {
                retorno = busca_largura(retorno, origem);
                flag4 = 1;
                printf("\nAtenção: Agora o grafo em memória é a Árvore BFS!\n\n");
                remove("busca_largura.txt");
            } else {
                retorno = gerar_lista_adjacencias(fp);
                retorno = busca_largura(retorno, origem);
                flag4 = 1;
                flag = 'b';
                printf("\nAtenção: Agora o grafo em memória é a Árvore BFS!\n\n");
                remove("busca_largura.txt");
            }
        } else if(funcao == 10) {
            if(flag == 'b') {
                retorno = busca_profundidade(retorno);
                flag4 = 1;
                printf("\nAtenção: Agora o grafo em memória é a Floresta DFS!\n\n");
                remove("busca_profundidade.txt");
            } else {
                retorno = gerar_lista_adjacencias(fp);
                retorno = busca_profundidade(retorno);
                flag4 = 1;
                flag = 'b';
                printf("\nAtenção: Agora o grafo em memória é a Floresta DFS!\n\n");
                remove("busca_profundidade.txt");
            }
        }

        contador++;
    }

    fclose(fp);
}