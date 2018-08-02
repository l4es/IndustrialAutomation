package eu.halaser.beamctrl;

import android.content.*;
import android.os.*;
import android.preference.*;
 
public class GlobalPrefs extends PreferenceActivity
{
 
   @Override
   protected void onCreate(Bundle savedInstanceState)
   {
      int themeVal=0;

      SharedPreferences SP = PreferenceManager.getDefaultSharedPreferences(getBaseContext());
      try
      {
         themeVal=Integer.parseInt(SP.getString("theme","1"));
      }
      catch (NumberFormatException nfe)
      {
      }
      if (themeVal==2) setTheme(R.style.LightTheme);
      else if (themeVal==3) setTheme(R.style.DarkTheme);

      super.onCreate(savedInstanceState);
      addPreferencesFromResource(R.layout.prefs);
   }
}

