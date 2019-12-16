#include <stdlib.h>
#include <string.h>
#include <Nixie.h>
#include <MultiDAC.h>
#include "messaging.h"

Nixie nixie(NIXIE_PIN_DATA, NIXIE_PIN_CLK, NIXIE_PIN_LATCH);
MultiDAC barGraph(GRAPH_PIN_DATA, GRAPH_PIN_CLK, GRAPH_PIN_LATCH);

void setup() {  
  Serial.begin(9600);
  Serial.println("ready");
  
  nixie.clear(numDigits);
  barGraph.clear(1);
}

void print_err(int err) {
  switch (err) {
    case SERIAL_READ_OK:
      printf("The operation completed successfully\n");
      break;
      
    case SERIAL_READ_ERR_FORMAT:
      printf("The message was improperly formatted or incomplete\n");
      break;
      
    case SERIAL_READ_ERR_EMPTY:
      printf("No message was found\n");
      break;
      
    case default:
      printf("Invalid error id\n");
      break;
  }
}

/* Reads and stores a null-terminated message from the serial port
 *
 * Returns size of message, including null character
 *
 */ 
int serial_read_message(char *msg) {
  char buf[SERIAL_MAX_MSG_LEN], read;
  int read_len = -1, ret;
  
  read = Serial.read();
  
  while (read_len < SERIAL_MAX_MSG_LEN && read != SERIAL_READ_END_CHAR) {
    switch (read) {
      case SERIAL_READ_START_CHAR:
        read_len = 0;
        continue;
        
      case SERIAL_READ_END_CHAR:
        if (read_len > 0) {
          ret = read_len;
        }
        else {
          ret = -SERIAL_READ_ERR_FORMAT;
        }
        break;
        
      case SERIAL_READ_ERR_CHAR:
        ret = -SERIAL_READ_ERR_EMPTY;
        break;
        
      case default:
        if (read_len < 0) {
          ret = -SERIAL_READ_ERR_FORMAT;
        }
        else {
          buf[read_len++] = read;          
        }
        break;
        
      read = Serial.read();
    }
  }
  
  return ret;
}

/* Using the last known message, fire off whatever action it requested */
void process_message(char *msg) {
  char type_s[3] = {0x00};
  int type;
  
  memcpy(type_s, msg, 2);
  type = atoi(type_s);
  
  switch(type)
  {
    case MSG_TYPE_DEMO:
      run_demo();
      break;
      
    case MSG_TYPE_MESSAGE:
      display_nixie_message(&msg[2]);
      break;
      
    case MSG_TYPE_CLEAR:
      clear_nixies();
      break;
      
    case MSG_TYPE_GRAPH:
      display_bargraph_message(&msg[2]);
      break;
  }
}

void loop() {
  char msg[SERIAL_MAX_MSG_LEN];
  int ret;
  
  if ((ret = serial_read_message(msg)) < 0) {
    print_err(ret);
  }
  
  process_message(msg);
}