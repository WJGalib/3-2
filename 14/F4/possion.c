#include<stdio.h>
#include<gsl/gsl_randist.h>
#include<time.h>

int main() {
    gsl_rng_default_seed = time(0);
    gsl_rng* r = gsl_rng_alloc (gsl_rng_taus);
    int v = gsl_ran_poisson (r, 1.75);
    printf("%d\n", v);
    v = gsl_ran_poisson (r, 1.75);
    printf("%d\n", v);
    v = gsl_ran_poisson (r, 1.75);
    printf("%d\n", v);
    v = gsl_ran_poisson (r, 1.75);
    printf("%d\n", v);
    v = gsl_ran_poisson (r, 1.75);
    printf("%d\n", v);
    v = gsl_ran_poisson (r, 1.75);
    printf("%d\n", v);
    v = gsl_ran_poisson (r, 1.75);
    printf("%d\n", v);
    v = gsl_ran_poisson (r, 1.75);
    printf("%d\n", v);
    v = gsl_ran_poisson (r, 1.75);
    printf("%d\n", v);
    v = gsl_ran_poisson (r, 1.75);
    printf("%d\n", v);
    v = gsl_ran_poisson (r, 1.75);
    printf("%d\n", v);
    v = gsl_ran_poisson (r, 1.75);
    printf("%d\n", v);
    v = gsl_ran_poisson (r, 1.75);
    printf("%d\n", v);
    v = gsl_ran_poisson (r, 1.75);
    printf("%d\n", v);
    return 0;
}

