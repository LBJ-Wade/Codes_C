#include <iomanip>
#include <vector>
#include "OdeSolver.h"

//=======================================================
// Code to solve coupled systems of ODE
// Hans A. Winther (2015) (hans.a.winther@gmail.com)
//=======================================================

//=======================================================
// The ODE dy/dx = -2xy 
// ===> y(x) = y(0) * Exp(-x^2)
//=======================================================

void ode1(double x, std::vector<double> &y, std::vector<double> &dydx){
  dydx[0] = -2.0*x*y[0];
}

//=======================================================
// The ODE {dy_1/dx = y_2, dy_2/dx = 1}
// ==> y_2(x) = y_2(0) + x
//     y_1(x) = y_1(0) + y_2(0)x + x^2/2
//=======================================================

void ode2(double x, std::vector<double> &y, std::vector<double> &dydx){
  dydx[0] = y[1];
  dydx[1] = 1.0;
}

//=======================================================
// Solve ODE system 1
//=======================================================

void solve_ode1(){
  std::vector<double> x, y, ic;
  double xmin, xmax, yini;
  int n, neq;
  bool verbose = false;

  std::cout << "==================" << std::endl;
  std::cout << "   Solve ODE 1    " << std::endl;
  std::cout << "==================" << std::endl;

  // Number of equations
  neq = 1;

  // Initial conditions for ODE1
  ic = std::vector<double>(neq,0.0);
  xmin = 0.0, xmax = 1.0;
  ic[0] = yini = 1.0;

  // Number of points between xmin and xmax
  // to store the solution in
  n = 20;

  // Set up solver for ODE1
  OdeSolver myode(n, neq, ode1);

  // Set initial conditions
  myode.set_initial_conditions(xmin, xmax, ic);

  // Solve
  myode.solve(verbose);

  // Get pointers to solution
  x = myode.x_array();
  y = myode.y_array(0);

  // Print data
  for(int i=0;i<n;i++){
    std::cout << std::setw(2) << i << " / " << n << "  x: " << std::setw(12) << x[i];
    std::cout << "  y: " << std::setw(12) << y[i] << " delta_y: " <<  std::setw(12) << y[i] - exp(-x[i]*x[i]) << std::endl; 
  }
}

//=======================================================
// Solve ODE system 2
//=======================================================

void solve_ode2(){
  std::vector<double> x, y1, y2, ic;
  double xmin, xmax, y1ini, y2ini;
  int n, neq;
  bool verbose = true;

  std::cout << "==================" << std::endl;
  std::cout << "   Solve ODE 2    " << std::endl;
  std::cout << "==================" << std::endl;

  // Number of equations
  neq = 2;

  // Initial conditions for ODE1
  ic = std::vector<double>(neq,0.0);
  xmin = 0.0, xmax = 1.0;
  ic[0] = y1ini = 1.0;
  ic[1] = y2ini = 1.0;

  // Number of points between xmin and xmax to store the solution in
  n = 10;

  // Set up solver for ODE2
  OdeSolver myode(n, neq, ode2);

  // Set initial conditions
  myode.set_initial_conditions(xmin, xmax, ic);

  // Change precision goal and performance params [epsilon, h_start, hmin]
  myode.set_precision(1e-20,1.0e-12,0.0);

  // Solve
  myode.solve(verbose);

  // Get solution
  x  = myode.x_array();
  y1 = myode.y_array(0);
  y2 = myode.y_array(1);

  // Print data
  for(int i=0;i<n;i++){
    std::cout <<  std::setw(2) << i+1 << " / " << n << "  x: " <<  std::setw(12) <<x[i];
    std::cout << "  y1: " <<  std::setw(12) << y1[i] << " delta_y1: " <<  std::setw(12) << y1[i] - 1.0 - x[i] - x[i]*x[i]/2.0;
    std::cout << "  y2: " <<  std::setw(12) << y2[i] << " delta_y2: " <<  std::setw(12) << y2[i] - 1.0 - x[i] << std::endl; 
  }
}

int main(int argv, char **argc){
  solve_ode1();
  solve_ode2();
}

