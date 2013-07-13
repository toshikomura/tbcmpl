
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
    nivel_lexico = 0;
    desloc = 0;

    dados = malloc ( sizeof (char)*TAM_TOKEN);
    categoria = malloc ( sizeof (char)*TAM_TOKEN);
    categoria2 = malloc ( sizeof (char)*TAM_TOKEN);
    tipo = malloc ( sizeof (char)*TAM_TOKEN);
    tipo_valor_referencia = malloc ( sizeof (char)*TAM_TOKEN);
    tipo_retorno = malloc ( sizeof (char)*TAM_TOKEN);
    nome_var_proc_func = malloc ( sizeof (char)*TAM_TOKEN);

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

    printf ( "\n\nTABELA DE SIMBOLOS\n\n");

    printf ( "|| simb | tipo | categ | rotu | tip_param | tip_retor | niv lex | desloc | qtd_param\n");

    tipos_parametros_l *tip_para_aux;
    no_tabela_simbolos_p *slot_tb_simb_aux;

    slot_tb_simb_aux = p_tb_simb->primeiro;
    while ( slot_tb_simb_aux != NULL) {
        printf ( "|| %s | %s | %s | %s | %s | %d | %d | %d ", slot_tb_simb_aux->simbolo, slot_tb_simb_aux->tipo, slot_tb_simb_aux->categoria, slot_tb_simb_aux->rotulo, slot_tb_simb_aux->tipo_retorno, slot_tb_simb_aux->nivel_lexico, slot_tb_simb_aux->desloc, slot_tb_simb_aux->qtd_parametros);

        tip_para_aux = slot_tb_simb_aux->primeiro_tipo_parametro;

        while ( tip_para_aux != NULL) {
            printf ( "|- %s | %s ", tip_para_aux->valor_referencia, tip_para_aux->tipo);
            tip_para_aux = tip_para_aux->prox;
        }

        printf ( "||\n");

        slot_tb_simb_aux = slot_tb_simb_aux->prox;
    }

}

/* Fun��o que insere um simbolo na tabela de simbolos */
void empilha_Simbolo_TB_SIMB ( char *simb, char *ca, char *rot, int nivel_l, int des) {

    char *simbol = malloc ( sizeof ( char)*TAM_TOKEN);
    char *cate = malloc ( sizeof ( char)*TAM_TOKEN);

    strcpy( simbol, simb);
    strcpy( cate, ca);

    no_tabela_simbolos_p *novo_slot_tb_simb = malloc( sizeof ( no_tabela_simbolos_p ));

    novo_slot_tb_simb->simbolo = simbol;
    novo_slot_tb_simb->categoria = cate;
    novo_slot_tb_simb->rotulo = rot;

    novo_slot_tb_simb->nivel_lexico = nivel_l;
    novo_slot_tb_simb->desloc = des;
    novo_slot_tb_simb->qtd_parametros = 0;

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

/* Fun��o que insere tipo de parametro da fun��o ou procedimento na tabela de simbolos */
/* Al�m disso a fun��o insere a quantidade de parametros */
void insere_tipo_parametro_Simbolo_TB_SIMB ( char *simb, char *va_re, char *ti, int qtd_param) {

    char *tip = malloc ( sizeof ( char)*TAM_TOKEN);
    char *var_refer = malloc ( sizeof ( char)*TAM_TOKEN);
    int i, j;
    tipos_parametros_l *tip_para_aux;
    no_tabela_simbolos_p *slot_tb_simb_aux;

    strcpy ( tip, ti);
    strcpy ( var_refer, va_re);

    for ( j = 0; j < qtd_param; j++) {

        tipos_parametros_l *tip_para = malloc ( sizeof ( tipos_parametros_l ));
        slot_tb_simb_aux = p_tb_simb->primeiro;

        tip_para->valor_referencia = var_refer;
        tip_para->tipo = tip;
        tip_para->prox = NULL;

        while ( slot_tb_simb_aux != NULL && strcmp( slot_tb_simb_aux->simbolo, simb)) {
            slot_tb_simb_aux->tipo = tip;
            slot_tb_simb_aux = slot_tb_simb_aux->prox;
        }

        if ( slot_tb_simb_aux != NULL) {

            if ( slot_tb_simb_aux->qtd_parametros == 0) {
                slot_tb_simb_aux->primeiro_tipo_parametro = tip_para;
                slot_tb_simb_aux->qtd_parametros = 1;
            }
            else {

                tip_para_aux = slot_tb_simb_aux->primeiro_tipo_parametro;

                for ( i = 1; i < slot_tb_simb_aux->qtd_parametros; i++)
                    tip_para_aux = tip_para_aux->prox;

                tip_para_aux->prox = tip_para;
                slot_tb_simb_aux->qtd_parametros++;
            }
        }
    }

    imprime_Simbolo_TB_SIMB ();

}

/* Fun��o que insere tipo de retorno de uma fun��o ou procedimento na tabela de simbolos */
void insere_tipo_retorno_Simbolo_TB_SIMB ( char *simb, char *ti) {

    char *tip = malloc ( sizeof ( char)*TAM_TOKEN);

    strcpy ( tip, ti);

    no_tabela_simbolos_p *slot_tb_simb_aux = p_tb_simb->primeiro;

    while ( slot_tb_simb_aux != NULL && strcmp( slot_tb_simb_aux->simbolo, simb))
        slot_tb_simb_aux = slot_tb_simb_aux->prox;

    if ( slot_tb_simb_aux != NULL)
        slot_tb_simb_aux->tipo_retorno = tip;

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
/* Se encontra retorna o nivel e deslocamento pelo parametros */
/* Se n�o encontra retorna -99 */
void procura_simb ( char *simb, int *nivel_lexico, int *desloc, char **tip ) {

    *nivel_lexico = -99; // valor nao muda se nao encontrar simb na tabela
    *desloc = -99;

    no_tabela_simbolos_p *slot_tb_simb_aux = p_tb_simb->primeiro;

    while ( slot_tb_simb_aux != NULL && strcmp( slot_tb_simb_aux->simbolo, simb ) )
        slot_tb_simb_aux = slot_tb_simb_aux->prox;

    if ( slot_tb_simb_aux != NULL){
        *nivel_lexico = slot_tb_simb_aux->nivel_lexico;
        *desloc = slot_tb_simb_aux->desloc;
        *tip = slot_tb_simb_aux->tipo;
    }

}

/* Fun��o que procura um simbolo de uma certa categoria na tabela de simbolos */
/* Se encontra retorna 1 */
/* Se n�o encontra retorna -99 */
int procura_cat ( char *simb, char *cat, char **rot, char **tip, int *nivel_lexico, int *desloc) {

    no_tabela_simbolos_p *slot_tb_simb_aux = p_tb_simb->primeiro;

    printf ( "procurando pelo simbolo %s com categoria %s\n", simb, cat);

    while ( slot_tb_simb_aux != NULL && ( strcmp( slot_tb_simb_aux->simbolo, simb ) || strcmp( slot_tb_simb_aux->categoria, cat )) ) {
        slot_tb_simb_aux = slot_tb_simb_aux->prox;
//        if ( slot_tb_simb_aux != NULL)
//            printf ( "%s %s %d %d\n", slot_tb_simb_aux->simbolo, slot_tb_simb_aux->categoria, strcmp( slot_tb_simb_aux->simbolo, simb ), strcmp( slot_tb_simb_aux->categoria, cat ));
    }

    if ( slot_tb_simb_aux != NULL) {
        *rot = slot_tb_simb_aux->rotulo;
        *nivel_lexico = slot_tb_simb_aux->nivel_lexico;
        *desloc = slot_tb_simb_aux->desloc;
        *tip = slot_tb_simb_aux->tipo;
        printf ( "achou\n");

        return 1;

    }
    else {
        printf ( "n�o achou\n");

        return -99;

    }

}

/* Fun��o que gera pr�ximo rotulo */
void gera_Proximo_Rotulo ( char **new_rotulo) {

    char *rotulo = malloc( sizeof (char) * TAM_ROTULO);

    sprintf( rotulo, "R%d", valor_rotulo);
    valor_rotulo++;

    *new_rotulo = rotulo;

}

/* Inicia pilha para rotulos */
void inicia_pilha_strings () {

    p_rotulos = malloc( sizeof (pilha_s));
    p_rotulos->primeiro = NULL;
    p_rotulos->tam = 0;

    p_nomes = malloc( sizeof (pilha_s));
    p_nomes->primeiro = NULL;
    p_nomes->tam = 0;
}

/* Fun��o que empilha rotulos */
void empilha_String ( pilha_s *p, char *str ) {

    strings_p *nova_string = malloc( sizeof ( strings_p ));

    nova_string->string = str;
    nova_string->prox = p->primeiro;
    p->primeiro = nova_string;
    p->tam++;

    printf ( "ELEMENTO SENDO COLOCADO DA PILHA %s\n", nova_string->string);
}

/* Fun��o que desempilha rotulos */
void desempilha_String ( pilha_s *p, char **str) {

    strings_p *string_retirada;

    if ( p->tam == 0){

        *str = NULL;
    }
    else
    {

        string_retirada = p->primeiro;
        p->primeiro = string_retirada->prox;
        p->tam--;

        printf ( "ELEMENTO RETIRADO DA PILHA %s\n", string_retirada->string);

        *str = string_retirada->string;

    }
}

/* Inicia pilha para deslocamentos */
void inicia_pilha_inteiros () {

    p_deslocamentos = malloc ( sizeof ( pilha_i));
    p_deslocamentos->primeiro = NULL;
    p_deslocamentos->tam = 0;

    p_num_vars = malloc ( sizeof ( pilha_i));
    p_num_vars->primeiro = NULL;
    p_num_vars->tam = 0;
}

/* Fun��o que empilha deslocamento */
void empilha_Inteiro ( pilha_i *p, int inteiro) {

    inteiros_p *novo_inteiro = malloc( sizeof ( inteiros_p ));

    novo_inteiro->inteiro = inteiro;
    novo_inteiro->prox = p->primeiro;
    p->primeiro = novo_inteiro;
    p->tam++;

}

/* Fun��o que desempilha deslocamentos */
int desempilha_Inteiro ( pilha_i *p) {

    inteiros_p *inteiro_retirado;

    if ( p->tam == 0){
        return -99;
    }
    else
    {
        inteiro_retirado = p->primeiro;
        p->primeiro = inteiro_retirado->prox;
        p->tam--;

        return inteiro_retirado->inteiro;

    }
}
