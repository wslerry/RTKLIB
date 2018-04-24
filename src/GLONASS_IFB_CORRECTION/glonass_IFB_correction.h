/* 
 *   GLONASS INTER-FREQUENCY PHASE BIASES (IFB) CORRECTION MODULE
 */

#ifndef GLONASS_IFB_CORRECTION_H
#define GLONASS_IFB_CORRECTION_H

#include "rtklib.h"

/* -------------------------------------------------------------------------------------------------------------- */
/* model paramters */

#define GLO_IFB_MIN_GLO_DT_SEARCH  -0.3   /* min glo_dt value while searching (cycles/frequency_number) */
#define GLO_IFB_MAX_GLO_DT_SEARCH   0.3   /* max glo_dt value while searching (cycles/frequency_number) */
#define GLO_IFB_SEARCH_STEP         0.02  /* glo_dt increment while searching (cycles/frequency_number) */

#define GLO_IFB_MIN_SATS     8            /* min 'fix' sats for glo_dt update */
#define GLO_IFB_MIN_GLO_SATS 4            /* min glonass 'fix' sats for glo_dt update  */

#define GLO_IFB_MAX_ADJUSTMENT_COUNT   1000  /* counting barrier (epochs) */
#define GLO_IFB_MAX_ADJUSTMENT_WINDOW  20    /* smoothing window (epochs) for glo_dt estimation during adjustment */
#define GLO_IFB_MAX_FIX_OUTAGE         200   /* max fix outage (epochs) allowed for adjustment mode */
#define GLO_IFB_VALIDATION_COUNT       200   /* prevent to switch to search mode 
                                                if adjustment_count is sufficient  */

#define GLO_IFB_FREEZE_COUNT           200   /* freeze glo_IFB state if current count has reached a threshold */
#define GLO_IFB_MAX_GLO_DT_DRIFT       0.05  /* max glo_dt drift allowed during multi-epoch adjustment */

#define GLO_IFB_SIGNAL_TO_RESET        1

/* -------------------------------------------------------------------------------------------------------------- */
/* basic types */

typedef enum {

  GLO_IFB_SEARCH_MODE,
  GLO_IFB_ADJUSTMENT_MODE,
  GLO_IFB_FROZEN_MODE

} glo_IFB_mode_t;

typedef struct {

  glo_IFB_mode_t mode;
  int            adjustment_count;
  int            fix_outage;

  double         glo_dt;            /* current glo_dt value (estimated parameter) */

  double         glo_dt_initial;    /* glo_dt value at the first adjustment step */
  double         delta_glo_dt;      /* glo_dt increment at the last step */

  int            signal_to_reset;   /* external signal to reset state (0: no actions; 1: to reset) */

} glo_IFB_t;

/* -------------------------------------------------------------------------------------------------------------- */
/* API */

extern void *glo_IFB_init();
extern int   glo_IFB_is_valid(const void *glo_IFB);
extern void  glo_IFB_free(void *glo_IFB);
extern void  glo_IFB_copy(const void *glo_IFB_src, void *glo_IFB_dst);

extern int    glo_IFB_is_enough_sats(const rtk_t *rtk);
extern void   glo_IFB_process(void *glo_IFB, rtk_t *rtk);
extern double glo_IFB_get_glo_dt(const void *glo_IFB);
extern double glo_IFB_get_delta_glo_dt(const void *glo_IFB);
extern void   glo_IFB_send_signal_to_reset(void *glo_IFB);

/* -------------------------------------------------------------------------------------------------------------- */

#endif /* #ifndef GLONASS_IFB_CORRECTION_H */
