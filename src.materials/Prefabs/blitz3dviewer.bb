
Graphics3D 1024,768

MoveMouse 400,300

pivot=CreatePivot()
PositionEntity pivot,0,1.5,0

camera=CreateCamera( pivot )
CameraClsColor camera,100,100,100

PositionEntity camera,0,50,-100


AmbientLight 150, 150, 160


light3=CreateLight()
LightColor light3,180,120,220
RotateEntity light3,0,90,0
PositionEntity light3, 40,20,0


light3=CreateLight()
LightColor light3,160,120,30
RotateEntity light3,0,0,0
PositionEntity light3, -40,20,0


TextureFilter "m_", 1+4


dude1=LoadAnimMesh( "vent02a.3ds" )
MoveEntity dude1, 0,10,0

dude1=LoadAnimMesh( "vent01a.3ds" )
MoveEntity dude1, -100,10,0

dude1=LoadAnimMesh( "crate01a.3ds" )
MoveEntity dude1, -200,10,0

dude1=LoadAnimMesh( "plant02a.3ds" )
MoveEntity dude1, -300,10,0

dude1=LoadAnimMesh( "barl01a.3ds" )
MoveEntity dude1, -400,10,0

dude1=LoadAnimMesh( "barl01b.3ds" )
MoveEntity dude1, -450,10,0


dude1=LoadAnimMesh( "comp01.3ds" )
MoveEntity dude1, 80,10,0

dude1=LoadAnimMesh( "comp02.3ds" )
MoveEntity dude1, 150,10,0

dude1=LoadAnimMesh( "comp03.3ds" )
MoveEntity dude1, 250,10,0


dude1=LoadAnimMesh( "comp04.3ds" )
MoveEntity dude1, 450,10,0


















While Not KeyHit(1)



If KeyDown( 203 )=True Then x#=x#-0.5
If KeyDown( 205 )=True Then x#=x#+0.5
If KeyDown( 208 )=True Then y#=y#-0.5
If KeyDown( 200 )=True Then y#=y#+0.5

If KeyDown( 205 )=True Then MoveEntity camera,5,0,0
If KeyDown( 203 )=True Then MoveEntity camera,-5,0,0
If KeyDown( 208 )=True Then MoveEntity camera,0,0,-5
If KeyDown( 200 )=True Then MoveEntity camera,0,0,5

x#=EntityX(camera)
y#=EntityY(camera)
z#=EntityZ(camera)
	

	
	Mouselook(camera)
;mouselook routine call

PositionEntity camera,x#,y#,z#

	
	UpdateWorld
	RenderWorld
	
	Text 0,0,"Tris Rendered: "+TrisRendered()
	 Text 0,20,"Mouse steers camera, cursors move camera, Escape to..erm, escape"
	Flip
	
	If KeyHit(59) Then
SaveBuffer(FrontBuffer(),"grab"+ss+".bmp"):ss=ss+1
EndIf
;Press F1 For a screenshot
	
	
	
	
Wend

ClearWorld

End




;Control
; Camera position, angle values
Global cam_x#,cam_z#,cam_pitch#,cam_yaw#,cam_speed#=.5		; Current
Global dest_cam_x#,dest_cam_z#,dest_cam_pitch#,dest_cam_yaw#	; Destination
Global ent_x#,ent_z#,ent_pitch#,ent_yaw#,ent_speed#=.5		; Current
Global dest_ent_x#,dest_ent_z#,dest_ent_pitch#,dest_ent_yaw#	; Destination

Function mouselook(camera)
	; Mouse look
	; ----------

	; Mouse x and y speed
	mxs=MouseXSpeed()
	mys=MouseYSpeed()
	
	; Mouse shake (total mouse movement)
	mouse_shake=Abs(((mxs+mys)/2)/1000.0)

	; Destination camera angle x and y values
	dest_cam_yaw#=dest_cam_yaw#-mxs
	dest_cam_pitch#=dest_cam_pitch#+mys

	; Current camera angle x and y values
	cam_yaw=cam_yaw+((dest_cam_yaw-cam_yaw)/5)
	cam_pitch=cam_pitch+((dest_cam_pitch-cam_pitch)/5)
	
	RotateEntity camera,cam_pitch#,cam_yaw#,0
	;RotateEntity camera,mxs,mys,0
		
	; Rest mouse position to centre of screen
	MoveMouse 400,300

		; Move camera using movement values
	MoveEntity camera,x#,y#,z#
		
End Function	









