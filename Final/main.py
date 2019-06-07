import video


VIDEO = "Spaghetti"
# VIDEO = "Flinstones"
frame_inicial = 0 if VIDEO == "Spaghetti" else 123 * 30
quantos_frames = 5 * 30

pasta_base = 'Arquivos/' + VIDEO + '/'
pasta_frames = 'Frames/'
arquivo_video_entrada = VIDEO + '.mp4'
arquivo_video_saida = VIDEO + '_recomposto.mp4'

video.separar_frames(pasta_base, arquivo_video_entrada, pasta_frames, frame_inicial, quantos_frames)
video.juntar_frames(pasta_base, pasta_frames, arquivo_video_saida)
