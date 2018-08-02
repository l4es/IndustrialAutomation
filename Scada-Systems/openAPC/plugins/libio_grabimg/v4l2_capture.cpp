/********************************************************************************************

These sources are distributed in the hope that they will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. All
the information given here, within the interface descriptions and within the specification
are subject to change without notice. Errors and omissions excepted.

These sources demonstrate the usage of the OpenAPC Plug-In programming interface. They can be
used freely according to the OpenAPC Dual License: As long as the sources and the resulting
applications/libraries/Plug-Ins are used together with the OpenAPC software, they are
licensed as freeware. When you use them outside the OpenAPC software they are licensed under
the terms of the GNU General Public License.

For more information please refer to http://www.openapc.com/oapc_license.php

*********************************************************************************************/

#ifdef ENV_LINUX

#include <fcntl.h>              /* low-level i/o */
#include <unistd.h>
#include <errno.h>
#include <malloc.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <asm/types.h>          /* for videodev2.h */
#include <linux/videodev2.h>
#include <assert.h>

#include "oapc_libio.h"
#include "libio_grabimg.h"
#include "convert.h"


static int xioctl(int fd,int request,void *arg)
{
   int r;

   do
   {
	   r = ioctl (fd, request, arg);
   }
   while (-1 == r && EINTR == errno);
   return r;
}


int open_device(struct instData *data)
{
   struct stat st;
   char        devName[100+4];

   snprintf(devName,100,"/dev/video%d",data->config.device);
   if (-1 == stat (devName,&st)) return OAPC_ERROR_DEVICE;
   if (!S_ISCHR (st.st_mode)) return OAPC_ERROR_DEVICE;
   data->fd = open(devName, O_RDWR /* required */ | O_NONBLOCK, 0);
   if (data->fd==-1) return OAPC_ERROR_DEVICE;

   {
      enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
      struct v4l2_fmtdesc fmt;
      struct v4l2_frmsizeenum frmsize;

      fmt.index = 0;
      fmt.type = type;
      data->width=0;
      while (ioctl(data->fd, VIDIOC_ENUM_FMT, &fmt) >= 0)
      {
         frmsize.pixel_format = fmt.pixelformat;
         frmsize.index = 0;
         while (ioctl(data->fd, VIDIOC_ENUM_FRAMESIZES, &frmsize) >= 0)
         {
            if (frmsize.type == V4L2_FRMSIZE_TYPE_DISCRETE) 
            {
               if (data->width<frmsize.discrete.width)
               {
                  printf("Discrete: %dx%d - 0x%X\n",frmsize.discrete.width,frmsize.discrete.height,frmsize.pixel_format);
                  data->width=frmsize.discrete.width;
                  data->height=frmsize.discrete.height;
                  data->pixelformat=frmsize.pixel_format;
               }
            } 
            else if (frmsize.type == V4L2_FRMSIZE_TYPE_STEPWISE) 
            {
               if (data->width==0)
               {
                  printf("Stepwise: %dx%d\n",frmsize.stepwise.max_width,frmsize.stepwise.max_height);
                  data->width=frmsize.stepwise.max_width;
                  data->height=frmsize.stepwise.max_height;
                  data->pixelformat=frmsize.pixel_format;
               }
            }
            frmsize.index++;
         }
         fmt.index++;
      }
   }

   return OAPC_OK;
}



static int init_mmap(struct instData *data)
{
   struct v4l2_requestbuffers req;

   memset(&req,0,sizeof(req));
   req.count               = 4;
   req.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
   req.memory              = V4L2_MEMORY_MMAP;

   if (-1 == xioctl (data->fd, VIDIOC_REQBUFS, &req)) return OAPC_ERROR_RESOURCE;
   if (req.count < 2) return OAPC_ERROR_NO_MEMORY;
   data->buffers =(buffer*)calloc (req.count, sizeof (*data->buffers));
   if (!data->buffers) return OAPC_ERROR_NO_MEMORY;

   for (data->n_buffers = 0; data->n_buffers < req.count; ++data->n_buffers)
   {
      struct v4l2_buffer buf;

      memset(&buf,0,sizeof(buf));
      buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
      buf.memory      = V4L2_MEMORY_MMAP;
      buf.index       = data->n_buffers;

      if (-1 == xioctl (data->fd, VIDIOC_QUERYBUF, &buf)) return OAPC_ERROR_RESOURCE;

      data->buffers[data->n_buffers].length = buf.length;
      data->buffers[data->n_buffers].start =mmap (NULL /* start anywhere */,
            buf.length,
            PROT_READ | PROT_WRITE /* required */,
            MAP_SHARED /* recommended */,
            data->fd, buf.m.offset);
      if (MAP_FAILED ==data->buffers[data->n_buffers].start) return OAPC_ERROR_RESOURCE;
   }
   return OAPC_OK;
}



int init_device(struct instData *data)
{
   struct v4l2_capability cap;
   struct v4l2_cropcap cropcap;
   struct v4l2_crop crop;
   struct v4l2_format fmt;
   unsigned int min;

   if (-1 == xioctl (data->fd, VIDIOC_QUERYCAP, &cap)) return OAPC_ERROR_DEVICE;
   if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) return OAPC_ERROR_RESOURCE;
   if (!(cap.capabilities & V4L2_CAP_STREAMING)) return OAPC_ERROR_RESOURCE;
   memset(&cropcap,0,sizeof(cropcap));
   cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

   if (0 == xioctl (data->fd, VIDIOC_CROPCAP, &cropcap))
   {
      crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
      crop.c = cropcap.defrect; /* reset to default */

      if (-1 == xioctl (data->fd, VIDIOC_S_CROP, &crop))
      {
         switch (errno)
         {
            case EINVAL:
               /* Cropping not supported. */
               break;
            default:
               /* Errors ignored. */
               break;
         }
      }
   }
   else
   {
                /* Errors ignored. */
   }
   memset(&fmt,0,sizeof(fmt));
   fmt.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
   fmt.fmt.pix.width       =data->width;
   fmt.fmt.pix.height      =data->height;
   fmt.fmt.pix.pixelformat =data->pixelformat; //V4L2_PIX_FMT_YUYV;
   fmt.fmt.pix.field       =V4L2_FIELD_NONE; //INTERLACED;

   if (-1 == xioctl (data->fd, VIDIOC_S_FMT, &fmt)) return OAPC_ERROR_RESOURCE;

   /* Note VIDIOC_S_FMT may change width and height. */

   /* Buggy driver paranoia. */
   min = fmt.fmt.pix.width * 2;
   if (fmt.fmt.pix.bytesperline < min) fmt.fmt.pix.bytesperline = min;
   min = fmt.fmt.pix.bytesperline * fmt.fmt.pix.height;
   if (fmt.fmt.pix.sizeimage < min) fmt.fmt.pix.sizeimage = min;
   return init_mmap (data);
}



int start_capturing(struct instData *data)
{
   unsigned int i;
   enum v4l2_buf_type type;

   for (i = 0; i < data->n_buffers; ++i)
   {
      struct v4l2_buffer buf;

      memset(&buf,0,sizeof(buf));
      buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
      buf.memory      = V4L2_MEMORY_MMAP;
      buf.index       = i;

      if (-1 == xioctl (data->fd, VIDIOC_QBUF, &buf)) return OAPC_ERROR_RESOURCE;
   }
   type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
   if (-1 == xioctl (data->fd, VIDIOC_STREAMON, &type)) return OAPC_ERROR_RESOURCE;
   return OAPC_OK;
}



void stop_capturing(struct instData *data)
{
   enum v4l2_buf_type type;

   type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
   xioctl (data->fd, VIDIOC_STREAMOFF, &type);
}



void uninit_device(struct instData *data)
{
   unsigned int i;

   for (i = 0; i < data->n_buffers; ++i)
	munmap (data->buffers[i].start, data->buffers[i].length);
   free (data->buffers);
}



int read_frame(struct instData *data)
{
   struct v4l2_buffer buf;

   memset(&buf,0,sizeof(buf));
   buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
   buf.memory = V4L2_MEMORY_MMAP;

   if (-1 == xioctl (data->fd, VIDIOC_DQBUF, &buf))
   {
      switch (errno)
      {
         case EAGAIN:
             return -1;
  		 case EIO:
				/* Could ignore EIO, see spec. */

				/* fall through */
         default:
        	 return -1;
      }
   }
   xioctl (data->fd, VIDIOC_QBUF, &buf);
   return buf.index;
}



static int grab_frame(struct instData *data)
{
   int               bidx;

   while (1)
   {
      fd_set fds;
      struct timeval tv;
      int r;

      FD_ZERO (&fds);
      FD_SET (data->fd, &fds);

      /* Timeout. */
      tv.tv_sec = 2;
      tv.tv_usec = 0;
      r = select (data->fd + 1, &fds, NULL, NULL, &tv);

      if ((r==0) || (r==-1))
      {
         if (EINTR == errno) continue;
         return OAPC_ERROR_NO_DATA_AVAILABLE;
      }

      bidx=read_frame(data);
      if (bidx>-1)
      {
         if (data->pixelformat==V4L2_PIX_FMT_YUYV)         yuv422_to_rgb24(data,(unsigned char*)data->buffers[bidx].start);
         else if (data->pixelformat==V4L2_PIX_FMT_YUV422P) yuv422p_to_rgb24(data,(unsigned char*)data->buffers[bidx].start);
         else
         {
            printf("Error: unsupported pixel format 0x%X\n",data->pixelformat);
        	assert(0);
         }
         return OAPC_OK;
      }
   }
}


int openVideoDevice(struct instData *data,int device_number)
{
   int ret;

   ret=open_device(data);
   if (ret==OAPC_OK) ret=init_device(data);
   if (ret==OAPC_OK) ret=start_capturing(data);
   if (ret!=OAPC_OK) return ret;
   return OAPC_OK;
}


int captureImage(struct instData *data)
{
   if (!data->rawBinBuf)
    data->rawBinBuf=oapc_util_alloc_bin_data(OAPC_BIN_TYPE_IMAGE,OAPC_BIN_SUBTYPE_IMAGE_RGB24,OAPC_COMPRESS_NONE,3*data->width*data->height);
   if (!data->rawBinBuf) return OAPC_ERROR_NO_MEMORY;
   return grab_frame(data);
}


void closeVideoDevice(struct instData *data)
{
   stop_capturing(data);
   uninit_device(data);
   close(data->fd);
}


#endif //ENV_LINUX
