/*
 *   GLONASS INTER-FREQUENCY PHASE BIASES (IFB) CORRECTION MODULE
 */

#include "glonass_IFB_correction.h"

#define MIN(x,y)    ( (x) <= (y) ? (x) : (y) )

/* -------------------------------------------------------------------------------------------------------------- */
/* API */

extern glo_IFB_t *glo_IFB_init()
{
  glo_IFB_t *glo_IFB = malloc(sizeof(glo_IFB_t));

  if ( glo_IFB == NULL ) {

    return 0;
  }

  glo_IFB->mode             = GLO_IFB_SEARCH_MODE;
  glo_IFB->adjustment_count = 0;
  glo_IFB->fix_outage       = 0;
  glo_IFB->glo_dt           = 0.0;
  glo_IFB->glo_dt_initial   = 0.0;
  glo_IFB->delta_glo_dt     = 0.0;
  glo_IFB->signal_to_reset  = 0;

  return glo_IFB;
}

extern int glo_IFB_is_valid(const glo_IFB_t *glo_IFB)
{
  if ( glo_IFB == NULL ) {

    return 0;
  }

  if ( glo_IFB->adjustment_count < 0
    || glo_IFB->adjustment_count > GLO_IFB_MAX_ADJUSTMENT_COUNT ) {

    return 0;
  }

  if ( glo_IFB->fix_outage < 0
    || glo_IFB->fix_outage > GLO_IFB_MAX_FIX_OUTAGE ) {

    return 0;
  }

  return 1;
}

extern void glo_IFB_free(glo_IFB_t *glo_IFB)
{
  assert( glo_IFB_is_valid(glo_IFB) );

  free(glo_IFB);
}

extern void glo_IFB_copy(const glo_IFB_t *glo_IFB_source, glo_IFB_t *glo_IFB_destination)
{
  assert( glo_IFB_is_valid(glo_IFB_source) );
  assert( glo_IFB_is_valid(glo_IFB_destination) );

  /* field-to-field copy */
  *glo_IFB_destination = *glo_IFB_source;
}

extern void glo_IFB_reset(glo_IFB_t *glo_IFB)
{
  assert( glo_IFB_is_valid(glo_IFB) );

  glo_IFB->mode             = GLO_IFB_SEARCH_MODE;
  glo_IFB->adjustment_count = 0;
  glo_IFB->fix_outage       = 0;
  glo_IFB->glo_dt           = 0.0;
  glo_IFB->glo_dt_initial   = 0.0;
  glo_IFB->delta_glo_dt     = 0.0;
  glo_IFB->signal_to_reset  = 0;
}

extern int glo_IFB_is_enough_sats(const rtk_t *rtk)
{
  int n_fix_sats = 0;
  int n_fix_glo  = 0;
  int sat;

  assert( rtk_is_valid(rtk) );

  /* count number of satellites available for ambiguity resolution */
  for (sat = 0; sat < MAXSAT; sat++) {

    if ( !rtk->ssat[sat].vsat[0] ) continue;

    if ( rtk->ssat[sat].fix[0] == 2
      || rtk->ssat[sat].fix[0] == 3 ) { /* 'fix' of 'hold' satellite status */

      n_fix_sats++;
      if ( rtk->ssat[sat].sys == SYS_GLO ) n_fix_glo++;
    }
  }

  if ( (n_fix_sats < GLO_IFB_MIN_SATS)
    || (n_fix_glo < GLO_IFB_MIN_GLO_SATS) ) {

    return 0;
  }

  return 1;
}

/* -------------------------------------------------------------------------------------------------------------- */
/* timestep operations */

static void search_mode_step(glo_IFB_t *glo_IFB, rtk_t *rtk)
{
  assert( glo_IFB_is_valid(glo_IFB) );
  assert( glo_IFB->mode == GLO_IFB_SEARCH_MODE );

  /* do not produce search step if not enough sats available */
  if ( !glo_IFB_is_enough_sats(rtk) ) {

    return;
  }

  glo_IFB->glo_dt += GLO_IFB_SEARCH_STEP;

  if ( glo_IFB->glo_dt > GLO_IFB_MAX_GLO_DT_SEARCH ) {

    glo_IFB->glo_dt -= GLO_IFB_MAX_GLO_DT_SEARCH - GLO_IFB_MIN_GLO_DT_SEARCH;
  }
}

/* calculate delta glo_dt optimizing one-epoch glonass residuals */
static double optimize_glo_dt_local(rtk_t *rtk)
{
  double delta_glo_dt, variance;
  double design_matrix[NSATGLO];
  int    freq_num[NSATGLO], freq_num_reference = 0;
  double residuals[NSATGLO];
  int    n_fix_glo = 0;
  int    i, sat;
  int    is_reference_defined = 0;

  assert( rtk_is_valid(rtk) );

  for (sat = 0; sat < MAXSAT; sat++) {

    if ( rtk->ssat[sat].sys != SYS_GLO ) continue;
    if ( !rtk->ssat[sat].vsat[0] )       continue;

    if ( rtk->ssat[sat].fix[0] == 2
      || rtk->ssat[sat].fix[0] == 3 ) { /* 'fix' of 'hold' satellite status */

      if ( rtk->ssat[sat].is_reference ) {

        freq_num_reference = rtk->ssat[sat].freq_num;
        is_reference_defined = 1;
      }
      else {

        freq_num[n_fix_glo]  = rtk->ssat[sat].freq_num;
        residuals[n_fix_glo] = rtk->ssat[sat].resc[0];
        n_fix_glo++;
      }
    }
  }

  assert( is_reference_defined );

  for (i = 0; i < n_fix_glo; i++) design_matrix[i] = -(freq_num[i] - freq_num_reference) * (CLIGHT / FREQ1_GLO);

  /* find optimum by solving least squares problem */
  lsq(design_matrix, residuals, 1, n_fix_glo, &delta_glo_dt, &variance);

  return delta_glo_dt;
}

static void adjustment_mode_step(glo_IFB_t *glo_IFB, rtk_t *rtk)
{
  double delta_glo_dt;
  int    window;
  double weight;

  assert( glo_IFB_is_valid(glo_IFB) );
  assert( rtk_is_valid(rtk) );

  /* do not update glo_IFB structure if solution status is not 'fix' */
  if ( rtk->sol.stat != SOLQ_FIX ) {

    return;
  }

  /* do not update glo_IFB structure if not enough sats available */
  if ( !glo_IFB_is_enough_sats(rtk) ) {

    return;
  }

  /* one-epoch delta_glo_dt estimation */
  delta_glo_dt = optimize_glo_dt_local(rtk);

  /* count adjustment steps until it reaches some large value */
  if ( glo_IFB->adjustment_count < GLO_IFB_MAX_ADJUSTMENT_COUNT ) {

    glo_IFB->adjustment_count++;
  }

  if ( glo_IFB->adjustment_count == 1 ) { /* first adjustment step */

    glo_IFB->glo_dt         += delta_glo_dt;
    glo_IFB->glo_dt_initial  = glo_IFB->glo_dt;
  }
  else {                                  /* glo_dt smoothing */

    window = MIN(glo_IFB->adjustment_count, GLO_IFB_MAX_ADJUSTMENT_WINDOW);
    weight = 1.0 / window;

    glo_IFB->glo_dt += weight * delta_glo_dt;
  }
}

/* -------------------------------------------------------------------------------------------------------------- */
/* mode transition operations */

static int check_search_to_adjustment_switch_condition(const glo_IFB_t *glo_IFB, const rtk_t *rtk)
{
  assert( glo_IFB_is_valid(glo_IFB) );
  assert( rtk_is_valid(rtk) );
  assert( glo_IFB->mode == GLO_IFB_SEARCH_MODE );

  if ( (rtk->sol.stat == SOLQ_FIX)
    && (glo_IFB_is_enough_sats(rtk)) ) {

    return 1;
  }

  return 0;
}

static void switch_search_to_adjustment_mode(glo_IFB_t *glo_IFB)
{
  assert( glo_IFB_is_valid(glo_IFB) );
  assert( glo_IFB->mode == GLO_IFB_SEARCH_MODE );

  glo_IFB->mode = GLO_IFB_ADJUSTMENT_MODE;
}

static int check_adjustment_to_search_switch_condition(const glo_IFB_t *glo_IFB, const rtk_t *rtk)
{
  assert( glo_IFB_is_valid(glo_IFB) );
  assert( rtk_is_valid(rtk) );
  assert( glo_IFB->mode == GLO_IFB_ADJUSTMENT_MODE );

  if ( (rtk->sol.stat != SOLQ_FIX) && (glo_IFB->adjustment_count < GLO_IFB_VALIDATION_COUNT)
    && (glo_IFB->fix_outage >= MIN(glo_IFB->adjustment_count, GLO_IFB_MAX_FIX_OUTAGE)) ) {

    return 1;
  }

  return 0;
}

static void switch_adjustment_to_search_mode(glo_IFB_t *glo_IFB)
{
  assert( glo_IFB_is_valid(glo_IFB) );
  assert( glo_IFB->mode == GLO_IFB_ADJUSTMENT_MODE );

  glo_IFB->adjustment_count = 0;
  glo_IFB->glo_dt_initial   = 0.0;
  glo_IFB->mode             = GLO_IFB_SEARCH_MODE;
}

static int check_adjustment_to_frozen_switch_condition(const glo_IFB_t *glo_IFB)
{
  assert( glo_IFB_is_valid(glo_IFB) );
  assert( glo_IFB->mode == GLO_IFB_ADJUSTMENT_MODE );

  if ( glo_IFB->adjustment_count >= GLO_IFB_FREEZE_COUNT ) {

    return 1;
  }

  return 0;
}

static void switch_adjustment_to_frozen_mode(glo_IFB_t *glo_IFB)
{
  assert( glo_IFB_is_valid(glo_IFB) );
  assert( glo_IFB->mode == GLO_IFB_ADJUSTMENT_MODE );

  glo_IFB->mode = GLO_IFB_FROZEN_MODE;
}

static int check_reset_condition(const glo_IFB_t *glo_IFB, const rtk_t *rtk)
{
  assert( glo_IFB_is_valid(glo_IFB) );
  assert( rtk_is_valid(rtk) );

  /* reset if 'to reset' signal is recieved */
  if ( glo_IFB->signal_to_reset == GLO_IFB_SIGNAL_TO_RESET ) {

    return 1;
  }

  /* reset if glo_dt drift is unacceptable */
  if ( (glo_IFB->mode == GLO_IFB_ADJUSTMENT_MODE)
    && (fabs(glo_IFB->glo_dt - glo_IFB->glo_dt_initial) > GLO_IFB_MAX_GLO_DT_DRIFT) ) {

    return 1;
  }

  return 0;
}

/* -------------------------------------------------------------------------------------------------------------- */
/* the main function */

extern void glo_IFB_process(glo_IFB_t *glo_IFB, rtk_t *rtk)
{
  double glo_dt_prev;

  assert( glo_IFB_is_valid(glo_IFB) );
  assert( rtk_is_valid(rtk) );

  glo_dt_prev = glo_IFB->glo_dt;

  /* update fix_outage field */
  if ( rtk->sol.stat == SOLQ_FLOAT ) glo_IFB->fix_outage++;
  if ( rtk->sol.stat == SOLQ_FIX )   glo_IFB->fix_outage = 0;
  if ( glo_IFB->fix_outage > GLO_IFB_MAX_FIX_OUTAGE ) glo_IFB->fix_outage = GLO_IFB_MAX_FIX_OUTAGE;

  /* check mode transition conditions and switch if it necessary */
  switch ( glo_IFB->mode ) {

    case GLO_IFB_SEARCH_MODE:
      if ( check_search_to_adjustment_switch_condition(glo_IFB, rtk) ) {

        switch_search_to_adjustment_mode(glo_IFB);
      }
      break;

    case GLO_IFB_ADJUSTMENT_MODE:
      if ( check_adjustment_to_search_switch_condition(glo_IFB, rtk) ) {

        switch_adjustment_to_search_mode(glo_IFB);
      }
      else if ( check_adjustment_to_frozen_switch_condition(glo_IFB) ) {

        switch_adjustment_to_frozen_mode(glo_IFB);
      }
      break;

    default: break;
  }

  /* produce glo_dt estimation step */
  switch ( glo_IFB->mode ) {

    case GLO_IFB_SEARCH_MODE:
      search_mode_step(glo_IFB, rtk);
      break;

    case GLO_IFB_ADJUSTMENT_MODE:
      adjustment_mode_step(glo_IFB, rtk);
      break;

    default: break;
  }

  /* check glo_IFB reset condition and reset if necessary */
  if ( check_reset_condition(glo_IFB, rtk) ) {

    glo_IFB_reset(glo_IFB);
  }

  glo_IFB->delta_glo_dt = glo_IFB->glo_dt - glo_dt_prev;
}
