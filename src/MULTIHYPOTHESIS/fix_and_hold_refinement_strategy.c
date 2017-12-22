/* 
 *   FIX-AND-HOLD refinement strategy (FXHR)
 */

#include "fix_and_hold_refinement_strategy.h"

double ratio_float = 0.0;

extern rtk_multi_t *rtk_multi_init_fxhr(prcopt_t opt)
{
    rtk_multi_t   *rtk_multi = rtk_multi_init(opt);
    rtk_history_t *hypothesis;
    rtk_t         *rtk;

    /* init float filter */
    opt.modear = ARMODE_OFF;
    opt.gpsmodear = ARMODE_OFF;
    opt.glomodear = GLO_ARMODE_OFF;
    opt.bdsmodear = ARMODE_OFF;
    hypothesis = rtk_history_init(opt);
    rtk = rtk_init(&opt);
    rtk_history_add(hypothesis, rtk);
    hypothesis->target_solution_status = SOLQ_FLOAT;
    rtk_multi_add(rtk_multi, hypothesis);
    
    rtk_free(rtk);

    return rtk_multi;
}

extern int rtk_multi_is_valid_fxhr(const rtk_multi_t *rtk_multi)
{
    int i;
    
    if ( !rtk_multi_is_valid(rtk_multi) ) {

        return 0;
    }
    if ( rtk_multi->n_hypotheses > N_HYPOTHESES_FXHR ) {
        
        return 0;
    }
    /* float filter (hypothesis) should be defined */
    if ( !rtk_history_is_valid(rtk_multi->hypotheses[0]) )  {

        return 0;
    }
    if ( rtk_multi->hypotheses[0]->target_solution_status != SOLQ_FLOAT ) {

        return 0;
    }
    for (i = 1; i < N_HYPOTHESES_FXHR; i++) {

        if ( rtk_multi->hypotheses[i] == NULL ) continue;
        if ( rtk_multi->hypotheses[i]->target_solution_status != SOLQ_FIX ) {

            return 0;
        }
    }

    return 1;
}

/* return: -1 : validation impossible, 0 : not valid, 1 : valid */
static int rtk_history_validate_fxhr(rtk_history_t *rtk_history)
{
    int i, sat, freq, n_epochs;
    int n_fix_epochs = 0;
    int n_fix_obs    = 0;
    double sqr_residuals_fix = 0.0;
    double rms_residuals_fix = 0.0;
    double fix_fraction;
    rtk_t *rtk;

    assert( rtk_history_is_valid(rtk_history) );
    n_epochs = rtk_history->history->length;
    assert( n_epochs > 0 );
    
    rtk_history->solution_quality = -1.0;

    if ( rtk_history->target_solution_status != SOLQ_FIX ) { /* validate only 'fix' hypotheses */
        
        return -1;
    }
    if ( n_epochs >= 2 ) {

        for (i = 0; i < (n_epochs-1); i++) {
            
            rtk = rtk_history_get_pointer(rtk_history, i);
            if ( rtk->sol.stat != SOLQ_FIX ) continue;
            n_fix_epochs++;
        }
        fix_fraction = ((double) n_fix_epochs) / (n_epochs - 1);
        if ( fix_fraction < MIN_FIX_FRACTION_FXHR ) {

            rtk_history->solution_quality = LOW_SOL_QUAL + 1.0;
            return -1;
        }
    }
    if ( n_epochs < MIN_EPOCHS_FXHR ) {

        return -1;
    }

    for (i = 0; i < n_epochs; i++) {
        
        rtk = rtk_history_get_pointer(rtk_history, i);
        if ( rtk->sol.stat != SOLQ_FIX ) continue;

        for (sat = 0; sat < MAXSAT; sat++) {
            for (freq = 0; freq < rtk->opt.nf; freq++) {
                
                if ( rtk->ssat[sat].vsat[freq] == 0 ) continue;
                if ( (rtk->ssat[sat].fix[freq] != 2)
                  && (rtk->ssat[sat].fix[freq] != 3) ) continue;
                if ( rtk->ssat[sat].resc[freq] == 0.0 
                  && rtk->ssat[sat].resp[freq] == 0.0 ) continue;
                
                sqr_residuals_fix += SQR(rtk->ssat[sat].resc[freq]);
                n_fix_obs++;
            }
        }
    }
      
    if ( n_fix_obs > 0 ) rms_residuals_fix = sqrt(sqr_residuals_fix / n_fix_obs);

    rtk_history->solution_quality = rms_residuals_fix;
    
    return (rms_residuals_fix < RESID_THRESH_FXHR);
}

extern void rtk_multi_split_fxhr(rtk_multi_t *rtk_multi, const rtk_input_data_t *rtk_input_data)
{
    static int split_outage = -1;
    rtk_history_t *hypothesis;
    int i;
    int is_fix_possible = 0; 
    int is_fix_hypothesis_of_low_quality = 0;
    rtk_t *rtk;
    rtk_t *rtk_tmp;
    
    assert( rtk_multi_is_valid_fxhr(rtk_multi) );
    assert( rtk_input_data_is_valid(rtk_input_data) );

    /* renew the base position */
    for (i = 0; i < N_HYPOTHESES_FXHR; i++ ) {
        
        hypothesis = rtk_multi->hypotheses[i];
        if ( hypothesis == NULL ) continue;
        
        rtk = rtk_history_get_pointer_to_last(hypothesis);
        memcpy(rtk->opt.rb, rtk_multi->opt.rb, sizeof(double) * 3);
        memcpy(rtk->rb, rtk_multi->opt.rb, sizeof(double) * 3);
    }
    
    split_outage++;
    if ( split_outage < SPLIT_INTERVAL_FXHR ) {

        return;
    }

    /* check is there a place for a new hypothesis */
    if ( rtk_multi->n_hypotheses >= N_HYPOTHESES_FXHR ) {

        /* check if 'fix' hypothesis is weak or not */
        hypothesis = rtk_multi->hypotheses[N_HYPOTHESES_FXHR-1];
        rtk = rtk_history_get_pointer_to_last(hypothesis);
        is_fix_hypothesis_of_low_quality = (rtk->sol.stat != SOLQ_FIX) 
                                        && (hypothesis->solution_quality >= LOW_SOL_QUAL);
        if ( !is_fix_hypothesis_of_low_quality ) {
            
            return;
        }
    }
    
    split_outage = 0;

    /* check if fix is possible */
    rtk = rtk_history_get_pointer_to_last(rtk_multi->hypotheses[0]);
    rtk_tmp = rtk_init(&rtk_multi->opt);
    rtk_copy(rtk, rtk_tmp);
    rtk_tmp->opt = rtk_multi->opt;
    rtkpos(rtk_tmp, rtk_input_data->obsd, rtk_input_data->n_obsd, rtk_input_data->nav);
    is_fix_possible = (rtk_tmp->sol.stat == SOLQ_FIX);
    ratio_float = rtk_tmp->sol.ratio;

    if ( !is_fix_possible ) {

        rtk_free(rtk_tmp);
        return;
    }

    /* initialize new hypothesis structure */
    rtk = rtk_history_get_pointer_to_last(rtk_multi->hypotheses[0]);
    rtk_copy(rtk, rtk_tmp);
    rtk_tmp->opt = rtk_multi->opt;
    hypothesis = rtk_history_init(rtk_multi->opt);
    hypothesis->target_solution_status = SOLQ_FIX;
    rtk_history_add(hypothesis, rtk_tmp);

    /* add new hypothesis if all checks are passed */
    if ( is_fix_hypothesis_of_low_quality ) rtk_multi_exclude(rtk_multi, N_HYPOTHESES_FXHR-1); /* kill weak hypothesis */
    rtk_multi_add(rtk_multi, hypothesis);

    rtk_free(rtk_tmp);
}

extern void rtk_multi_qualify_fxhr(rtk_multi_t *rtk_multi)
{
    int i;
    rtk_history_t *hypothesis;

    assert( rtk_multi_is_valid_fxhr(rtk_multi) );
    
    /* exclude bad hypotheses */
    for (i = 1; i < N_HYPOTHESES_FXHR; i++) { /* note: i == 0 is for float filter (never excluded) */
        
        hypothesis = rtk_multi->hypotheses[i];
        if ( hypothesis == NULL ) continue;

        if ( rtk_history_validate_fxhr(hypothesis) == 0 ) {
            
            rtk_multi_exclude(rtk_multi, i);
        }
    }
    
}

extern void rtk_multi_merge_fxhr(rtk_multi_t *rtk_multi)
{
    rtk_history_t *hypothesis = NULL;
    rtk_t *rtk = NULL;

    assert( rtk_multi_is_valid_fxhr(rtk_multi) );

    if ( rtk_multi->n_hypotheses == 1 ) { /* output float */

        hypothesis = rtk_multi->hypotheses[0];
        rtk = rtk_history_get_pointer_to_last(hypothesis);
        rtk->sol.ratio = ratio_float;
    }
    
    if ( rtk_multi->n_hypotheses > 1 ) { /* output fix */
        
        hypothesis = rtk_multi->hypotheses[1];
        rtk = rtk_history_get_pointer_to_last(hypothesis);
    }
    
    assert( rtk_history_is_valid(hypothesis) );
    
    rtk_copy(rtk, rtk_multi->rtk_out);
    
}
