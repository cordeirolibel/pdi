
import cv2
import os
import numpy as np
import time
ZOOM = 4

class Video:
	def __init__(self,path):
		self.path = path
		self.cap = cv2.VideoCapture(path)
		self.frame = None
		self.frame_n = 0

	def __del__(self):
		self.cap.release()


	def update(self):
		if "Flintstones_img" in self.path and self.frame is not None:
			return self.frame
			
		ret, frame = self.cap.read()
		
		#se o video acabou
		if not ret:
			#carrega novamente
			self.cap.set(cv2.CAP_PROP_POS_FRAMES,0)
			ret, frame = self.cap.read()
			self.frame_n = 0

		self.frame_n += 1
		self.frame = frame

	def last_frame(self):
		return self.frame



class Play:

	def __init__(self):
		self.videos_path  = ["Flintstones","Spaghetti","Avatar","Flintstones_img"]
		self.original_pos = "_recomposto" 
		self.waifu_pos    = "_waifu"
		self.dcscn_pos	  = "_DCSCN"
		self.polygons_pos	  = "_nosso_algoritmo"
		self.bluray_pos	  = "_bluray"
		self.video_type = "original"
		self.video_resize_type = 'sem'
		self.video_i = 0
		self.pause = False
		self.zoom  = False
		self.is_fullscreen = True
		self.last_time = time.time()
		self.last_dim_screen = (0,0)
		self.videos = list()

		#load all videos 
		for video_name in self.videos_path:

			video = dict()
			#original
			path = os.path.join(video_name,video_name+self.original_pos+".mp4")
			video['original'] = Video(path)

			#waifu
			path = os.path.join(video_name,video_name+self.waifu_pos+".mp4")
			video['waifu'] = Video(path)

			#dcscn
			path = os.path.join(video_name,video_name+self.dcscn_pos+".mp4")
			video['dcscn'] = Video(path)

			#nosso algoritmo
			path = os.path.join(video_name,video_name+self.polygons_pos+".mp4")
			video['polygons'] = Video(path)

			#bluray
			path = os.path.join(video_name,video_name+self.bluray_pos+".mp4")
			video['bluray'] = Video(path)

			self.videos.append(video)

	def __del__(self):
		for video in self.videos:
			del video
		cv2.destroyAllWindows()

	def get_frame(self):
		try:
			frame = self.videos[self.video_i][self.video_type].last_frame()
		except:
			print("Error: video [%d %s] not found!"%(self.video_i,self.video_type))
			frame = None

		#empty frame
		if frame is None:
			frame = np.zeros((10,10,3))
		elif self.videos_path[self.video_i] == "Avatar" and\
			self.video_type == "bluray":
			#16:9 to 4:3
			#  |--dx--|--------new_x---------|--dx--|
			size = frame.shape
			new_x = size[0]*4/3
			dx = int((size[1]-new_x)/2)
			frame = frame[:,dx:-dx]
		#zoom
		elif self.zoom:
			pixels = (int(size[0]*(1-1/ZOOM)/2),int(size[1]*(1-1/ZOOM)/2))
			frame  = frame[pixels[0]:-pixels[0],pixels[1]:-pixels[1]]

		frame = frame.copy()
		
		#resize
		dim_frame  = (frame.shape[1],frame.shape[0])
		dim_screen = cv2.getWindowImageRect('video')[2:]

		#if dont change so much
		if abs(self.last_dim_screen[0] - dim_screen[0]) + abs(self.last_dim_screen[0] - dim_screen[0]) <= 2:
			dim_screen = self.last_dim_screen
		self.last_dim_screen = dim_screen

		alpha = min([dim_screen[0]/dim_frame[0],dim_screen[1]/dim_frame[1]])
		dim_frame2 = (int(dim_frame[0]*alpha),int(dim_frame[1]*alpha))


		if self.video_resize_type == 'bicubic':
			frame = cv2.resize(frame, dim_frame2, interpolation = cv2.INTER_CUBIC )
		elif self.video_resize_type == 'nearest':
			frame = cv2.resize(frame, dim_frame2, interpolation = cv2.INTER_NEAREST )

		return frame

	#load the frame of all videos
	def update_frames(self):
		try:
			video = self.videos[self.video_i]
		except:
			print("Error: video [%d] not found!"%(self.video_i))
			video = None

		#empty video
		if video is None:
			return 

		for key_type in video:
			video[key_type].update()
			if self.videos_path[self.video_i] == "Avatar" and \
			   video[key_type].frame_n%24 == 0 and \
			   key_type == "bluray" :
				video[key_type].update() #ignore frame 25

	#add text to a frame
	def add_text(self,frame):
		try:
			text =  self.videos_path[self.video_i] + '\n' +\
					self.video_type.capitalize() + '\n' #+\
					#self.video_resize_type.capitalize()
		except:
			print("Error: video [%d] not found!"%(self.video_i))
			text = ''
		
		frame = frame.copy()
		size = frame.shape
		font                   = cv2.FONT_HERSHEY_COMPLEX
		bottomLeftCornerOfText = [int(size[1]*0.78),int(size[0]*0.05)]
		new_line               = int(size[0]*0.05)
		fontScale              = (size[0]/600)
		lineType               = int(size[0]/650+1)
		delta_black            = int(size[0]/200)

		#write lines
		k_line = 1
		for line in text.split('\n'):
			
			#black border
			cv2.putText(frame,line, 
				tuple(bottomLeftCornerOfText),
				font, 
				fontScale,
				(0,0,0),
				lineType+delta_black)

			#white text
			cv2.putText(frame,line, 
				tuple(bottomLeftCornerOfText),
				font, 
				fontScale,
				(255,255,255),
				lineType)

			k_line+=1
			if k_line == 3:
				fontScale *= 0.7
				bottomLeftCornerOfText[1] += int(new_line*0.7)
			else:
				bottomLeftCornerOfText[1] += new_line

		return frame 

	#my better waitKey
	def wait_key(self):
		delta1 = 33#1000/30
		try:
			if self.videos_path[self.video_i] == "Avatar":
				delta1 = 42
		except:
			None

		#How much time has passed
		now = time.time()
		delta2 = (now - self.last_time)*1000
		
		#how much fault
		delta = delta1 - delta2
		delta = np.clip(delta,1,delta1).astype(int)

		#print("    passou %.2f, delta de %.2f"%(delta2,delta))

		#wait
		key = cv2.waitKey(delta)
		
		#save time
		self.last_time = time.time()

		return key

	def change_frame(self,frame):
		try:
			dif = np.array_equal(self.frame_ant,frame)
		except:
			dif = False

		self.frame_ant = frame
		return dif

	#loop playing
	def run(self):

		#fullscreen
		cv2.namedWindow("video", cv2.WND_PROP_FULLSCREEN)
		cv2.setWindowProperty("video",cv2.WND_PROP_FULLSCREEN,cv2.WINDOW_FULLSCREEN)
		a=0
		while(True):
			#show frame 
			if not self.pause:
				self.update_frames()

			frame = self.get_frame()
			frame = self.add_text(frame)

			#wait
			key = self.wait_key()

			#exit
			if key == ord('q') or cv2.getWindowProperty('video',cv2.WND_PROP_VISIBLE) < 1:
				del self
				break

			#draw
			cv2.imshow('video',frame)
			

			### Keyboard
			#pause
			if key == ord(' '):
				self.pause = not self.pause
			#change type 
			elif key == ord('w'):
				self.video_type = 'waifu'
			elif key == ord('o'):
				self.video_type = 'original'
			elif key == ord('d'):
				self.video_type = 'dcscn'
			elif key == ord('p'):
				self.video_type = 'polygons'
			elif key == ord('r'):
				self.video_type = 'bluray'
			#change resize type 
			elif key == ord('b'):
				self.video_resize_type = 'bicubic'
			elif key == ord('n'):
				self.video_resize_type = 'nearest'
			elif key == ord('m'):
				self.video_resize_type = 'sem'
			#zoom
			elif key == ord('z'):
				self.zoom = not self.zoom
			#reload
			elif key == 13: #enter
				if self.is_fullscreen: 
					cv2.setWindowProperty("video",cv2.WND_PROP_FULLSCREEN,cv2.WINDOW_NORMAL)
					cv2.setWindowProperty("video",cv2.WND_PROP_FULLSCREEN,cv2.WINDOW_FULLSCREEN)
			#change video
			elif ord('1')<=key and key<=ord('9'):
				self.video_i = int(chr(key))-1
				if self.is_fullscreen: 
					cv2.setWindowProperty("video",cv2.WND_PROP_FULLSCREEN,cv2.WINDOW_NORMAL)
					cv2.setWindowProperty("video",cv2.WND_PROP_FULLSCREEN,cv2.WINDOW_FULLSCREEN)
			#fullscreen
			elif key == 200 or (key==27 and self.is_fullscreen): 
				if self.is_fullscreen: 
					cv2.setWindowProperty("video",cv2.WND_PROP_FULLSCREEN,cv2.WINDOW_NORMAL)
					self.is_fullscreen = False
				else:
					cv2.setWindowProperty("video",cv2.WND_PROP_FULLSCREEN,cv2.WINDOW_FULLSCREEN)
					self.is_fullscreen = True


play = Play()
play.run()	

print("end")







