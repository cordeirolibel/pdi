/*============================================================================*/
/* Exemplo: segmenta��o de uma imagem em escala de cinza.                     */
/*----------------------------------------------------------------------------*/
/* Autor: Bogdan T. Nassu                                                     */
/* Universidade Tecnol�gica Federal do Paran�                                 */
/*============================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "pdi.h"

/*============================================================================*/

#define INPUT_IMAGE "arroz.bmp"

// TODO: ajuste estes par�metros!
#define NEGATIVO 0
#define THRESHOLD 0.8f
#define ALTURA_MIN 1
#define LARGURA_MIN 1
#define N_PIXELS_MIN 1

/*============================================================================*/

typedef struct
{
    float label;
    Retangulo roi;
    int n_pixels;

} Componente;

/*============================================================================*/

void binariza (Imagem* in, Imagem* out, float threshold);
int rotula (Imagem* img, Componente** componentes, int largura_min, int altura_min, int n_pixels_min);
void inunda(float label,Imagem* f, int x0, int y0);

/*============================================================================*/

int main ()
{
    int i;

    // Abre a imagem em escala de cinza, e mant�m uma c�pia colorida dela para desenhar a sa�da.
    Imagem* img = abreImagem (INPUT_IMAGE, 1);
    if (!img)
    {
        printf ("Erro abrindo a imagem.\n");
        exit (1);
    }

    Imagem* img_out = criaImagem (img->largura, img->altura, 3);
    cinzaParaRGB (img, img_out);

    // Segmenta a imagem.
    if (NEGATIVO)
        inverte (img, img);
    binariza (img, img, THRESHOLD);
    salvaImagem (img, "01 - binarizada.bmp");

    Componente* componentes;
    int n_componentes;
    clock_t tempo_inicio = clock ();
    n_componentes = rotula (img, &componentes, LARGURA_MIN, ALTURA_MIN, N_PIXELS_MIN);
    clock_t tempo_total = clock () - tempo_inicio;

    printf ("Tempo: %d\n", (int) tempo_total);
    printf ("%d componentes detectados.\n", n_componentes);

    // Mostra os objetos encontrados.
    for (i = 0; i < n_componentes; i++)
        desenhaRetangulo (componentes [i].roi, criaCor (1,0,0), img_out);
    salvaImagem (img_out, "02 - out.bmp");

    // Limpeza.
    free (componentes);
    destroiImagem (img_out);
    destroiImagem (img);
    return (0);
}

/*----------------------------------------------------------------------------*/
/** Binariza��o simples por limiariza��o.
 *
 * Par�metros: Imagem* in: imagem de entrada. Se tiver mais que 1 canal,
 *               binariza cada canal independentemente.
 *             Imagem* out: imagem de sa�da. Deve ter o mesmo tamanho da
 *               imagem de entrada.
 *             float threshold: limiar.
 *
 * Valor de retorno: nenhum (usa a imagem de sa�da). */

void binariza (Imagem* in, Imagem* out, float threshold)
{
    if (in->largura != out->largura || in->altura != out->altura || in->n_canais != out->n_canais)
    {
        printf ("ERRO: binariza: as imagens precisam ter o mesmo tamanho e numero de canais.\n");
        exit (1);
    }

    for (int y = 0; y < in->altura; y++)
        for (int x = 0; x < in->largura; x++)
            for (int canal = 0; canal < in->n_canais; canal++)
                out->dados[canal][y][x] = in->dados[canal][y][x] > threshold;
}

/*============================================================================*/
/* ROTULAGEM                                                                  */
/*============================================================================*/
/** Rotulagem usando flood fill. Marca os objetos da imagem com os valores
 * [0.1,0.2,etc].
 *
 * Par�metros: Imagem* img: imagem de entrada E sa�da.
 *             Componente** componentes: um ponteiro para um vetor de sa�da.
 *               Supomos que o ponteiro inicialmente � inv�lido. Ele ir�
 *               apontar para um vetor que ser� alocado dentro desta fun��o.
 *               Lembre-se de desalocar o vetor criado!
 *             int largura_min: descarta componentes com largura menor que esta.
 *             int altura_min: descarta componentes com altura menor que esta.
 *             int n_pixels_min: descarta componentes com menos pixels que isso.
 *
 * Valor de retorno: o n�mero de componentes conexos encontrados. */

int rotula (Imagem* img, Componente** componentes, int largura_min, int altura_min, int n_pixels_min)
{
    // TODO: escreva esta fun��o.
	// Use a abordagem com flood fill recursivo.
	// Observe que o par�metro 'componentes' � um ponteiro para um vetor, ent�o a aloca��o dele deve ser algo como:
	// *componentes = malloc (sizeof (Componente) * n);
	// Dependendo de como voc� fizer a sua implementa��o, pode ser tamb�m interessante alocar primeiro um vetor maior do que o necess�rio, ajustando depois o tamanho usando a fun��o realloc.

    //cria uma matriz auxiliar, marcando pixels de
    // background com 0 e os de foreground com -1.
    Imagem* img_aux = criaImagem (img->largura, img->altura, 1);
    for (int y = 0; y < img->altura; y++)
        for (int x = 0; x < img->largura; x++)
            img_aux->dados[0][y][x] = (img->dados[0][y][x]==0) ? 0 : -1;

    float label = 0.1;

    // Para cada Pixel
    for (int y = 0; y < img_aux->altura; y++)
    {
        for (int x = 0; x < img_aux->largura; x++)
        {
            if (img_aux->dados[0][y][x] == -1.0)
            {
                inunda(label,img_aux,x,y);
                label += 0.1;
            }
        }
    }


    // Limpeza
    destroiImagem (img_aux);
    return ((int)(label*10));
        
}

// Inundacao recursivo.
void inunda(float label,Imagem* img, int x0, int y0)
{

    img->dados[0][y0][x0] = label;

    //para cada vizinho-4
    if(img->dados[0][y0-1][x0]==-1)
        inunda(label,img,x0,y0-1);
    if(img->dados[0][y0+1][x0]==-1)
        inunda(label,img,x0,y0+1);
    if(img->dados[0][y0][x0-1]==-1)
        inunda(label,img,x0-1,y0);
    if(img->dados[0][y0][x0+1]==-1)
        inunda(label,img,x0+1,y0);

}

/*============================================================================*/
