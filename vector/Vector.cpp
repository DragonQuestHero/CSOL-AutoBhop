#include "../client.h"

void Vector::AngleMatrix ( QAngle& Rotation, float ( *matrix )[3] )
{
	float sp, sy, sr, cp, cy, cr, radx, rady, radz;

	radx = Rotation.x * ( M_PI*2 / 360 );
	rady = Rotation.y * ( M_PI*2 / 360 );
	radz = Rotation.z * ( M_PI*2 / 360 );

	sp = sin ( radx ); 
	sy = sin ( rady ); 
	sr = sin ( radz ); 

	cp = cos ( radx );
	cy = cos ( rady );
	cr = cos ( radz );
		
	matrix[0][0] = cp * cy;
	matrix[0][1] = cp * sy;
	matrix[0][2] = -sp;
	
	matrix[1][0] = 1 * sr * sp * cy + 1 * cr * -sy;
	matrix[1][1] = 1 * sr * sp * sy + 1 * cr * cy;
	matrix[1][2] = 1 * sr * cp;
		
	matrix[2][0] = cr * sp * cy + -sr * -sy;
	matrix[2][1] = cr * sp * sy + -sr * cy;
	matrix[2][2] = cr * cp;
}

void Vector::VectorRotate ( Vector& In, QAngle& Rotation )
{
	float matRotate[3][3];

	AngleMatrix ( Rotation, matRotate );

	x = In.Dot( matRotate[0] );
	y = In.Dot( matRotate[1] );
	z = In.Dot( matRotate[2] );
}

QAngle Vector::ToEulerAngles()
{
	float Pitch, Yaw, Length;

	Length = Length2D();

	if ( Length > 0 )
	{
		Pitch = ( atan2 ( -z, Length ) * 180 / M_PI );

		if ( Pitch < 0 )
		{
			Pitch += 360;
		}

		Yaw = ( atan2 ( y, x ) * 180 / M_PI );

		if ( Yaw < 0 )
		{
			Yaw += 360;
		}
	}
	else
	{
		Pitch = ( z > 0.0f ) ? 270 : 90;
		Yaw = 0;
	}
			
	return QAngle ( Pitch, Yaw, 0 );
}

QAngle Vector::ToEulerAngles ( Vector* PseudoUp )
{
	Vector Left;

	float	Length, Yaw, Pitch, Roll;

	Left.CrossProduct ( *PseudoUp, *this );
	
	Left.Normalize();
		
	Length = Length2D();

	if ( PseudoUp )
	{        
		if ( Length > 0.001 )
		{                   							
			Pitch = ( atan2 ( -z, Length ) * 180 / M_PI );

			if ( Pitch < 0 )
			{
				Pitch += 360;
			}
        
			Yaw = ( atan2 ( y, x ) * 180 / M_PI );

			if ( Yaw < 0 )
			{
				Yaw += 360;
			}

			float up_z = ( Left[1] * x ) - ( Left[0] * y );
                                                     
			Roll = ( atan2 ( Left[2], up_z ) * 180 / M_PI );

			if ( Roll < 0 )
			{
				Roll += 360;
			}
		}
		else
		{
			Yaw = ( atan2 ( y, x ) * 180 / M_PI );

			if ( Yaw < 0 )
			{
				Yaw += 360;
			}
		
			Pitch = ( atan2 ( -z, Length ) * 180 / M_PI);
		
			if ( Pitch < 0 )
			{
				Pitch += 360;
			}

			Roll = 0;
		}
	}
	else
	{
		if ( Length > 0 )
		{
			Pitch = ( atan2 ( -z, Length ) * 180 / M_PI );

			if ( Pitch < 0 )
			{
				Pitch += 360;
			}

			Yaw = ( atan2 ( y, x ) * 180 / M_PI );

			if ( Yaw < 0 )
			{
				Yaw += 360;
			}
		}
		else
		{
			Pitch = ( z > 0.0f ) ? 270 : 90;
			Yaw = 0;
		}
	}

	return  QAngle ( Pitch, Yaw, Roll );
}