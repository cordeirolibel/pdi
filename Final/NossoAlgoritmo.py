import numpy as np
import cv2
from skimage.segmentation import quickshift
from enum import Enum


class Video(Enum):
    SPAGHETTI = 1
    AVATAR = 2
    FLINTSTONES = 3


def encontra_contornos(array_regioes):
    result = []

    for label_regiao in range(array_regioes.max() + 1):
        mascara_regiao = (array_regioes == label_regiao).astype("uint8") * 255
        contornos, h = cv2.findContours(mascara_regiao, mode=cv2.RETR_TREE, method=cv2.CHAIN_APPROX_SIMPLE)
        result.append(contornos)

    return result


def pinta_contorno(im_contornar, contorno, line_thickness=1):
    return cv2.drawContours(im_contornar, [contorno], 0, (0, 0, 0), line_thickness)


def comparar(val):
    return val[4]


def pinta_regioes(im_original, proporcao, array_regioes, array_contornos_por_regiao):
    im_out = np.ones((im_original.shape[0] * proporcao, im_original.shape[1] * proporcao, im_original.shape[2])).astype(float)
    im_out *= -1

    # divide por canal
    b, g, r = im_original[:, :, 0], im_original[:, :, 1], im_original[:, :, 2]
    cores = []
    for label_regiao in range(array_regioes.max() + 1):
        mask = (array_regioes == label_regiao)
        bo = np.median(b[mask]) / 255.0
        go = np.median(g[mask]) / 255.0
        ro = np.median(r[mask]) / 255.0
        cores.append((bo, go, ro, label_regiao, mask.sum()))

    cores.sort(key=comparar, reverse=True)

    for cor in cores:
        for contorno in array_contornos_por_regiao[cor[3]]:
            cv2.drawContours(im_out, [contorno * proporcao], 0, cor[0:3], -1)  # Pintar a região
            cv2.drawContours(im_out, [contorno * proporcao], 0, cor[0:3], proporcao)  # Pintar a borda

    return im_out


def nosso_algoritmo(im):

    # Separa a imagem em regiões e encontra os contornos dessas regiões
    print('\tEncontrando regiões e contornos...')
    regioes = quickshift(im, kernel_size=kernel_size, max_dist=max_dist, ratio=ratio)
    contornos_por_regiao = encontra_contornos(regioes)

    # Gera uma imagem 4 vezes maior
    print('\tAumentando imagem...')
    im_regioes_pintadas_4x = pinta_regioes(im, 4, regioes, contornos_por_regiao)

    # Preenche pixels que ficaram vazios utilizando Inpainting
    print('\tInpainting...')
    mascara = (im_regioes_pintadas_4x[:, :, 0] == -1).astype('uint8')
    im_saida = im_regioes_pintadas_4x.copy()
    im_saida[im_saida == -1] = 0
    im_saida = (im_saida * 255).astype('uint8')
    im_saida = cv2.inpaint(im_saida, mascara, 3, cv2.INPAINT_NS)

    return im_saida


VIDEO = Video.SPAGHETTI

# Parâmetros do quickshift!
if VIDEO == Video.SPAGHETTI:
    kernel_size = 3
    max_dist = 4
    ratio = 1
    numero_imagens = 150
    pasta_base = "Spaghetti"
elif VIDEO == Video.AVATAR:
    kernel_size = 3
    max_dist = 4
    ratio = 1
    numero_imagens = 240
    pasta_base = "Avatar"
elif VIDEO == Video.FLINTSTONES:
    kernel_size = 3
    max_dist = 5
    ratio = 1
    numero_imagens = 150
    pasta_base = "Flintstones"
else:
    exit(0)

caminho_entrada = pasta_base + '/Frames/'
caminho_saida = pasta_base + '/out/'

for i in range(numero_imagens):
    print('\tAbrindo imagem ' + str(i) + '...')
    im_in = cv2.imread(caminho_entrada + str(i).zfill(3) + '.png')

    im_final = nosso_algoritmo(im_in)

    print('\tSalvando imagem ' + str(i) + '...')
    cv2.imwrite(caminho_saida + str(i).zfill(3) + '.png', im_final)
