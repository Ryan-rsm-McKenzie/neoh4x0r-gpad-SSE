#include "CameraRotation.h"
#include "unions.h"
#include "Logger.h"
#include "RE/PlayerCamera.h"
float CameraRotation::Length_NiPoint3(NiPoint3 const& point)
{
	float length = static_cast<float>( sqrt((point.x*point.x)+(point.y*point.y)+(point.z*point.z)) );
	return length;
}
NiPoint3 CameraRotation::Normalize_NiPoint3(NiPoint3 const& point)
{
	NiPoint3 pt;
	pt.x=point.x;
	pt.y=point.y;
	pt.z=point.z;
	float X,Y,Z;
	const float length = Length_NiPoint3(point);
	if(length != 0)
	{
		float num = 1 / length;
		X = (pt.x * num);
		Y = (pt.y * num);
		Z = (pt.z * num);
		pt.x=X;
		pt.y=Y;
		pt.z=Z;
	}
	return pt;
}
void CameraRotation::TestCameraRotation()
{
	RotateCamera(0,0,5);
}
void CameraRotation::RotateCamera(float const& angleX, float const& angleY, float const& angleZ)
{
	RE::PlayerCamera* playerCamera = RE::PlayerCamera::GetSingleton();
	TESCameraState* camstate = playerCamera ? playerCamera->cameraState : NULL;
	if (camstate)
	{
		//float unk1 = playerCamera->unkB4[0];
		//float unk2 = playerCamera->unkB4[1];
		//float unk3 = playerCamera->unkB4[2];
		//float unk4 = playerCamera->unkB4[3];
		//float unk5 = playerCamera->unkB4[4];
		//float unk6 = playerCamera->unkB4[5];

		PlayerCharacter* pPC = (*g_thePlayer);

		_u_uint32_float_s posx(playerCamera->pos.x);
		_u_uint32_float_s posy(playerCamera->pos.y);
		_u_uint32_float_s posz(playerCamera->pos.z);
		_u_uint32_float_s rotx(playerCamera->rot.x);
		_u_uint32_float_s roty(playerCamera->rot.y);
		_u_uint32_float_s rotz(playerCamera->rot.z);
		_u_uint32_float_s unk1(playerCamera->unk15C);

		Log::VerboseDebug("playerCamera->posX: [0x%08x] [%f]", playerCamera->pos.x, posx.f);
		Log::VerboseDebug("playerCamera->posY: [0x%08x] [%f]", playerCamera->pos.y, posy.f);
		Log::VerboseDebug("playerCamera->posZ: [0x%08x] [%f]", playerCamera->pos.z, posz.f);
		Log::VerboseDebug("playerCamera->rotX: [0x%08x] [%f]", playerCamera->rot.x, rotx.f);
		Log::VerboseDebug("playerCamera->rotY: [0x%08x] [%f]", playerCamera->rot.y, roty.f);
		Log::VerboseDebug("playerCamera->rotZ: [0x%08x] [%f]", playerCamera->rot.z, rotz.f);
		Log::VerboseDebug("playerCamera->unk15C: [0x%08x] [%f]", playerCamera->unk15C, unk1.f);

		//rotx.f = 1.4f;
		//playerCamera->unkB4[3] = rotx.i;
		//PlayerCharacter* pPC = (*g_thePlayer);
		//if(pPC) {
		//	CALL_MEMBER_FN(playerCamera, UpdateThirdPerson)(pPC->actorState.IsWeaponDrawn());
		//}

		if(pPC)
		{
			posx.f = pPC->pos.x;
			posy.f = pPC->pos.y;
			posz.f = pPC->pos.z;
			rotx.f = pPC->rot.x;
			roty.f = pPC->rot.y;
			rotz.f = pPC->rot.z;
			playerCamera->pos.x = posx.i;
			playerCamera->pos.y = posy.i;
			playerCamera->pos.z = posz.i;
			playerCamera->rot.x = rotx.i;
			playerCamera->rot.y = roty.i;
			playerCamera->rot.z = rotz.i;
			NiAVObject::ControllerUpdateContext ctx;
			camstate->camera->cameraNode->UpdateWorldData(&ctx);
		}


		//NiNode* cameraNode = state->camera->cameraNode;
		//if(cameraNode)
		//{
		//	//NiNode* controller = state->controllerNode;
		//}
		//NiNode* controller = NULL;
		//if(camstate->stateId == PlayerCamera::kCameraState_FirstPerson)
		//{
		//	FirstPersonState* state = (FirstPersonState*)camstate;
		//	controller = state->controllerNode;
		//}
		//else if(camstate->stateId == PlayerCamera::kCameraState_ThirdPerson1 ||
		//	camstate->stateId == PlayerCamera::kCameraState_ThirdPerson2)
		//{
		//	ThirdPersonState* state = (ThirdPersonState*)camstate;
		//	controller = state->controllerNode;
		//}

		//if(controller)
		//{
		//	/*
		//	parent node: 'NPC Root [Root]'
		//	parent node: 'NPC'
		//	parent node: 'skeleton.nif'
		//	parent node: '(null)'
		//	parent node: '(null)'
		//	parent node: 'ObjectLODRoot'
		//	parent node: 'shadow scene node'
		//	parent node: 'WorldRoot Node'
		//	*/

		//	//const char* parent_name = "WorldRoot Node";
		//	const char* parent_name = "NPC";
		//	Log::VerboseDebug("--------------");
		//	NiNode* parent = controller->m_parent;
		//	bool bFoundParent = false;
		//	while(parent)
		//	{
		//		if(parent && parent->m_name)
		//		{
		//			if(strcmp(parent->m_name,parent_name)==0)
		//			{
		//				bFoundParent = true;
		//				break;
		//			}
		//			Log::VerboseDebug("parent node: '%s'", parent->m_name);
		//		}
		//		parent = parent->m_parent;
		//	}
		//	if(bFoundParent)
		//	{
		//		if(parent)
		//		{
		//			Log::VerboseDebug("Found parent node: '%s'", parent->m_name);
		//			//const float x_angle = 0;
		//			//const float y_angle = 0;
		//			//const float z_angle = 0.5f;
		//			const float x_angle = 0; // for the camera this is yaw/roll
		//			const float y_angle = 0.5f; // for the camera this is left/right
		//			const float z_angle = 0; // for the camera this is up/down
		//			Rotation rot;
		//			NiMatrix33 xrot = rot.RotateMatrix2D_X(x_angle).ToNiMatrix33();
		//			NiMatrix33 yrot = rot.RotateMatrix2D_Y(y_angle).ToNiMatrix33();
		//			NiMatrix33 zrot = rot.RotateMatrix2D_Z(z_angle).ToNiMatrix33();
		//			NiMatrix33 xyz_rot = ( xrot * yrot * zrot );
		//			//NiMatrix33 t_rot = parent->m_localTransform.rot * xyz_rot;
		//			//parent->m_localTransform.rot = t_rot;
		//			//NiPoint3 local_pos = parent->m_localTransform.pos;
		//			////NiPoint3 world_pos = parent->m_worldTransform.pos;
		//			//
		//			//NiPoint3 pos = t_rot * local_pos;
		//			//parent->m_localTransform.pos = pos;

		//			//NiAVObject::ControllerUpdateContext ctx;
		//			//parent->UpdateWorldData(&ctx);

		//			if(camstate->camera && camstate->camera->cameraNode)
		//			{
		//				if(camstate->camera->cameraNode)
		//				{
		//					if(camstate->camera->cameraNode->m_children.m_size)
		//					{
		//						NiCamera* nicamera = (NiCamera*)camstate->camera->cameraNode->m_children.m_data[0];
		//						NiMatrix33 t_rot = nicamera->m_localTransform.rot * xyz_rot;
		//						//nicamera->m_localTransform.rot = t_rot;
		//						NiPoint3 local_pos = nicamera->m_localTransform.pos;
		//						nicamera->m_localTransform.pos = t_rot * local_pos;
		//						NiAVObject::ControllerUpdateContext ctx;
		//						nicamera->UpdateWorldData(&ctx);
		//					}
		//				}
		//			}
		//		}
		//	}
		//}

	}

#pragma region old code
//		NiNode* cameraNode = playerCamera->cameraNode;
//		if(cameraNode)
//		{
//			/* [WorldRoot CameraNode] */
//
//			if(cameraNode->m_children.m_size)
//			{
//				PlayerCharacter* pPC = (PlayerCharacter*)*g_thePlayer;
//				NiCamera* nicamera = (NiCamera*)cameraNode->m_children.m_data[0];
//				if(nicamera && pPC)
//				{
//					const float x_angle = angleX;
//					const float y_angle = angleY;
//					const float z_angle = angleZ;
//					NiTransform local_transform = nicamera->m_localTransform;
//					NiTransform world_transform = nicamera->m_worldTransform;
//					NiPoint3 local_pos = local_transform.pos;
//					NiPoint3 world_pos = world_transform.pos;
//					//NiPoint3* ptr_world_pos = new NiPoint3(world_pos.x, world_pos.y, world_pos.z);
//					//NiPoint3 screen_pos = NiPoint3();
//					NiPoint3 local_screen = NiPoint3();
//					NiPoint3 player_screen = NiPoint3();
//					NiPoint3 lookat_screen = NiPoint3();
//
//
//					float x;
//					float y;
//					float z;
//					NiPoint3* ptr_local_world = new NiPoint3(local_pos.x,local_pos.y,local_pos.z);
//					NiPoint3* ptr_player_world = new NiPoint3(pPC->pos.x,pPC->pos.y,pPC->pos.z);
//					if(nicamera->WorldPtToScreenPt3(ptr_local_world,&x,&y,&z))
//					{
//						//Log::VerboseDebug("ptr_lookat=[%f,%f,%f]",ptr_lookat->x, ptr_lookat->y, ptr_lookat->z);
//						local_screen = NiPoint3(x,y,z);
//					}
//					if(nicamera->WorldPtToScreenPt3(ptr_player_world,&x,&y,&z))
//					{
//						//Log::VerboseDebug("ptr_lookat=[%f,%f,%f]",ptr_lookat->x, ptr_lookat->y, ptr_lookat->z);
//						player_screen = NiPoint3(x,y,z);
//					}
//
//					// borrowed from: http://svn.netsolutions.dynu.com:81/websvn/gta4/mods/ScriptHookDotNet.Extender/trunk/ScriptHookDotNet.Extender/Extensions/ExtensionsCore.cs
//					//NiPoint3 heading = local_pos - pPC->pos;
//					NiPoint3 heading = local_screen - player_screen;
//					heading = Normalize_NiPoint3(heading);
//					float distance = Length_NiPoint3(heading);
//					NiPoint3 direction = heading / distance;
//					direction = Normalize_NiPoint3(direction);
//					NiPoint3 origin = NiPoint3(pPC->pos.x, pPC->pos.y, pPC->pos.z);
//					NiPoint3 lookat = (origin + direction);
//
//					NiPoint3* ptr_lookat =  new NiPoint3(lookat.x,lookat.y,lookat.z);
//					if(nicamera->WorldPtToScreenPt3(ptr_lookat,&x,&y,&z))
//					{
//						//Log::VerboseDebug("ptr_lookat=[%f,%f,%f]",ptr_lookat->x, ptr_lookat->y, ptr_lookat->z);
//						lookat_screen = NiPoint3(x,y,z);
//					}
//
//					//Log::VerboseDebug("screen_pos=[%f,%f,%f]",screen_pos.x, screen_pos.y, screen_pos.z);
//					Rotation xrot;
//					Rotation yrot;
//					Rotation zrot;
//					NiMatrix33 t_xrot = xrot.RotateMatrix2D_X(x_angle).ToNiMatrix33();
//					NiMatrix33 t_yrot = yrot.RotateMatrix2D_Y(y_angle).ToNiMatrix33();
//					NiMatrix33 t_zrot = zrot.RotateMatrix2D_Z(z_angle).ToNiMatrix33();
//					enum
//					{
//						kRotationOrder_Default=0,
//						kRotationOrder_ZYX = 1,
//						kRotationOrder_ZXY = 2,
//						kRotationOrder_YXZ = 3,
//						kRotationOrder_YZX = 4,
//						kRotationOrder_XZY = 5,
//						kRotationOrder_XYZ = 6, // default ?
//					};
//					int RotationOrder = kRotationOrder_ZYX;
//					PointMatrix3 m_rot;
//
//					switch(RotationOrder)
//					{
//						case kRotationOrder_ZYX: m_rot.FromNiMatrix33(t_xrot * t_yrot * t_zrot); break;
//						case kRotationOrder_ZXY: m_rot.FromNiMatrix33(t_yrot * t_xrot * t_zrot); break;
//						case kRotationOrder_YXZ: m_rot.FromNiMatrix33(t_zrot * t_xrot * t_yrot); break;
//						case kRotationOrder_YZX: m_rot.FromNiMatrix33(t_xrot * t_zrot * t_yrot); break;
//						case kRotationOrder_XZY: m_rot.FromNiMatrix33(t_yrot * t_zrot * t_xrot); break;
//						case kRotationOrder_XYZ:
//						case kRotationOrder_Default: m_rot.FromNiMatrix33(t_zrot * t_yrot * t_xrot);break;
//					}
//
//					nicamera->m_localTransform.rot = m_rot.ToNiMatrix33() * local_transform.rot;
//					nicamera->m_localTransform.pos = nicamera->m_localTransform.rot * lookat_screen;
//					NiAVObject::ControllerUpdateContext ctx;
//					cameraNode->UpdateWorldData(&ctx);
//				}
//			}
//
//#pragma region old-code
//			//if(cameraNode->m_children.m_size)
//			//{
//			//	/* [WorldRoot Camera] */
//			//	NiAVObject* niavobj = cameraNode->m_children.m_data[0];
//			//	if(niavobj)
//			//	{
//			//		const float x_angle = angleX;
//			//		const float y_angle = angleY;
//			//		const float z_angle = angleZ;
//			//		NiCamera* nicamera = (NiCamera*)niavobj;
//			//		if(nicamera)
//			//		{
//			//			NiTransform local_transform = nicamera->m_localTransform;
//			//			NiTransform world_transform = nicamera->m_worldTransform;
//			//			NiPoint3 local_pos = local_transform.pos;
//			//			NiPoint3 world_pos = world_transform.pos;
//			//			NiPoint3* ptr_world_pos = new NiPoint3(world_pos.x, world_pos.y, world_pos.z);
//			//			NiPoint3 screen_pos = NiPoint3();
//
//			//			NiPoint3 player_pos = NiPoint3();
//			//			PlayerCharacter* pPC = (PlayerCharacter*)*g_thePlayer;
//			//			if(pPC)
//			//			{
//			//				player_pos = pPC->pos;
//			//				ptr_world_pos->x = player_pos.x;
//			//				ptr_world_pos->y = player_pos.y;
//			//				ptr_world_pos->z = player_pos.z;
//			//			}
//			//			float x;
//			//			float y;
//			//			float z;
//			//			if(nicamera->WorldPtToScreenPt3(ptr_world_pos,&x,&y,&z)) { screen_pos = NiPoint3(x,y,z); }
//			//			/* Rotation
//			//				The default camera rotation matrix is:
//			//				   X Y Z
//			//				X [0 0 1]
//			//				Y [1 0 0]
//			//				Z [0 1 0]
//			//				** this means that -
//			//				X-Axis movedto Z-Axis
//			//				Y-Axis movedto X-Axis
//			//				Z-Axis movedto Y-Axis
//			//				EG:
//			//				XRotation = Up/Down
//			//				YRotation = pitch/yaw/roll/tilt
//			//				ZRotation = Left/Right
//			//				(basically X and Z rotations are inverted)
//			//			*/
//			//			Rotation xrot;
//			//			Rotation yrot;
//			//			Rotation zrot;
//			//			NiMatrix33 t_xrot = xrot.RotateMatrix2D_X(x_angle).ToNiMatrix33();
//			//			NiMatrix33 t_yrot = yrot.RotateMatrix2D_Y(y_angle).ToNiMatrix33();
//			//			NiMatrix33 t_zrot = zrot.RotateMatrix2D_Z(z_angle).ToNiMatrix33();
//			//
//			//			enum
//			//			{
//			//				kRotationOrder_Default=0,
//			//				kRotationOrder_ZYX = 1,
//			//				kRotationOrder_ZXY = 2,
//			//				kRotationOrder_YXZ = 3,
//			//				kRotationOrder_YZX = 4,
//			//				kRotationOrder_XZY = 5,
//			//				kRotationOrder_XYZ = 6, // default ?
//			//			};
//			//			int RotationOrder = kRotationOrder_ZYX;
//			//			PointMatrix3 m_rot;
//
//			//			switch(RotationOrder)
//			//			{
//			//				case kRotationOrder_ZYX: m_rot.FromNiMatrix33(t_xrot * t_yrot * t_zrot); break;
//			//				case kRotationOrder_ZXY: m_rot.FromNiMatrix33(t_yrot * t_xrot * t_zrot); break;
//			//				case kRotationOrder_YXZ: m_rot.FromNiMatrix33(t_zrot * t_xrot * t_yrot); break;
//			//				case kRotationOrder_YZX: m_rot.FromNiMatrix33(t_xrot * t_zrot * t_yrot); break;
//			//				case kRotationOrder_XZY: m_rot.FromNiMatrix33(t_yrot * t_zrot * t_xrot); break;
//			//				case kRotationOrder_XYZ:
//			//				case kRotationOrder_Default: m_rot.FromNiMatrix33(t_zrot * t_yrot * t_xrot);break;
//			//			}
//
//			//			// do the rotation
//			//			// TODO
//			//			/* The nicamera worldtransform does not seem to do anything */
//			//			NiPoint3 pos;
//			//			//pos.x= ptr_world_pos->x;
//			//			//pos.y= ptr_world_pos->y;
//			//			//pos.z= ptr_world_pos->z;
//			//			pos.x= screen_pos.x;
//			//			pos.y= screen_pos.y;
//			//			pos.z= screen_pos.z;
//			//			nicamera->m_localTransform.rot = m_rot.ToNiMatrix33() * local_transform.rot;
//			//			nicamera->m_localTransform.pos = nicamera->m_localTransform.rot * pos;
//			//			//nicamera->m_worldTransform.rot = m_rot.ToNiMatrix33() * world_transform.rot;
//			//			//nicamera->m_worldTransform.pos = nicamera->m_worldTransform.rot * screen_pos;
//			//			NiAVObject::ControllerUpdateContext ctx;
//			//			nicamera->UpdateWorldData(&ctx);
//			//		}
//			//	}
//			//}
//#pragma endregion
//		}
#pragma endregion
}
