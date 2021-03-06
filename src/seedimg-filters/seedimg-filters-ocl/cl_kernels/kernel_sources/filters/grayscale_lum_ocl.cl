__kernel void grayscale_lum(__global uchar4* inp_pix, __global uchar4* res_pix, ulong amt_pixels) {
      int num = get_global_id(0);
      if(num >= amt_pixels)
          return;
      
      uchar lin = (0.2126f * (inp_pix[num].x / 255.0f) +
                   0.7152f * (inp_pix[num].y / 255.0f) +
                   0.0722f * (inp_pix[num].z / 255.0f)) *
                   255;
          
	  res_pix[num] = (uchar4)(lin, lin, lin, inp_pix[num].w);
}
