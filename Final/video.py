import os
import cv2
import glob

def separar_frames(pasta_principal, nome_arquivo_video, pasta_salvar_frames, frame_inicial, quantos_frames):
    print("===> Separa\n")

    if not os.path.exists(pasta_principal + pasta_salvar_frames):
        os.mkdir(pasta_principal + pasta_salvar_frames)

    # Abre o vídeo
    cap = cv2.VideoCapture(pasta_principal + nome_arquivo_video)

    # Seleciona o frame inicial
    #cap.set(cv2.CAP_PROP_POS_FRAMES, frame_inicial)

    if cap.isOpened():
        k = 0
        while True:
            ret, frame = cap.read()

            # Seleciona o frame inicial
            if k < frame_inicial:
                k += 1
                continue

            #salva imagem
            cv2.imwrite(pasta_principal + pasta_salvar_frames + "%03d.png" % (k-frame_inicial), frame)
            k += 1

            #print
            if (k-frame_inicial)%25 == 0:
                print(k-frame_inicial,'/',quantos_frames)

            #condicao de parada
            if k>=frame_inicial+quantos_frames:
                break



def juntar_frames(pasta_principal, pasta_frames, nome_video_saida, frames_por_segundo=30):
    print("===> Junta\n")
    img_array = []

    size = 0
    for filename in sorted(glob.glob(pasta_principal + pasta_frames + '*.png')):
        img = cv2.imread(filename)
        height, width, layers = img.shape
        size = (width, height)
        img_array.append(img)

    out = cv2.VideoWriter(pasta_principal + nome_video_saida, cv2.VideoWriter_fourcc(*'mp4v'), frames_por_segundo, size)

    for i in range(len(img_array)):
        out.write(img_array[i])
    out.release()
