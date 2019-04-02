#include "contiki.h"
#include "sys/etimer.h"
#include "button-sensor.h"
#include "batmon-sensor.h"

#include <stdio.h>

/*---------------------------------------------------------------------------*/
#define BUF_SIZE 8
static struct etimer et_sensor;
static struct etimer et_uart;
static int buffer[BUF_SIZE];
static int buf_c = 0;

/*---------------------------------------------------------------------------*/
PROCESS(sensor_process, "Sensor process");
PROCESS(uart_process, "UART process");

/*---------------------------------------------------------------------------*/
AUTOSTART_PROCESSES(&sensor_process,&uart_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(sensor_process, ev, data)
{
  PROCESS_BEGIN();

  SENSORS_ACTIVATE(batmon_sensor);

  etimer_set(&et_sensor, CLOCK_SECOND); // a cada segundo

  while(1) {
    PROCESS_WAIT_EVENT();
    if(ev == PROCESS_EVENT_TIMER)  // se passou um segundo
    {
        etimer_reset(&et_sensor); // reinicia timer

        int val = batmon_sensor.value(BATMON_SENSOR_TYPE_TEMP); // lê sensor

        buffer[buf_c++] = val; // atualiza buffer
        buf_c = buf_c % BUF_SIZE;

        printf("Leu %d\n", val);
    }
  }

  PROCESS_END();
}

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(uart_process, ev, data)
{
  PROCESS_BEGIN();

  etimer_set(&et_uart, CLOCK_SECOND*10); // a cada 10 segundos

  while(1) {
    PROCESS_WAIT_EVENT();
    if(ev == PROCESS_EVENT_TIMER)  // se passaram 10 segundos
    {
        etimer_reset(&et_uart); // reinicia timer

        int avg = 0;
        for(int i=0;i<BUF_SIZE;i++) {
            avg += buffer[i];
        }
        avg = avg/BUF_SIZE;
        printf("Temperatura media: %d\n", avg);
    }
  }

  PROCESS_END();
  }
