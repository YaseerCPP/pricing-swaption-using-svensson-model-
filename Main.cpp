#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>

// Function to calculate the spot rate using the Svensson model
double calculateSpotRate(double t, double beta0, double beta1, double beta2, double beta3, double tau1, double tau2) {
    double term1 = beta0;
    double term2 = beta1 * (1 - exp(-t / tau1)) / (t / tau1);
    double term3 = beta2 * ((1 - exp(-t / tau1)) / (t / tau1) - exp(-t / tau1));
    double term4 = beta3 * ((1 - exp(-t / tau2)) / (t / tau2) - exp(-t / tau2));
    return term1 + term2 + term3 + term4;
}

// Function to calculate the discount factor
double calculateDiscountFactor(double spotRate, double t) {
    return exp(-spotRate * t);
}

// Function to calculate the present value of the fixed leg of a swap
double calculateFixedLegPV(double notional, double fixedRate, int swapMaturity, double beta0, double beta1, double beta2, double beta3, double tau1, double tau2) {
    double fixedLegPV = 0.0;

    for (int t = 1; t <= swapMaturity; ++t) {
        double spotRate = calculateSpotRate(t, beta0, beta1, beta2, beta3, tau1, tau2);
        double discountFactor = calculateDiscountFactor(spotRate, t);
        fixedLegPV += notional * fixedRate * discountFactor;
    }

    return fixedLegPV;
}

// Function to calculate the present value of the floating leg of a swap
double calculateFloatingLegPV(double notional, int swapMaturity, double beta0, double beta1, double beta2, double beta3, double tau1, double tau2) {
    double floatingLegPV = 0.0;

    for (int t = 1; t <= swapMaturity; ++t) {
        double spotRate = calculateSpotRate(t, beta0, beta1, beta2, beta3, tau1, tau2);
        double discountFactor = calculateDiscountFactor(spotRate, t);
        floatingLegPV += notional * discountFactor;
    }

    return floatingLegPV;
}

// Function to calculate the forward swap rate
double calculateForwardSwapRate(double notional, int swapMaturity, double beta0, double beta1, double beta2, double beta3, double tau1, double tau2) {
    double fixedLegPV = calculateFixedLegPV(notional, 1.0, swapMaturity, beta0, beta1, beta2, beta3, tau1, tau2);
    double floatingLegPV = calculateFloatingLegPV(notional, swapMaturity, beta0, beta1, beta2, beta3, tau1, tau2);
    return floatingLegPV / fixedLegPV;
}

// Function to calculate the Black's model swaption price
double calculateSwaptionPrice(double notional, double strikeRate, double forwardSwapRate, double maturity, double volatility, bool isPayerSwaption) {
    double d1 = (log(forwardSwapRate / strikeRate) + 0.5 * volatility * volatility * maturity) / (volatility * sqrt(maturity));
    double d2 = d1 - volatility * sqrt(maturity);

    double nd1 = 0.5 * (1.0 + erf(d1 / sqrt(2.0)));
    double nd2 = 0.5 * (1.0 + erf(d2 / sqrt(2.0)));

    if (isPayerSwaption) {
        return notional * (forwardSwapRate * nd1 - strikeRate * nd2);
    } else {
        return notional * (strikeRate * (1.0 - nd2) - forwardSwapRate * (1.0 - nd1));
    }
}

int main() {
    // Swaption details
    double notional = 1000000.0; // Notional amount
    double strikeRate = 0.05; // Strike rate of the swaption
    double maturity = 2.0; // Maturity of the swaption in years
    double volatility = 0.2; // Volatility of the forward swap rate
    bool isPayerSwaption = true; // True for payer swaption, false for receiver swaption

    // Swap details
    int swapMaturity = 5; // Maturity of the swap in years

    // Svensson model parameters
    double beta0 = 0.02;
    double beta1 = -0.01;
    double beta2 = 0.03;
    double beta3 = -0.02;
    double tau1 = 2.0;
    double tau2 = 10.0;

    // Calculate the forward swap rate
    double forwardSwapRate = calculateForwardSwapRate(notional, swapMaturity, beta0, beta1, beta2, beta3, tau1, tau2);

    // Calculate the swaption price using Black's model
    double swaptionPrice = calculateSwaptionPrice(notional, strikeRate, forwardSwapRate, maturity, volatility, isPayerSwaption);

    std::cout << "The price of the swaption is: $" << swaptionPrice << std::endl;

    return 0;
}
