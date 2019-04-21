/*****************************************************************************
 *                                                                           *
 * BOSCH RainGauge LoraWan Interface                                         *
 *                                                                           *
 * (c) 2016 Global Quality Corp                                              *
 * mailto:sudhir@gqc.com                                                     *
 * Global Quality Corp                                                       *
 *                                                                           *
 * Pal Alto, CA. 94040 USA                                                   *
 *                                                                           *
 * PERMISSION GRANTED TO USE IN NON-COMMERCIAL AND EDUCATIONAL PROJECTS      *
 * NO RESPONSIBILITY ASSUMED FOR DAMAGE OR LOSS OF ANY HARDWARE OR SOFTWARE  *
 *                                                                           *
 * version 0.2                                                               *
 * last change 7, Dec 2016                                              *
 *                                                                           *
 ****************************************************************************/
#include "mbed.h"
#include "onewire.h"
#include "DS18X20.h"
#include "DS2450.h"

uint8_t gSensorIDs[MAXSENSORS][OW_ROMCODE_SIZE];
uint8_t nSensors;
uint32_t nErreur;
uint32_t nErreur_DS18X20;
uint32_t nErreur_DS2450;
uint32_t nErreur_CRC;
uint32_t nErreur_BUS;
uint32_t nErreur_DS2450_START;
uint32_t nErreur_BUSY;
uint32_t nErreur_RESET;
uint32_t nMesure;
uint32_t micro_seconds;
Serial pc(USBTX, USBRX); // tx, rx
BusOut bugleds(LED1, LED2, LED3, LED4);

/**
*     @brief DS18X20_show_temp
*    @param  [in] id[] = rom_code
*    @param  [in] n number of id[n]
*    @param  [out] text temp in degre celsius

*     @date 20/06/2011
*/
/*
void DS18X20_show_temp(uint8_t subzero, uint8_t cel, uint8_t cel_frac_bits,char *text) {
    uint16_t decicelsius;
    char s[10];
    float temperature;
    sprintf(text,"");
    sprintf(s,"%s", (subzero)?"-":"+");
    strcat(text,s);
    decicelsius = DS18X20_temp_to_decicel(subzero, cel, cel_frac_bits);
    temperature = decicelsius;
    temperature = temperature/10;
    sprintf(s,"%4.1f", temperature);
    strcat(text,s);

}
*/

int main(void) {
    uint8_t sp[25];
    uint8_t num_sensor;
    uint8_t subzero, cel, cel_frac_bits;
    Timer timer;

    pc.printf("version 2017-06-2\n");
    pc.printf("Qantum 1 wire interface test!\n");



     ow_PullUp();
    if (search_sensors(&nSensors,&gSensorIDs[0][0])) {
        pc.printf("no Sensors found\n");
        return -1;
    } else {
        pc.printf("Sensors found : %d\n", nSensors);

        for (num_sensor=0; num_sensor<nSensors; ++num_sensor) {
            char text_id[25];
            ow_show_id( &gSensorIDs[num_sensor][0], OW_ROMCODE_SIZE,text_id );
            pc.printf("%s Sensor # %d is a ",text_id, num_sensor+1);
            //DS18X20
            if (( gSensorIDs[num_sensor][0] == DS18S20_ID) || ( gSensorIDs[num_sensor][0] == DS18B20_ID)) {
                if ( gSensorIDs[num_sensor][0] == DS18S20_ID)
                    pc.printf("DS18S20/DS1820 ");
                if ( gSensorIDs[num_sensor][0] == DS18B20_ID)
                    pc.printf("DS18B20 ");
/*                    
                if ( DS18X20_get_power_status( &gSensorIDs[num_sensor][0] ) ==
                        DS18X20_POWER_PARASITE )
                    pc.printf( "parasite" );
                else pc.printf( "externally" );
                pc.printf( " powered\n" );
8/
            }
            //DS2450
 /*
            if ( gSensorIDs[num_sensor][0] == DS2450_ID) {
                pc.printf("DS2450 ");
                for (uint8_t i=0 ; i< DS2450_SP_SIZE; i++ )     // DS2450 POWER extern
                    sp[i]=0;
                sp[4]=0x40;
                if (DS2450_configure_page(&gSensorIDs[num_sensor][0], DS2450_PAGE3,&sp[0]))
                    pc.printf("CRC Error conf page \n");
                if (DS2450_read_page(&gSensorIDs[num_sensor][0], DS2450_PAGE3,&sp[0]))
                    ;
                else
                    printf( "\n" );
                for ( char i=0 ; i< DS2450_SP_SIZE; i++ )
                    printf(":%2.2X",sp[i]);
                printf( "\n" );
                if (DS2450_configure_channel_ADC(&gSensorIDs[num_sensor][0],DS2450_ADCA,DS2450_12_BIT ,DS2450_IR_5V1))
                    pc.printf("CRC Error conf ADC\n");
                if (DS2450_configure_channel_ADC(&gSensorIDs[num_sensor][0],DS2450_ADCB,DS2450_1_BIT ,DS2450_IR_5V1))
                    pc.printf("CRC Error conf ADC\n");
                if (DS2450_configure_channel_ADC(&gSensorIDs[num_sensor][0],DS2450_ADCC,DS2450_1_BIT,DS2450_IR_5V1))
                    pc.printf("CRC Error conf ADC\n");
                if (DS2450_configure_channel_ADC(&gSensorIDs[num_sensor][0],DS2450_ADCD,DS2450_1_BIT,DS2450_IR_5V1))
                    pc.printf("CRC Error conf ADC\n");
                uint16_t adc[4];
                if (DS2450_start_and_read_ADC(&gSensorIDs[num_sensor][0], &adc[0]))
                    pc.printf("CRC Error read ADC\n");
                for (uint8_t i=0; i<4; ++i )
                    pc.printf(" adc%d:%X %f |",i,adc[i],adc[i]*5.1/65535);
                pc.printf("\n");
            }
*/
        }
        nErreur=0;
        nErreur_CRC=0;
        nErreur_BUS=0;
        nErreur_DS18X20=0;
        nErreur_DS2450=0;
        nErreur_DS2450_START=0;
        nErreur_BUSY=0;
        nErreur_RESET=0;
        nMesure=0;
        timer.start();
/*
        while (1) {
            wait(1);            // temps attente
            nMesure++;
            pc.printf("Mesure:%d erreur%d\n",nMesure,nErreur);
            bugleds=nErreur;
            switch (DS18X20_start_meas(DS18X20_POWER_EXTERN, 0 )) { // start measure ALL DS18X20
                case DS18X20_OK :
                    wait_ms(DS18B20_TCONV_12BIT);
                    break;
                case DS18X20_START_FAIL:
                    nErreur++;
                    nErreur_DS18X20++;
                    pc.printf("sensor DS18X20 : Start meas. failed \n");
                    break;
            }

            for (num_sensor=0; num_sensor<nSensors; ++num_sensor) {
                //DS2450
                if (gSensorIDs[num_sensor][0] == DS2450_ID) {
                    micro_seconds=timer.read_us();
                    uint16_t adc[4];
                    if (DS2450_start_and_read_ADC(&gSensorIDs[num_sensor][0], &adc[0])) // 3 essais avant erreur
                        if (DS2450_start_and_read_ADC(&gSensorIDs[num_sensor][0], &adc[0]))
                            switch (DS2450_start_and_read_ADC(&gSensorIDs[num_sensor][0], &adc[0])) {
                                case OW_OK:

                                    break;
                                case OW_BUSY:
                                    nErreur++;
                                    nErreur_DS2450++;
                                    nErreur_BUSY++;

                                    break;
                                case OW_ERROR:
                                    nErreur++;
                                    nErreur_DS2450++;
                                    nErreur_BUS++;

                                    break;
                                case OW_ERROR_CRC:
                                    nErreur++;
                                    nErreur_DS2450++;
                                    nErreur_CRC++;
                                    pc.printf("Error:%d\n",nErreur);
                                    pc.printf("ErrorDS2450_busy:%d\n",nErreur_BUSY);
                                    pc.printf("ErrorDS2450_start:%d\n",nErreur_DS2450_START);
                                    pc.printf("ErrorDS2450:%d\n",nErreur_DS2450);
                                    pc.printf("ErrorDS18X20:%d\n",nErreur_DS18X20);
                                    pc.printf("ErrorCRC:%d\n",nErreur_CRC);
                                    pc.printf("ErrorBUS:%d\n",nErreur_BUS);
                                    pc.printf("ErrorRESET:%d\n",nErreur_RESET);
                                    pc.printf("Mesure:%d\n",nMesure);
                                    break;
                                case OW_SHORT_CIRCUIT:
                                    nErreur++;
                                    nErreur_DS2450++;
                                    nErreur_RESET++;
                                    break;
                                case OW_ERROR_BAD_ID:

                                    break;
                            }
                    micro_seconds = timer.read_us()- micro_seconds; // duree lecture DS2450
                    pc.printf("sensor %d : time:%umicros\n",num_sensor+1,micro_seconds);
                    for (uint8_t i=0; i<4; ++i )
                        pc.printf(" adc%d:%X %f |",i,adc[i],adc[i]*5.1/65535);
                    pc.printf("\n");
                }

                //TEMP DS1820
                if ( (gSensorIDs[num_sensor][0] == DS18S20_ID) ||( gSensorIDs[num_sensor][0] == DS18B20_ID))
                    switch (DS18X20_read_meas( &gSensorIDs[num_sensor][0], &subzero, &cel, &cel_frac_bits)) {
                        case DS18X20_OK :
                            char text[25];
                            DS18X20_show_temp(subzero, cel, cel_frac_bits,text);
                            pc.printf("sensor %d : %s\n",num_sensor+1,text);
                            break;
                        case OW_ERROR:
                            nErreur++;
                            nErreur_BUS++;
                            nErreur_DS18X20++;
                            pc.printf("sensor %d : BUS Error\n",num_sensor+1);
                            break;
                        case DS18X20_ERROR_CRC:
                            nErreur++;
                            nErreur_DS18X20++;
                            nErreur_CRC++;
                            pc.printf("sensor %d : CRC Error\n",num_sensor+1);
                            break;
                    }
            }
*/
        }
    }
}
