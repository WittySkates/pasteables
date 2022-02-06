#ifndef __websockets__
#define __websockets__

#include "ESPAsyncWebServer.h"
#include "filesystems.h"

/* 
Credits to Nuno Santos's blog for websocket help. See article below:
 https://techtutorialsx.com/2018/09/13/esp32-arduino-web-server-receiving-data-from-javascript-websocket-client/ for websocket help 
 */

int new_model_tflite_len;
char * new_model_tflite;

void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len){
	if(type == WS_EVT_CONNECT){
		Serial.println("Websocket client connection received");
	}
	else if(type == WS_EVT_DISCONNECT){
		Serial.println("Client disconnected");
		Serial.println("-----------------------");
	} 
	else if(type == WS_EVT_DATA){
		//data packet
		AwsFrameInfo * info = (AwsFrameInfo*)arg;

		/* Receives model and saves it. This only handles model data, if anything other than a 
		valid model is sent the program will run into a model error on reboot and crash. */
		if(String(server->url()).equals("/modelWS")){

			if(info->index == 0){
				new_model_tflite_len = info->len;
				new_model_tflite = new char[new_model_tflite_len];
			}

			for(size_t i = 0; i < len; i++){
				new_model_tflite[info->index + i] = (char)data[i];
			}

			if((info->index + len) == info->len){
				if(info->final){
					Serial.println((String)"Model size: " + new_model_tflite_len);
					for(size_t i=0; i < new_model_tflite_len; i++){
       					printf("%02x ", new_model_tflite[i]);
					}
					Serial.println();
					spiffWriteNewModel(new_model_tflite, new_model_tflite_len, "/userModel.txt");
					printf("ws[%s][%u] %s-message end\n", server->url(), client->id(), (info->message_opcode == WS_TEXT)?"text":"binary");
					client->text("Model received");
					delete[] new_model_tflite;
				}
			}
		}
		/* Receives data from another websocket */
		else if(String(server->url()).equals("/otherWS")){
			printf("Message from other websocket: %s\n", (char*)data);
			return;
		}
	}
}

#endif