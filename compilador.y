
// Testar se funciona corretamente o empilhamento de par�metros
// passados por valor ou por refer�ncia.


%{
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "compilador.h"

/* Vari�veis globais incluidas */
char dados[TAM_DADOS];
int num_vars = 0;
int num_vars_inicial = 0;
int nivel_lexico = 0;
int desloc = 0;

/* Fun��o para corrigir erro de vers�es */
void yyerror (char const *);

%}

%token PROGRAM ABRE_PARENTESES FECHA_PARENTESES
%token VIRGULA PONTO_E_VIRGULA DOIS_PONTOS PONTO
%token T_BEGIN T_END VAR IDENT ATRIBUICAO
%token NUMERO
%token SOMA SUBTRACAO MULTIPLICACAO DIVISAO
%token ENQUANTO PARA FACA REPITA ATE SE ENTAO SENAO

%%

programa    :{
             geraCodigo (NULL, "INPP");
             }
             PROGRAM IDENT
             ABRE_PARENTESES lista_idents FECHA_PARENTESES PONTO_E_VIRGULA
             bloco PONTO {
             sprintf ( dados, "DMEM %d", num_vars);
             geraCodigo( NULL, dados);
             geraCodigo (NULL, "PARA");
             }
;

bloco       :
              parte_declara_vars
              {
              }

              comando_composto
              ;




parte_declara_vars:  var
;


var         : { } VAR declara_vars
            |
;

declara_vars: declara_vars declara_var
            | declara_var
;

declara_var : {
              num_vars_inicial = num_vars;
              }
              lista_id_var DOIS_PONTOS
              tipo
              { /* AMEM */
              sprintf ( dados, "AMEM %d", num_vars);
              geraCodigo( NULL, dados);
              }
              PONTO_E_VIRGULA
;

tipo        : IDENT
            {
            int j;
                for( j = num_vars_inicial; j < num_vars; j++){
                    sprintf ( tb_simb[ j ].tipo, "%s", token);
                }
            }
;

lista_id_var: lista_id_var VIRGULA IDENT
            { /* insere �ltima vars na tabela de s�mbolos */
            sprintf ( tb_simb[ num_vars ].simbolo, "%s", token);
            sprintf ( tb_simb[ num_vars ].categoria, "%s", "var_simples");
            tb_simb[ num_vars ].nivel_lexico = nivel_lexico;
            tb_simb[ num_vars ].desloc = desloc;
            desloc++;
            num_vars++;
            }
            | IDENT
            { /* insere vars na tabela de s�mbolos */
            sprintf ( tb_simb[ num_vars ].simbolo, "%s", token);
            sprintf ( tb_simb[ num_vars ].categoria, "%s", "var_simples");
            tb_simb[ num_vars ].nivel_lexico = nivel_lexico;
            tb_simb[ num_vars ].desloc = desloc;
            desloc++;
            num_vars++;
            }
;

lista_idents: lista_idents VIRGULA IDENT
            | IDENT
;


comando_composto: T_BEGIN comandos T_END
            {

            }
;

comandos:   comandos atribuicao
            | comandos repeticao
            | comandos condicao
            | atribuicao
            | repeticao
            | condicao
;

atribuicao: IDENT
            {
            if ( ! procura_simb( token ) ){
                sprintf ( dados, "Simbolo '%s' nao foi declarada", token);
                imprimeErro( dados );
            }
            } ATRIBUICAO expressao_fraca PONTO_E_VIRGULA
;

repeticao: ENQUANTO
            {
            gera_Proximo_Rotulo();
            geraCodigo( rotulo, "NADA");
            } ABRE_PARENTESES expressao_fraca FECHA_PARENTESES FACA comando_composto PONTO_E_VIRGULA
            {
            gera_Proximo_Rotulo();
            geraCodigo( rotulo, "NADA");
            }

condicao: SE ABRE_PARENTESES expressao_fraca FECHA_PARENTESES ENTAO comando_composto

expressao_fraca: expressao_fraca SOMA expressao_forte
            {
            geraCodigo (NULL, "SOMA");
            }
            | expressao_fraca SUBTRACAO expressao_forte
            {
            geraCodigo (NULL, "SUBT");
            }
            | expressao_forte
;

expressao_forte: expressao_forte MULTIPLICACAO expressao_fator
            {
            geraCodigo (NULL, "MULT");
            }
            | expressao_forte DIVISAO expressao_forte
            {
            geraCodigo (NULL, "DIVI");
            }
            | expressao_fator
;

expressao_fator: IDENT
            {
            geraCodigo (NULL, "CRVL");
            }
            | NUMERO
            {
            sprintf ( dados, "CRCT %s", token);
            geraCodigo (NULL, dados);
            }
            | expressao_fraca SOMA expressao_fraca
            {
            geraCodigo (NULL, "SOMA");
            }
            | expressao_fraca SUBTRACAO expressao_fraca
            {
            geraCodigo (NULL, "SUBT");
            }
            | expressao_fraca MULTIPLICACAO expressao_fraca
            {
            geraCodigo (NULL, "MULT");
            }
            | expressao_fraca DIVISAO expressao_fraca
            {
            geraCodigo (NULL, "DIVI");
            }
            | ABRE_PARENTESES expressao_fraca FECHA_PARENTESES
;

%%

void yyerror (char const *message)
{
    fputs (message, stderr);
    fputc ('\n', stderr);
}

main (int argc, char** argv) {
   FILE* fp;
   extern FILE* yyin;

   if (argc<2 || argc>2) {
         printf("usage compilador <arq>a %d\n", argc);
         return(-1);
      }

   fp=fopen (argv[1], "r");
   if (fp == NULL) {
      printf("usage compilador <arq>b\n");
      return(-1);
   }


/* -------------------------------------------------------------------
 *  Inicia a Tabela de S�mbolos
 * ------------------------------------------------------------------- */
  tb_simb = malloc( sizeof( tabela_simbolos) * TAM_TB_SIMB);

   yyin=fp;
   yyparse();

/* Impress�o da tabela de simbolos */
    int i;
    printf("\n\nTABELA DE SIMBOLOS\n\n");
    for( i = 0; i < TAM_TB_SIMB ; i++ ){
        printf( "| %s | %s | %d | %d | %s |\n", tb_simb[i].simbolo, tb_simb[i].categoria, tb_simb[i].nivel_lexico, tb_simb[i].desloc, tb_simb[i].tipo);
    }

   return 0;
}
