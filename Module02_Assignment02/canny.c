//Implmentation of Canny part 2

#include <stdio.h>                  /*  Marr-Hildreth.c  (or marrh.c) */
#include <math.h>
#define  PICSIZE 256
#define  MAXMASK 100

	int MaxTest(double x,double y,double z);

	int    pic[PICSIZE][PICSIZE];
	double outpicY[PICSIZE][PICSIZE];
	double outpicX[PICSIZE][PICSIZE];
	double peaks[PICSIZE][PICSIZE];
	double maskY[MAXMASK][MAXMASK];
	double maskX[MAXMASK][MAXMASK];
	double mag[PICSIZE][PICSIZE];
	double final[PICSIZE][PICSIZE];
	int histogram[256];
	double ival[256][256];

int main(int argc, char **argv)
{
	
	int i, j, p, q, mr, centx, centy, sum, cutoff, lo, hi, flag;
    double  sig, maskvalX, maskvalY, sum1, sum2, maxival, dir, per;
	
	FILE    *fo1,*fo2,*fo3,*fp1,*fopen();
	char    *foobar;

	//file input
	argc--; argv++;
	foobar = *argv;
	fp1=fopen(foobar,"rb");

	//mag output
	argc--; argv++;
	foobar = *argv;
	fo1=fopen(foobar,"wb");
	
	//peaks output
	argc--; argv++;
	foobar = *argv;
	fo2=fopen(foobar,"wb");
	
	//threshold output
	argc--; argv++;
	foobar = *argv;
	fo3=fopen(foobar,"wb");

	//sigma input
	argc--; argv++;
	foobar = *argv;
	sig = atof(foobar);
	
	//percent input
	argc--; argv++;
	foobar = *argv;
	per = atof(foobar);
	
	//===================================
	
	//Code to ignore input header
	fscanf(fp1,"P5\n");
	fscanf(fp1,"256 256\n");
	fscanf(fp1,"255\n");

	mr = (int)(sig * 3);
	centx = (MAXMASK / 2);
	centy = (MAXMASK / 2);

	//Fix Input
	for (i=0;i<256;i++)
	{ for (j=0;j<256;j++)
			{
			  pic[i][j]  =  getc (fp1);
			  pic[i][j] &= 0377;
			}
	}

	//========CANNY PART 1: Gradient Magnitude==========
	
	for (p=-mr;p<=mr;p++)
	{  for (q=-mr;q<=mr;q++)
	   {
			maskvalX = -p * exp(-((p*p+q*q) / (2*sig*sig)));
			maskvalY = -q * exp(-((p*p+q*q) / (2*sig*sig)));
			maskX[q+centy][p+centx] = maskvalX;
			maskY[q+centy][p+centx] = maskvalY;
	   }
	}

	for (i=mr;i<256-mr;i++)
	{ for (j=mr;j<256-mr;j++)
	  {
		 sum1 = 0;
		 sum2 = 0;
		 for (p=-mr;p<=mr;p++)
		 {
			for (q=-mr;q<=mr;q++)
			{
			   sum1 += pic[i+p][j+q] * maskX[p+centy][q+centx];
			   sum2 += pic[i+p][j+q] * maskY[p+centy][q+centx];
			}
		 }
		 outpicX[i][j] = sum1;
		 outpicY[i][j] = sum2;
	  }
	}

	maxival = 0;
	for (i=mr;i<256-mr;i++)
	{ for (j=mr;j<256-mr;j++)
	  {
		 ival[i][j]=sqrt((double)((outpicX[i][j]*outpicX[i][j]) +
								  (outpicY[i][j]*outpicY[i][j])));
		 if (ival[i][j] > maxival)
			maxival = ival[i][j];

	   }
	}
	
	for (i=0;i<256;i++){ 
		for (j=0;j<256;j++){
			mag[i][j] = (ival[i][j] / maxival) * 255;  
		}
	}
	
	//========CANNY PART 2: Finding Peaks==========
	
	for (i=mr;i<256-mr;i++){ 
		for (j=mr;j<256-mr;j++){
			peaks[i][j] = 0;
			
			if (outpicX[i][j] == 0.0)
				outpicX[i][j] = 0.00001;
			
			dir = outpicY[i][j] / outpicX[i][j];
			
			if ((dir <= 0.4142)&&(dir > -0.4142)){
				if ((mag[i][j] > mag[i][j-1])&& (mag[i][j] > mag[i][j+1])){
					peaks[i][j] = 255;
				}
			}
			
			else if ((dir <= 2.4142)&&(dir > 0.4142)){
				if ((mag[i][j] > mag[i+1][j+1])&&(mag[i][j] > mag[i-1][j-1])){
					peaks[i][j] = 255;
				}
			}
			
			else if ((dir <= -0.4142)&&(dir > -2.4142)){
				if ((mag[i][j] > mag[i-1][j+1])&&(mag[i][j] > mag[i+1][j-1])){
					peaks[i][j] = 255;
				}
			}
			else{
				if ((mag[i][j] > mag[i-1][j])&&(mag[i][j] > mag[i+1][j])){
					peaks[i][j] = 255;
				}
			}
        }
	}
	
	//Outputting magnitude and peaks
	fprintf(fo1,"P5\n256 256\n255\n");
	fprintf(fo2,"P5\n256 256\n255\n");
	for (i=0;i<256;i++){
		for (j=0;j<256;j++){
			fprintf(fo1,"%c",(char)((int)(mag[i][j])));
			fprintf(fo2,"%c",(char)((int)(peaks[i][j])));
		}
	}
	
	//========CANNY PART 4: Automatically Computing Thresholds==========
	
	//convert percent input to decimal
	per /= 100;
	
	for (i=0;i<256;i++){
		for(j=0;j<256;j++){
			histogram[(int)mag[i][j]]++;
		}
	}
	
	cutoff = per * PICSIZE * PICSIZE;
	
	sum = 0;
	hi = 255;
	for (i>256;i>0;i--){
		sum += histogram[i];
		
		if (sum > cutoff){
			hi = i;
			break;
		}
	}
	lo = 0.35 * hi;
	
	//printf("hi: %d\tlow: %d",hi,lo);
	
	//========CANNY PART 3: Double Thresholding==========
	
	for (i=0;i<256;i++){
		for (j=0;j<256;j++){
			if (peaks[i][j] == 255){
				if (ival[i][j] > hi){
					peaks[i][j] = 0;
					final[i][j] = 255;
				}
				else if (ival[i][j] < lo){
					peaks[i][j] = 0;
					final[i][j] = 0;
				}
			}
		}
	}
	flag = 1;
	while (flag){
		flag = 0;
		for (i=0;i<256;i++){
			for (j=0;j<256;j++){
				if (peaks[i][j] == 255){
					for (p=-1;p<=1;p++){
						for (q=-1;q<=1;q++){
							if (final[i+p][j+q] == 255){
								peaks[i][j] = 0;
								final[i][j] = 255;
								flag = 1;
							}
						}
					}
				}
			}
		}
	}
	
	fprintf(fo3,"P5\n256 256\n255\n");
	for (i=0;i<256;i++){
		for (j=0;j<256;j++){
			fprintf(fo3,"%c",(char)((final[i][j])));
		}
	}
	
	//close files and return
	fclose(fp1);
	fclose(fo1);
	fclose(fo2);
	fclose(fo3);
	
	return 0;
}
