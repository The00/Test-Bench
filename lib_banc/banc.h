/*///////////////////////////////////////////////////
Test Bench Arduino Code
V2.1
/////////////////////////////////////////////////////*/
#ifndef BANC_LIB
#define BANC_LIB
#include "Arduino.h"

/// --- MAPPING ARDUINO --- ///

#define THRUST_IN1 A5
#define THRUST_IN2 A4
#define P_CURRENT A2
#define P_VOLTAGE A3

#define P_PLUGGED A1
#define T1_PLUGGED 9 //5
#define T1_ENABLED 12 //6
#define T2_PLUGGED 4 //7
#define T2_ENABLED 7 //8
#define RPM_PLUGGED 3
#define RPM_ENABLED 1 //4

#define ESC_OUT 11
#define RPM_INPUT_IT 1 // pin 2



#define MAX_ECARTYPE 20
#define MAX_SAMPLE 100
#define MAX_TIMEOUT 100
#define COEFF1 1.03
#define COEFF2 1.05
//#define B1_T -0.63
//#define B2_T -0.71
#define DEL_STR ";"
#define DEL_CAR ';'
#define L_TRAME_CONFIG 20
#define ESC_LOW 900

/// --- RPM --- ///

struct rpm_data{

    float rpm; // vitesse mesurée
    float ecart_type; // permet de voir si la mesure est valable
    int time;  // temps d'execution de la mesure en ms
    int nbr_sample_r;

} ;
typedef struct rpm_data rpm_data;

void rpm_isr();
void init_rpm();
rpm_data read_rpm(unsigned int nbr_sample,unsigned int timeout);
void stop_rpm();
float calcul_ecart_type(int tailletab,int data[],float moyenne);
float calcul_moyenne(int tailletab,int data[]);


/// --- THRUST --- ///

struct thrust_data{

    int sensor1;
    int sensor2;
} ;
typedef struct thrust_data thrust_data;

thrust_data read_thrust();

/// --- POWER --- ///

struct power_data{
    int voltage;
    int current;
};

typedef struct power_data power_data;

power_data read_power();


/// --- STATE SENSORS ---///

void init_pins();
void getState_sensors(byte *state);


/// --- DATA --- ///

enum mode_enum {
CONFIGURATION,
ACQUISITION
};

int ftoa(char *a, double f, int precision);
int build_data_string(char*str,rpm_data *rpm, thrust_data *thrust, power_data *power,byte *state);
void decode_trame(char buffer[],mode_enum *mode,unsigned int *sampling_delay, unsigned int *nbr_sample, unsigned int *timeout); // decode la trame entrante et affecte les variables avec les nouvelles valeurs



#endif
