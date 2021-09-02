#ifndef STAT_H__
#define STAT_H__

// Settings Time
#define TimeBase_Sec   0.25
#define TimeFreq       80000
#define kTime          (double)(1/(float)TimeFreq)
#define TimeBase       (uint32_t)((double)TimeBase_Sec*TimeFreq)

typedef uint32_t stat_pulses_t;
typedef uint64_t stat_time_t;
void stat_SetDeatTime(float t);
void stat_add (stat_pulses_t n, stat_time_t t);
void stat_accumulate (stat_pulses_t n, stat_time_t t);
void stat_check (unsigned int k_num);
void stat_reset (void);
void stat_calc_cps(float *cps, uint8_t* delta);

#endif /* STAT_H__ */
