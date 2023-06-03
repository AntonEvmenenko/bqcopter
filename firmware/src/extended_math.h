#ifndef EXTENDED_MATH_H
#define EXTENDED_MATH_H

#include "math.h"
#include "stdint.h"

#define PI 3.14159265358979323846

typedef int16_t vector3i16[ 3 ];
typedef int32_t vector3i32[ 3 ];
typedef float vector3f[ 3 ];
typedef float matrix3f[ 3 ][ 3 ];

typedef struct 
{
    float q0;
    float q1;
    float q2;
    float q3;    
} quaternionf;

#define EMPTY_VECTOR3 { 0, 0, 0 }
#define EMPTY_MATRIX3 { { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } }

#define VECTOR3_SUM( result, a, b ) \
    do { \
        uint8_t _i; \
        for ( _i = 0; _i < 3; ++_i ) { \
            ( result )[ _i ] = ( a )[ _i ] + ( b )[ _i ]; \
        } \
    } while( 0 )

#define VECTOR3_SCALE( result, vector, factor ) \
    do { \
        uint8_t _i; \
        for ( _i = 0; _i < 3; ++_i ) { \
            ( result )[ _i ] = ( vector )[ _i ] * ( factor ); \
        } \
    } while( 0 )
    
#define VECTOR3_COPY( dst, src ) \
    do { \
        uint8_t _i; \
        for ( _i = 0; _i < 3; ++_i ) { \
            ( dst )[ _i ] = ( src )[ _i ]; \
        } \
    } while( 0 )

#define VECTOR3_LENGTH_SQUARED( result, vector ) \
    do { \
        result = ( vector )[ 0 ] * ( vector )[ 0 ] + \
                 ( vector )[ 1 ] * ( vector )[ 1 ] + \
                 ( vector )[ 2 ] * ( vector )[ 2 ]; \
    } while( 0 )

#define MATRIX3_SUM( result, a, b ) \
    do { \
        uint8_t _i, _j; \
        for ( _i = 0; _i < 3; ++_i ) { \
            for ( _j = 0; _j < 3; ++_j ) { \
                ( result )[ _i ][ _j ] = ( a )[ _i ][ _j ] + ( b )[ _i ][ _j ]; \
            } \
        } \
    } while( 0 )

#define MATRIX3_SCALE( result, matrix, factor ) \
    do { \
        uint8_t _i, _j; \
        for ( _i = 0; _i < 3; ++_i ) { \
            for ( _j = 0; _j < 3; ++_j ) { \
                ( result )[ _i ][ _j ] = ( matrix )[ _i ][ _j ] * ( factor ); \
            } \
        } \
    } while( 0 )

#define MATRIX3_MULT( result, a, b ) \
    do { \
        uint8_t _i, _j, _k; \
        for ( _i = 0; _i < 3; _i++ ) { \
            for ( _j = 0; _j < 3; _j++ ) { \
                ( result )[ _i ][ _j ] = 0; \
                for ( _k = 0; _k < 3; _k++ ) { \
                    ( result )[ _i ][ _j ] += ( a )[ _i ][ _k ] * ( b )[ _k ][ _j ]; \
                } \
            } \
        } \
    } while( 0 )

#define RANGE( result, x, min, max ) \
    do { \
        if (x >= max) { \
            result = max; \
        } else if (x <= min) { \
            result = min; \
        } else { \
            result = x; \
        } \
    } while( 0 )

int16_t sqr( int16_t x );
float sqrf( float x );
void vector3f_to_rotation_matrix3f( matrix3f result, vector3f vector );
void DCM_to_Euler_angles( float* roll, float* pitch, float* yaw, matrix3f DCM );
float vector3f_dot( vector3f a, vector3f b );
void vector3f_cross( vector3f result, vector3f a, vector3f b );
void matrix3f_normalize( matrix3f matrix );
void quaternionf_to_Euler_angles( float* roll, float* pitch, float* yaw, quaternionf q );

#endif
