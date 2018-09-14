void drawMenu()
{
  //page: 1 = Main Menu
  //page: 2 = Sub Menu

  if (page == 1)
  {
    display.setTextSize(1);
    display.clearDisplay();
    display.setTextColor(BLACK, WHITE);
    display.setCursor(15, 0);
    display.print("MAIN MENU");
    display.drawFastHLine(0, 10, 83, BLACK);

    displayMenuItem(menuItem[menuitem - 1], 15, false);
    displayMenuItem(menuItem[menuitem], 25, true);
    displayMenuItem(menuItem[menuitem + 1], 35, false);

    display.display();
  }

  else if (page == 2)
  {
    if (menuitem == 1) //Temperature and Humidity
    {
      temperature();
    }
    else if (menuitem == 2) //Compass
    {
      // calibrate if not done already
      if (mpu.magbias[0] == 0 && mpu.magbias[1] == 0) magcalMPU9250();
      compass();
    }
    else if (menuitem == 3) //Level
    {
      level();
    }
    else if (menuitem == 4) //Altitude
    {
      altitude();
    }
    else if (menuitem == 5) // Set air pressure at sea level
    {
      displayIntMenuPage(menuItem[5], hPaMSL + 900);
    }
    // Menu Item 6 toggles backlight
    else if (menuitem == 7) //Adjust Backlight Brightness
    {
      displayIntMenuPage(menuItem[7], brightness);
    }
    else if (menuitem == 8) // Calibrate Compass
    {
      magcalMPU9250();
      page = 1;  // only run once
    }
    else if (menuitem == 9) //Adjust Contrast
    {
      displayIntMenuPage(menuItem[9], contrast);
    }
  }
}


void displayMenuItem(String item, int position, boolean selected)
{
  if (selected)
  {
    display.setTextColor(WHITE, BLACK);
  } else
  {
    display.setTextColor(BLACK, WHITE);
  }
  display.setCursor(0, position);
  display.print(item);
}
