package eu.halaser.beamctrl;

import android.util.*;
import java.io.*;
import java.lang.*;
import java.net.*;
import org.xmlpull.v1.*;


class BeamConnection extends Thread
{
   boolean running=true;
   private XmlPullParser parser = Xml.newPullParser();

   ConnectionData data;

   private ScanService  service;

   BeamConnection(ConnectionData in_data,ScanService in_service)
   {
      data=in_data;
      service=in_service;
      start();
   }


   private void parseData(String xmlString)
   {
      boolean update=false;

      try
      {
         parser.setInput(new StringReader(xmlString));

         int eventType = parser.getEventType();
         while (eventType != XmlPullParser.END_DOCUMENT)
         {
            String attrValue,attrValue2;

            switch (eventType)
            {
               case XmlPullParser.START_DOCUMENT:
                  break;
               case XmlPullParser.START_TAG:
                  if (parser.getName().equalsIgnoreCase("BCMsg"))
                  {
                     attrValue=parser.getAttributeValue(null,"ID");
                     if (attrValue!=null) data.setID(attrValue);
                     attrValue=parser.getAttributeValue(null,"Time");
                     if (attrValue!=null) data.setTime(attrValue);
                     update=true;
                  }
                  else if (parser.getName().equalsIgnoreCase("Data"))
                  {
                     attrValue=parser.getAttributeValue(null,"State");
                     if (attrValue!=null) data.setState(attrValue,service);
                     attrValue=parser.getAttributeValue(null,"Text");
                     if (attrValue!=null) data.setText(attrValue);
                     attrValue=parser.getAttributeValue(null,"Project");
                     if (attrValue!=null) data.setProject(attrValue);
                     attrValue=parser.getAttributeValue(null,"Parts");
                     if (attrValue!=null) data.setParts(attrValue);

                     attrValue=parser.getAttributeValue(null,"MaxSlices");
                     attrValue2=parser.getAttributeValue(null,"CurrSlice");
                     if ((attrValue!=null) && (attrValue2!=null)) data.setProgress(attrValue,attrValue2);

                     update=true;
                  }
            }
            eventType = parser.next();
         }
      }
      catch (XmlPullParserException xpe)
      {

      }
      catch (IOException ioe)
      {

      }
      if (update) data.updateUI();
   }


   public void run()
   {
      String recvBuffer="";
      Socket sock;

      while (running)
      {
         try
         {
            sock=new Socket();
            sock.connect(new InetSocketAddress(data.host,11355),1000);

            try
            {
               BufferedReader in=new BufferedReader(new InputStreamReader(sock.getInputStream()));
               while (running)
               {
                  int    pos;
                  String readString;

                  readString=in.readLine();
                  if (readString==null) break;
                  recvBuffer=recvBuffer+readString;
                  pos=recvBuffer.indexOf("<BCMsg/>");
                  if (pos<=0) pos=recvBuffer.indexOf("</BCMsg>");
                  if (pos>0)
                  {
                     // end of XML document found
                     pos+=8;

                     String xmlBuffer=recvBuffer.substring(0,pos);
                     xmlBuffer=xmlBuffer.trim();
                     recvBuffer=recvBuffer.substring(pos);
                     recvBuffer=recvBuffer.trim();

                     parseData(xmlBuffer);
                  }
               }
               sock.close();
               data.setState(null,service);
            }
            catch (IOException ioe2)
            {
               sock.close();
               data.setState(null,service);
            }
         }
         catch (SocketTimeoutException ste)
         {
            try
            {
               Thread.sleep(1000); // wait for one second until next connection attempt
            }
            catch (InterruptedException ie)
            {

            }
         }
         catch (IOException ioe)
         {
            try
            {
               Thread.sleep(1000); // wait for one second until next connection attempt
            }
            catch (InterruptedException ie)
            {

            }
         }
      }
   }
}
