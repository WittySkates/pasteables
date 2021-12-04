#ifndef __websockets__
#define __websockets__

#include "ESPAsyncWebServer.h"
#include "filesystems.h"

void onWsEventModel(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len){
	if(type == WS_EVT_CONNECT){
		Serial.println("Websocket client connection received for sending model data");
	}
	else if(type == WS_EVT_DISCONNECT){
		Serial.println("Client disconnected");
		Serial.println("-----------------------");
	} 
	// Receives model and saves it. This only handles model data, if anything other than a valid model is sent the program will run into a model error on reboot and crash.
	else if(type == WS_EVT_DATA){
		//data packet
		AwsFrameInfo * info = (AwsFrameInfo*)arg;

		int new_model_tflite_len = info->len;
		char new_model_tflite[new_model_tflite_len];

		for(size_t i=0; i < len; i++){
			new_model_tflite[info->index + i] = (char)data[i];
		}

		if((info->index + len) == info->len){
			if(info->final){
				Serial.println((String)"Model size: " + new_model_tflite_len);
				spiffWriteNewModel(new_model_tflite, new_model_tflite_len, "/userModel.txt");
				printf("ws[%s][%u] %s-message end\n", server->url(), client->id(), (info->message_opcode == WS_TEXT)?"text":"binary");
				client->text("Model received");
			}
		}
	}
}

void onWsEventOther(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len){
	if(type == WS_EVT_CONNECT){
		Serial.println("Websocket client connection received for other");
	}
	else if(type == WS_EVT_DISCONNECT){
		Serial.println("Client disconnected");
		Serial.println("-----------------------");
	} 
	else if(type == WS_EVT_DATA){
		//data packet
		AwsFrameInfo * info = (AwsFrameInfo*)arg;

		Serial.println((char *)data);
	
		if((info->index + len) == info->len){
			if(info->final){
				printf("ws[%s][%u] %s-message end\n", server->url(), client->id(), (info->opcode == WS_TEXT)?"text":"binary");
				client->text("Data received");
			}
		}
	}
}

#endif