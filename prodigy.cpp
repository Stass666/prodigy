#define RAST 20
#define RES 5
#define MAX 1200
#define MAXNRG 3000
#define WID 700
#define HIG 700
#define BOTSIZE 2
#define SPD 0.005
#define PI 3.14159265
#include <ctime>
#include <windows.h>
#include <cmath>
#include <GL/gl.h> 
#include <stdio.h>  


RECT r;

HWND hwnd;
HDC hdc;
HGLRC hrc;
POINT pt;
double* result = (double*) malloc(sizeof(double) * 1);	
void EnableOpenGL (HWND hwnd, HDC *hdc, HGLRC *hrc);
void DisableOpenGL (HWND hwnd, HDC hdc, HGLRC hrc);
double col[MAX+1][3],tmpd,tmpd2,BLIZOST=(SPD*RAST)*(SPD*RAST);

void Mir();
void Run();


float moux,mouy;
int population,bgr=0,tmpi;
bool bQuit = false,q=true,save=false,training=true;
long iter=0;

//Timer t = Timer();


void sp(double x,double y, int c,double size){
	glPointSize(size);	
	glBegin (GL_POINTS);

	glColor3f (col[c][0], col[c][1], col[c][2]);
	glVertex2f (x, y);
	glEnd ();				
}

double rast(double x1,double y1,double x2,double y2){
	return sqrt((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1));
}


struct Base{
	public:
		int energy;
		double x,y,vector,speed;
		bool home;
};

struct Bot{
	public:
		double x,y,vector,speed,energy;
		unsigned int old,target,home;
		bool alive,raid,say;
};

Bot zm[MAX];
Base gg[2];

double vec(double x1,double y1,double x2,double y2){
	double angle = atan((y2-y1)/(x2-x1));
	if(x2<x1)angle+=PI;
	return angle;
}


void ggstep(int n){
		if(gg[n].energy<1){
			gg[n].x=(rand()-16384.0f)/24576.0f;
			gg[n].y=(rand()-16384.0f)/24576.0f;
			gg[n].energy=MAXNRG;			
			gg[n].vector=(double)rand()/16384*PI;			
			gg[n].speed=(float)rand()/32768.0f*(SPD/5)+SPD/10;
		}	
		if(n==0)gg[n].energy-=2;
		
			if(rand()<70 && n>0 )gg[n].vector=vec(gg[0].x,gg[0].y,gg[n].x,gg[n].y)+rand()/32768.0f*PI-PI/2;
			if(rand()<70 && n==0)gg[n].vector=vec(gg[0].x,gg[0].y,0,0)+rand()/32768.0f*PI-PI/2;
	
			gg[n].x+=cos(gg[n].vector)*gg[n].speed;
			gg[n].y+=sin(gg[n].vector)*gg[n].speed;
			if(gg[n].x> 1)gg[n].x-=2;
			if(gg[n].x<-1)gg[n].x+=2;
			if(gg[n].y> 1)gg[n].y-=2;
			if(gg[n].y<-1)gg[n].y+=2;
			sp(gg[n].x,gg[n].y,n,5+35.0f*gg[n].energy/MAXNRG);
			

}

void step(int n){

			population++;
			
			for(int j=0;j<MAX;j++){
				if(((zm[n].x-zm[j].x)*(zm[n].x-zm[j].x) + (zm[n].y-zm[j].y)*(zm[n].y-zm[j].y))<BLIZOST && !zm[n].say && zm[j].say){
					if(zm[n].raid && (zm[n].target>(zm[j].target+RAST))){
						zm[n].target=zm[j].target+RAST;
						zm[n].vector=vec(zm[n].x,zm[n].y,zm[j].x,zm[j].y);
					}
					if(!zm[n].raid && (zm[n].home>(zm[j].home+RAST))){
						zm[n].home=zm[j].home+RAST;
						zm[n].vector=vec(zm[n].x,zm[n].y,zm[j].x,zm[j].y);
					}
						
				}
			}
			
		for(int i=0;i<RES;i++){
			if(rast(gg[i].x,gg[i].y,zm[n].x,zm[n].y)<0.02 && gg[i].energy>0){
				if(zm[n].raid && !gg[i].home){
					gg[i].energy--;
					zm[n].vector+=PI;
					zm[n].raid=0;
				}
				if(!zm[n].raid && gg[i].home){
					zm[n].vector+=PI;
					gg[i].energy++;
					zm[n].raid=1;
				}
				if(gg[i].home && gg[i].energy>3000){
					zm[n].x=gg[i].x;
					zm[n].y=gg[i].y;
				}
				if(!gg[i].home)zm[n].target=0;
				if( gg[i].home)zm[n].home=0;
				
			}
		}
			
					
			while(zm[n].vector<0)zm[n].vector+=PI*2;
			while(zm[n].vector>PI*2)zm[n].vector-=PI*2;	
			
			if(zm[n].energy<0)zm[n].alive=false;
			zm[n].old++;
			zm[n].x+=cos(zm[n].vector)*zm[n].speed;
			zm[n].y+=sin(zm[n].vector)*zm[n].speed;

					
				
			if(zm[n].x> 1)zm[n].x-=2;
			if(zm[n].x<-1)zm[n].x+=2;
			if(zm[n].y> 1)zm[n].y-=2;
			if(zm[n].y<-1)zm[n].y+=2;
			sp(zm[n].x,zm[n].y,zm[n].raid,BOTSIZE);
			
			zm[n].target++;zm[n].home++;
			population++;
			zm[n].vector+=rand()/327680.0f-0.05;

}







	







LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch(message) {

	case WM_CREATE:
        return 0;
    case WM_CLOSE:
        PostQuitMessage (0);
        return 0;

    case WM_DESTROY:
        return 0;

    case WM_KEYDOWN:
        switch (wParam)
        {
		case VK_ESCAPE:PostQuitMessage(0);return 0;
		case 32:q=!q;return 0;
		case 84:training=!training;return 0;
		case 83:save=true;return 0;
        }
        return 0;	
		
		
		
		default:
			return DefWindowProc(hwnd, message, wParam, lParam);
	}
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	WNDCLASSEX wc;
	MSG msg;

	/* zero out the struct and set the stuff we want to modify */
	memset(&wc,0,sizeof(wc));
	wc.cbSize		 = sizeof(WNDCLASSEX);
	wc.lpfnWndProc	 = WndProc; /* This is where we will send messages to */
	wc.lpszClassName = "WindowClass";
	RegisterClassEx(&wc);
	hwnd = CreateWindowEx(WS_EX_CLIENTEDGE,"WindowClass","Caption", WS_VISIBLE|WS_OVERLAPPEDWINDOW,
		20, 20, WID, HIG, NULL,NULL,NULL,NULL);
	EnableOpenGL (hwnd, &hdc, &hrc);





	Mir();


	EnableOpenGL (hwnd, &hdc, &hrc);		
		

	while (!bQuit & population>0)if (PeekMessage (&msg, NULL, 0, 0, PM_REMOVE)){
	
		if (msg.message == WM_QUIT){
			bQuit = true;
		}else{
			TranslateMessage (&msg);
			DispatchMessage (&msg);
        }
	}else{
		moux=((float)pt.x/(WID/2))-1;
		mouy=1-((float)pt.y/(HIG/2));
		glClearColor (1.0f, 1.0f, 1.0f, 0.0f);		
//		glClearColor (0.0f, 0.0f, 0.0f, 0.0f);
		glClear (GL_COLOR_BUFFER_BIT);
//		glEnable( GL_POINT_SMOOTH );
		glEnable( GL_BLEND );
		glPushMatrix ();
		//	glRotatef (theta, 0.2f, 0.3f, 0.3f);
		
//		glBegin (GL_QUADS);
		
			GetCursorPos(&pt);
			ScreenToClient (hwnd, &pt);
			Run();
			
//		glEnd ();
		glPopMatrix ();
		SwapBuffers (hdc);

        //    theta += 0.05f;

		
		
		
		
		

	}
	
	DisableOpenGL (hwnd, hdc, hrc);
	return msg.wParam;
}




	
void Run(){
			
	
iter++;
	population=0;		
	
		
		for(int i=0;i<MAX;i++)zm[i].say=rand()%2;
  		for(int i=0;i<MAX;i++)if(zm[i].alive)step(i);




		

//		if(population==0)Mir();
		for(int i=0;i<RES;i++){
			ggstep(i);
		}
	
		
		
}

void Mir(){
	srand(time(0));
	for(int i=0;i<MAX;i++){for(int j=0;j<3;j++)col[i][j]=rand()/65536.0f;}
	for(int j=0;j<3;j++)col[0][j]=0.0f;col[0][0]=1.0f;
	for(int j=0;j<3;j++)col[1][j]=0.0f;col[1][2]=1.0f;
	
	for(int j=0;j<3;j++)col[MAX][j]=0.9;
	for(int i=0;i<MAX;i++){
		zm[i].x=(double)rand()/16384-1;
		zm[i].y=(double)rand()/16384-1;
		zm[i].vector=(double)rand()/16384*PI;
		zm[i].speed=(float)rand()/32768.0f*(SPD*2)+SPD/2;		
		zm[i].alive=true;
		zm[i].energy=0.5;
		zm[i].raid=1;
		zm[i].target=10000;
		zm[i].home=10000;

	}
//	zm[1].speed+=.002;
	for(int i=0;i<RES;i++){
		gg[i].x=(rand()-16384.0f)/24576.0f;
		gg[i].y=(rand()-16384.0f)/24576.0f;
		gg[i].energy=MAXNRG;
		gg[i].home=0;
		gg[i].vector=rand()/16384.0f*PI;			
		gg[i].speed=(float)rand()/32768.0f*(SPD/5)+SPD/10;	
	}
	gg[0].home=1;

	population=1;
}







void EnableOpenGL (HWND hwnd, HDC *hdc, HGLRC *hrc)
{
    PIXELFORMATDESCRIPTOR pfd;
    int iFormat;

    /* get the device context (DC) */
    *hdc = GetDC (hwnd);

    /* set the pixel format for the DC */
    ZeroMemory (&pfd, sizeof (pfd));
    pfd.nSize = sizeof (pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | 
      PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;
    iFormat = ChoosePixelFormat (*hdc, &pfd);
    SetPixelFormat (*hdc, iFormat, &pfd);

    /* create and enable the render context (RC) */
    *hrc = wglCreateContext( *hdc );
    wglMakeCurrent( *hdc, *hrc );

}


void DisableOpenGL (HWND hwnd, HDC hdc, HGLRC hrc)
{
    wglMakeCurrent (NULL, NULL);
    wglDeleteContext (hrc);
    ReleaseDC (hwnd, hdc);
}


