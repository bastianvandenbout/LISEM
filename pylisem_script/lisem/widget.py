from lisem import *
import sys
from vispy import scene
from vispy import app
from vispy.io import load_data_file, read_png
from ipywidgets import Button, Layout, VBox, HBox, Dropdown, Checkbox
from vispy import app, gloo

GlobalChannelId = 1

class MapView(scene.SceneCanvas):

	def __init__(self, width = 700, height = 400):
		scene.SceneCanvas.__init__(self,size=(width,height),keys='interactive')
		
		global GlobalChannelId
		
		scene.SceneCanvas.unfreeze(self)
		self.width = width
		self.height = height
		app.Canvas.size = self.width, self.height
		self.gwidth = 700 
		self.gheight = 400
		self.rwidth = width
		self.rheight = height
		self.x = 0
		self.y = 0
		self.xorg = 0
		self.yorg = 0
		self._timer = app.Timer(1.0/30.0, connect=self.on_timer, start=True)
		self.changed = False
		self.Channel = GlobalChannelId
		GlobalChannelId =  GlobalChannelId+ 1
		
		
		# Set up a viewbox to display the image with interactive pan/zoom
		self.view = super().central_widget.add_view()

		# Create the image
		self.img_data = GeoPlot(self.rwidth, self.rheight,self.x,self.gwidth,self.y,self.gheight,self.Channel)

		self.image = scene.visuals.Image(self.img_data, interpolation='nearest',
									parent=self.view.scene, method='impostor')


		app.Canvas.show(self)
		self.click = False
		
		print("CHannel")
		print(self.Channel)
		scene.SceneCanvas.freeze(self)
		lisem.lisem_python.RegisterChannelUpdateCallback(self.Channel,self.internal_OnChannelUpdateCallBack)
		app.run()
		
	def internal_OnChannelUpdateCallBack(self, is_focus, region):
		self.changed =True
	
	def __del__(self):
	# body of destructor
		lisem.lisem_python.ClearChannelUpdateCallback(self.Channel)
		ClearLayers(self.Channel)
		print("Destruct GeoPlot")
		
	def Replot(self):
		self.changed = True
	
	def ZoomTo(self,ulx,uly,sizex,sizey):
		self.x = ulx
		self.y = uly
		self.gheight = sizey
		self.gwidth = sizex
				
	def ZoomToMap(self, map):
		csx = map.CellSizeX()
		csy = map.CellSizeY()
		ulx = map.West()
		uly = map.North()
		nx = map.Cols()
		ny = map.Rows()
		if csx < 0.0:
			ulx = ulx + nx*csx
		if csy < 0.0:
			uly = uly + ny*csy
		if csx < 0.0:
			csx = csx*(-1.0)
		if csy < 0.0:
			csy = csy*(-1.0)
		self.ZoomTo(ulx,uly,float(nx)*csx,float(ny)*csy)

	# Implement key presses
	def on_key_press(self,event):
		if event.key in ['Left', 'Right']:
			canvas.update(self)

	def on_mouse_move(self,event):

		if self.click:
			dx = event.pos[0] - self.xorg
			dy = event.pos[1] - self.yorg
			dx = self.gwidth/self.width * dx
			dy = self.gheight/self.height * dy
			self.x = self.x - dx
			self.y = self.y + dy
			self.changed = True
		self.xorg = event.pos[0] 
		self.yorg = event.pos[1]

	def on_mouse_wheel(self,event):
		delta = event.delta[1]
		factor = 1.0
		if delta > 0:  # Zoom in
			factor = 0.9
		elif delta < 0:  # Zoom out
			factor = 1 / 0.9 
		self.gwidth = self.gwidth * factor
		self.gheight = self.gheight * factor
		if factor < 1.0:
			self.x = self.x + 0.5 * (1.0-factor)*self.gwidth
			self.y = self.y + 0.5 * (1.0-factor)*self.gheight
		else:
			self.x = self.x - 0.5 * (factor-1.0)*self.gwidth
			self.y = self.y - 0.5 * (factor-1.0)*self.gheight
		self.changed =True
	 
#	def on_resize(event):
#		
#
#	def on_draw(event):
#		
#		
	def on_mouse_press(self,event):
		self.click = True

	def on_mouse_release(self,event):
		self.click = False
	   
	def on_timer(self,event):
		if(self.changed):
			self.image.set_data(GeoPlot(self.rwidth, self.rheight,self.x,self.gwidth,self.y,self.gheight,self.Channel))
			app.Canvas.update(self)
			self.changed = False
		
	
	def AddMap(self, map):
		lisem.lisem_python.AddLayer(map,self.Channel)
		
#	def on_key_press(event):
#		
#	 
#	def on_mouse_double_click(event):
#		
#	   
#	def on_key_release(event):
#		



#b1 = VBox([Button(description='Layer',
#           layout=Layout(width='25%', height='25px')), Dropdown(options=['All'], layout=Layout(width='25%', height='25px'))])
#b2 = VBox([Button(description ="Zoom to"), Checkbox(description ="Show")])
#b3 = VBox([Checkbox(description ="Hillshade"), Checkbox(description ="DEM")])
#b4 = VBox([Button(description='Toggle DEM',
#           layout=Layout(width='25%', height='25px')), Dropdown(options=['All'], layout=Layout(width='25%', height='25px'))])
#b5 = VBox([Checkbox(description ="3D"), Checkbox(description ="Envi.")])
#b6 = Button(description='Save Image')
#b = HBox([b1,b2,b3,b5,b6])
#b




