#ifndef IMAGE_H
#define IMAGE_H

// Definition of a structure used to generalize the images structures
static const struct imageStruct{
  unsigned int		width;
  unsigned int		height;
  unsigned char		image_pixels[];
};

#endif
