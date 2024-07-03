#include<stdio.h>
#include<stdlib.h>
#include<math.h>

// Size
#define NNX 543
#define NNY 543
#define NX 512
#define NY 512
#define MAXWINDOW 9

//Image Properties
int nx,ny,p;

double I[NNX][NNY],In[NNX][NNY]; //0 for temp
double tarrmax[NNX][NNY], tarrmin[NX][NY];

// Images
double enhanced[NX][NY],corrupted[NX][NY];

int comp(const void *a,const void *b)
{ 
    return (*(double*)a-*(double*)b);
}


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

void storeImage(char *target_file, double b[NX][NX])
{
    //Temporaries
    int i,j;

    //Storing Image
    FILE* target = fopen(target_file, "w");
    if (target == NULL) 
    {
        printf("Error opening target file: %s\n",target_file);
        exit(1);
    }
    
    fprintf(target,"P2 %d %d\n%d\n",nx,ny,p);
    for(i=0;i<ny;i++)
    {
        for(j=0;j<nx;j++)
            fprintf(target,"%d ",(int)b[i][j]);
        fprintf(target, "\n");
    }
    
    fclose(target);
}

void createPadding(double b[NX][NY],double a[NNX][NNY])
{
    // Temporaries
    int i,j,l; 
    for(i=0;i<ny;i++)
        for(j=0;j<nx;j++)
            a[MAXWINDOW/2+i][MAXWINDOW/2+j]=b[i][j];

    // Create Buffer
    for (l = MAXWINDOW/2 - 1; l >= 0; l--) 
    {
        int startRow = l;
        int endRow = ny + MAXWINDOW - l - 2;
        int startCol = l;
        int endCol = nx + MAXWINDOW - l - 2;

        // Copy upper row
        for (i = startCol; i <= endCol; i++) 
            a[startRow][i] = a[startRow + 1][i];

        // Copy the bottom row
        for (i = startCol; i <= endCol; i++) 
            a[endRow][i] = a[endRow - 1][i];

        // Copy the left column
        for (i = startRow; i <= endRow; i++) 
            a[i][startCol] = a[i][startCol + 1];

        // Copy the right column
        for (i = startRow; i <= endRow; i++) 
            a[i][endCol] = a[i][endCol - 1];
    }

    // Test Final Result
    for(i=0;i<nx+MAXWINDOW-1;i++)
    {
        for(j=0;j<ny+MAXWINDOW-1;j++)
        {
            if(a[i][j]<0||a[i][j]>p)
                printf("(%d )",a[i][j]);
        }
        //printf("\n");
    }
}

void removePadding(double a[NNX][NNY],double b[NX][NY])
{
    //Temporaries
    int i,j;

    //Extract
    for(i=0;i<ny;i++)
        for(j=0;j<nx;j++)
            b[i][j]=a[MAXWINDOW/2+i][MAXWINDOW/2+j];
}


double alpha_trimmed_mean(int t, double alpha, double X[t])
{
    // Temporaries
    int i;

    double result=0.0;

    int trim=(int)floor(alpha*t); // Trim factor

    for(i=trim; i<(t-trim); i++)
    {
        result+=X[i];
        //printf("%lf ",result);
    }
    return result/(t-2.0*trim);
}



void processImpulseNoiseDetection(double a[NNX][NNY],double b[NNX][NNY])
{
    // Temporaries
    int i,j,x,y;
    

    // R(0) parameters
    int Ld=2; 
    double alpha=0.5; //0.5; // between 0.0 and 0.5
    int t=2*Ld+1;

    // R(1) parameters
    int s=t*t*.49; //trim
    double T=50.0;//50.0; // Threshold
    int N= 9;//9; // No of good pixels
    double Wu=(double)40, Wl=(double)10; // Threshold
    double W[MAXWINDOW*MAXWINDOW];

    for(i=0;i<NNX;i++)
    {
        for(j=0;j<NNY;j++)
        {
            if((MAXWINDOW/2<=i && i<MAXWINDOW/2+ny) && (MAXWINDOW/2<=j&&j<MAXWINDOW/2+nx))
            {
                
                // Window creation of size t*t=(2*Ld+1)*(2*Ld+1)
                int count=0;
                
                for(x=-Ld;x<=Ld;x++)
                    for(y=-Ld;y<=Ld;y++)
                        W[count++]=a[i+x][j+y];


                qsort(W,count,sizeof(double),comp);
                
                // R(0) -  Residue
                double trimmed_mean=alpha_trimmed_mean(t*t,alpha,W);

                double r=abs(a[i][j]-trimmed_mean);


                // R(1)
                int E=0;
                for(x=-Ld;x<=Ld;x++)
                {
                    for(y=-Ld;y<=Ld;y++)
                    {
                        if(x!=0 || y!=0)
                            E+=(abs(a[i+x][j+y]-a[i][j])<T)?1:0; // Gamma
                    }
                }
               

                // R(1)
                int Gamma=(E>=N)?0:1;
                double R1=Gamma*r;
                
                // Fuzzy logic
                double n=0.0;
                if(R1>Wu)
                    n=1.0;
                else if(R1<=Wl)
                    n=0.0;
                else 
                    n=(R1-Wl)/(Wu-Wl);

                // Further refinement
                if( 1 && W[s]<=a[i][j] && a[i][j]<= W[t*t-s-1])
                {
                    n=0.0;
                }

                

                // Final value
                double median= W[(t*t)/2];
                b[i][j]=(n*median+(1.0-n)*a[i][j]);
                
                a[i][j]=b[i][j]; // Immediate usage of update value 

            }
            else
                b[i][j]=a[i][j];
        }
    }
}




void main(int argc, char* argv[])
{
    char* CORRUPTED;
    char* ENHANCED;

    if (argc==4)
    {
        CORRUPTED = argv[1];
        ENHANCED = argv[2];
    }
    else
    {
        CORRUPTED = "a.pgm";
        ENHANCED = "a_out.pgm";

    }

    loadImage(CORRUPTED,corrupted);

    createPadding(corrupted,I);

    processImpulseNoiseDetection(I,tarrmax);  
    removePadding(tarrmax,tarrmin);
    createPadding(tarrmin,tarrmax);  
    processImpulseNoiseDetection(tarrmax,In);
    
    int counter=1;
    while(--counter>=1)
    {
        //1st 2 iter
        processImpulseNoiseDetection(In,tarrmax);  
        removePadding(tarrmax,tarrmin);
        createPadding(tarrmin,tarrmax);  
        processImpulseNoiseDetection(tarrmax,In);
    }

    removePadding(In,enhanced);

    storeImage(ENHANCED,enhanced);
}