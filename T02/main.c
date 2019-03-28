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

#ifndef TEXTO
#define INPUT_IMAGE "arroz.bmp"
#define NEGATIVO 0
#define THRESHOLD 0.8f
#define ALTURA_MIN 2
#define LARGURA_MIN 2
#define N_PIXELS_MIN 100
#endif

#ifdef TEXTO
#define INPUT_IMAGE "documento-3mp.bmp"
#define NEGATIVO 1
#define THRESHOLD 0.4f
#define ALTURA_MIN 10
#define LARGURA_MIN 5
#define N_PIXELS_MIN 50
#endif

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
void inunda(float label, Imagem* img, int x0, int y0, int *n_pixels, Retangulo *retangulo);
double*** Integral (Imagem* in);

/*============================================================================*/

int main ()
{
    int i;

    // Abre a imagem em escala de cinza, e mantém uma cópia colorida dela para desenhar a saída.
    Imagem* img = abreImagem (INPUT_IMAGE, 1);
    if (!img)
    {
        printf ("Erro abrindo a imagem.\n");
        exit (1);
    }

    Imagem* img_out = criaImagem (img->largura, img->altura, 3);
    cinzaParaRGB (img, img_out);

    double*** integral = Integral (img_out);
    printf("Teste: %.2f", integral[0][10][10]);
    return 0;

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
/** Binarização simples por limiarização.
 *
 * Parâmetros: Imagem* in: imagem de entrada. Se tiver mais que 1 canal,
 *               binariza cada canal independentemente.
 *             Imagem* out: imagem de saída. Deve ter o mesmo tamanho da
 *               imagem de entrada.
 *             float threshold: limiar.
 *
 * Valor de retorno: nenhum (usa a imagem de saída). */

void binariza (Imagem* in, Imagem* out, float threshold)
{
    if (in->largura != out->largura || in->altura != out->altura || in->n_canais != out->n_canais)
    {
        printf ("ERRO: binariza: as imagens precisam ter o mesmo tamanho e numero de canais.\n");
        exit (1);
    }

    for (int canal = 0; canal < in->n_canais; canal++)
        for (int y = 0; y < in->altura; y++)
            for (int x = 0; x < in->largura; x++)
                out->dados[canal][y][x] = in->dados[canal][y][x] > threshold;
}

/*============================================================================*/
/* ROTULAGEM                                                                  */
/*============================================================================*/
/** Rotulagem usando flood fill. Marca os objetos da imagem com os valores
 * [0.1,0.2,etc].
 *
 * Parâmetros: Imagem* img: imagem de entrada E saída.
 *             Componente** componentes: um ponteiro para um vetor de saída.
 *               Supomos que o ponteiro inicialmente é inválido. Ele irá
 *               apontar para um vetor que será alocado dentro desta função.
 *               Lembre-se de desalocar o vetor criado!
 *             int largura_min: descarta componentes com largura menor que esta.
 *             int altura_min: descarta componentes com altura menor que esta.
 *             int n_pixels_min: descarta componentes com menos pixels que isso.
 *
 * Valor de retorno: o número de componentes conexos encontrados. */

int rotula (Imagem* img, Componente** componentes, int largura_min, int altura_min, int n_pixels_min)
{
    Retangulo retangulo;
    int altura, largura, numero_pixels = 0;
    float label = 0;
    *componentes = malloc (sizeof (Componente) * 10000);
    int numero_componentes = 0;

    // Cria uma matriz auxiliar, marcando pixels de
    // background com 0 e os de foreground com -1.
    Imagem* img_aux = criaImagem (img->largura, img->altura, 1);
    for (int y = 0; y < img->altura; y++)
        for (int x = 0; x < img->largura; x++)
            img_aux->dados[0][y][x] = (img->dados[0][y][x]==0) ? 0 : -1;

    // Para cada Pixel
    for (int y = 0; y < img_aux->altura; y++)
    {
        for (int x = 0; x < img_aux->largura; x++)
        {
            if (img_aux->dados[0][y][x] == -1.0)
            {
                numero_pixels = 0;
                label = (float)(numero_componentes + 1) / 10.0;
                retangulo = criaRetangulo(y,y,x,x);

                inunda(label, img_aux, x, y, &numero_pixels, &retangulo);

                altura = retangulo.b - retangulo.c;
                largura = retangulo.d - retangulo.e;

                if ((numero_pixels > n_pixels_min) && (largura > largura_min) && (altura > altura_min))
                {
                    (*componentes)[numero_componentes].label = label;
                    (*componentes)[numero_componentes].n_pixels = numero_pixels;
                    (*componentes)[numero_componentes].roi = retangulo;

                    numero_componentes++;
                }
            }
        }
    }

    // Ajustando o tamanho do vetor
    *componentes = realloc(*componentes, sizeof (Componente) * numero_componentes);

    // Limpeza
    destroiImagem (img_aux);
    return numero_componentes;
}

// Inundacao recursivo.
void inunda(float label, Imagem* img, int x0, int y0, int *n_pixels, Retangulo *retangulo)
{
    img->dados[0][y0][x0] = label;

    (*n_pixels)++;

    // Atualiza retangulo
    if (x0 > retangulo->d)
        retangulo->d = x0;
    if (x0 < retangulo->e)
        retangulo->e = x0;
    if (y0 > retangulo->b)
        retangulo->b = y0;
    if (y0 < retangulo->c)
        retangulo->c = y0;

    // Para cada vizinho-4
    if(y0 > 0)
        if(img->dados[0][y0-1][x0]==-1)
            inunda(label, img, x0, y0-1, n_pixels, retangulo);

    if(y0 < (img->altura - 1))
        if(img->dados[0][y0+1][x0]==-1)
            inunda(label, img, x0, y0+1, n_pixels, retangulo);

    if(x0 > 0)
        if(img->dados[0][y0][x0-1]==-1)
            inunda(label, img, x0-1, y0, n_pixels, retangulo);

    if(x0 < (img->largura - 1))
        if (img->dados[0][y0][x0+1]==-1)
            inunda(label, img, x0+1, y0, n_pixels, retangulo);
}

/*============================================================================*/

double*** Integral (Imagem* in)
{
    double*** out = malloc(sizeof (double) * in->n_canais * in->largura * in->altura);

    for (int canal = 0; canal < in->n_canais; canal++)
    {
        out[canal][0][0] = in->dados[canal][0][0];

        for (int y = 1; y < in->altura; y++)
            out[canal][y][0] = in->dados[canal][y][0] + out[canal][y-1][0];

        for (int x = 1; x < in->largura; x++)
            out[canal][0][x] = in->dados[canal][0][x] + out[canal][0][x-1];

        for (int y = 1; y < in->altura; y++)
            for (int x = 1; x < in->largura; x++)
                out[canal][y][x] =  in->dados[canal][y][x] -
                                    out[canal][y][x-1] -
                                    out[canal][y-1][x] +
                                    out[canal][y-1][x-1];
    }

    return out;
}

void Normaliza (Imagem* in, Imagem* out)
{
    for (int canal = 0; canal < in->n_canais; canal++)
    {
        double minimo = 1, maximo = 0;

        for (int y = 0; y < in->altura; y++)
            for (int x = 0; x < in->largura; x++)
            {
                minimo = in->dados[canal][y][x] < minimo ? in->dados[canal][y][x] : minimo;
                maximo = in->dados[canal][y][x] > maximo ? in->dados[canal][y][x] : maximo;
            }

        for (int y = 0; y < in->altura; y++)
            for (int x = 0; x < in->largura; x++)
            {
                out->dados[canal][y][x] = (in->dados[canal][y][x] - minimo) / (maximo - minimo);
            }
    }
}
