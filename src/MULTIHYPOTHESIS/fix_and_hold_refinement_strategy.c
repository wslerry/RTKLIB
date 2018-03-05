/* 
 *   FIX-AND-HOLD refinement strategy (FXHR)
 */

#include "fix_and_hold_refinement_strategy.h"

#define MIN(x, y)  ( (x) <= (y) ) ? (x) : (y)

extern rtk_multi_t *rtk_multi_init_fxhr(prcopt_t opt)
{
    rtk_multi_t   *rtk_multi = rtk_multi_init(opt);
    rtk_t         *rtk;
    prcopt_t      opt_cont = opt;
    int index_new;

    /* init continuous filter */
    opt_cont.modear = ARMODE_CONT;
    opt_cont.gpsmodear = ARMODE_CONT;
    opt_cont.glomodear = GLO_ARMODE_OFF;
    opt_cont.bdsmodear = ARMODE_OFF;
    rtk = rtk_init(&opt_cont);
    index_new = rtk_multi_add(rtk_multi, rtk);
    rtk_multi->hypotheses[index_new]->target_solution_status = SOLQ_FLOAT;
    rtk_multi->index_main = index_new;
    
    assert( index_new == 0 );
    
    rtk_free(rtk);
    
    /* init fix-and-hold filter */
    rtk = rtk_init(&opt);
    index_new = rtk_multi_add(rtk_multi, rtk);
    rtk_multi->hypotheses[index_new]->target_solution_status = SOLQ_FIX;
    rtk_multi->index_main = index_new;
    
    assert( index_new == 1 );
    
    rtk_free(rtk);
    
    /* fix-and-hold filter is the main */
    rtk_multi->index_main = 1;
    
    assert( rtk_multi_is_valid_fxhr(rtk_multi) );

    return rtk_multi;
}

extern int rtk_multi_is_valid_fxhr(const rtk_multi_t *rtk_multi)
{
    if ( !rtk_multi_is_valid(rtk_multi) ) {

        return 0;
    }
    if ( rtk_multi->n_hypotheses != N_HYPOTHESES_FXHR ) {
        
        return 0;
    }
    if ( rtk_multi->hypotheses[0]->target_solution_status != SOLQ_FLOAT ) {

        return 0;
    }
    if ( rtk_multi->hypotheses[1]->target_solution_status != SOLQ_FIX ) {

        return 0;
    }
    if ( rtk_multi->index_main != 1 ) {
        
        return 0;
    }

    return 1;
}

static int are_solutions_close(const double pos_1[3], const double pos_2[3])
{
    double delta;

    delta = sqrt(SQR(pos_2[0]-pos_1[0]) + SQR(pos_2[1]-pos_1[1]) + SQR(pos_2[2]-pos_1[2]));
    if ( delta < RTK_POS_THRESH_FXHR ) {

        return 1;
    };

    return 0;
}

static double get_fix_fraction(rtk_hypothesis_t *hypothesis)
{
    int i, n_epochs;
    int n_fix_epochs = 0;
    double fix_fraction;
    rtk_stats_t *stats;
    
    n_epochs = hypothesis->stats_history->length;
    
    if ( n_epochs < 2 ) return 1.0; 
    
    /* the oldest epoch is allowed to be float and is not accounted */
    for (i = 0; i < (n_epochs-1); i++) {
            
        stats = rtk_hypothesis_get_stats_pointer(hypothesis, i);
        if ( stats->solution_status != SOLQ_FIX ) continue;
        n_fix_epochs++;
    }
    fix_fraction = ((double) n_fix_epochs) / (n_epochs - 1);

    return fix_fraction;
}

static int get_number_of_alternative_fixes(rtk_multi_t *rtk_multi)
{
    int i, n_epochs, n_epochs0, n_epochs1;
    int n_alternative_fixes = 0;
    rtk_stats_t *stats0, *stats1;
    
    assert( rtk_multi_is_valid_fxhr(rtk_multi) );
    
    n_epochs0 = rtk_multi->hypotheses[0]->stats_history->length;
    n_epochs1 = rtk_multi->hypotheses[1]->stats_history->length;
    
    n_epochs = MIN(n_epochs0, n_epochs1);

    for (i = 0; i < n_epochs; i++) {
        
        stats0 = rtk_hypothesis_get_stats_pointer(rtk_multi->hypotheses[0], i);
        stats1 = rtk_hypothesis_get_stats_pointer(rtk_multi->hypotheses[1], i);
        
        if ( stats1->solution_status != SOLQ_FIX ) {
            
            if ( stats0->solution_status == SOLQ_FIX ) n_alternative_fixes++;
            continue;
        }

        if ( stats0->solution_status == SOLQ_FIX ) {
            
            if ( !are_solutions_close(stats0->position, stats1->position) ) {
                
                n_alternative_fixes++;
            }
        }
    }
    
    return n_alternative_fixes;
}

static double get_rms_residuals_fix(rtk_hypothesis_t *hypothesis)
{
    int i, sat, freq, n_epochs;
    int n_fix_obs = 0;
    double sqr_residuals_fix = 0.0;
    double rms_residuals_fix = 0.0;
    rtk_stats_t *stats;

    n_epochs = hypothesis->stats_history->length;
    
    for (i = 0; i < n_epochs; i++) {
        
        stats = rtk_hypothesis_get_stats_pointer(hypothesis, i);
        if ( stats->solution_status != SOLQ_FIX ) continue;
        
        for (sat = 0; sat < MAXSAT; sat++) {
            for (freq = 0; freq < hypothesis->rtk->opt.nf; freq++) {
                
                if ( (stats->carrier_fix_status[sat][freq] != 2)
                  && (stats->carrier_fix_status[sat][freq] != 3) ) continue; /* satellite is not used for fix */
                if ( stats->residuals_carrier[sat][freq] == 0.0 )  continue; /* residuals are not defined */
                
                sqr_residuals_fix += SQR(stats->residuals_carrier[sat][freq]);
                n_fix_obs++;
            }
        }
    }
    
    if ( n_fix_obs > 0 ) rms_residuals_fix = sqrt(sqr_residuals_fix / n_fix_obs);

    return rms_residuals_fix;
}

/* return: -1 : validation impossible, 0 : not valid, 1 : valid */
static int rtk_multi_validate_hypothesis_fxhr(rtk_multi_t *rtk_multi, int index)
{
    int n_epochs;
    int n_alternative_fixes;
    rtk_hypothesis_t *rtk_hypothesis;
    double rms_residuals_fix;
    double fix_fraction;
    
    assert( rtk_multi_is_valid_fxhr(rtk_multi) );
    
    rtk_hypothesis = rtk_multi->hypotheses[index];
    n_epochs = rtk_hypothesis->stats_history->length;
    
    rtk_hypothesis->solution_quality = -1.0;
    
    if ( rtk_hypothesis->target_solution_status != SOLQ_FIX ) { /* validate only 'fix' hypotheses */
        
        return -1;
    }
    if ( n_epochs >= 2 ) {

        fix_fraction = get_fix_fraction(rtk_hypothesis);
        if ( fix_fraction < MIN_FIX_FRACTION_FXHR ) {

            rtk_hypothesis->solution_quality = LOW_SOL_QUAL + 1.0;
            return -1;
        }
    }
    if ( n_epochs < MIN_EPOCHS_FXHR ) {

        return -1;
    }

    n_alternative_fixes = get_number_of_alternative_fixes(rtk_multi);
    rms_residuals_fix   = get_rms_residuals_fix(rtk_hypothesis);

    rtk_hypothesis->solution_quality = rms_residuals_fix;
    
    if ( rms_residuals_fix >= RESID_THRESH_FXHR ) {
        
        trace(2, "fix is discarded (large res)\n");
        large_res_out_counter = CODE_SHOW_DURATION;
    }
    if ( (n_alternative_fixes >= MIN_ALTERNATIVE_FIXES_FXHR)
      && (rms_residuals_fix >= RESID_FINE_TRESH_FXHR) ) {
        
        trace(2, "fix is discarded (alternative fix)\n");
        alter_fix_out_counter = CODE_SHOW_DURATION;
    }
    
    trace(2, "number of alternative fixes: %d\n", n_alternative_fixes);
    
    return (rms_residuals_fix < RESID_THRESH_FXHR) 
        && ((n_alternative_fixes < MIN_ALTERNATIVE_FIXES_FXHR) 
            || (rms_residuals_fix < RESID_FINE_TRESH_FXHR));
}

extern void rtk_multi_split_fxhr(rtk_multi_t *rtk_multi, const rtk_input_data_t *rtk_input_data)
{
    return;
}

extern void rtk_multi_qualify_fxhr(rtk_multi_t *rtk_multi)
{
    prcopt_t opt;
    rtk_hypothesis_t *hypothesis0, *hypothesis1;
    
    assert( rtk_multi_is_valid_fxhr(rtk_multi) );
    
    /* exclude fix-and-hold hypothesis if validation failed */
    hypothesis0 = rtk_multi->hypotheses[0];
    hypothesis1 = rtk_multi->hypotheses[1];
        
    if ( rtk_multi_validate_hypothesis_fxhr(rtk_multi, 1) == 0 ) {
            
        opt = hypothesis1->rtk->opt;
        rtk_hypothesis_reset(hypothesis1, hypothesis0->rtk);
        hypothesis1->rtk->opt = opt;
        
        rtk_hypothesis_copy_stats_history(hypothesis0, hypothesis1, hypothesis0->stats_history->length-1);
    }
}

extern void rtk_multi_merge_fxhr(rtk_multi_t *rtk_multi)
{    
    assert( rtk_multi_is_valid_fxhr(rtk_multi) );
    
    rtk_copy(rtk_multi->hypotheses[1]->rtk, rtk_multi->rtk_out);
}
