void temperature()
{

  display.setTextSize(1);
  display.clearDisplay();
  display.setTextColor(BLACK, WHITE);
  display.setCursor(18, 0);
  display.print("Weather");
  display.drawFastHLine(0, 10, 83, BLACK);
  display.setCursor(5, 15);
  display.print(bme.readTemperature());
  display.print(" Deg C");
  display.setCursor(5, 25);
  display.print(bme.readHumidity());
  display.print(" % RH");
  display.setCursor(5, 35);
  display.print(bme.readPressure() / 100.0F);
  display.print(" hPa");
  display.display();
  delay(1000);
}

