void compass()
{
  int pointer = 0;
  display.setTextSize(1);
  display.clearDisplay();
  display.setTextColor(BLACK, WHITE);
  display.setCursor(0, 0);
  display.print("Compass");
  display.drawFastHLine(0, 8, 40, BLACK);
  display.drawCircle(59, 23, 23, BLACK); // big circle
  if (mpu.readByte(MPU9250_ADDRESS, INT_STATUS) & 0x01)
  {
    mpu.readMagData(mpu.magCount);  // Read the x/y/z adc values
    mpu.getMres();

    //display.setCursor(0, 15);
    /*    display.print((int)(mpu.magCount[0]));
        display.print(" ");
        display.print((int)(mpu.magCount[1]));
        display.print(" ");
        display.println((int)(mpu.magCount[2]));
        display.print((int)(mpu.magbias[0]));
        display.print(" ");
        display.print((int)(mpu.magbias[1]));
        display.print(" ");
        display.println((int)(mpu.magbias[2]));
    */
    //display.print((int)(90.0-atan(mpu.my/mpu.mx) * RAD_TO_DEG));
    //display.println(" deg");

    // Calculate the magnetometer values in milliGauss
    // Include factory calibration per data sheet and user environmental
    // corrections
    // Get actual magnetometer value, this depends on scale being set
    // mpu.mx = (float)(mpu.magCount[0] - mpu.magbias[0]) * mpu.mRes * mpu.magCalibration[0];         mpu.magbias[0];
    // mpu.my = (float)(mpu.magCount[1] - mpu.magbias[1]) * mpu.mRes * mpu.magCalibration[1];
    // mpu.mz = (float)(mpu.magCount[2] - mpu.magbias[2]) * mpu.mRes * mpu.magCalibration[2];
    mpu.mx = mpu.magCount[0] - mpu.magbias[0];
    mpu.my = mpu.magCount[1] - mpu.magbias[1];
    mpu.mz = mpu.magCount[2] - mpu.magbias[2];

    // pointer = int(90.0-atan(mpu.my/mpu.mx) * RAD_TO_DEG);
    // orient so top of screen is "north"
    // pointer = int(atan(mpu.my/mpu.mx) * RAD_TO_DEG);

    /*   display.print((int)(mpu.mx));
       display.print(" ");
       // display.setCursor(25, 15);
       display.print((int)(mpu.my));
       display.print(" ");
       // display.setCursor(50, 15);
       display.println((int)(mpu.mz));
       // display.print(" mG");
       //display.setCursor(0,35);
       //display.print(" ");
    */
    pointer = int(atan2(mpu.my, mpu.mx) * RAD_TO_DEG);
    pointer *= -1;
    if (pointer < 0) pointer = 360 + pointer;
    display.setTextSize(2);
    display.setCursor(0, 20);

    display.print(pointer);
    display.setCursor(0, 39);
    display.setTextSize(1);
    display.print("degrees");
  }
  mpu.updateTime();
  display.display();
  delay(500);
}

void magcalMPU9250()
{
  // Serial.println("set compass");
  uint16_t ii = 0, sample_count = 0;
  int32_t mag_bias[3] = {0, 0, 0};
  int16_t mag_max[3] = {0, 0, 0}, mag_min[3] = {0, 0, 0}, mag_temp[3] = {0, 0, 0};

  display.setTextSize(1);
  display.clearDisplay();
  display.setTextColor(BLACK, WHITE);
  display.setCursor(0, 0);
  display.print("Set Compass");
  display.drawFastHLine(0, 10, 83, BLACK);
  display.setCursor(0, 15);
  display.print("Wave device in a figure eight until done!");
  display.display();
  delay(4000);

  sample_count = 64;
  for (ii = 0; ii < sample_count; ii++) {
    mpu.readMagData(mag_temp); // Read the mag data
    for (int jj = 0; jj < 3; jj++) {
      if (mag_temp[jj] > mag_max[jj]) mag_max[jj] = mag_temp[jj];
      if (mag_temp[jj] < mag_min[jj]) mag_min[jj] = mag_temp[jj];
    }
    delay(135); // at 8 Hz ODR, new mag data is available every 125 ms
  }
  display.setTextSize(1);
  display.clearDisplay();
  display.setTextColor(BLACK, WHITE);
  display.setCursor(0, 0);
  display.print("Set Compass");
  display.drawFastHLine(0, 10, 83, BLACK);
  display.setCursor(0, 15);
  display.print("x: ");
  display.print(mag_max[0]);
  display.print(" ");
  display.println(mag_min[0]);
  display.print("y: ");
  display.print(mag_max[1]);
  display.print(" ");
  display.println(mag_min[1]);
  display.print("z: ");
  display.print(mag_max[2]);
  display.print(" ");
  display.println(mag_min[2]);
  display.display();

  mpu.magbias[0] = (mag_max[0] + mag_min[0]) / 2; // get mid point x mag bias in counts
  mpu.magbias[1] = (mag_max[1] + mag_min[1]) / 2; // get mid point y mag bias in counts
  mpu.magbias[2] = (mag_max[2] + mag_min[2]) / 2; // get mid point z mag bias in counts

  // save the settings in the eeprom, scale stored values by 1/4 to be -126 to +127
  EEPROM.write(eeMagBias, int8_t(mpu.magbias[0] / 4));;
  EEPROM.write(eeMagBias + 1, int8_t(mpu.magbias[1] / 4));
  EEPROM.write(eeMagBias + 2, int8_t(mpu.magbias[2] / 4));

  delay(10000);
}
