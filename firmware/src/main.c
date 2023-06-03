#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_usart.h"
#include "time.h"
#include "L3G4200D.h"
#include "ADXL345.h"
#include "HMC5883L.h"
#include "ESC.h"
#include "extended_math.h"
#include "MadgwickAHRS.h"
#include "UART.h"
#include "RF24.h"
#include "CircularBuffer.h"

const float k_gyroscope = 90. * PI / ( 67. * 20. * 180. ); // correction for gyro; for angle in radians
float kp_roll = 2500, kd_roll = 600; // PD regulator components for roll
float kp_pitch = 2500, kd_pitch = 600; // PD regulator components for pitch
float kp_yaw = 5000, kd_yaw = 500; // PD regulator components for yaw
int16_t k_u_throttle = 4, k_u_roll = 5, k_u_pitch = 5; // factors for roll and pitch controls
int16_t k_u_camera = 3; // factors for camera control
const int16_t calibration_iterations_count = 2000; // gyroscope and accelerometer calibration
const int16_t NRF24L_watchdog_initial = 10;

unsigned long previous_timestamp_us = 0, dt_us = 0;
float dt_s = 0.;

int16_t calibration_counter = calibration_iterations_count;
uint8_t calibration = 1;
int16_t NRF24L_watchdog = NRF24L_watchdog_initial;

int16_t u_throttle = 0, u_roll = 0, u_pitch = 0; 
int16_t camera_control_enabled = 0;

vector3i16 gyroscope = EMPTY_VECTOR3, accelerometer = EMPTY_VECTOR3, compass = EMPTY_VECTOR3;
vector3i32 gyroscope_sum = EMPTY_VECTOR3, accelerometer_sum = EMPTY_VECTOR3;
vector3f gyroscope_correction = EMPTY_VECTOR3;
vector3f accelerometer_average = EMPTY_VECTOR3, accelerometer_correction = EMPTY_VECTOR3;
vector3f Euler_angles = EMPTY_VECTOR3, Euler_angles_previous = EMPTY_VECTOR3;

enum {
    CONTROL = 0,

    C2Q_WANT_ALL = 1,

    C2Q_NEW_ROLL_P = 2,
    C2Q_NEW_ROLL_D = 3,
    C2Q_NEW_PITCH_P = 4,
    C2Q_NEW_PITCH_D = 5,
    C2Q_NEW_YAW_P = 6,
    C2Q_NEW_YAW_D = 7,

    Q2C_CURRENT_ROLL_P = 8,
    Q2C_CURRENT_ROLL_D = 9,
    Q2C_CURRENT_PITCH_P = 10,
    Q2C_CURRENT_PITCH_D = 11,
    Q2C_CURRENT_YAW_P = 12,
    Q2C_CURRENT_YAW_D = 13,

    Q2C_CURRENT_ROLL = 14,
    Q2C_CURRENT_PITCH = 15,
    Q2C_CURRENT_YAW = 16
};

struct NRF24L_output_item {
    uint8_t id;
    float data;
};

CircularBuffer<NRF24L_output_item, 64> NRF24L_output_buffer;

void NRG24L_send_package(uint8_t id, float data)
{
    NRF24L_output_item temp = {id, data};
    NRF24L_output_buffer.push_back(temp);
}

int main(void)
{
    __enable_irq();

    ESC_init( );
    UART_init();
    SysTick_init( );
    I2C_init( I2C1, 200000 );
    setupL3G4200D( 2000 );
    setupADXL345( );
    //setupHMC5883L( );
	
    uint8_t addresses[][6] = {"1Node","2Node"};

    RF24 radio(0, 0);

    radio.begin();
    radio.setChannel(90);
    radio.setPALevel(RF24_PA_MAX);
    radio.setRetries(2,15);
    radio.enableAckPayload();
    radio.enableDynamicPayloads();
    radio.openWritingPipe(addresses[0]);
    radio.openReadingPipe(1,addresses[1]);
    radio.startListening();

    delay_ms( 500 );
    
    uint8_t data[5];

    while( 1 )
    {
        uint8_t pipe_number;
        if(radio.available(&pipe_number)){
            radio.read(data, 5);

            if ( data[0] == CONTROL ) {
                u_throttle = data[ 1 ];
                u_roll = data[ 2 ] - 128;
                u_pitch = data[ 3 ] - 128;
                camera_control_enabled = data[ 4 ];
            } else if (data[0] == C2Q_WANT_ALL) {
                NRG24L_send_package(Q2C_CURRENT_ROLL_P, kp_roll);
                NRG24L_send_package(Q2C_CURRENT_ROLL_D, kd_roll);
                NRG24L_send_package(Q2C_CURRENT_PITCH_P, kp_pitch);
                NRG24L_send_package(Q2C_CURRENT_PITCH_D, kd_pitch);
                NRG24L_send_package(Q2C_CURRENT_YAW_P, kp_yaw);
                NRG24L_send_package(Q2C_CURRENT_YAW_D, kd_yaw);
            } else if (data[0] == C2Q_NEW_ROLL_P) {
                kp_roll = *((float*)(data + 1));
            } else if (data[0] == C2Q_NEW_ROLL_D) {
                kd_roll = *((float*)(data + 1));
            } else if (data[0] == C2Q_NEW_PITCH_P) {
                kp_pitch = *((float*)(data + 1));
            } else if (data[0] == C2Q_NEW_PITCH_D) {
                kd_pitch = *((float*)(data + 1));
            } else if (data[0] == C2Q_NEW_YAW_P) {
                kp_yaw = *((float*)(data + 1));
            } else if (data[0] == C2Q_NEW_YAW_D) {
                kd_yaw = *((float*)(data + 1));
            }

            if (NRF24L_output_buffer.get_size() > 0) {
                NRF24L_output_item item = NRF24L_output_buffer.pop_front();
                uint8_t* pointer = (uint8_t*)(&(item.data));
                uint8_t buffer[] = {item.id, pointer[0], pointer[1], pointer[2], pointer[3]};
                radio.writeAckPayload(pipe_number, buffer, 5);
            } else {
                NRG24L_send_package(Q2C_CURRENT_ROLL, Euler_angles[0]);
                NRG24L_send_package(Q2C_CURRENT_PITCH, Euler_angles[1]);
                NRG24L_send_package(Q2C_CURRENT_YAW, Euler_angles[2]);
            }

            NRF24L_watchdog = NRF24L_watchdog_initial;
        }

        if ( !( NRF24L_watchdog ? NRF24L_watchdog-- : 0 ) ) {
            u_throttle = 0;
            u_roll = 0;
            u_pitch = 0;
            camera_control_enabled = 0;
        }
        
        getGyroValues( gyroscope, gyroscope + 1, gyroscope + 2 );
        getAccelValues( accelerometer, accelerometer + 1, accelerometer + 2 );
        //getRawCompassValues( compass, compass + 1, compass + 2 );

        if ( calibration ) {
            VECTOR3_SUM( gyroscope_sum, gyroscope_sum, gyroscope );
            VECTOR3_SUM( accelerometer_sum, accelerometer_sum, accelerometer );

            if ( !( calibration_counter ? calibration_counter-- : 0 ) ) {
                calibration = 0;
                VECTOR3_SCALE( gyroscope_correction, gyroscope_sum, -1. / (float)calibration_iterations_count );
                VECTOR3_SCALE( accelerometer_average, accelerometer_sum, 1. / (float)calibration_iterations_count );
                float g_squared = 0;
                VECTOR3_LENGTH_SQUARED( g_squared, accelerometer_average );
                VECTOR3_SCALE( accelerometer_correction, accelerometer_average, -1 );
                accelerometer_correction[ 2 ] += sqrtf( g_squared );
            }
        } else {
            vector3f gyroscope_corrected = EMPTY_VECTOR3, accelerometer_corrected = EMPTY_VECTOR3;
            VECTOR3_SUM( gyroscope_corrected, gyroscope, gyroscope_correction );
            VECTOR3_SCALE( gyroscope_corrected, gyroscope_corrected, k_gyroscope );
            VECTOR3_SUM( accelerometer_corrected, accelerometer, accelerometer_correction );

            unsigned long timestamp_us = get_us_from_start( );
            
            if ( !previous_timestamp_us ) {
                previous_timestamp_us = timestamp_us;
                continue;
            }
            
            dt_us = timestamp_us - previous_timestamp_us;
            previous_timestamp_us = timestamp_us;
            dt_s = ( float )dt_us / 1000000.;
            
            // IMU from http://www.x-io.co.uk/open-source-imu-and-ahrs-algorithms/
            
            sampleFreq = 1. / dt_s;
            MadgwickAHRSupdate( gyroscope_corrected[ 0 ], gyroscope_corrected[ 1 ], gyroscope_corrected[ 2 ],
                                accelerometer_corrected[ 0 ], accelerometer_corrected[ 1 ], accelerometer_corrected[ 2 ],
                                //( float )compass[ 0 ], ( float )compass[ 1 ], ( float )compass[ 2 ] );
                                0., 0., 0. );
            quaternionf q = { q0, q1, q2, q3 };
            
            quaternionf_to_Euler_angles( Euler_angles, Euler_angles + 1, Euler_angles + 2, q );

            if (u_throttle) {
                vector3i16 u = EMPTY_VECTOR3;

                u[ 0 ] = ( int )( Euler_angles[ 0 ] * kp_roll +
                                  ( Euler_angles[ 0 ] - Euler_angles_previous[ 0 ] ) * kd_roll / dt_s );
                u[ 1 ] = ( int )( Euler_angles[ 1 ] * kp_pitch +
                                  ( Euler_angles[ 1 ] - Euler_angles_previous[ 1 ] ) * kd_pitch / dt_s );
                u[ 2 ] = ( int )( Euler_angles[ 2 ] * kp_yaw +
                                  ( Euler_angles[ 2 ] - Euler_angles_previous[ 2 ] ) * kd_yaw / dt_s );

                u[ 0 ] += u_roll * k_u_roll;
                u[ 1 ] += u_pitch * k_u_pitch;

                if (camera_control_enabled) {
                    float sin_cos_pi_4 = sin(PI / 4.);

                    UART_read();

                    int16_t x = (int16_t)((float)(_positionX - _positionY)*sin_cos_pi_4);
                    int16_t y = (int16_t)((float)(_positionX + _positionY)*sin_cos_pi_4);

                    u[ 0 ] += y * k_u_camera;
                    u[ 1 ] += -x * k_u_camera;
                }

                VECTOR3_COPY( Euler_angles_previous, Euler_angles );

                RANGE(_M1_POWER, _PWM_MIN_SIGNAL + u_throttle * k_u_throttle - u[ 0 ] - u[ 2 ], _PWM_MIN_SIGNAL, _PWM_MAX_SIGNAL);
                RANGE(_M2_POWER, _PWM_MIN_SIGNAL + u_throttle * k_u_throttle + u[ 0 ] - u[ 2 ], _PWM_MIN_SIGNAL, _PWM_MAX_SIGNAL);
                RANGE(_M3_POWER, _PWM_MIN_SIGNAL + u_throttle * k_u_throttle - u[ 1 ] + u[ 2 ], _PWM_MIN_SIGNAL, _PWM_MAX_SIGNAL);
                RANGE(_M4_POWER, _PWM_MIN_SIGNAL + u_throttle * k_u_throttle + u[ 1 ] + u[ 2 ], _PWM_MIN_SIGNAL, _PWM_MAX_SIGNAL);
            } else {
                turnMotorsOff();
            }
        }
    }
}
