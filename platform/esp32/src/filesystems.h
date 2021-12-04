#ifndef __filesystems__
#define __filesystems__

#include "FS.h"
#include "SPIFFS.h"
#include "model_data.h"

// Writes a new model with parameters
void spiffWriteNewModel(char  newModel[], int newModelLen, std::string path){
	bool modelExists = SPIFFS.exists(path.c_str());
	if(modelExists){
		Serial.println((String)"Deleting old " + path.c_str() + " model");
		SPIFFS.remove(path.c_str());
	}
	Serial.println((String)"Writing new model to " + path.c_str());
	File file =  SPIFFS.open(path.c_str(), FILE_WRITE); 
	if(!file){
		Serial.println("Failed to open file for writing");
		return;
	}
	for(int i  = 0; i < newModelLen; i++){
		file.print(newModel[i]);
	}		
	file.close();
}

/*
Don't need to write the default model to memory because it already exists (model.cpp). This function was used to test the SPIFF filesystem
and see if writing and reading a model works as intended. This was necessary because there was no functionality to get a user model
to the device to write to memory and read from.
*/
// Writes the defualt model to the ESP32. If a model already exists it will just return.
void spiffManualWriteDefualtModel(){
	//SPIFFS.remove("/model.txt");
	bool modelExists = SPIFFS.exists("/model.txt");
	if(!modelExists){
		Serial.println("File does not exist. Writing default model to /model.txt");
		File file =  SPIFFS.open("/model.txt", FILE_WRITE); 
		if(!file){
			Serial.println("Failed to open file for writing");
			return;
		}
		for(int i  = 0; i < converted_model_tflite_len; i++){
			file.print(converted_model_tflite[i]);
		}		
		file.close();
	}
	else{
		Serial.println("Default model already exists, skipping step");
	}
}

void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
	Serial.printf("Listing directory: %s\r\n", dirname);

	File root = fs.open(dirname);
	if(!root){
		Serial.println("− failed to open directory");
		return;
	}
	if(!root.isDirectory()){
		Serial.println(" − not a directory");
		return;
	}

	File file = root.openNextFile();
	while(file){
		if(file.isDirectory()){
			Serial.print("  DIR : ");
			Serial.println(file.name());
			if(levels){
				listDir(fs, file.name(), levels -1);
			}
		} 
		else {
			Serial.print("  FILE: ");
			Serial.print(file.name());
			Serial.print("\tSIZE: ");
			Serial.println(file.size());
		}
		file = root.openNextFile();
	}
}

void readFile(fs::FS &fs, const char * path){
   Serial.printf("Reading file: %s\r\n", path);

   File file = fs.open(path);
   if(!file || file.isDirectory()){
	   Serial.println("− failed to open file for reading");
	   return;
   }

   Serial.println("− read from file:");
   while(file.available()){
	  Serial.write(file.read());
   }
}

void writeFile(fs::FS &fs, const char * path, const char * message){
   Serial.printf("Writing file: %s\r\n", path);

   File file = fs.open(path, FILE_WRITE);
   if(!file){
	  Serial.println("− failed to open file for writing");
	  return;
   }
   if(file.print(message)){
	  Serial.println("− file written");
   }else {
	  Serial.println("− frite failed");
   }
}

void appendFile(fs::FS &fs, const char * path, const char * message){
   Serial.printf("Appending to file: %s\r\n", path);

   File file = fs.open(path, FILE_APPEND);
   if(!file){
	  Serial.println("− failed to open file for appending");
	  return;
   }
   if(file.print(message)){
	  Serial.println("− message appended");
   } else {
	  Serial.println("− append failed");
   }
}

void renameFile(fs::FS &fs, const char * path1, const char * path2){
   Serial.printf("Renaming file %s to %s\r\n", path1, path2);
   if (fs.rename(path1, path2)) {
	  Serial.println("− file renamed");
   } else {
	  Serial.println("− rename failed");
   }
}

void deleteFile(fs::FS &fs, const char * path){
	Serial.printf("Deleting file: %s\r\n", path);
   if(fs.remove(path)){
	  Serial.println("− file deleted");
   } else {
	  Serial.println("− delete failed");
   }
}

#endif