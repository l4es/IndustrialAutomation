package com.oapc;

import android.view.*;
import android.view.ViewGroup.LayoutParams;
import android.content.*;
import android.widget.*;
import android.graphics.*;

public class BasePanel extends View 
{
           int            m_myID;
   private int            m_borderNum,m_size_x,m_size_y;
   private int            m_fgColour;
           Context        ctx; 
           AbsoluteLayout absLayout;
           FrameLayout    rootLayout;
   
   BasePanel(AbsoluteLayout parent,Context context,int x,int y,int width,int height)
   {
      super(context);
      ctx=context;
      m_myID=0;
      rootLayout=new FrameLayout(context);
      parent.addView(rootLayout,new AbsoluteLayout.LayoutParams(width, height, x, y));
      rootLayout.addView(absLayout,new LayoutParams(LayoutParams.FILL_PARENT,LayoutParams.FILL_PARENT));
      init(parent,context);
   }

   
   
   /**
    * The root element
    * @param parent
    * @param context
    * @param myID
    */
   BasePanel(AbsoluteLayout parent,Context context,int myID)
   {
      super(context);
      ctx=context;
      m_myID=myID;
      rootLayout=new FrameLayout(context);
      parent.addView(rootLayout,new AbsoluteLayout.LayoutParams(LayoutParams.FILL_PARENT,LayoutParams.FILL_PARENT,0,0));
      init(parent,context);
   }
   
   
   
   private void init(ViewGroup parent,Context context)
   {
      rootLayout.addView(this,new LayoutParams(LayoutParams.FILL_PARENT,LayoutParams.FILL_PARENT));
      absLayout=new AbsoluteLayout(context);
      rootLayout.addView(absLayout,new LayoutParams(LayoutParams.FILL_PARENT,LayoutParams.FILL_PARENT));
      m_borderNum=-1;
   }

   
   
   void close()
   {
      //TODO: if (m_myID==0) MainWin.g_objectList.deleteObjects(m_myID,1);   
   }

   
   
   protected void onDraw(Canvas dc)
   {
      super.onDraw(dc);
      paintBackground(dc);
	  paintContents(dc);
	  paintBorder(dc);
   }

   
   
   protected void paintBackground(Canvas dc)
   {
      Paint paint;
      
      m_size_x =getWidth();
      m_size_y =getHeight();
      
/*      paint=new Paint();
      paint.setARGB(255,(m_bgColour & 0xFF0000)>>16,(m_bgColour & 0x00FF00)>>8,(m_bgColour & 0x000FF));
      paint.setStyle(Paint.Style.FILL);
      dc.drawRect(0, 0,m_size_x,m_size_y,paint);*/      
   }
   
   
   
   void SetBackgroundColour(int colour)
   {
      setBackgroundColor(0xFF000000 | colour);
   }



   void SetForegroundColour(int colour)
   {
      m_fgColour=colour;
   }



   void paintContents(Canvas dc)
   {
      /*wxNode     *node;
      flowObject *object;

      node=g_objectList.getDrawableObject(NULL);
      while (node)
      {
         object=(flowObject*)node->GetData();
         if ((object->data.type==HMI_TYPE_IMAGE) && (object->data.isChildOf==m_myID))
         {
#ifdef ENV_PLAYER
            oapc_thread_mutex_lock(g_imagePaintMutex);
#endif
            if ((((hmiImage*)object)->bitmap->Ok()) && (((hmiImage*)object)->m_isVisible))
             dc.DrawBitmap(*((hmiImage*)object)->bitmap,((hmiImage*)object)->getPos().x/1000.0,((hmiImage*)object)->getPos().y/1000.0,true);
#ifdef ENV_PLAYER
            oapc_thread_mutex_unlock(g_imagePaintMutex);
#endif
         }
         node=g_objectList.getDrawableObject(node);
      }*/
   }

   
   
   private void setLighterColour(int colour,int light,Paint paint)
   {
      int r,g,b;
 	
      r=((colour & 0xFF0000)>>16)+150; if (r>255) r=255;
      g=((colour & 0xFF00)>>8)+150;    if (g>255) g=255;
      b=(colour & 0xFF)+150;           if (b>255) b=255;
      paint.setARGB(255,r,g,b);
   }

   
   
   void paintBorder(Canvas dc)
   {
	   Paint paint;
	  
      if (m_borderNum==0) return;      
      
      paint=new Paint();
      paint.setARGB(255,(m_fgColour & 0xFF0000)>>16,(m_fgColour & 0x00FF00)>>8,(m_fgColour & 0x000FF));
      paint.setStyle(Paint.Style.STROKE);
      paint.setStrokeWidth(1);
      
      
      if (m_borderNum==1)
	   {
	      dc.drawLine(m_size_x,0,0,0,paint);      dc.drawLine(0,0,0,m_size_y,paint);
	      dc.drawLine(m_size_x-3,1,1,1,paint);    dc.drawLine(1,1,1,m_size_y-2,paint);
	
          setLighterColour(m_fgColour,150,paint);
	      dc.drawLine(1,m_size_y-1,m_size_x-1,m_size_y-1,paint); dc.drawLine(m_size_x-1,m_size_y-1,m_size_x-1,0,paint);
	      dc.drawLine(1,m_size_y-2,m_size_x-2,m_size_y-2,paint); dc.drawLine(m_size_x-2,m_size_y-2,m_size_x-2,0,paint);
	   }
      else if (m_borderNum==2)
	   {
	      dc.drawLine(1,m_size_y-1,m_size_x-1,m_size_y-1,paint); dc.drawLine(m_size_x-1,m_size_y-1,m_size_x-1,0,paint);
	      dc.drawLine(1,m_size_y-2,m_size_x-2,m_size_y-2,paint); dc.drawLine(m_size_x-2,m_size_y-2,m_size_x-2,0,paint);
	
          setLighterColour(m_fgColour,150,paint);
	      dc.drawLine(m_size_x,0,0,0,paint);      dc.drawLine(0,0,0,m_size_y,paint);
	      dc.drawLine(m_size_x-3,1,1,1,paint);    dc.drawLine(1,1,1,m_size_y-2,paint);
	   } 
	   else if (m_borderNum==3) dc.drawRect(0,0,m_size_x,m_size_y,paint);
	   else if (m_borderNum==4)
	   {
	      dc.drawRect(0,0,m_size_x,m_size_y,paint);
	      dc.drawRect(2,2,m_size_x-4,m_size_y-4,paint);
	   }
	   else if (m_borderNum==5)
	   {
		  paint.setPathEffect(new DashPathEffect(new float[] {10,1}, 0)); // dot
	      dc.drawRect(0,0,m_size_x,m_size_y,paint);
	   }
	   else if (m_borderNum==6)
	   {
		   paint.setPathEffect(new DashPathEffect(new float[] {10,5}, 0)); // short dash
	      dc.drawRect(0,0,m_size_x,m_size_y,paint);
	   }
	   else if (m_borderNum==7)
	   {
         paint.setPathEffect(new DashPathEffect(new float[] {10,5,10,1}, 0)); // dot dash
	      dc.drawRect(0,0,m_size_x,m_size_y,paint);
	   }
	   else if (m_borderNum==8)
	   {
		   paint.setPathEffect(new DashPathEffect(new float[] {5,10}, 0)); // long dash
	      dc.drawRect(0,0,m_size_x,m_size_y,paint);
	   }
   }

   
   
   void setBorderNum(int borderNum)
   {
      m_borderNum=borderNum;
   }

}
