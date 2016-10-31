
#include <iostream>
#include <sstream>
#include <chrono>
#include <cstdlib>
#include <cmath>
#include <string>
#include <thread>
#include <mutex>
#include <atomic>

#include "threadSafeQueue.hpp"
#include "hashMap.hpp"

#define mul_mod(a,b,m) (( (long long) (a) * (long long) (b) ) % (m))

const int PISIZE = 1001;

/* return the inverse of x mod y */
int inv_mod(int x, int y)
{
	int q, u, v, a, c, t;

	u = x;
	v = y;
	c = 1;
	a = 0;
	do {
		q = v / u;

		t = c;
		c = a - q * c;
		a = t;

		t = u;
		u = v - q * u;
		v = t;
	} while (u != 0);
	a = a % y;
	if (a < 0)
		a = y + a;
	return a;
}

/* return (a^b) mod m */
int pow_mod(int a, int b, int m)
{
	int r, aa;

	r = 1;
	aa = a;
	while (1) {
		if (b & 1)
			r = mul_mod(r, aa, m);
		b = b >> 1;
		if (b == 0)
			break;
		aa = mul_mod(aa, aa, m);
	}
	return r;
}

/* return true if n is prime */
int is_prime(int n)
{
	int r, i;
	if ((n % 2) == 0)
		return 0;

	r = (int)(sqrt(n));
	for (i = 3; i <= r; i += 2)
		if ((n % i) == 0)
			return 0;
	return 1;
}

/* return the prime number immediatly after n */
int next_prime(int n)
{
	do {
		n++;
	} while (!is_prime(n));
	return n;
}

unsigned int computePiDigit(int n)
{
	std::cout << ".";
	std::cout.flush();
	int av, a, vmax, N, num, den, k, kq, kq2, t, v, s, i;
	double sum = 0;

	N = (int)((n + 20) * std::log(10) / std::log(2));

	for (a = 3; a <= (2 * N); a = next_prime(a)) {

		vmax = (int)(std::log(2 * N) / std::log(a));
		av = 1;
		for (i = 0; i < vmax; i++)
			av = av * a;

		s = 0;
		num = 1;
		den = 1;
		v = 0;
		kq = 1;
		kq2 = 1;

		for (k = 1; k <= N; k++) {

			t = k;
			if (kq >= a) {
				do {
					t = t / a;
					v--;
				} while ((t % a) == 0);
				kq = 0;
			}
			kq++;
			num = mul_mod(num, t, av);

			t = (2 * k - 1);
			if (kq2 >= a) {
				if (kq2 == a) {
					do {
						t = t / a;
						v++;
					} while ((t % a) == 0);
				}
				kq2 -= a;
			}
			den = mul_mod(den, t, av);
			kq2 += 2;

			if (v > 0) {
				t = inv_mod(den, av);
				t = mul_mod(t, num, av);
				t = mul_mod(t, k, av);
				for (i = v; i < vmax; i++)
					t = mul_mod(t, a, av);
				s += t;
				if (s >= av)
					s -= av;
			}

		}

		t = pow_mod(10, n - 1, av);
		s = mul_mod(s, t, av);
		sum = std::fmod(sum + (double)s / (double)av, 1.0);
	}

	return static_cast<unsigned int>(sum * 1e9 / 100000000);
}

double powneg(unsigned long long b, long long pow)
{
	double r = std::pow(b, -pow);
	return 1.0 / r;
}

unsigned long long s(unsigned long long j, unsigned long long n)
{
	const unsigned long long D = 14;
	const unsigned long long M = static_cast<unsigned long long>(std::pow(16, D));
	const unsigned long long SHIFT = 4 * D;
	const unsigned long long MASK = M - 1;

	unsigned long long s = 0;
	unsigned long long k = 0;
	while (k <= n)
	{
		unsigned long long r = 8 * k + j;
		unsigned long long v = static_cast<unsigned long long>(std::pow(16ul, n - k)) % r;
		s = (s + (v << SHIFT) / r) & MASK;
		k += 1;
	}
	unsigned long long t = 0;
	k = n + 1;
	while (true)
	{
		unsigned long long xp = static_cast<unsigned long long>(powneg(16, n - k) * M);
		unsigned long long newt = t + xp / (8 * k + j);
		if (t == newt)
			break;
		else
			t = newt;
		k += 1;
	}

	return s + t;
}

unsigned long long piDigitHex(unsigned long long n)
{
	const unsigned long long D = 14;
	const unsigned long long M = static_cast<unsigned long long>(std::pow(16, D));
	const unsigned long long SHIFT = 4 * D;
	const unsigned long long MASK = M - 1;

	n -= 1;
	unsigned long long x = (4 * s(1, n) - 2 * s(4, n) - s(5, n) - s(6, n)) & MASK;

	return x;
}

int main() {
	const unsigned int THREADCOUNT = std::thread::hardware_concurrency();
  ThreadSafeQueue<int> tsq;
  HashMap hm;
	for (int digit = 1; digit <= PISIZE; digit++) {
    tsq.enqueue(digit);
		// std::cout << computePiDigit(digit);
	}

	typedef std::chrono::high_resolution_clock Time;
	auto t0 = Time::now();
  std::atomic<uint16_t> resource(0);



  auto threadFunction = [&resource, &tsq, &hm](uint16_t which) {
    while (tsq.getSize() > 1) {
        int value = tsq.dequeue();
        hm.put(value, computePiDigit(value));
      }
  };

	std::thread* myThreads[THREADCOUNT];

	for (int i = 0; i < THREADCOUNT; i++) {
		myThreads[i] = new std::thread(threadFunction, i);
	}

	for (int i = 0; i < THREADCOUNT; i++) {
		myThreads[i]->join();
	}


	typedef std::chrono::milliseconds milli;
	typedef std::chrono::duration<float> duration;
	auto t1 = Time::now();
	duration difference = t1 - t0;
	std::cout << std::endl;
	std::cout << "Took " << difference.count() << " milliseconds long" << std::endl;
  std::cout << std::endl;
  std::cout << "3.";
  for (int digit = 1; digit <= PISIZE; digit++) {
    int value = hm.get(digit);
    if (value != -1) {
      std::cout << value;
    }
  }

  std::cout << std::endl;
	return 0;
}
