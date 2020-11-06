//
// Created by denn nevera on 06/11/2020.
//

#include "Image.h"

void Image::savePPM(const char *fname) const
{
  char *tmp = new char[3*width*height];
  for (int i=0;i<3*width*height;i++) {
    tmp[i] = int(std::fmax(0,std::fmin(255,int(pix[i]*256.f))));
  }

  FILE *fp = fopen(fname, "wb");
  if (!fp) {
    perror(fname);
    exit(1);
  }

  fprintf(fp, "P6\n");
  fprintf(fp, "%ld %ld\n", width, height);
  fprintf(fp, "255\n");
  fwrite(tmp, 3*width*height, 1, fp);
  fclose(fp);
}
