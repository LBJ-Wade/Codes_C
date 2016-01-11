#ifndef SPLINEHEADER_INC
#define SPLINEHEADER_INC
#include <stdlib.h>
#include <math.h>
#include <iostream>

#define MIN(x,y) ((x)>(y)? (y) : (x))

#if defined(DOUBLE)
typedef double realT;
#elif defined(LONGDOUBLE)
typedef long double realT;
#elif defined(FLOAT)
typedef float realT;
#else
typedef double realT;
#endif

//=======================================================
// Spline class
//=======================================================

class Spline {
  private:
    std::string name;
    realT *y, *x, *y2;
    realT x_start, x_end;
    int n, type;   
  public:
    
    //====================================================
    // Make a qubic spline and use it...
    //
    // If x-array has nice properties we can use direct lookup:
    //
    // type = 0 : arbritrary (binary search used)
    // type = 1 : x is linear spaced
    // type = 2 : x is logaritmic
    //
    // dydx1, dydxn are dy/dx at the boundary 
    // Use >0.99e30 if not known to get the so-called natural spline
    //=====================================================

    Spline(): n(0), x(NULL), y(NULL), y2(NULL), type(0), x_start(0.0), x_end(0.0), name("") {}

    Spline(realT *xin, realT *yin, int nin, realT dydx1, realT dydxn, int typein, std::string namein){
      create_spline(xin, yin, nin, dydx1, dydxn, typein, namein);
    }
    ~Spline(){
      clean();
    }

    Spline& operator=(const Spline &f){
      n       = f.n;
      type    = f.type;
      x_start = f.x_start;
      x_end   = f.x_end;
      name    = f.name;
      if(n>0){
        x  = new realT[n];
        y  = new realT[n];
        y2 = new realT[n];
      } else {
        x = y = y2 = NULL;
      }

      return *this;
    }

    realT operator()(const realT& x){
      return f(x);
    }

    // Make a spline
    void create_spline(realT *xin, realT *yin, int nin, realT dydx1, realT dydxn, int typein, std::string namein){
      realT sig, p, *u, un;
      int i;

      // Set constants
      name = namein;
      type = typein;
      n    = nin;
      x_start = xin[0];
      x_end = xin[n-1];

      // Allocate memory
      u        = new realT[n];
      this->y  = new realT[n];
      this->y2 = new realT[n];
      this->x  = new realT[n];
      for (i = 0; i<n; i++) {
        y[i]  = yin[i];
        x[i]  = xin[i];
        y2[i] = 0.0;
      }

      // Boundary conditions for the spline at left end
      if (dydx1 > 0.99e30){
        y2[0] = u[0]  = 0.0;
      } else {
        y2[0] = -0.5;
        u[0]  = (3.0/(x[1]-x[0]))*((y[1]-y[0])/(x[1]-x[0])-dydx1);
      }

      // Create spline by solving recurence relation
      for (i=1;i<n-1;i++){
        sig = (x[i]-x[i-1])/(x[i+1]-x[i-1]);
        p = sig*y2[i-1]+2.0;
        y2[i] = (sig-1.0)/p;
        u[i] = (y[i+1]-y[i])/(x[i+1]-x[i]) - (y[i]-y[i-1])/(x[i]-x[i-1]);
        u[i] = (6.0*u[i]/(x[i+1]-x[i-1])-sig*u[i-1])/p;
      }

      // Boundary condition for the spline at right end
      if (dydxn > 0.99e30){
        y2[n-1] = 0.0;
      } else {
        un = (3.0/(x[n-1]-x[n-2]))*(dydxn-(y[n-1]-y[n-2])/(x[n-1]-x[n-2]));
        y2[n-1] = (un-0.5*u[n-2])/(0.5*y2[n-2]+1.0);
      }

      // Calculate y''
      for (i = n-2; i>=0; i--) y2[i] = y2[i]*y2[i+1]+u[i];

      delete[] u;
    }

    // Extract function value from the spline. NB: If x0 is outside range return the closest value.
    realT f(realT x0){
      int klo, khi, k;
      realT h, b, a, result;

      // Type of spline
      if (type == 1) {
        klo = MIN(int((x0 - x_start)/(x_end-x_start)*(n-1)),n-2);
        khi = klo + 1;
      } else if (type == 2) {
        klo = MIN(int((log(x0/x_start))/log(x_end/x_start)*(n-1)),n-2);
        khi = klo + 1;
      } else {
        klo = 0;
        khi = n-1;
        while (khi-klo>1) {
          k = (khi+klo) >> 1;
          if (x[k]>x0) {
            khi = k;
          }else {
            klo = k;
          }
        }
      }
      h = x[khi]-x[klo];
      if (h == 0.0){
        std::cout << "Error h=0 in spline " << name << " at x = " << x0 << std::endl;
        std::cout << "The x-values must be distict. Exiting: Index = " << khi << " " << klo << std::endl;
        std::cout << "Spline has n = " << n << "xstart-end = " << x_start << " " << x_end << std::endl;
        exit(1);
      }
      a = (x[khi]-x0)/h;
      b = (x0-x[klo])/h;
      result = (a*y[klo]+b*y[khi]+((a*a*a-a)*y2[klo]+(b*b*b-b)*y2[khi])*(h*h)/6.0);

      return result;
    }
    
    // Extract the derivative of the splined function
    realT dfdx(realT x0){
      int klo, khi, k;
      realT h, b, a, result;

      // Type of spline
      if (type == 1) {
        klo = MIN(int((x0 - x_start)/(x_end-x_start)*(n-1)),n-2);
        khi = klo + 1;
      } else if (type == 2) {
        klo = MIN(int((log(x0/x_start))/log(x_end/x_start)*(n-1)),n-2);
        khi = klo + 1;
      } else {
        klo = 0;
        khi = n-1;
        while (khi-klo>1) {
          k = (khi+klo) >> 1;
          if (x[k]>x0) {
            khi = k;
          }else {
            klo = k;
          }
        }
      }
      h = x[khi]-x[klo];
      if (h == 0.0){
        std::cout << "Spline error: h=0 - the x-values must be distict. Exiting!" << std::endl;
        exit(1);
      }
      a = (x[khi]-x0)/h;
      b = (x0-x[klo])/h;
      result = (y[khi]-y[klo])/h + h/6.0*(-(3*a*a-1)*y2[klo] + (3*b*b-1)*y2[khi]);

      return result;
    }

    // Delete all arrays in the spline
    void clean(){
      if(x  != NULL) delete[] x;
      if(y  != NULL) delete[] y;
      if(y2 != NULL) delete[] y2;
      x = y = y2 = NULL;
    }
};

#endif
