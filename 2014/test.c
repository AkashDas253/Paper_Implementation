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
double enhanced[NX][NY],corrupted[NX][NY], real[NX][NY];

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
    double SSIM = ( (2 * mean_x * mean_y + C1) * (2 * covariance_xy + C2) ) / ( (mean_x * mean_x + mean_y * mean_y + C1) * ( variance_x +  variance_y + C2) );

    printf("Sum of Square=%lf\n",sqsum);
    printf("RMSE=%lf\n",RMSE);
    printf("PSNR=%lf\n",20.0*log10(p/RMSE));
    printf("IEF=%lf\n",(IEF_neu/IEF_deno));
    printf("SSIM=%lf\n",SSIM);
}

void copy_matrix(double from[NNX][NNY],double to[NNX][NNY])
{
    // Temporaries
    int i,j;
    for(i=0;i<NNX;i++)
        {
            for(j=0;j<NNY;j++)
            {
                to[i][j]=from[i][j];
            }
        }
}

double M_func(int n,int k, double A[n])
{
    // Temporaries
    int i;
    double result=-A[n/2];

    qsort(A,n,sizeof(double),comp);
    
    if(n%2==1)
    {
        for(i=0; i< k; i++)
        {
            
            result+=(A[n/2-i]+A[i+n/2]);
        }
        
        return result/(2*k-1.0);
    }
    else
    {
        for(i=n/2-k; i<n/2+k; i++)
        {
            //printf(" %lf-",A[i]);
            result+=A[i]/(2.0*k);
        }
    }       

    return result;
}

int algorithm(double a[NNX][NNY],double b[NNX][NNY])
{
    // Temporaries
    int i,j,x,y,counter;
    double R[MAXWINDOW*MAXWINDOW],V[MAXWINDOW*MAXWINDOW];

    //Parameters
    int Smax=5,Smin=1;
    int k1=3,k2=3;
    double Tmax=0.999, Tmin=0.989000, alpha=0.001;
    double eta=10.0;
    int d=0;
    
    int Ninit=9;
    double Tinit=Tmax;
    int power=2;
    int M=Smin;

    for(i=0;i<NNX;i++){
        for(j=0;j<NNY;j++){
            
            if( (a[i][j]<1.0*p && a[i][j]>0.0) || (!((MAXWINDOW/2<=i && i<MAXWINDOW+nx-1) && (MAXWINDOW/2<=j && j<MAXWINDOW+ny-1))))
            {   b[i][j]=a[i][j];    continue;}
            
            // Processing Noise pixel Pxy

            // Internal parameters
            double T=Tinit;
            int N=Ninit;
            
            // Removing Noise
            while(1)
            {
                counter=0;
                for(x=-M; x<=M; x++)
                {
                    for(y=-M; y<=M; y++)
                    {
                        R[counter++]=a[i+x][y+j];
                    }
                }

                // Mean
                double mew=M_func((2*M+1)*(2*M+1),k1,R);

                // Check
                for(counter=0; counter<(2*M+1)*(2*M+1); counter++)
                {
                    V[counter]=(R[counter]-mew)*(R[counter]-mew);
                    //printf("%lf ",V[x]);
                }

                // Deviation
                double sigma = M_func((2*M+1)*(2*M+1),k2,V);
                


                if(sigma<=eta){  
                    b[i][j]=mew;  
                    //printf("Taking mew++%d %d %lf\n",i,j,b[i][j]);
                    break;  
                }
                

                // Degree of Membership
                double membership=exp(-0.5*(a[i][j]-mew)*(a[i][j]-mew)/(sigma*sigma));

                //if(membership<0.0000000||membership>1.0)   
                //    printf("Error in degree of membership: %lf %lf %lf %lf\n",a[i][j],membership, mew, sigma);

                //b[i][j]=membership*p;
                //break;

                if(membership > T){   
                    b[i][j]=a[i][j];  
                    //printf("Remaining same(threshold, membership) %lf %d %d %lf %lf %lf\n",a[i][j],i,j,T,membership,b[i][j]);  
                    break;  
                }

                // Good Set of Pixels are considered
                //Magnitude of G
                int G_mod=0;
                
                for(x=-M; x<=M; x++)
                {
                    for(y=-M; y<=M; y++)
                    {
                        double membership_k=exp(-0.5*(double)(a[x+i][j+y]-mew)*(a[x+i][j+y]-mew)/(sigma * sigma));
                        if((x!=0 || y!=0)&&((a[i][j]!=0 && a[i][j]!=p)|| membership_k>T))
                        {
                            G_mod+=1;
                        }
                    }
                }

                

                if( 1 && G_mod<N && T>Tmin)
                {
                    T-=alpha;
                    //printf("xcf trftdcr\n");
                    continue;
                }

                if( 1 && G_mod<N && M<Smax)
                {
                    M+=1;
                    continue;
                }


                if( 1 && G_mod < N && M>Smax && T<Tmin)
                {
                    N-=1;
                    if(N<=1)
                    {
                        Smax+=1;
                        N=1;
                    }
                }

                // A noisy pixel unresolved
                d+=1;

                // Total weight
                double W=0.0;
                // New pixel value
                double Pnew=0.0;

                for(x=-M; x<=M; x++)
                {
                    for(y=-M; y<=M; y++)
                    {
                        double membership_k=exp(-0.5*(double)(a[x+i][j+y]-mew)*(a[x+i][j+y]-mew)/(sigma * sigma));
                        if((x!=0 || y!=0)&&((a[i][j]!=0 && a[i][j]!=p)|| membership_k>T))
                        {
                            W+=pow((i-x)*(i-x)+(j-y)*(j-y),(double)-power);
                            Pnew+=pow((i-x)*(i-x)+(j-y)*(j-y),-power)*a[i][j];
                            if(membership_k>1.0)
                                printf("%lf-LL\n",membership_k);
                        }
                    }
                }
                Pnew/=W;
                
                b[i][j]=Pnew;
                //printf("\nWeighted average == %lf*%d %d*%lf %lf %d %lf\n",a[i][j],i,j, T,M,G_mod, b[i][j]);
                break;
            }

        }
    }   
    return d;     
}

void main(int argc, char* argv[])
{

    char* REAL="Lena/original_lena.pgm";
    
    char* CORRUPTED="Lena/90.pgm";
    char* ENHANCED="Lena_out/90.pgm";

    loadImage(REAL,real);
    loadImage(CORRUPTED,corrupted);

    createPadding(corrupted,I);

    printf("STARTING");
    int d=999999;
    while( 1.0*d>.1*nx*ny)
    {
        d=algorithm(I,In);
        printf("\n %d PIXELS UNRESOLVED AMONG %d", d, nx*ny);
        copy_matrix(In,I);
    }

    removePadding(In,enhanced);
    
    evaluate();
    
    storeImage(ENHANCED,enhanced);
}