void draw0(Player *player){
		
		Pixel *pixel = malloc(sizeof(Pixel));
		int width = frogImage.width;
		int height = frogImage.height;

		int *frogPtr = (int *) frogImage.image_pixels;	
		int i = 0;

		for(int y = 0;y<height;y++){
			for(int x = 0;x<width;x++){
				if(frogPtr[i]!=0){
					pixel -> color = frogPtr[i];
					pixel -> x = (player -> x) + x;
					pixel -> y = (player -> y) + y;	
				
					drawPixel(pixel);
				}
				i++;
			}
		}
		
		free(pixel);
}

void draw1(Player *player){
		
		Pixel *pixel = malloc(sizeof(Pixel));
		int width = frogImage.width;
		int height = frogImage.height;

		int *frogPtr = (int *) frogImage.image_pixels;	
		int i = 0;

		for(int y = (height - 1);y>=0;y--){
			for(int x = 0;x<width;x++){
				if(frogPtr[i]!=0){
					pixel -> color = frogPtr[i];
					pixel -> x = (player -> x) + x;
					pixel -> y = (player -> y) + y;	
				
					drawPixel(pixel);
				}
				i++;
			}
		}
		
		free(pixel);
}

void draw2(Player *player){
		
		Pixel *pixel = malloc(sizeof(Pixel));
		int width = frogImage.width;
		int height = frogImage.height;

		int *frogPtr = (int *) frogImage.image_pixels;	
		int i = 0;

		for(int x = (width - 1);x>=0;x--){
			for(int y = (height - 1);y>=0;y--){
				if(frogPtr[i]!=0){
					pixel -> color = frogPtr[i];
					pixel -> x = (player -> x) + x;
					pixel -> y = (player -> y) + y;	
				
					drawPixel(pixel);
				}
				i++;
			}
		}
		
		free(pixel);
}

void draw3(Player *player){
		
		Pixel *pixel = malloc(sizeof(Pixel));
		int width = frogImage.width;
		int height = frogImage.height;

		int *frogPtr = (int *) frogImage.image_pixels;	
		int i = 0;

		for(int x = 0;x<width;x++){
			for(int y = 0;y<height;y++){
				if(frogPtr[i]!=0){
					pixel -> color = frogPtr[i];
					pixel -> x = (player -> x) + x;
					pixel -> y = (player -> y) + y;	
				
					drawPixel(pixel);
				}
				i++;
			}
		}
		
		free(pixel);
}