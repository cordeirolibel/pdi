
import cv2
import os
import numpy as np
import time

class Video:
	def __init__(self,path):
		self.path = path
		self.cap = cv2.VideoCapture(path)
		self.frame = None

	def __del__(self):
		self.cap.release()


	def update(self):
		ret, frame = self.cap.read()

		#se o video acabou
		if not ret:
			#carrega novamente
			#self.cap.release()
			#self.cap = cv2.VideoCapture(self.path)
			self.cap.set(cv2.CAP_PROP_POS_FRAMES, 0)
			ret, frame = self.cap.read()

		self.frame = frame

	def last_frame(self):
		return self.frame



class Play:

	def __init__(self):
		self.videos_path  = ["Flintstones","Spaghetti","Avatar"]
		self.original_pos = "_recomposto" 
		self.waifu_pos    = "_waifu"
		self.fps = 30
		self.video_i = 0
		self.video_type = "original"
		self.pause = False
		self.is_fullscreen = True
		self.last_time = time.time()
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

	
	#my better waitKey
	def wait_key(self):
		delta1 = 33#1000/self.fps

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


	#loop playing
	def run(self):

		#fullscreen
		cv2.namedWindow("video", cv2.WND_PROP_FULLSCREEN)
		cv2.setWindowProperty("video",cv2.WND_PROP_FULLSCREEN,cv2.WINDOW_FULLSCREEN)
		a = time.time()

		while(True):
			#show frame 
			if not self.pause:
				self.update_frames()

			#wait
			key = self.wait_key()

			#exit
			if key == ord('q') or cv2.getWindowProperty('video',cv2.WND_PROP_VISIBLE) < 1:
				del self
				break

			#draw
			cv2.imshow('video',self.get_frame())
			
			### Keyboard
			#pause
			if key == ord(' '):
				self.pause = not self.pause
			#change type
			elif key == ord('w'):
				self.video_type = 'waifu'
				run_one_time = True
			elif key == ord('o'):
				self.video_type = 'original'
				run_one_time = True
			#change video
			elif ord('0')<=key and key<=ord('9'):
				self.video_i = int(chr(key))
				run_one_time = True
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






