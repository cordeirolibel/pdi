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

//void binariza (Imagem* in, Imagem* out, float threshold);
//int rotula (Imagem* img, Componente** componentes, int largura_min, int altura_min, int n_pixels_min);
//void inunda(float label, Imagem* img, int x0, int y0, int *n_pixels, Retangulo *retangulo);
void filtroMediaIngenuo (Imagem* in, Imagem* out, int tamanho_janela);
void filtroMediaSeparavel (Imagem* in, Imagem* out, int tamanho_janela);
void filtroMediaIntegral (Imagem* in, Imagem* out, int tamanho_janela);
double*** Integral (Imagem* in);

/*============================================================================*/

int main ()
{
    clock_t tempo;
    // Abre a imagem em escala de cinza, e mantém uma cópia colorida dela para desenhar a saída.
    Imagem* img_cinza = abreImagem (INPUT_IMAGE, 1);
    if (!img_cinza)
    {
        printf ("Erro abrindo a imagem.\n");
        exit (1);
    }

    Imagem* img = criaImagem (img_cinza->largura, img_cinza->altura, 3);
    cinzaParaRGB (img_cinza, img);

    Imagem* img_borrada = criaImagem (img->largura, img->altura, 3);

    int tamanho_janela = 17; //Tem que ser número ímpar: é o lado da janela quadrada (por exemplo 5 significa janela 5 x 5).

    //Ingenuo
    tempo = clock ();
    filtroMediaIngenuo (img, img_borrada, tamanho_janela);
    tempo = clock () - tempo;
    printf ("Tempo Ingenuo: %d\n", (int) tempo);
    salvaImagem (img_borrada, "01 - borrada - ingenuo.bmp");

    //Separavel
    tempo = clock ();
    filtroMediaSeparavel (img, img_borrada, tamanho_janela);
    tempo = clock () - tempo;
    printf ("Tempo Separavel: %d\n", (int) tempo);
    salvaImagem (img_borrada, "01 - borrada - separavel.bmp");

    //Integral
    tempo = clock ();
    filtroMediaIntegral (img, img_borrada, tamanho_janela);
    tempo = clock () - tempo;
    printf ("Tempo Integral: %d\n", (int) tempo);
    salvaImagem (img_borrada, "01 - borrada - integral.bmp");

    //Limpeza
    destroiImagem (img);
    destroiImagem (img_borrada);
    destroiImagem (img_cinza);
    return 0;
}

/*============================================================================*/

void filtroMediaIngenuo (Imagem* in, Imagem* out, int tamanho_janela)
{
    if (tamanho_janela % 2 == 0)
    {
        printf("O tamanho da janela utilizada no filtro da média deve ser um número ímpar!");
        return;
    }

    int offset = (tamanho_janela - 1) / 2;
    int area_janela;

    //para cada pixel da imagem
    for (int canal = 0; canal < in->n_canais; canal++)
        for (int y = 0; y < in->altura; y++)
            for (int x = 0; x < in->largura; x++)
            {
                area_janela = 0;
                out->dados[canal][y][x] = 0;

                //para cada pixel do kernel
                for(int yo = y-offset; yo <= y+offset; yo++)
                    for(int xo = x-offset; xo <= x+offset; xo++)
                    {
                        //esta dentro da imagem?
                        if((xo<0)||(yo<0)||(in->altura<=yo)||(in->largura<=xo))
                            continue;

                        //soma o pixel
                        out->dados[canal][y][x] += in->dados[canal][yo][xo];
                        area_janela += 1;
                    }
                out->dados[canal][y][x] /= area_janela;
            }

}

void filtroMediaSeparavel (Imagem* in, Imagem* out, int tamanho_janela)
{
    if (tamanho_janela % 2 == 0)
    {
        printf("O tamanho da janela utilizada no filtro da média deve ser um número ímpar!");
        return;
    }

    int offset = (tamanho_janela - 1) / 2;
    int area_janela;

    // Cria mais uma imagem auxiliar
    Imagem* img_vertical = criaImagem (in->largura, in->altura, in->n_canais);
    
    //===================
    // ==> Vertical
    //para cada pixel
    for (int canal = 0; canal < in->n_canais; canal++)
        for (int y = 0; y < in->altura; y++)
            for (int x = 0; x < in->largura; x++){
                area_janela = 0;
                img_vertical->dados[canal][y][x] = 0;

                //para cada pixel na janela vertical  
                for(int yo = y-offset; yo <= y+offset; yo++)
                {
                    //esta dentro da imagem?
                    if((yo<0)||(in->altura<=yo))
                        continue;

                    //soma o pixel
                    img_vertical->dados[canal][y][x] += in->dados[canal][yo][x];
                    area_janela += 1;
                }
                img_vertical->dados[canal][y][x] /= area_janela;
            }

    //===================
    // ==> Horizontal
    //para cada pixel
    for (int canal = 0; canal < in->n_canais; canal++)
        for (int y = 0; y < in->altura; y++)
            for (int x = 0; x < in->largura; x++){
                area_janela = 0;
                out->dados[canal][y][x] = 0;

                //para cada pixel na janela vertical  
                for(int xo = x-offset; xo <= x+offset; xo++)
                {
                    //esta dentro da imagem?
                    if((xo<0)||(in->largura<=xo))
                        continue;

                    //soma o pixel
                    out->dados[canal][y][x] += img_vertical->dados[canal][y][xo];
                    area_janela += 1;
                }
                out->dados[canal][y][x] /= area_janela;
            }

    //===================
    // ==> Horizontal

    // Limpeza.
    destroiImagem (img_vertical);
            
}

/*============================================================================*/

void filtroMediaIntegral (Imagem* in, Imagem* out, int tamanho_janela)
{
    if (tamanho_janela % 2 == 0)
    {
        printf("O tamanho da janela utilizada no filtro da média deve ser um número ímpar!");
        return;
    }

    int offset = (tamanho_janela - 1) / 2;
    int area_janela;
    double soma_janela;
    double*** integral = Integral(in);

    for (int canal = 0; canal < in->n_canais; canal++)
        for (int y = 0; y < in->altura; y++)
            for (int x = 0; x < in->largura; x++)
            {
                // Ajusta o offset nos casos em que estiver sendo analisada a região próxima à borda da janela
                int offset_local = offset;
                if (x < offset_local)
                    offset_local = x;
                if (in->largura - x - 1 < offset_local)
                    offset_local = in->largura - x - 1;
                if (y < offset_local)
                    offset_local = y;
                if (in->altura - y - 1 < offset_local)
                    offset_local = in->altura - y - 1;

                if (offset_local == 0)
                {
                    // Se estiver bem na borda da imagem, apenas copia o valor do pixel.
                    out->dados[canal][y][x] = in->dados[canal][y][x];
                }
                else
                {
                    area_janela = (offset_local * 2 + 1) * (offset_local * 2 + 1);
                    soma_janela = integral[canal][y + offset_local][x + offset_local];

                    // Subtrai a região fora da janela
                    if (y > offset_local)
                        soma_janela -= integral[canal][y - offset_local - 1][x + offset_local];

                    if (x > offset_local)
                        soma_janela -= integral[canal][y + offset_local][x - offset_local - 1];

                    // Soma bloco que foi subtraído duas vezes
                    if ((y > offset_local) && (x > offset_local))
                        soma_janela += integral[canal][y - offset_local - 1][x - offset_local - 1];

                    out->dados[canal][y][x] = soma_janela / area_janela;
                }
            }

    //destroy imagem integral
    for (int canal = 0; canal < in->n_canais; canal++)
    {
        for (int y = 0; y < in->altura; y++)
            free(integral[canal][y]);
        free(integral[canal]);
    }
    free(integral);
}


double*** Integral (Imagem* in)
{
    // Alocando a matriz de saída
    double*** out = malloc(sizeof (double**) * in->n_canais);
    for (int canal = 0; canal < in->n_canais; canal++)
    {
        out[canal] = malloc(sizeof (double*) * in->altura);

        for (int y = 0; y < in->altura; y++)
            out[canal][y] = malloc(sizeof (double) * in->largura);
    }

    // Calculando a integral
    for (int canal = 0; canal < in->n_canais; canal++)
    {
        out[canal][0][0] = in->dados[canal][0][0];

        for (int y = 1; y < in->altura; y++)
            out[canal][y][0] = in->dados[canal][y][0] + out[canal][y-1][0];

        for (int x = 1; x < in->largura; x++)
            out[canal][0][x] = in->dados[canal][0][x] + out[canal][0][x-1];

        for (int y = 1; y < in->altura; y++)
            for (int x = 1; x < in->largura; x++)
                out[canal][y][x] =  in->dados[canal][y][x] +
                                    out[canal][y][x-1] +
                                    out[canal][y-1][x] -
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
