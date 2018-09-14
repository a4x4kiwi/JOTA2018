void level() {

  float accX, accY, accZ;
  int angleX, angleY, x, y;

  display.setTextSize(1);
  display.clearDisplay();
  display.setTextColor(BLACK, WHITE);
  display.setCursor(0, 0);
  display.print("Level");
  display.drawFastHLine(0, 8, 30, BLACK);
  display.drawCircle(59, 23, 23, BLACK);
  display.drawCircle(59, 23, 11, BLACK);
  display.drawCircle(59, 23, 5, BLACK);


  if (mpu.readByte(MPU9250_ADDRESS, INT_STATUS) & 0x01)
  {
    mpu.readAccelData(mpu.accelCount);  // Read the x/y/z adc values
    mpu.getAres();

    // Now we'll calculate the accleration value into actual g's
    // This depends on scale being set

    accX = (float)mpu.accelCount[0] * mpu.aRes - mpu.accelBias[0];
    accY = (float)mpu.accelCount[1] * mpu.aRes - mpu.accelBias[1];
    accZ = (float)mpu.accelCount[2] * mpu.aRes - mpu.accelBias[2];

    angleX = (int)(atan2(accX, accZ) * 180 / PI);
    angleY = (int)(atan2(accY, accZ) * 180 / PI);

    display.setCursor(0, 10);
    display.print("x=");
    display.print( angleX);
    display.setCursor(0, 40);
    display.print("y=");
    display.print( angleY);

    display.fillCircle(autoScale(-20, 20, 35, 83, angleX ), autoScale(-20, 20, 47, 0, angleY), 3, BLACK);

    display.display();
    delay(100);

  }
}

