#include<stdio.h>
#include<stdlib.h>
#include<math.h>

// Size
#define NX 512
#define NY 512


//Image Properties
int nx,ny,p;

// Images
double enhanced[NX][NY],corrupted[NX][NY], real[NX][NY];


void loadImage(char *source_file, double a[NX][NY] )
{
    //Temporaries
    int i,j,l;

    //Copying image
    FILE * source = fopen(source_file, "r");
    if (source == NULL) 
    {
        printf("Error opening source file: %s\n",source_file);
        exit(1);
    }
    fscanf(source,"P2 %d %d %d ",&nx,&ny,&p);
    printf("%d %d\n",nx,ny);
    for(i=0;i<ny;i++)
        for(j=0;j<nx;j++)
        {
            fscanf(source,"%d ",&l);
            a[i][j]=(double)l;
        }
    
    fclose(source);
}

void evaluate()
{
    //Temporaries
    int i,j;

    double x,s,r;
    double IEF_neu=0,IEF_deno=0;
    double sqsum=0.0;

    double L=p;
    double C1 = (0.01 * L) * (0.01 * L);
    double C2 = (0.03 * L) * (0.03 * L);
    double mean_x = 0.0, mean_y = 0.0, variance_x = 0.0, variance_y = 0.0, covariance_xy = 0.0;

    //Pixel-wise Scanning
    for(i=0;i<NY;i++)
        for(j=0;j<NX;j++)
        {
            s=real[i][j];
            r=corrupted[i][j];
            x=enhanced[i][j];

            sqsum+=1.0*((r-x)*(r-x))/(NX*NY);
            IEF_neu+=1.0*(s-r)*(s-r)/(NX*NY);
            IEF_deno+=1.0*(s-x)*(s-x)/(NX*NY);
            //if(abs(enhanced[i][j]-s)>100)
            // printf("PSNR=%d %d %d\n",i,j,enhanced[i][j]);

            mean_x += s / (1.0 * NX * NY);
            mean_y += x / (1.0 * NX * NY);
            variance_x += s*s / (1.0 * NX * NY);
            variance_y += x*x / (1.0 * NX * NY);
            covariance_xy += s*x / (1.0 * NX * NY);

            //printf("%d %d %d\n",s,r,x);
            
        }
    
    double RMSE=sqrt(sqsum);

    // Calculate SSIM
    variance_x-=mean_x*mean_x;
    variance_y-=mean_y*mean_y;
    covariance_xy-=mean_x*mean_y;
    double SSIM = ((2 * mean_x * mean_y + C1) * (2 * covariance_xy + C2)) /((mean_x * mean_x + mean_y * mean_y + C1) * ( variance_x +  variance_y + C2));

    printf("Sum of Square=%lf\n",sqsum);
    printf("RMSE=%lf\n",RMSE);
    printf("PSNR=%lf\n",20.0*log10(p/RMSE));
    printf("IEF=%lf\n",(IEF_neu/IEF_deno));
    printf("SSIM=%lf\n",SSIM);
}

void main(int argc, char* argv[])
{
    char* REAL;
    char* CORRUPTED;
    char* ENHANCED;

    if (argc==1) 
    {
        REAL = "a_real.pgm";
        CORRUPTED = "a.pgm";
        ENHANCED = "a_out.pgm";
    } 
    else if(argc==4)
    {
        REAL = argv[1];
        CORRUPTED = argv[2];
        ENHANCED = argv[3];
    }
    else if(argc==2 && argv[1][0]=='L')
    {
        REAL = "Lena/original_lena.pgm";
        CORRUPTED = "Lena/10.pgm";
        ENHANCED = "Lena_out/10.pgm";
    }

    loadImage(REAL,real);
    loadImage(CORRUPTED,corrupted);

    system("gcc -o use use.c ");

    char command[100];

    snprintf(command, sizeof(command), "./use %s %s", CORRUPTED, ENHANCED);

    loadImage(ENHANCED,enhanced);

    evaluate();
}