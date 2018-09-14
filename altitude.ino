void altitude()
{

  display.setTextSize(1);
  display.clearDisplay();
  display.setTextColor(BLACK, WHITE);
  display.setCursor(12, 0);
  display.print("Altitude");
  display.drawFastHLine(0, 10, 83, BLACK);
  display.setCursor(5, 15);
  display.print(bme.readAltitude(hPaMSL + 900),0);
  display.print(" m ASL");
  display.setCursor(5, 25);
  display.print(bme.readPressure() / 100.0F,1);
  display.print(" hPa");
  
  display.setCursor(5, 40);
  display.print("Cal ");
  display.print(hPaMSL + 900);
  display.print(" hPa");
  
  display.display();
  delay(100);
}
