#include "Arduino.h"
#include "banc.h"

/// --- STATE SENSOR ---///

void init_pins()
{
    pinMode(P_PLUGGED,INPUT);
    pinMode(T1_PLUGGED,INPUT);
    pinMode(T2_PLUGGED,INPUT);
    pinMode(RPM_PLUGGED,INPUT);
    pinMode(T1_ENABLED,INPUT);
    pinMode(T2_ENABLED,INPUT);
    pinMode(RPM_ENABLED,INPUT);
}


void getState_sensors(byte *state)
{
    *state =0;

    *state = ((*state) | digitalRead(P_PLUGGED));  // power sensor

    *state = ((*state) | (!digitalRead(T1_PLUGGED))<<1); // thrust 1
    *state = ((*state) | (digitalRead(T1_ENABLED))<<2);

    *state = ((*state) | (!digitalRead(T2_PLUGGED))<<3);   // thrust 2
    *state = ((*state) | (digitalRead(T2_ENABLED))<<4);

    *state = ((*state) | (!digitalRead(RPM_PLUGGED))<<5); // rpm sensor
    *state = ((*state) | (digitalRead(RPM_ENABLED))<<6);

    if((*state & 127)==127) *state=255;

}

/// --- DATA --- ///

// converti un float en string avec le nbr de décimales voulues. Retourne le nombre de caractères écrits (caractère de fin de chaîne non compris
int ftoa(char *a, double f, int precision)
{
 long p[] = {0,10,100,1000,10000,100000,1000000};
 int nbr_byte=0;
 long heiltal = (long)f;

 nbr_byte=sprintf(a,"%d",heiltal);
 nbr_byte = nbr_byte + sprintf((a+nbr_byte),".");

 if(precision<7) {
        long decimal = abs((long)((f - heiltal) * p[precision]));
        nbr_byte = nbr_byte + sprintf((a+nbr_byte),"%d",decimal);
    }
 return nbr_byte;
}


int build_data_string(char*str,rpm_data *rpm, thrust_data *thrust, power_data *power, byte *state)
{
    int offset=0;
    offset = sprintf(str,"%03d",*state);
    offset =offset + sprintf((str+offset),DEL_STR);


    offset =offset + sprintf((str+offset),"%04d",power->current);
    offset =offset + sprintf((str+offset),DEL_STR);
    offset =offset + sprintf((str+offset),"%04d",power->voltage);
    offset =offset + sprintf((str+offset),DEL_STR);


    offset =offset + sprintf((str+offset),"%04d",thrust->sensor1);
    offset =offset + sprintf((str+offset),DEL_STR);
    offset =offset + sprintf((str+offset),"%04d",thrust->sensor2);
    offset =offset + sprintf((str+offset),DEL_STR);

    offset =offset + sprintf((str+offset),"%05d",(int)(rpm->rpm));
    offset =offset + sprintf((str+offset),DEL_STR);

    offset =offset + sprintf((str+offset),"%03d",(int)(rpm->ecart_type));
    offset =offset + sprintf((str+offset),DEL_STR);
    offset =offset + sprintf((str+offset),"%02d",(rpm->nbr_sample_r));
    offset =offset + sprintf((str+offset),DEL_STR);

    return offset;
}

void getSubString(char chaine_in[],char chaine_out[],int lenght,int offset)
{   int i;

    for(i=0;i<lenght;i++)
    {
        chaine_out[i]=chaine_in[i+offset];
    }
    chaine_out[i]='\0';
}

// decode la trame entrante et affecte les variables avec les nouvelles valeurs
void decode_trame(char buffer[],mode_enum *mode,unsigned int *sampling_delay, unsigned int *nbr_sample, unsigned int *timeout)
{
    char transit[5];

    getSubString(buffer,transit,3,0);
    *nbr_sample = atoi(transit);

    getSubString(buffer,transit,4,4);
    *timeout = atoi(transit);

    getSubString(buffer,transit,4,9);
    *sampling_delay = atoi(transit);

    getSubString(buffer,transit,1,14);
    if(atoi(transit)==1)  *mode = ACQUISITION;
    else *mode = CONFIGURATION;
}



