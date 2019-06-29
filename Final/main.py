import video


VIDEO = "Spaghetti"
#VIDEO = "Flintstones"
# VIDEO = "Avatar"

#TYPE = "waifu"
#TYPE = "original"
TYPE = "DCSCN"
#=====================
# === Frames do Video

if VIDEO == "Spaghetti":
	frame_inicial = 0  
elif VIDEO == "Flintstones":
	frame_inicial = 123 * 30

quantos_frames = 5 * 30

#=====================
# === Pastas e aquivos

pasta_base = VIDEO + '/'
pasta_frames = TYPE+'/'

arquivo_video_entrada = VIDEO + '.mp4'
arquivo_video_saida = VIDEO + '_'+TYPE+".mp4"

if TYPE == "original":
	arquivo_video_saida = VIDEO + "_recomposto.mp4"
	pasta_frames = "Frames/"
	
#=====================
# === Separa o video

#video.separar_frames(pasta_base, arquivo_video_entrada, pasta_frames, frame_inicial, quantos_frames)
video.juntar_frames(pasta_base, pasta_frames, arquivo_video_saida)

print('Fim')