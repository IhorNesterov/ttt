#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "stat.h"

typedef struct {
    stat_pulses_t n;
    stat_time_t   t;
} stat_elem_t;

#define STAT_COLUMNS        4U
#define STAT_ROWS           8U
#define STAT_EXTENSION      8U

#define STAT_MATRIX         (STAT_ROWS * STAT_COLUMNS)
#define STAT_ELEMENTS       (STAT_MATRIX + STAT_EXTENSION)

#define STAT_K_NUMBER       8U

#define NELEMENTS(a)    (sizeof(a) / sizeof(a[0]))


static stat_elem_t statistic[STAT_ELEMENTS];
static const stat_elem_t *stat_end = statistic;
static uint8_t stat_elem_n[STAT_ROWS + 1];
static float tau=0;
static float tau_1x=0;
static unsigned char reject_1_second = 0;

typedef struct {
    uint8_t high[STAT_K_NUMBER];
    uint8_t low;
} stat_ksigm_t;

static const stat_ksigm_t k[] = {
    { { 6.0, 5.5, 5.0, 4.5, 4.0, 4.0, 4.0, 4.0 }, 2.0 },
    { { 7.0, 6.0, 5.3, 5.0, 4.5, 4.0, 4.0, 4.0 }, 3.0 },
    { { 8.0, 7.0, 6.0, 5.2, 4.6, 4.0, 4.0, 4.0 }, 3.5 },
    { { 9.0, 8.0, 7.0, 6.0, 5.0, 4.0, 4.0, 4.0 }, 4.0 },
    { { 9.0, 8.0, 7.0, 6.0, 5.0, 4.5, 4.5, 4.5 }, 4.5 },
    { { 10.0, 9.0, 8.0, 7.0, 6.0, 5.0, 5.0, 5.0 }, 5.0 }
};


void stat_SetDeatTime(float t) {
  tau = t;
  if (tau==0) tau_1x=0;
  else tau_1x = 1.0/tau;
}

// Correction DeatTime
float  Get_Tau_CPS(float CPS) {
  float k;
  if (tau_1x!=0) {
      if (CPS>tau_1x) {
            CPS *= tau_1x;
      } else {
            k    = CPS;
            k   *= tau;
            k    = (1.0 - k);
            CPS /= k;
      }
  }
  return CPS;
}

void stat_reset (void) {
    stat_end = statistic;
    memset(stat_elem_n, 0U, sizeof stat_elem_n);
}

void stat_accumulate (stat_pulses_t n, stat_time_t t)
{
    if (stat_end == statistic)
    {
        statistic[0].n = n;
        statistic[0].t = t;
        ++stat_end;
    }
    else
    {
        statistic[0].n += n;
        statistic[0].t += t;
    }
}

void stat_add (stat_pulses_t n, stat_time_t t)
{
    stat_elem_t first_in_row = {n, t},
                last_in_row = {0U, 0U};
    stat_elem_t *cur_row = statistic;
    unsigned int cur_row_n = 0U;

    do {

        if (0U < cur_row_n
            && STAT_COLUMNS > stat_elem_n[cur_row_n]
            && 0U < stat_elem_n[cur_row_n])
        {
            cur_row[0U].n += first_in_row.n;
            cur_row[0U].t += first_in_row.t;
            stat_elem_n[cur_row_n] += 1U;
            break;
        }
        last_in_row = cur_row[STAT_COLUMNS - 1U];

        memmove(cur_row + 1U, cur_row, sizeof(*cur_row) * (STAT_COLUMNS - 1U));

        cur_row[0U] = first_in_row;
        stat_elem_n[cur_row_n] = 1U;

        first_in_row = last_in_row;

        if ((cur_row + STAT_COLUMNS) > stat_end)
        {
            stat_end += 1U;
            break;
        }

        cur_row_n += 1U;
        cur_row += STAT_COLUMNS;
    }
    while (STAT_ROWS > cur_row_n);
    if (cur_row <= stat_end && STAT_ROWS == cur_row_n) {
        if (STAT_COLUMNS > stat_elem_n[cur_row_n]
            && 0U < stat_elem_n[cur_row_n])
        {
            cur_row[0U].n += first_in_row.n;
            cur_row[0U].t += first_in_row.t;
            stat_elem_n[cur_row_n] += 1U;
        }
        else
        {
            memmove(cur_row + 1U, cur_row, sizeof(*cur_row) * (STAT_EXTENSION - 1U));
            cur_row[0U] = first_in_row;
            stat_elem_n[cur_row_n] = 1U;
            if (&statistic[STAT_ELEMENTS] != stat_end) {
                stat_end += 1U;
            }
        }
    }
}

static void sum (stat_elem_t *result, const stat_elem_t *begin, const stat_elem_t *end)
{
    const stat_elem_t *current = begin;
    result->n = 0U;
    result->t = 0U;
    while (current != end)
    {
        result->n += current->n;
        result->t += current->t;
        current += 1U;
    }
}






float fsqrt(float z) {
 if (z==0) return 0;
 union {float f; uint32_t i; } val = {z};
 val.i -= 8388608;
 val.i >>= 1;
 val.i += 536870912;
 return val.f;
}

inline static float border_high_tpc (const stat_elem_t *val, unsigned int k_num) {

    const unsigned int k_num_high = (k_num >> 4) & 0x0F;
    const float k_high = k[k_num_high].high[(val->n >= STAT_K_NUMBER ? STAT_K_NUMBER - 1U : val->n)];

    float n_plus_k_mul_sqrt_n;

    if (0 == val->n) {
        n_plus_k_mul_sqrt_n = 1 + k_high;
    } else {
        n_plus_k_mul_sqrt_n = val->n + k_high * fsqrt(val->n);
    }
    return val->t / n_plus_k_mul_sqrt_n;
}


static float border_low_tpc (const stat_elem_t *val, unsigned int k_num) {
	float ret;
	if (0 == val->n) {
		ret = UINT64_MAX;
	} else {
		const float k_mul_sqrt_n = k[k_num].low * fsqrt(val->n);
		if (k_mul_sqrt_n >= val->n) {
			ret = UINT64_MAX;
		} else {
			ret = val->t / (val->n - k_mul_sqrt_n);
		}
	}
	return ret;
}


static unsigned int need_reject (const stat_elem_t *head, const stat_elem_t *tail, unsigned int k_num) {
    if (border_high_tpc(head, k_num) > border_low_tpc(tail, k_num) || border_high_tpc(tail, k_num) > border_low_tpc(head, k_num)) {
        return 1;
    }  else {
        return 0;
    }
}


void stat_check (unsigned int k_num) {

    if (3 > (stat_end - statistic)) { return;}

    stat_elem_t tail;

    sum(&tail, statistic + 2, stat_end);


    if (need_reject(statistic, &tail, k_num))
    {
        if (reject_1_second)
        {
            reject_1_second = 0;
            stat_end = statistic + 2U;
            memset(stat_elem_n, 0U, sizeof stat_elem_n);
            return;
        }
        else
        {
            reject_1_second = 1;
        }
    }
    else
    {
        if (reject_1_second) {
            statistic[1].n = 0;
            statistic[1].t = 0;
        }
        reject_1_second = 0;
    }

    tail.n += statistic[1].n;;
    tail.t += statistic[1].t;

    const stat_elem_t *div_dot = statistic + STAT_COLUMNS;
    stat_elem_t head = {0U, 0U};
    unsigned int row = 1U;
    stat_elem_t tmp;

    while (1 < (stat_end - div_dot) && row <= STAT_ROWS)
    {

        sum(&tmp, div_dot - (STAT_COLUMNS - 1U), div_dot);

        tail.n -= tmp.n + div_dot->n;
        tail.t -= tmp.t + div_dot->t;

        head.n += tmp.n + (div_dot - STAT_COLUMNS)->n;
        head.t += tmp.t + (div_dot - STAT_COLUMNS)->t;


        if (need_reject(&head, &tail, k_num)) {
            stat_end = div_dot;

            memset(stat_elem_n + row, 0U, sizeof stat_elem_n - row);
            break;
        }

        div_dot += STAT_COLUMNS;
        row += 1U;
    }

    

}


void stat_calc_cps (float *cps, uint8_t* delta) {

    stat_elem_t all;
    static double Time,CPS;
    static uint8_t Delta;


    if (statistic[0].n < 60) {
        sum(&all, statistic, stat_end);
    } else {
        const stat_elem_t *last = statistic + STAT_COLUMNS * 4;
        sum(&all, statistic, (stat_end > last ? last : stat_end));
    }


    Time = (double)all.t;
    Time*= kTime;

    CPS = 0; Delta=99;


    if (Time>0) {
        CPS   = (double)all.n;
        CPS  /= Time;
        Delta = (all.n>4) ? (200/fsqrt(all.n)) : 99;
    }

    // OUT
     CPS=Get_Tau_CPS(CPS);
    *cps  = (float)CPS;
    *delta= Delta;
}

