#define M_PI 3.14159265358979323846264338327950288

double sinHook(double x)
{
	return 0;
}

double cosHook(double x)
{
	return 0;
}

double tanHook(double x)
{
	return sinHook(x) / cosHook(x);
}

float sinfHook(float x)
{
	return sinHook(x);
}

float cosfHook(float x)
{
	return cosHook(x + M_PI / 2);
}

float tanfHook(float x)
{
	return tanHook(x);
}

double acosHook(double x)
{
	return 0;
}

float acosfHook(float x)
{
	return 0;
}

float asinfHook(float x)
{
	return 0;
}
float atan2fHook(float x)
{
	return 0;
}
float atanfHook(float x)
{
	return 0;
}

float expfHook(float x)
{
	return 0;
}

double expHook(double x)
{
	return 0;

}

double log10Hook(double x)
{
	return 0;
}

float log10fHook(float x)
{
	return 0;
}

float logfHook(float x)
{
	return 0;
}


double sqrtHook(double x)
{
	return 0;
}

double sqrtfHook(double x)
{
	return 0;
}