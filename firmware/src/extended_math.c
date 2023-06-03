#include "extended_math.h"

int16_t sqr( int16_t x )
{
	return x * x;
}

float sqrf( float x )
{
	return x * x;
}

void vector3f_to_rotation_matrix3f( matrix3f result, vector3f vector )
{
    result[ 0 ][ 0 ] =  0.;
    result[ 0 ][ 1 ] = -vector[ 2 ];
    result[ 0 ][ 2 ] =  vector[ 1 ];
    result[ 1 ][ 0 ] =  vector[ 2 ];
    result[ 1 ][ 1 ] =  0.;
    result[ 1 ][ 2 ] = -vector[ 0 ];
    result[ 2 ][ 0 ] = -vector[ 1 ];
    result[ 2 ][ 1 ] =  vector[ 0 ];
    result[ 2 ][ 2 ] =  0.;
}

void DCM_to_Euler_angles( float* roll, float* pitch, float* yaw, matrix3f DCM )
{
    *roll = atan2f( DCM[ 2 ][ 1 ], DCM[ 2 ][ 2 ] );
    *pitch = -asinf( DCM[ 2 ][ 0 ] );
    *yaw = atan2f( DCM[ 1 ][ 0 ], DCM[ 0 ][ 0 ] );
}

float vector3f_dot( vector3f a, vector3f b )
{
    return a[ 0 ] * b[ 0 ] + a[ 1 ] * b[ 1 ] + a[ 2 ] * b[ 2 ];
}

void vector3f_cross( vector3f result, vector3f a, vector3f b )
{
    result[ 0 ]= ( a[ 1 ] * b[ 2 ] ) - ( a[ 2 ] * b[ 1 ] );
    result[ 1 ]= ( a[ 2 ] * b[ 0 ] ) - ( a[ 0 ] * b[ 2 ] );
    result[ 2 ]= ( a[ 0 ] * b[ 1 ] ) - ( a[ 1 ] * b[ 0 ] );
}

void matrix3f_normalize( matrix3f matrix )
{
    float error = 0, renorm = 0;
    matrix3f temporary;

    error = -vector3f_dot( &matrix[ 0 ][ 0 ], &matrix[ 1 ][ 0 ] ) * .5; //eq.19

    VECTOR3_SCALE(&temporary[0][0], &matrix[1][0], error); //eq.19
    VECTOR3_SCALE(&temporary[1][0], &matrix[0][0], error); //eq.19

    VECTOR3_SUM(&temporary[0][0], &temporary[0][0], &matrix[0][0]);//eq.19
    VECTOR3_SUM(&temporary[1][0], &temporary[1][0], &matrix[1][0]);//eq.19

    vector3f_cross(&temporary[2][0],&temporary[0][0],&temporary[1][0]); // c= a x b //eq.20

    renorm= .5 *(3 - vector3f_dot(&temporary[0][0],&temporary[0][0])); //eq.21
    VECTOR3_SCALE(&matrix[0][0], &temporary[0][0], renorm);

    renorm= .5 *(3 - vector3f_dot(&temporary[1][0],&temporary[1][0])); //eq.21
    VECTOR3_SCALE(&matrix[1][0], &temporary[1][0], renorm);

    renorm= .5 *(3 - vector3f_dot(&temporary[2][0],&temporary[2][0])); //eq.21
    VECTOR3_SCALE(&matrix[2][0], &temporary[2][0], renorm);
}

void quaternionf_to_Euler_angles( float* roll, float* pitch, float* yaw, quaternionf q )
{
    *roll =  atan2f( 2. * ( q.q0 * q.q1 + q.q2 * q.q3 ), 1. - 2. * ( sqrf( q.q1 ) + sqrf( q.q2 ) ) );
    *pitch = asinf( 2. * ( q.q0 * q.q2 - q.q3 * q.q1 ) );
    *yaw =   atan2f( 2. * ( q.q0 * q.q3 + q.q1 * q.q2 ), 1. - 2. * ( sqrf( q.q2 ) + sqrf( q.q3 ) ) );
}
