#include <iostream>
#include <iomanip>
#include <string>


std::string PI = "-pi";
std::string FIB = "-fib";
std::string E = "-e";

double calculatePower (double n, double toThe) {
  double result = n;
  for(int i = 0; i < toThe - 1; i++) {
    result = result * n;
  }
  return result;
}


int factorial(int n) {
  return (n == 1 ? n : n * factorial(n - 1));
}

int computeFibonacciOfN(int n) {
   if (n <= 1) return n;
   return computeFibonacciOfN(n-1) + computeFibonacciOfN(n-2);
}

long double calculateEwithNIterations(int n) {
    long double e = 1;
    for (int k = 1; k < n; k++) e = e + (1.0 / factorial(k));
    return e;
}

int calculatePiSign (int k) {
  if (k % 2 == 0) return -1;
  else return 1;
}

long double calcPiToNthPlaces (int n) {
  std::cout << std::fixed;
  std::cout << std::setprecision(n);
  long double pi = 0;

  for (double k = 1.0; k < 10000000; k++) {
    long double toadd = ((-1) * calculatePiSign(k + 1)) / ((2 * k) - 1);
    pi = pi + toadd;
  }
  return 4 * pi;
}

int main (int argc, char* argv[]) {
  std::string str(argv[1]);

  if (argc == 1) {
    std::cout << "Need to add arguments" << std::endl;
    std::cout << "--- Assign 1 Help ---" << std::endl;
    std::cout << "-fib [n] : Compute the fibonacci of [n]" << std::endl;
    std::cout << "-e [n] : Compute the value of 'e' using [n] iterations" << std::endl;
    std::cout << "-pi [n] : Compute Pi to [n] digits" << std::endl;
    return 0;
  } else if (FIB.compare(argv[1]) == 0 && (std::stoi(argv[2]) < 1 || std::stoi(argv[2]) > 40) ) {
    std::cout << "-fib needs an argument between 0 - 40" << std::endl;
    return 0;
  } else if (E.compare(argv[1]) == 0 && (std::stoi(argv[2]) < 0 || std::stoi(argv[2]) > 30) ) {
    std::cout << "-e needs an argument between 0 - 30" << std::endl;
    return 0;
  } else if (PI.compare(argv[1]) == 0 && (std::stoi(argv[2]) < 0 || std::stoi(argv[2]) > 10) ) {
    std::cout << "-pi needs an argument between 0 - 10" << std::endl;
    return 0;
  }

  if (argc > 1 && FIB.compare(argv[1]) == 0) {
    std::cout << "fib of " << argv[2] << ": " << computeFibonacciOfN(std::stoi(argv[2])) << std::endl;
  } else if (argc > 1 && E.compare(argv[1]) == 0) {
    std::cout << "e using " << argv[2] << " iterations: " << calculateEwithNIterations(std::stoi(argv[2])) << std::endl;
  } else if (argc > 1 && PI.compare(argv[1]) == 0) {
    std::cout << "pi to " << argv[2] << " places " << calcPiToNthPlaces(std::stoi(argv[2])) << std::endl;
  } else {
    std::cout << "--- Assign 1 Help ---" << std::endl;
    std::cout << "-fib [n] : Compute the fibonacci of [n]" << std::endl;
    std::cout << "-e [n] : Compute the value of 'e' using [n] iterations" << std::endl;
    std::cout << "-pi [n] : Compute Pi to [n] digits" << std::endl;
  }
  return 0;
}
