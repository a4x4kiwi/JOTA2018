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

    if (menuitem == 1)
    {
      displayMenuItem("", 15, false);
      displayMenuItem(menuItem1, 25, true);
      displayMenuItem(menuItem2, 35, false);
    }
    else if (menuitem == 2)
    {
      displayMenuItem(menuItem1, 15, false);
      displayMenuItem(menuItem2, 25, true);
      displayMenuItem(menuItem3, 35, false);
    }
    else if (menuitem == 3)
    {
      displayMenuItem(menuItem2, 15, false);
      displayMenuItem(menuItem3, 25, true);
      displayMenuItem(menuItem4, 35, false);
    }
    else if (menuitem == 4)
    {
      displayMenuItem(menuItem3, 15, false);
      displayMenuItem(menuItem4, 25, true);
      displayMenuItem(menuItem5, 35, false);
    }
    else if (menuitem == 5)
    {
      displayMenuItem(menuItem4, 15, false);
      displayMenuItem(menuItem5, 25, true);
      displayMenuItem(menuItem6, 35, false);
    }
    else if (menuitem == 6)
    {
      displayMenuItem(menuItem5, 15, false);
      displayMenuItem(menuItem6, 25, true);
      displayMenuItem(menuItem7, 35, false);
    }
    else if (menuitem == 7)
    {
      displayMenuItem(menuItem6, 15, false);
      displayMenuItem(menuItem7, 25, true);
      displayMenuItem(menuItem8, 35, false);
    }
    else if (menuitem == 8)
    {
      displayMenuItem(menuItem7, 15, false);
      displayMenuItem(menuItem8, 25, true);
      displayMenuItem(menuItem9, 35, false);
    }
    else if (menuitem == 9)
    {
      displayMenuItem(menuItem8, 15, false);
      displayMenuItem(menuItem9, 25, true);
      displayMenuItem("", 35, false);
    }
    display.display();
  }
  else if (page == 2)
  {
    if (menuitem == 1)
    {
      temperature();
    }
    else if (menuitem == 2)
    {
      // calibrate if not done already
      if (mpu.magbias[0] == 0 && mpu.magbias[1] == 0) magcalMPU9250();
      compass();
    }
    else if (menuitem == 3)
    {
      //Call Level
    }
    else if (menuitem == 4)
    {
      altitude();
    }
    else if (menuitem == 5)
    {
      displayIntMenuPage(menuItem5, hPaMSL + 900);
    }
    // Item 6 toggles backlight
    else if (menuitem == 7)
    {
      displayIntMenuPage(menuItem7, brightness);
    }
    else if (menuitem == 8)
    {
      displayIntMenuPage(menuItem8, contrast);
    }
    else if (menuitem == 9)
    {
      magcalMPU9250();
      page = 1;  // only run once
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
