#include <OgreNewt_CollisionPrimitives.h>
#include <OgreNewt_Tools.h>

#include "Ogre.h"

namespace OgreNewt
{

	namespace CollisionPrimitives
	{

		// OgreNewt::CollisionPrimitives::Box
		Box::Box( World* world, Ogre::Vector3 size, Ogre::Quaternion orient, Ogre::Vector3 pos ) : Collision( world )
		{
			float matrix[16];

			OgreNewt::Converters::QuatPosToMatrix( orient, pos, &matrix[0] );

			m_col = NewtonCreateBox( m_world->getNewtonWorld(), (float)size.x, (float)size.y, (float)size.z, &matrix[0] );
		}


		// OgreNewt::CollisionPrimitives::Ellipsoid
		Ellipsoid::Ellipsoid( World* world, Ogre::Vector3 size, Ogre::Quaternion orient, Ogre::Vector3 pos ) : Collision( world )
		{
			float matrix[16];

			OgreNewt::Converters::QuatPosToMatrix( orient, pos, &matrix[0] );

			m_col = NewtonCreateSphere( m_world->getNewtonWorld(), (float)size.x, (float)size.y, (float)size.z, &matrix[0] );
		}


		// OgreNewt::CollisionPrimitives::Cylinder
		Cylinder::Cylinder( World* world, Ogre::Real radius, Ogre::Real height, 
									Ogre::Quaternion orient, Ogre::Vector3 pos ) : Collision( world )
		{
			float matrix[16];

			OgreNewt::Converters::QuatPosToMatrix( orient, pos, &matrix[0] );

			m_col = NewtonCreateCylinder( m_world->getNewtonWorld(), (float)radius, (float)height, &matrix[0] );
		}


		// OgreNewt::CollisionPrimitives::Capsule
		Capsule::Capsule( World* world, Ogre::Real radius, Ogre::Real height, 
									Ogre::Quaternion orient, Ogre::Vector3 pos ) : Collision( world )
		{
			float matrix[16];

			OgreNewt::Converters::QuatPosToMatrix( orient, pos, &matrix[0] );

			m_col = NewtonCreateCapsule( m_world->getNewtonWorld(), (float)radius, (float)height, &matrix[0] );
		}


		// OgreNewt::CollisionPrimitives::Cone
		Cone::Cone( World* world, Ogre::Real radius, Ogre::Real height, 
									Ogre::Quaternion orient, Ogre::Vector3 pos ) : Collision( world )
		{
			float matrix[16];

			OgreNewt::Converters::QuatPosToMatrix( orient, pos, &matrix[0] );

			m_col = NewtonCreateCone( m_world->getNewtonWorld(), (float)radius, (float)height, &matrix[0] );
		}

		// OgreNewt::CollisionPrimitives::ChamferCylinder
		ChamferCylinder::ChamferCylinder( World* world, Ogre::Real radius, Ogre::Real height, 
									Ogre::Quaternion orient, Ogre::Vector3 pos ) : Collision( world )
		{
			float matrix[16];

			OgreNewt::Converters::QuatPosToMatrix( orient, pos, &matrix[0] );

			m_col = NewtonCreateChamferCylinder( m_world->getNewtonWorld(), (float)radius, (float)height, &matrix[0] );
		}


		
		// OgreNewt::CollisionPrimitives::ConvexHull
		ConvexHull::ConvexHull( World* world, Ogre::SceneNode* node, Ogre::Quaternion orient, Ogre::Vector3 pos ) : Collision( world )
		{
#ifdef _DEBUG
			FILE* debug;
			debug =	fopen("ConvexHull.txt","wt");
#endif	// _DEBUG

			Ogre::Vector3 scale(1.0,1.0,1.0);
			

			//get the mesh!
			Ogre::MovableObject* obj = node->getAttachedObject(0);
			Ogre::MeshPtr mesh = ((Ogre::Entity*)obj)->getMesh();

			//get scale
			scale = node->getScale();

			//find number of submeshes
			unsigned short sub = mesh->getNumSubMeshes();

#ifdef _DEBUG
			fprintf( debug, "Mesh Found. Number of SubMeshes %d\n", sub );
			fprintf( debug, "Counting total number of vertices...\n" );
#endif	// _DEBUG

			size_t total_verts = 0;

			Ogre::VertexData* v_data;
			bool addedShared = false;

			for (unsigned short i=0;i<sub;i++)
			{
				Ogre::SubMesh* sub_mesh = mesh->getSubMesh(i);
				if (sub_mesh->useSharedVertices)
				{
					if (!addedShared)
					{
						v_data = mesh->sharedVertexData;
						total_verts += v_data->vertexCount;

#ifdef _DEBUG
						fprintf( debug, "\tSubMesh %d uses shared vertices, counted %d vertices\n", i, v_data->vertexCount );
#endif	// _DEBUG

						addedShared = true;
					}
				}
				else
				{
					v_data = sub_mesh->vertexData;
					total_verts += v_data->vertexCount;

#ifdef _DEBUG
					fprintf( debug, "\tSubMesh %d uses its own vertices, counted %d vertices\n", i, v_data->vertexCount );
#endif	// _DEBUG

				}
			}
		
			addedShared = false;

#ifdef _DEBUG
			fprintf( debug, "\tFound %d total vertices in this mesh!\n", total_verts );
#endif	// _DEBUG

			//make array to hold vertex positions!
			Ogre::Vector3* vertices = new Ogre::Vector3[total_verts];
			unsigned int offset = 0;

			//loop back through, adding vertices as we go!
			for (unsigned short i=0;i<sub;i++)
			{
				Ogre::SubMesh* sub_mesh = mesh->getSubMesh(i);
				Ogre::VertexDeclaration* v_decl;
				const Ogre::VertexElement* p_elem;
				float* v_Posptr;
				size_t v_count;
		
				v_data = NULL;

				if (sub_mesh->useSharedVertices)
				{
					if (!addedShared)
					{
						v_data = mesh->sharedVertexData;
						v_count = v_data->vertexCount;
						v_decl = v_data->vertexDeclaration;
						p_elem = v_decl->findElementBySemantic( Ogre::VES_POSITION );
						addedShared = true;
					}
				}
				else
				{
					v_data = sub_mesh->vertexData;
					v_count = v_data->vertexCount;
					v_decl = v_data->vertexDeclaration;
					p_elem = v_decl->findElementBySemantic( Ogre::VES_POSITION );
				}

				if (v_data)
				{
					size_t start = v_data->vertexStart;
					//pointer
					Ogre::HardwareVertexBufferSharedPtr v_sptr = v_data->vertexBufferBinding->getBuffer( p_elem->getSource() );
					unsigned char* v_ptr = static_cast<unsigned char*>(v_sptr->lock( Ogre::HardwareBuffer::HBL_READ_ONLY ));
					unsigned char* v_offset;

#ifdef _DEBUG
					fprintf( debug, "\t\tAbout to add %d Vertices, from start position %d\n", v_count, start );
#endif	// _DEBUG

					//loop through vertex data...
					for (size_t j=start; j<(start+v_count); j++)
					{
						//get offset to Position data!
						v_offset = v_ptr + (j * v_sptr->getVertexSize());
						p_elem->baseVertexPointerToElement( v_offset, &v_Posptr );

						//now get vertex positions...
						vertices[offset].x = *v_Posptr; v_Posptr++;
						vertices[offset].y = *v_Posptr; v_Posptr++;
						vertices[offset].z = *v_Posptr; v_Posptr++;

						vertices[offset] *= scale;

#ifdef _DEBUG
						fprintf( debug, "\t\t\tAdded Vertex %d [x]%f [y]%f [z]%f\n", j, vertices[offset].x, vertices[offset].y, vertices[offset].z );
#endif	// _DEBUG

						offset++;
					}

					//unlock buffer
					v_sptr->unlock();
				}

		
			}

#ifdef _DEBUG
			fprintf( debug, "\nCreating ConvexHull...\n" );
#endif	// _DEBUG

			float matrix[16];

			OgreNewt::Converters::QuatPosToMatrix( orient, pos, &matrix[0] );
	
			//okay, let's try making the ConvexHull!
			m_col = NewtonCreateConvexHull( m_world->getNewtonWorld(), (int)total_verts, (float*)&vertices[0].x, sizeof(Ogre::Vector3), &matrix[0] );

#ifdef _DEBUG
			fprintf( debug, "\nCreating ConvexHull...done.\n" );
			fclose( debug );
#endif	// _DEBUG

			delete []vertices;

		}


		// OgreNewt::CollisionPrimitives::ConvexHull
		ConvexHull::ConvexHull( World* world, Ogre::Vector3* verts, int vertcount, Ogre::Quaternion orient, Ogre::Vector3 pos ) : Collision(world)
		{
			float matrix[16];
			OgreNewt::Converters::QuatPosToMatrix( orient, pos, &matrix[0] );

			// create a simple pyramid collision primitive using the Newton Convex Hull interface.
			// this function places the center of mass 1/3 up y from the base.

			//make the collision primitive.
			m_col = NewtonCreateConvexHull( m_world->getNewtonWorld(), vertcount, &verts[0].x, sizeof(Ogre::Vector3), &matrix[0]);

		}





		TreeCollision::TreeCollision( World* world, Ogre::SceneNode* node, bool optimize ) : Collision( world )
		{

#ifdef _DEBUG
			FILE* debug;
			debug = fopen("TreeCollision.txt","wt");
#endif	// _DEBUG

			Ogre::Vector3 scale;
			

			m_col = NewtonCreateTreeCollision( m_world->getNewtonWorld(), NULL );
			NewtonTreeCollisionBeginBuild( m_col );

			//now get the mesh!
			Ogre::MovableObject* obj = node->getAttachedObject(0);
			Ogre::MeshPtr mesh = ((Ogre::Entity*)obj)->getMesh();

			//get scale
			scale = node->getScale();

			//find number of sub-meshes
			unsigned short sub = mesh->getNumSubMeshes();

#ifdef _DEBUG
			fprintf( debug, "Mesh found. Number of Submeshes %d\n", sub );
#endif	// _DEBUG

			for (unsigned short cs=0;cs<sub;cs++)
			{

#ifdef _DEBUG
				fprintf( debug, "\tLooping through SubMesh %d of %d\n", cs, sub );
#endif	// _DEBUG

				Ogre::SubMesh* sub_mesh = mesh->getSubMesh(cs);

				//vertex data!
				Ogre::VertexData* v_data;

				if (sub_mesh->useSharedVertices)
				{
#ifdef _DEBUG
					fprintf( debug, "\t\tSubMesh uses Shared Vertices..\n." );
#endif	// _DEBUG
					v_data = mesh->sharedVertexData;
				}
				else
				{
#ifdef _DEBUG
					fprintf( debug, "\t\tSubMesh uses it's own vertex data...\n" );
#endif	// _DEBUG
					v_data = sub_mesh->vertexData;
				}
		
				//let's find more information about the Vertices...
				Ogre::VertexDeclaration* v_decl = v_data->vertexDeclaration;
				const Ogre::VertexElement* p_elem = v_decl->findElementBySemantic( Ogre::VES_POSITION );
		
				// get pointer!
				Ogre::HardwareVertexBufferSharedPtr v_sptr = v_data->vertexBufferBinding->getBuffer( p_elem->getSource() );
				unsigned char* v_ptr = static_cast<unsigned char*>(v_sptr->lock( Ogre::HardwareBuffer::HBL_READ_ONLY ));
		
				//now find more about the index!!
				Ogre::IndexData* i_data = sub_mesh->indexData;
				size_t index_count = i_data->indexCount;
				size_t poly_count = index_count / 3;
#ifdef _DEBUG
				fprintf( debug, "\t\t\tIndexCount: %d  Therefore PolyCount: %d\n", index_count, poly_count );
#endif	// _DEBUG
		
				// get pointer!
				Ogre::HardwareIndexBufferSharedPtr i_sptr = i_data->indexBuffer;
		
				// 16 or 32 bit indices?
				bool uses32bit = ( i_sptr->getType() == Ogre::HardwareIndexBuffer::IndexType::IT_32BIT );
				unsigned long* i_Longptr;
				unsigned short* i_Shortptr;
		
		
				if ( uses32bit)
				{
#ifdef _DEBUG
					fprintf( debug, "\t\t\tIndex is 32-bit\n" );
#endif	// _DEBUG
					i_Longptr = static_cast<unsigned long*>(i_sptr->lock( Ogre::HardwareBuffer::HBL_READ_ONLY ));
					
				}
				else
				{
#ifdef _DEBUG
					fprintf( debug, "\t\t\tIndex is 16-bit\n" );
#endif	// _DEBUG
					i_Shortptr = static_cast<unsigned short*>(i_sptr->lock( Ogre::HardwareBuffer::HBL_READ_ONLY ));
				}


				//now loop through the indices, getting polygon info!
				int i_offset = 0;

				for (size_t i=0; i<poly_count; i++)
				{
					Ogre::Vector3 poly_verts[3];
					unsigned char* v_offset;
					float* v_Posptr;
					int idx;

					if (uses32bit)
					{
						for (int j=0;j<3;j++)
						{
							idx = i_Longptr[i_offset+j];		// index to first vertex!
							v_offset = v_ptr + (idx * v_sptr->getVertexSize());
							p_elem->baseVertexPointerToElement( v_offset, &v_Posptr );
							//now get vertex position from v_Posptr!
							poly_verts[j].x = *v_Posptr; v_Posptr++;
							poly_verts[j].y = *v_Posptr; v_Posptr++;
							poly_verts[j].z = *v_Posptr; v_Posptr++;

							poly_verts[j] *= scale;
						}
	
#ifdef _DEBUG
						fprintf( debug, "\t\t\tPoly[%d] = [%d][%d][%d]\n", i, i_Longptr[i_offset], i_Longptr[i_offset+1], i_Longptr[i_offset+2] );				
#endif	// _DEBUG
					}
					else
					{
						for (int j=0;j<3;j++)
						{
							idx = i_Shortptr[i_offset+j];		// index to first vertex!
							v_offset = v_ptr + (idx * v_sptr->getVertexSize());
							p_elem->baseVertexPointerToElement( v_offset, &v_Posptr );
							//now get vertex position from v_Posptr!
							poly_verts[j].x = *v_Posptr; v_Posptr++;
							poly_verts[j].y = *v_Posptr; v_Posptr++;
							poly_verts[j].z = *v_Posptr; v_Posptr++;

							poly_verts[j] *= scale;
						}
#ifdef _DEBUG
						fprintf( debug, "\t\t\tPoly[%d] = [%d][%d][%d]\n", i, i_Shortptr[i_offset], i_Shortptr[i_offset+1], i_Shortptr[i_offset+2] );
#endif	// _DEBUG
					}

#ifdef _DEBUG
					fprintf( debug, "\t\t\t\tVert 1: [x]%f [y]%f [z]%f\n", poly_verts[0].x, poly_verts[0].y, poly_verts[0].z );
					fprintf( debug, "\t\t\t\tVert 2: [x]%f [y]%f [z]%f\n", poly_verts[1].x, poly_verts[1].y, poly_verts[1].z );
					fprintf( debug, "\t\t\t\tVert 3: [x]%f [y]%f [z]%f\n", poly_verts[2].x, poly_verts[2].y, poly_verts[2].z );
#endif	// _DEBUG

					
					NewtonTreeCollisionAddFace( m_col, 3, (float*)&poly_verts[0].x, sizeof(Ogre::Vector3), cs );
					i_offset += 3;
				}

				//unlock the buffers!
				v_sptr->unlock();
				i_sptr->unlock();
	
			}
			//done!
			NewtonTreeCollisionEndBuild( m_col, optimize );

#ifdef _DEBUG
			fclose(debug);
#endif	// _DEBUG

		}


		// OgreNewt::CollisionPrimitives::CompoundCollision
		CompoundCollision::CompoundCollision( World* world, std::vector<OgreNewt::Collision*> col_array ) : Collision( world )
		{
			//get the number of elements.
			int num = col_array.size();

			// create simple array.
			NewtonCollision** array = new NewtonCollision*[num];

			for (int i=0;i<num;i++)
			{
				array[i] = (col_array[i]->getNewtonCollision());
			}

			m_col = NewtonCreateCompoundCollision( world->getNewtonWorld(), num, array );

			delete[] array;

		}

		
		// OgreNewt::CollisionPrimitives::Pyramid
		Pyramid::Pyramid( World* world, Ogre::Vector3 size, Ogre::Quaternion orient, Ogre::Vector3 pos ) : Collision( world )
		{
			float matrix[16];

			OgreNewt::Converters::QuatPosToMatrix( orient, pos, &matrix[0] );

			// create a simple pyramid collision primitive using the Newton Convex Hull interface.
			// this function places the center of mass 1/3 up y from the base.

			float* vertices = new float[15];
			unsigned short idx = 0;

			// make the bottom base.
			for (int ix=-1; ix<=1; ix+=2)
			{
				for (int iz=-1; iz<=1; iz+=2)
				{
					vertices [idx++] = (size.x/2.0) * ix;
					vertices [idx++] = -(size.y/3.0);
					vertices [idx++] = (size.z/2.0) * iz;
				}
			}

			// make the tip.
			vertices [idx++] = 0.0f;
			vertices [idx++] = (size.y*2.0/3.0);
			vertices [idx++] = 0.0f;

			//make the collision primitive.
			m_col = NewtonCreateConvexHull( m_world->getNewtonWorld(), 5, vertices, sizeof(float)*3, &matrix[0]);

			delete []vertices;

		}



	}	// end namespace CollisionPrimitives

}	// end namespace OgreNewt




