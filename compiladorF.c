
/* -------------------------------------------------------------------
 *            Aquivo: compilador.c
 * -------------------------------------------------------------------
 *              Autor: Bruno Muller Junior
 *               Data: 08/2007
 *      Atualizado em: [15/03/2012, 08h:22m]
 *
 * -------------------------------------------------------------------
 *
 * Fun��es auxiliares ao compilador
 *
 * ------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compilador.h"


/* -------------------------------------------------------------------
 *  vari�veis globais
 * ------------------------------------------------------------------- */

/* Arquivo MEPA */
FILE* fp;

/* Indica qual � o rotulo */
int valor_rotulo;

void inicia_variaveis_globais () {

    fp = NULL;
    valor_rotulo = 0;
    num_vars = 0;
    num_vars_inicial = 0;
    nivel_lexico = 0;
    desloc = 0;

    dados = malloc ( sizeof (char)*TAM_TOKEN);
    categoria = malloc ( sizeof (char)*TAM_TOKEN);

}

/* Fun��o que gera c�digo MEPA */
void geraCodigo (char* rot, char* comando) {

  if (fp == NULL) {
    fp = fopen ("MEPA", "w");
  }

  if ( rot == NULL ) {
    fprintf(fp, "     %s\n", comando); fflush(fp);
  } else {
    fprintf(fp, "%s: %s \n", rot, comando); fflush(fp);
  }
}

/* Fun��o que imprime erros de compila��o */
int imprimeErro ( char* erro ) {
  fprintf (stderr, "Erro na linha %d - %s\n", nl, erro);
  exit(-1);
}

/* Inicia pilha da tabela de simbolos */
void inicia_pilha_tabela_simbolos () {

    p_tb_simb = malloc( sizeof (pilha_tb_simb));
    p_tb_simb->primeiro = NULL;
    p_tb_simb->tam = 0;
}

/* Fun��o que impreime pilha da tabela de simbolos */
void imprime_Simbolo_TB_SIMB () {

    printf("\n\nTABELA DE SIMBOLOS\n\n");

    no_tabela_simbolos_p *slot_tb_simb_aux;

    slot_tb_simb_aux = p_tb_simb->primeiro;
    while ( slot_tb_simb_aux != NULL) {
        printf("|| %s | %s | %s | %d | %d ||\n", slot_tb_simb_aux->simbolo, slot_tb_simb_aux->tipo, slot_tb_simb_aux->categoria, slot_tb_simb_aux->nivel_lexico, slot_tb_simb_aux->desloc);
        slot_tb_simb_aux = slot_tb_simb_aux->prox;
    }

}

/* Fun��o que insere um simbolo na tabela de simbolos */
void empilha_Simbolo_TB_SIMB ( char *simb, char *ca, int nivel_l, int des) {

    char *simbol = malloc ( sizeof ( char)*TAM_TOKEN);
    char *cate = malloc ( sizeof ( char)*TAM_TOKEN);

    strcpy( simbol, simb);
    strcpy( cate, ca);

    no_tabela_simbolos_p *novo_slot_tb_simb = malloc( sizeof ( no_tabela_simbolos_p ));

    novo_slot_tb_simb->simbolo = simbol;
    novo_slot_tb_simb->categoria = cate;
    novo_slot_tb_simb->nivel_lexico = nivel_l;
    novo_slot_tb_simb->desloc = des;

    novo_slot_tb_simb->prox = p_tb_simb->primeiro;

    p_tb_simb->primeiro = novo_slot_tb_simb;
    p_tb_simb->tam++;

    imprime_Simbolo_TB_SIMB ();

}

/* Fun��o que insere tipo do simbolo na tabela de simbolos */
void insere_tipo_Simbolo_TB_SIMB ( char *ti, int qtd_vars) {

    char *tip = malloc ( sizeof ( char)*TAM_TOKEN);
    int i;

    strcpy( tip, ti);

    no_tabela_simbolos_p *slot_tb_simb_aux = p_tb_simb->primeiro;

    for ( i = 0; i < qtd_vars; i++) {
        slot_tb_simb_aux->tipo = tip;
        slot_tb_simb_aux = slot_tb_simb_aux->prox;
    }

    imprime_Simbolo_TB_SIMB ();

}

/* Fun��o que retira um simbolo da tabela de simbolos */
void desempilha_Simbolo_TB_SIMB ( char **simb) {

    no_tabela_simbolos_p *slot_tb_simb_retirado;

    if ( p_tb_simb->tam == 0){

        *simb = NULL;
    }
    else
    {
        slot_tb_simb_retirado = p_tb_simb->primeiro;
        p_tb_simb->primeiro = slot_tb_simb_retirado->prox;
        p_tb_simb->tam--;

        *simb = slot_tb_simb_retirado->simbolo;

    }

    imprime_Simbolo_TB_SIMB ();

}

/* Fun��o que procura um simbolo na tabela de simbolos */
/* Se encontra retorna o nivel e deslocamento */
/* Se n�o encontra retorna NULL */
procura_simb ( char *simb, int *nivel_lexico, int *desloc ) {

    int i;
    *nivel_lexico = -99; // valor nao muda se nao encontrar simb na tabela
    *desloc = -99;

    no_tabela_simbolos_p *slot_tb_simb_aux = p_tb_simb->primeiro;

    while ( slot_tb_simb_aux != NULL && strcmp( slot_tb_simb_aux->simbolo, simb ) ){
        slot_tb_simb_aux = slot_tb_simb_aux->prox;
    }

    if ( slot_tb_simb_aux != NULL){
        *nivel_lexico = slot_tb_simb_aux->nivel_lexico;
        *desloc = slot_tb_simb_aux->desloc;
    }

}

/* Fun��o que gera pr�ximo rotulo */
void gera_Proximo_Rotulo (char **new_rotulo) {

    char *rotulo = malloc( sizeof (char) * TAM_ROTULO);

    sprintf( rotulo, "R%d", valor_rotulo);
    valor_rotulo++;

    *new_rotulo = rotulo;

}

/* Inicia pilha para rotulos */
void inicia_pilha_rotulos () {

    p_rotulos = malloc( sizeof (pilha_r));
    p_rotulos->primeiro = NULL;
    p_rotulos->tam = 0;
}

/* Fun��o que empilha rotulos */
void empilha_Rotulo ( char *rot ) {

    rotulos_p *novo_rotulo = malloc( sizeof ( rotulos_p ));

    novo_rotulo->rotulo = rot;
    novo_rotulo->prox = p_rotulos->primeiro;
    p_rotulos->primeiro = novo_rotulo;
    p_rotulos->tam++;

}

/* Fun��o que desempilha rotulos */
void desempilha_Rotulo ( char **rotulo) {

    rotulos_p *rotulo_retirado;

    if ( p_rotulos->tam == 0){

        *rotulo = NULL;
    }
    else
    {
        rotulo_retirado = p_rotulos->primeiro;
        p_rotulos->primeiro = rotulo_retirado->prox;
        p_rotulos->tam--;

        *rotulo = rotulo_retirado->rotulo;

    }
}

/* Inicia pilha para deslocamentos */
void inicia_pilha_deslocamentos () {

    p_deslocamentos = malloc( sizeof (pilha_d));
    p_deslocamentos->primeiro = NULL;
    p_deslocamentos->tam = 0;
}

/* Fun��o que empilha deslocamento */
void empilha_Deslocamento ( int deslocamento ) {

    deslocamentos_p *novo_deslocamento = malloc( sizeof ( deslocamentos_p ));

    novo_deslocamento->deslocamento = deslocamento;
    novo_deslocamento->prox = p_deslocamentos->primeiro;
    p_deslocamentos->primeiro = novo_deslocamento;
    p_deslocamentos->tam++;

}

/* Fun��o que desempilha deslocamentos */
int desempilha_Deslocamento () {

    deslocamentos_p *deslocamento_retirado;

    if ( p_deslocamentos->tam == 0){

        return -99;
    }
    else
    {
        deslocamento_retirado = p_deslocamentos->primeiro;
        p_deslocamentos->primeiro = deslocamento_retirado->prox;
        p_deslocamentos->tam--;

        return deslocamento_retirado->deslocamento;

    }
}
