__kernel void grayscale_lum(__global uchar4* inp_pix, __global uchar4* res_pix) {
      int num = get_global_id(0);
      if(num >= get_global_size(0))
          return;
      
      uchar avg = (uchar)((0.2126f * ((float)res_pix[num].x / 255.0f) +
                           0.7152f * ((float)res_pix[num].y / 255.0f) +
                           0.0722f * ((float)res_pix[num].z / 255.0f)) *
                           255);
          
	  res_pix[num].x = avg;
      res_pix[num].y = avg;
      res_pix[num].z = avg;
} 
