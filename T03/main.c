/*============================================================================*/
/* Exemplo: segmentação de uma imagem em escala de cinza.                     */
/*----------------------------------------------------------------------------*/
/* Autor: Bogdan T. Nassu                                                     */
/* Universidade Tecnológica Federal do Paraná                                 */
/*============================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "pdi.h"

/*============================================================================*/


#define INPUT_IMAGE "GT2.bmp"
#define THRESHOLD 0.8f


/*============================================================================*/

typedef struct
{
    float label;
    Retangulo roi;
    int n_pixels;

} Componente;

/*============================================================================*/

void achaLuz (Imagem* in, Imagem* out, float threshold);

/*============================================================================*/

int main ()
{
    clock_t tempo;
    // Abre a imagem em escala de cinza, e mantém uma cópia colorida dela para desenhar a saída.
    Imagem* img_entrada = abreImagem (INPUT_IMAGE, 3);
    if (!img_entrada)
    {
        printf ("Erro abrindo a imagem.\n");
        exit (1);
    }

    Imagem* img_luz =      criaImagem (img_entrada->largura, img_entrada->altura, 3);
    Imagem* img_borrada1 = criaImagem (img_entrada->largura, img_entrada->altura, 3);
    Imagem* img_borrada2 = criaImagem (img_entrada->largura, img_entrada->altura, 3);
    Imagem* img_borrada3 = criaImagem (img_entrada->largura, img_entrada->altura, 3);
    Imagem* aux =          criaImagem (img_entrada->largura, img_entrada->altura, 3);
    Imagem* img_saida =    criaImagem (img_entrada->largura, img_entrada->altura, 3);

    int tamanho_janela1 = 7; 
    int tamanho_janela2 = 13; 
    int tamanho_janela3 = 19; 

    //buscar fonte de luz
    achaLuz(img_entrada,img_luz,THRESHOLD);

    //borra 1
    blur (img_luz,      img_borrada1, tamanho_janela1, tamanho_janela1, NULL);
    blur (img_borrada1, aux,          tamanho_janela1, tamanho_janela1, NULL);
    blur (aux,          img_borrada1, tamanho_janela1, tamanho_janela1, NULL);

    //borra 2
    blur (img_luz,      img_borrada2, tamanho_janela2, tamanho_janela2, NULL);
    blur (img_borrada2, aux,          tamanho_janela2, tamanho_janela2, NULL);
    blur (aux,          img_borrada2, tamanho_janela2, tamanho_janela2, NULL);

    //borra 2
    blur (img_luz,      img_borrada3, tamanho_janela3, tamanho_janela3, NULL);
    blur (img_borrada3, aux,          tamanho_janela3, tamanho_janela3, NULL);
    blur (aux,          img_borrada3, tamanho_janela3, tamanho_janela3, NULL);

    //soma na imagem original
    soma (img_borrada1, img_borrada2, 1, 1, aux);
    soma (aux,          img_borrada3, 1, 1, img_saida);

    //salva
    salvaImagem (img_saida, "saida.bmp");
    
    //Limpeza
    destroiImagem (img_entrada);
    destroiImagem (img_luz);
    destroiImagem (aux);
    destroiImagem (img_borrada1);
    destroiImagem (img_borrada2);
    destroiImagem (img_borrada3);
    destroiImagem (img_saida);
    return 0;
}




void achaLuz (Imagem* in, Imagem* out, float threshold)
{
    if (in->largura != out->largura || in->altura != out->altura || in->n_canais != out->n_canais)
    {
        printf ("ERRO: binariza: as imagens precisam ter o mesmo tamanho e numero de canais.\n");
        exit (1);
    }

    for (int y = 0; y < in->altura; y++)
        for (int x = 0; x < in->largura; x++)
            for (int canal = 0; canal < in->n_canais; canal++)
                out->dados[canal][y][x] = in->dados[canal][y][x] > threshold ? in->dados[canal][y][x] : 0;
}