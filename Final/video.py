import numpy as np
import cv2
import glob


def abrir_videos(caminho_video, caminho_salvar_frames, frame_inicial, frame_final):
    cap = cv2.VideoCapture(caminho_video)

    #cap.set(2, 0.5)
    k = 0
    while cap.isOpened():
        if k < frame_inicial:
            cap.read()
            k += 1
            continue

        ret, frame = cap.read()
        cv2.imwrite(caminho_salvar_frames + "%3d.png" % k, frame)

        if k == frame_final:
            break
        k += 1


def salvar_video(caminho_frames, caminho_salvar_video, frames_por_segundo = 30):
    img_array = []
    size = 0
    for filename in sorted(glob.glob(caminho_frames + '*.png')):
        img = cv2.imread(filename)
        height, width, layers = img.shape
        size = (width, height)
        img_array.append(img)

    out = cv2.VideoWriter(caminho_salvar_video, cv2.VideoWriter_fourcc(*'mp4v'), frames_por_segundo, size)

    for i in range(len(img_array)):
        out.write(img_array[i])
    out.release()


#pasta_frames = 'flinstones/in/'
#abrir_videos('flinstones.mp4', pasta_frames, 123*30, 128*30)

pasta_frames = 'spaghetti/in/'
abrir_videos('spaghetti.mp4', pasta_frames, 0, 5*30)

# Fazer coisas aqui

#salvar_video(pasta_frames, 'flinstones_recomposto.mp4')
salvar_video(pasta_frames, 'spaghetti_recomposto.mp4')
