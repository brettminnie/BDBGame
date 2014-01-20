// ********************************************************************************************************************
Newton Game Dynamic 1.32

- made SDK compatible with Win32 and Mac OS X 
- fixed bug when calculating isotropic friction correction.
- fixed estrange bug that made joints and contacts acting too soft.
- fixed crash bug on collision tree with degenerated triangles (slivers).
- significant improvement of collision tree builder optimizer, now much faster.
- refactoring all tutorials samples code (added the tool box some utility functions)
- remove the P4 optimization option from the comipler, library should now work on celerons.
- fixed crash bug when calculating over 128 contacts between a convex primitive and a tree mesh.
- added the dry rolling friction to the custom joint library, good for ball games like pool, or pinballs etc.


// ********************************************************************************************************************
Newton Game Dynamic beta 1.31

- improved character controller. 
- improved coulomb kenetic friction model.
- added support for user defined custom joints.
- added global adaptive coumlomb friction model.
- re-arranged sample tutorials to add the custom joint library.
- fixed ray casting cone primitives now return the correct intersection.
- added solver configuration mode, allowing for speed vs acuracy setups.
- fixed access violation crash bug when building very small convex hulls.
- fixed collision tree modifier now inhered collision ID from the child geometry.
- fixed collision primitive ray cast now return the correct normal at the intersection point
- reduced minimum viscous damping coefficient on rigid body from 0,1 to 1.0e-4, default still 0.1
- fixed bug with collision tree allowing them to act like dynamics body if non zero mass was assigned.
- fixed Collision bug. When the origin of sphere was inside a box collision returned not contact points.
- fixed un-initialized Jacobian when setting angular constraint row on bodies attached to world by joints.
- replaced vehicle tires integrator from RK2 to implicit, this allow for more tight and easier to setup suspensions.
- fixed bug with slider and corkscrew joint jacobian using the wrong direction vector, causing the objects to wobble.
- fixed ray cast with collision tree sometime reporting false collision when a ray was close to a very small polygon.


// ********************************************************************************************************************
Newton Game Dynamic beta 1.30

- lots of more features
- vehicle joint 
- added library version.
- improve character controller (in progress)
- added more implicit collision primitives
- added collision primitive deformation matrix
- added arbitrary convex hull collision primitive
- Ragdoll joint now can take and arbitrary collision primitive
- added continues collision mode for high speed objects (in progress)
- fixed spurious crash bug in collision tree optimizer
- added ray cast function 
- function add impulse no longer take the world as argument
- function callback onbodyLeaveworld does not take the world as argument
- fixed bug in utility callback tracking bodies actives in the scene
- several internal bug fixes.
- move to visual studio net 2003
- To link with libraries you will need to declare _NEWTON_USE_LIB before you include newton.h in your project
- if you are linking visual with VS 6. You will need to declare the following in any of you C or C++ modules
	#if (_MSC_VER >= 1300) 
		//VC7 or later, building with pre-VC7 runtime libraries
		//defined by VC6 C libs
		extern "C" long _ftol (double); 
		extern "C" long _ftol2( double dblSource ) 
		{ 
			return _ftol( dblSource ); 
		}
	#endif

// ********************************************************************************************************************
Newton Game Dynamic beta 1.23

- fixed bug with up vector joint in some case producing random crashes  
- added notify application when a body is about to be activate or deactivated
- fixed another bug in collision tree causing to drop face is some cases
- fixed camera stouter one frame behind the player in all tutorials.    

// ********************************************************************************************************************
Newton Game Dynamic beta 1.22

- Fixed Collision tree optimizer crash when large mesh with lot of non manifold faces
- Improve dynamics solver much faster especially when handling large pile of objects 
- Increase max number objects in a single pile to 512, from 256 (use about 200k more of memory, no performance degradation)
- Combined common joint functionality in to generic function (collision state, user data, destruction)
- Added up vector constraint, this is useful to control first and third person characters.
- Added corkscrew constraint.
- Added collision tree tutorial A
- Added material tutorial
- Added up character tutorial using vector (advance tutorial
- Added using joints tutorial 
- Improved existing tutorials
- Fixed bug on existing tutorials that was causing bodies to spin the wrong direction making it looks like the physics solution was inacurate


// ********************************************************************************************************************
Newton Game Dynamic beta 1.21

- Fixed engine hangs in a infinite loop when calling NewtonDestroBody
- Fixed pointer to a vector of 3 dgFloat32 know can be literally an array of tree floats and not the first three element of a 4d vector


// ********************************************************************************************************************
Newton Game Dynamic beta 1.20

-Fixed bodies can be destroy from any call back or at any time
-Fixed joints can be destroy from any call back or at any time
-Remove DX9 tutorials and replaced with open gl tutorial
-Added Joints can disable collision of the linked bodies
-Added Bodies can disable collision with other bodies in the same chain or hierarchy 
-Added Body iterator function for debugging and for other application purpose
-Added Collision Iterator function for debugging and other purpose
-Added context parameter to the buoyancy force call back
-Added Inertia tensor is clipped to between the interval [0.01 * mass - 100.0 * mass], 
 this prevent extreme coriolis forces behavior that could lead to explosion.	
-Added full ball and socket joint with user feedback
-Added full hinge joint with user feedback
-Added full slider joint with user feedback
-Added full Rag doll constraint 



// ********************************************************************************************************************
Newton Game Dynamic beta 1.10

-Remove time limit from beta (this is still a beta version, no responsible for mal functions if you use it in an final project)
-Fixed bug in optimize Collision tree. When there were too many degenerated faces.
-Fixed bug in optimize Collision tree. When the surface does not conform a manifold (like Quake3 BSPs)
-Fixed bug: filter polygon with zero area before passing then to tree collision

// ********************************************************************************************************************
Newton Game Dynamic beta 1.00

This is a beta demo of Newton Game dynamics. the simulation runs for 10 minutes of is launch and the it shut down.
This is done in order to prevent user for trying to uses an incomplete product in a final product.
This time limit will be removed for the beta in the final release.

-Fixed jitter bug on non Intel CPU
-Fixed jitter on slow CPU
-Better function documentation
-Fixed bug when passing zero mass to a rigid body


Enjoy
Newton



