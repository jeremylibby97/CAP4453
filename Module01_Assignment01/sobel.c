#include <stdio.h>                          /* Sobel.c */
#include <math.h>

int pic[256][256];
int outpicx[256][256];
int outpicy[256][256];
int maskx[3][3] = {{-1,0,1},{-2,0,2},{-1,0,1}};
int masky[3][3] = {{1,2,1},{0,0,0},{-1,-2,-1}};
double ival[256][256],maxival;

int main(argc,argv)
int argc;
char **argv;
{
	int i,j,p,q,mr,sum1,sum2;
	double loThreshold, hiThreshold;
	FILE *fo1, *hi, *lo, *fp1, *fopen();
	char *foobar;

	//Read in input image
	argc--; argv++;
	foobar = *argv;
	fp1=fopen(foobar,"rb");

	//Magnitude Output
	argc--; argv++;
	foobar = *argv;
	fo1=fopen(foobar,"wb");
	
	//Lo Threshold Output
	argc--; argv++;
	foobar = *argv;
	lo=fopen(foobar,"wb");
	
	//Hi Threadshold Output
	argc--; argv++;
	foobar = *argv;
	hi=fopen(foobar,"wb");

	//Lo Threshold Input
	argc--; argv++;
	foobar = *argv;
	loThreshold = atof(foobar);
	
	//Hi Threadshold Input
	argc--; argv++;
	foobar = *argv;
	hiThreshold = atof(foobar);

	//=======================================
	
	//Code to ignore input header
	fscanf(fp1,"P5\n");
	fscanf(fp1,"256 256\n");
	fscanf(fp1,"255\n");
	
	for (i=0;i<256;i++)
	{ for (j=0;j<256;j++)
		{
		  pic[i][j]  =  getc (fp1);
		  pic[i][j]  &= 0377;
		}
	}

	mr = 1;
	for (i=mr;i<256-mr;i++)
	{ for (j=mr;j<256-mr;j++)
	  {
		 sum1 = 0;
		 sum2 = 0;
		 for (p=-mr;p<=mr;p++)
		 {
			for (q=-mr;q<=mr;q++)
			{
			   sum1 += pic[i+p][j+q] * maskx[p+mr][q+mr];
			   sum2 += pic[i+p][j+q] * masky[p+mr][q+mr];
			}
		 }
		 outpicx[i][j] = sum1;
		 outpicy[i][j] = sum2;
	  }
	}

	maxival = 0;
	for (i=mr;i<256-mr;i++)
	{ for (j=mr;j<256-mr;j++)
	  {
		 ival[i][j]=sqrt((double)((outpicx[i][j]*outpicx[i][j]) +
								  (outpicy[i][j]*outpicy[i][j])));
		 if (ival[i][j] > maxival)
			maxival = ival[i][j];

	   }
	}

	
	
	fprintf(fo1,"P5\n256 256\n255\n");
	fprintf(lo,"P5\n256 256\n255\n");
	fprintf(hi,"P5\n256 256\n255\n");
	for (i=0;i<256;i++){ 
		for (j=0;j<256;j++){
			//Magnitude output
			ival[i][j] = (ival[i][j] / maxival) * 255;  
			fprintf(fo1,"%c",(char)((int)(ival[i][j])));

			//Low Threshold Output
			if (ival[i][j] > loThreshold)
				fprintf(lo,"%c",(char)(255));
			else
				fprintf(lo,"%c",(char)(0));

			//High Threshold Output
			if (ival[i][j] > hiThreshold)
				fprintf(hi,"%c",(char)(255));
			else
				fprintf(hi,"%c",(char)(0));
		}
	}
	
	fclose(fp1);
	fclose(fo1);
	fclose(lo);
	fclose(hi);
	
	return 0;

}
