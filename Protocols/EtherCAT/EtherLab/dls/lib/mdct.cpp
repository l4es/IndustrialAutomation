/******************************************************************************
 *
 *  $Id$
 *
 *  This file is part of the Data Logging Service (DLS).
 *
 *  DLS is free software: you can redistribute it and/or modify it under the
 *  terms of the GNU General Public License as published by the Free Software
 *  Foundation, either version 3 of the License, or (at your option) any later
 *  version.
 *
 *  DLS is distributed in the hope that it will be useful, but WITHOUT ANY
 *  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 *  FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 *  details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with DLS. If not, see <http://www.gnu.org/licenses/>.
 *
 *****************************************************************************/

#include <stdlib.h>
#include <math.h>
#include <fftw3.h>

//#define DEBUG

#ifdef DEBUG
#include <stdio.h>
#endif

#include "mdct.h"

/*****************************************************************************/

namespace LibDLS {

/*****************************************************************************/

static int global_buffers_initialized = 0;
static double *sin_win_buffer[MDCT_MAX_EXP2 + 1];
static double *w_r[MDCT_MAX_EXP2 + 1];
static double *w_i[MDCT_MAX_EXP2 + 1];
static double pi;

/*****************************************************************************/

/**
   Initialisiert die Puffer für eine Dimension, die durch eine
   Zweierpotenz gegeben ist.

   \param exp2 Zweierexponent der Dimension

   \return 0 bei Erfolg, sonst negativer Fehlercode
*/

int mdct_init(unsigned int exp2)
{
    unsigned int i, dim;

    /* Dimension muss im gültigen Bereich liegen! */
    if (exp2 < MDCT_MIN_EXP2 || exp2 > MDCT_MAX_EXP2) return -1;

    /* Dimension berechnen */
    dim = 1 << exp2;

#ifdef DEBUG
    printf("MDCT: Init dimension %d\n", dim);
#endif

    /* Beim ersten Aufruf die globalen Puffer initialisieren */
    if (!global_buffers_initialized)
    {
#ifdef DEBUG
        printf("MDCT: Initializing global buffers\n");
#endif

        for (i = MDCT_MIN_EXP2; i <= MDCT_MAX_EXP2; i++)
        {
            sin_win_buffer[i] = 0;
            w_r[i] = 0;
            w_i[i] = 0;
        }

        // Pi bestimmen
        pi = 4.0 * atan(1.0);

        global_buffers_initialized = 1;
    }

    if (!sin_win_buffer[exp2])
    {
        /* Speicher für Sinus-Fensterfunktion reservieren */
        if ((sin_win_buffer[exp2]
             = (double *) malloc(sizeof(double) * dim)) == NULL) return -3;

        /* Speicher mit Sinus-Fensterfunktion füllen */
        for (i = 0; i < dim; i++)
        {
            sin_win_buffer[exp2][i] = sin(M_PI * (i + 0.5) / dim);
        }
    }

    if (!w_r[exp2])
    {
        /* Speicher für Realteil des Einheitskreises füllen */
        if (!(w_r[exp2] = (double *) malloc(sizeof(double) * dim / 4)))
            return -4;

        // w = diag(exp(-j*2*pi*(t+1/8)/N));
        for (i = 0; i < dim / 4; i++)
            w_r[exp2][i] = cos(2 * pi * (i + 1.0 / 8) / dim);
    }

    if (!w_i[exp2])
    {
        /* Speicher für Imaginärteil des Einheitskreises füllen */
        if (!(w_i[exp2] = (double *) malloc(sizeof(double) * dim / 4)))
            return -5;

        // w = diag(exp(-j*2*pi*(t+1/8)/N));
        for (i = 0; i < dim / 4; i++)
            w_i[exp2][i] = - sin(2 * pi * (i + 1.0 / 8) / dim);
    }

    return 0;
}

/*****************************************************************************/

/**
   Löscht alle allokierten Speicherbereiche.

   Diese Funktion sollte nur vor beendigung des Prozesses
   aufgerufen werden.
*/

void mdct_cleanup()
{
    unsigned int exp2;

    if (!global_buffers_initialized) return;

#ifdef DEBUG
    printf("MDCT: Cleaning global buffers\n");
#endif

    for (exp2 = MDCT_MIN_EXP2; exp2 < MDCT_MAX_EXP2; exp2++)
    {
        if (sin_win_buffer[exp2]) free(sin_win_buffer[exp2]);
        if (w_r[exp2]) free(w_r[exp2]);
        if (w_i[exp2]) free(w_i[exp2]);
    }

    global_buffers_initialized = 0;
}

/*****************************************************************************/

/**
   Führt eine einzelne MDC-Transformation aus.

   Verfahren nach Marios Athineos, marios@ee.columbia.edu
   http://www.ee.columbia.edu/~marios/
   Copyright (c) 2002 by Columbia University.

   Siehe kompression/fftw-test/mdct4.m

   \param exp2 Zweierexponent der Dimension
   \param x Input-Speicher mit 2^(exp2) double-Werten
   \param y Output-Speicher für 2^(exp2)/2 double-Werte
*/

void mdct(unsigned int exp2, const double *x, double *y)
{
    unsigned int n, n4, m, t;
    double *rot, *c_r, *c_i;
    fftw_complex *in, *out;
    fftw_plan p;

    // Variablen vorbelegen
    n = 1 << exp2;
    n4 = n / 4;
    m = n / 2;

    // Speicher reservieren
    rot = (double *) malloc(sizeof(double) * n);
    c_r = (double *) malloc(sizeof(double) * n4);
    c_i = (double *) malloc(sizeof(double) * n4);
    in = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * n4);
    out = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * n4);

    // t = (0:(N4-1)).';
    // rot(t+1,:) = -x(t+3*N4+1,:);
    // t = (N4:(N-1)).';
    // rot(t+1,:) =  x(t-N4+1,:);

    // Rotation. Sinusfenster hinzugefügt (Marios ohne Fenster).

    for (t = 0; t < n4; t++)
        rot[t] = - sin_win_buffer[exp2][n4 * 3 + t] * x[n4 * 3 + t];
    for (t = n4; t < n; t++)
        rot[t] = sin_win_buffer[exp2][t - n4] * x[t - n4];

    // t = (0:(N4-1)).';
    // c = (rot(2*t+1,:)-rot(N-1-2*t+1,:))-j*(rot(M+2*t+1,:)-rot(M-1-2*t+1,:));

    for (t = 0; t < n4; t++)
    {
        c_r[t] = rot[2 * t] - rot[n - 2 * t - 1];
        c_i[t] = - rot[m + 2 * t] + rot[m - 2 * t - 1];
    }

    // c = 0.5 * w * c

    for (t = 0; t < n4; t++)
    {
        in[t][0] = 0.5 * (w_r[exp2][t] * c_r[t] - w_i[exp2][t] * c_i[t]);
        in[t][1] = 0.5 * (w_r[exp2][t] * c_i[t] + w_i[exp2][t] * c_r[t]);
    }

    // c = fft(c, N4);

    p = fftw_plan_dft_1d(n4, in, out, FFTW_FORWARD, FFTW_PATIENT);
    fftw_execute(p);

    // c = (2 / sqrtN) * w * c

    // Marios berechnet normalisierte FFT (Faktor 1/sqrt(n) vor der Summe).
    // FFTW berechnet unnormalisierte FFT.
    // Zur Kompatiblität mit dem alten Verfahren so lassen!

    for (t = 0; t < n4; t++)
    {
        // Jeweils " / sqrt(n)" entfernt
        c_r[t] = 2.0 * (w_r[exp2][t] * out[t][0] - w_i[exp2][t] * out[t][1]);
        c_i[t] = 2.0 * (w_r[exp2][t] * out[t][1] + w_i[exp2][t] * out[t][0]);
    }

    // t = (0:(N4-1)).';
    // y(2*t+1,:)     =  real(c(t+1,:));
    // y(M-1-2*t+1,:) = -imag(c(t+1,:));

    for (t = 0; t < n4; t++)
    {
        y[2 * t] = c_r[t];
        y[m - 2 * t - 1] = -c_i[t];
    }

    // Speicher wieder freigeben
    free(rot);
    free(c_r);
    free(c_i);
    fftw_destroy_plan(p);
    fftw_free(in);
    fftw_free(out);
}

/*****************************************************************************/

/**
   Führt eine inverse MDC-Transformation aus.

   Verfahren nach Marios Athineos, marios@ee.columbia.edu
   http://www.ee.columbia.edu/~marios/
   Copyright (c) 2002 by Columbia University.

   Siehe kompression/fftw-test/imdct4.m

   \param exp2 Zweierexponent der Dimension
   \param x Input-Speicher mit 2^(exp2)/2 double-Koeffizienten
   \param y Output-Speicher für 2^(exp2) double-Werte
*/

void imdct(unsigned int exp2, const double *x, double *y)
{
    unsigned int n, m, two_n, t;
    double *c_r, *c_i, *rot;
    fftw_plan p;
    fftw_complex *in, *out;

    // Variablen vorbelegen
    n = (1 << exp2) / 2;
    m = n / 2;
    two_n = 2 * n;

    // Speicher reservieren
    c_r = (double *) malloc(sizeof(double) * m);
    c_i = (double *) malloc(sizeof(double) * m);
    rot = (double *) malloc(sizeof(double) * two_n);
    in = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * m);
    out = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * m);

    // t = (0:(M-1)).';
    // c = x(2*t+1,:) + j*x(N-1-2*t+1,:);

    for (t = 0; t < m; t++)
    {
        c_r[t] = x[2 * t];
        c_i[t] = x[n - 2 * t - 1];
    }

    // c = (0.5*w)*c;

    for (t = 0; t < m; t++)
    {
        in[t][0] = 0.5 * (w_r[exp2][t] * c_r[t] - w_i[exp2][t] * c_i[t]);
        in[t][1] = 0.5 * (w_r[exp2][t] * c_i[t] + w_i[exp2][t] * c_r[t]);
    }

    // c = fft(c,M);

    p = fftw_plan_dft_1d(m, in, out, FFTW_FORWARD, FFTW_PATIENT);
    fftw_execute(p);

    // c = (8 / sqrtN) * w * c

    for (t = 0; t < m; t++)
    {
        c_r[t] = 8.0 * (w_r[exp2][t] * out[t][0] - w_i[exp2][t] * out[t][1]);
        c_i[t] = 8.0 * (w_r[exp2][t] * out[t][1] + w_i[exp2][t] * out[t][0]);
    }

    // t = (0:(M-1)).';
    // rot(2*t+1,:)   = real(c(t+1,:));
    // rot(N+2*t+1,:) = imag(c(t+1,:));

    for (t = 0; t < m; t++)
    {
        rot[2 * t] = c_r[t] / two_n; // " / two_n" hinzugefügt. Liefert selbe
                                     // Werte, wie altes Verfahren. fp
        rot[n + 2 * t] = c_i[t] / two_n;
    }

    // t = (1:2:(twoN-1)).';
    // rot(t+1,:) = -rot(twoN-1-t+1,:);

    for (t = 1; t < two_n; t += 2)
    {
        rot[t] = -rot[two_n - t - 1];
    }

    // t = (0:(3*M-1)).';
    // y(t+1,:) =  rot(t+M+1,:);
    // t = (3*M:(twoN-1)).';
    // y(t+1,:) = -rot(t-3*M+1,:);

    for (t = 0; t < 3 * m; t++) y[t] = rot[t + m];
    for (t = m * 3; t < two_n; t++) y[t] = -rot[t - m * 3];

    for (t = 0; t < two_n; t++)
    {
        y[t] *= sin_win_buffer[exp2][t];
    }

    // Speicher wieder freigeben
    free(c_r);
    free(c_i);
    free(rot);
    fftw_destroy_plan(p);
    fftw_free(in);
    fftw_free(out);
}

/*****************************************************************************/

} // namespace

/*****************************************************************************/
