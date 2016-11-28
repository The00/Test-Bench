#include "Arduino.h"
#include "banc.h"


//variables globales
volatile unsigned long Dtime;
volatile unsigned long Dtime1;
volatile byte i;
volatile byte it_done;

void rpm_isr()
{ Dtime=Dtime1;
  Dtime1 =micros();
  if(Dtime > 0) it_done=1;
}

void init_rpm()
{
    Dtime=0;
    Dtime1=0;
    i =0;
    it_done =0;
    attachInterrupt(RPM_INPUT_IT,rpm_isr,RISING);
}

float calcul_moyenne(int tailletab,int data[])
{   byte i_local;
    unsigned long result=0;

    for(i_local=0;i_local<tailletab;i_local++) result +=data[i_local];

    return (float(result)/tailletab);
}

float calcul_ecart_type(int tailletab,int data[],float moyenne)
{
    byte i_local;
    float result=0;

   /* Serial.println();
    Serial.println("debug ecartype");
    Serial.print("taille tab: ");
    Serial.println(tailletab);
    Serial.print("moyenne: ");
    Serial.println(moyenne);
*/
    for(i_local=0;i_local<tailletab;i_local++) result +=sq(data[i_local]-moyenne);
/*
    Serial.print("somme: ");
    Serial.println(result);
    Serial.println(result/tailletab);
*/
    return sqrt(result/tailletab);
}

void switch_extremum(int tailletab,int data[],float moyenne)
{
    byte i_local=0;
    byte i_extr=0;
    float transit=0;
    float transit2=0;

    transit=abs(data[i_local]-moyenne);
    for(i_local=1;i_local<tailletab;i_local++)
    {
       transit2=abs(data[i_local]-moyenne);
       if(transit2>transit)
       {
           i_extr=i_local;
           transit=transit2;
       }
    }

    data[i_extr]=data[tailletab-1];

}



void analyse_data(int tailletab, int data[], rpm_data *result)
{
    static byte nb_appel =0;

    result->rpm = calcul_moyenne(tailletab,data);
    result->ecart_type = calcul_ecart_type(tailletab,data,result->rpm);
    //Serial.print("ecart type: ");
    //Serial.println(result->ecart_type);

    if((result->ecart_type) > MAX_ECARTYPE)
    {
        if(tailletab > ((tailletab+nb_appel)/2)) // si on a éliminé moins de la moitié des valeurs
        {   nb_appel++;
            switch_extremum(tailletab,data,result->rpm);
            analyse_data(tailletab-1,data,result);
        }
        else // la série de valeurs est inexploitable
        {
            result->rpm=-1;
            result->nbr_sample_r=tailletab;
        }
    }
    else result->nbr_sample_r=tailletab;

    nb_appel=0;
}


rpm_data read_rpm(unsigned int nbr_sample,unsigned int timeout)
{
  unsigned long init_time=millis();
  unsigned long time=init_time;
  int sample[MAX_SAMPLE]={0};
  int periode;
  int indice=0;
  rpm_data result;

  i=0;
  it_done=0;

    if(nbr_sample > MAX_SAMPLE) nbr_sample = MAX_SAMPLE;
    if(timeout > MAX_TIMEOUT) timeout = MAX_TIMEOUT;
   // init_rpm();

   while (i<nbr_sample && ((time - init_time)<timeout))
  {
    time=millis();
    if(it_done)
    {
      periode=Dtime1-Dtime;
      if(periode>20) // si l'impulsion est supérieure  à 20 µs, elle est utilisé
        {
        sample[i]=periode;
        i++;
        }
      it_done=0;
     }
  }

  //stop_rpm();

   // for(indice=0;indice<i;indice++) Serial.println(sample[indice]);

    //Serial.print("ecart type brut: ");
    //test = calcul_ecart_type(i,sample,calcul_moyenne(i,sample));
    //Serial.println(test);
   // Serial.print("nbr de sample brut: ");
   // Serial.println(i);
   // Serial.println();

    analyse_data(i,sample,&result);

    if(result.rpm >0) result.rpm =  60/(result.rpm*pow(10,-6));  // conversion periode -> tr/mn

    result.time = millis()-init_time;

    return result;
}


void stop_rpm()
{
    detachInterrupt(RPM_INPUT_IT);
}







