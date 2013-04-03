#include <math.h>

double Correlate(double apples[], double oranges[], int arraySize)
{
    double valueForN = 0.0;
    double N = arraySize;
    double AVal;
    double BVal;

    double Asum = 0;
    double Bsum = 0;
    double ASquared = 0;  //this is Aij^2
    double BSquared = 0;  //this is Bij^2
    double AB = 0;

    int k;
    for (k = 0; k < arraySize; k++)
    {
        AVal = apples [k];
        BVal = oranges[k];
        Asum += AVal;                   Bsum += BVal;
        ASquared += (AVal*AVal);        BSquared += (BVal*BVal);
        AB += (AVal * BVal);
    }
    if( N <= 0)
        return valueForN;//no data to report on

    double Abar = 0;
    double Bbar = 0;
    Abar = Asum / N;
    Bbar = Bsum / N;

    double numerator = AB   - Bbar   * Asum   - Abar * Bsum     + Abar   * Bbar   * N;
    double denom_1 = sqrt(ASquared   - ((Asum   * Asum)  /N));
    double denom_2 = sqrt(BSquared   - ((Bsum   * Bsum)  /N));

    double answer = numerator / (denom_1 * denom_2);

    return answer;
}
