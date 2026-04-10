#include <Arduino.h>
#include "mpu.h"

MPU6050 mpu(Wire, 0.98, 0.02);

void setup() {

	Serial.begin(115200);
	Wire.begin();
	mpu.begin();
	mpu.calibrate(true);
}

void loop(){
	// Atualiza todos os dados
	mpu.update();  

	// Lê os ângulos de orientação
	float roll = mpu.getAngleX();
	float pitch = mpu.getAngleY();
	float yaw = mpu.getAngleZ();

	Serial.print("Roll: "); Serial.print(roll);
	Serial.print(" | Pitch: "); Serial.print(pitch);
	Serial.print(" | Yaw: "); Serial.println(yaw);

	delay(10);
}