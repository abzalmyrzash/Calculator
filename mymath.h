unsigned long long factorial (unsigned int n) {
	if (n == 1 || n == 0) return 1;
	return n * factorial(n - 1);
}

double factorial_d(double n) {
	if (n == 1 || n == 0) return 1;
	return n * factorial(n - 1);
}
