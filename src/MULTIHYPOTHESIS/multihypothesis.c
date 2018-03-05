#include "multihypothesis.h"

int alter_fix_out_counter = 0;
int large_res_out_counter = 0;

/* -------------------------------------------------------------------------- */
/* manipulation functions --------------------------------------------------- */

/* rtk_stats */

static rtk_stats_t *rtk_stats_init()
{
    rtk_stats_t *rtk_stats = malloc(sizeof(rtk_stats_t));
    if ( rtk_stats == NULL ) {
        
        return NULL;
    }
    
    rtk_stats->solution_status = SOLQ_NONE;
    memset(rtk_stats->residuals_carrier,  0, sizeof(double) * MAXSAT * NFREQ);
    memset(rtk_stats->carrier_fix_status, 0, sizeof(int)    * MAXSAT * NFREQ);
    memset(rtk_stats->position, 0, sizeof(double) * 3);
    
    return rtk_stats;
}

static int rtk_stats_is_valid(const rtk_stats_t *rtk_stats)
{
    int solstat;
    
    if ( rtk_stats == NULL ) {
        
        return 0;
    }
    
    solstat = rtk_stats->solution_status;
    if ( (solstat < 0) || (solstat > MAXSOLQ) ) {
        
        return 0;
    }
    
    return 1;
}

static void rtk_stats_free(rtk_stats_t *rtk_stats)
{
    assert( rtk_stats_is_valid(rtk_stats) );
    
    free(rtk_stats);
}

static rtk_stats_t *rtk_get_stats(const rtk_t *rtk)
{
    int sat, freq;
    rtk_stats_t *rtk_stats= rtk_stats_init();
    
    assert( rtk_is_valid(rtk) );
    assert( rtk_stats_is_valid(rtk_stats) );
    
    rtk_stats->solution_status = rtk->sol.stat;
    
    for (sat = 0; sat < MAXSAT; sat++) {
        
        for (freq = 0; freq < rtk->opt.nf; freq++) {
            
            if ( rtk->ssat[sat].vsat[freq] == 0 ) { /* satellite is not valid */
                
                rtk_stats->carrier_fix_status[sat][freq] = 0;     
            }
            else { /* satellite is valid */
                
                rtk_stats->carrier_fix_status[sat][freq] = rtk->ssat[sat].fix[freq];
            }
            
            rtk_stats->residuals_carrier[sat][freq] = rtk->ssat[sat].resc[freq];
        }
    }
    
    memcpy(rtk_stats->position, rtk->sol.rr, sizeof(double) * 3);
    
    return rtk_stats;
}

/* rtk_stats_queue */

static rtk_stats_queue_t *rtk_stats_queue_init()
{
    int i, j;
    rtk_stats_queue_t *rtk_stats_queue = malloc(sizeof(rtk_stats_queue_t));
    if ( rtk_stats_queue == NULL ) {

        return NULL;
    }
    
    rtk_stats_queue->length = 0;

    for (i = 0; i < MAX_STATS_QUEUE; i++) {
        
        rtk_stats_queue->offset[i] = i;
        rtk_stats_queue->stats[i] = rtk_stats_init();
        if ( rtk_stats_queue->stats[i] == NULL ) {
            
            for (j = 0; j < i; j++) rtk_stats_free(rtk_stats_queue->stats[j]);
            free(rtk_stats_queue);
            return NULL;
        }
    }
    
    return rtk_stats_queue;
}

static int rtk_stats_queue_is_valid(const rtk_stats_queue_t *rtk_stats_queue)
{
    int i;
    int offsets_check[MAX_STATS_QUEUE] = {0};
    
    if ( rtk_stats_queue == NULL ) {
        
        return 0;
    }
    if ( (rtk_stats_queue->length < 0) || (rtk_stats_queue->length > MAX_STATS_QUEUE) ) {
        
        return 0;
    }

    for (i = 0; i < MAX_STATS_QUEUE; i++) {   
        
        if ( !rtk_stats_is_valid(rtk_stats_queue->stats[i]) ) {
            
            return 0;
        }
        if ( (rtk_stats_queue->offset[i] < 0) || (rtk_stats_queue->offset[i] >= MAX_STATS_QUEUE) ) {
            
            return 0;
        }
        offsets_check[rtk_stats_queue->offset[i]] = 1;
    }
    
    for (i = 0; i < MAX_STATS_QUEUE; i++) {
        
        if ( offsets_check[i] == 0 ) {

            return 0;
        }
    }
    
    return 1;
}

static void rtk_stats_queue_free(rtk_stats_queue_t *rtk_stats_queue)
{
    int i;
    assert( rtk_stats_queue_is_valid(rtk_stats_queue) );

    for (i = 0; i < MAX_STATS_QUEUE; i++) {
        
        rtk_stats_free(rtk_stats_queue->stats[i]);
    }
    
    free(rtk_stats_queue);
}

static int rtk_stats_queue_is_index_valid(const rtk_stats_queue_t *rtk_stats_queue, int index)
{
    int is_index_in_bounds;
    assert( rtk_stats_queue_is_valid(rtk_stats_queue) );

    is_index_in_bounds = (index >= 0) && (index < MAX_STATS_QUEUE) && (index < rtk_stats_queue->length);
    if ( !is_index_in_bounds ) {
        
        return 0;
    }
    
    return 1;
}

static void rtk_stats_queue_cut(rtk_stats_queue_t *rtk_stats_queue, int n_cut)
{
    int i;
    int offsets_cut[MAX_STATS_QUEUE];
    
    assert( rtk_stats_queue_is_valid(rtk_stats_queue) );
    assert( n_cut >= 0 );
    
    if ( n_cut > rtk_stats_queue->length ) {
        
        n_cut = rtk_stats_queue->length;
    }
    
    if ( n_cut > 0 ) {
        for (i = 0; i < n_cut; i++) {
            offsets_cut[i] = rtk_stats_queue->offset[i];
        }
        for (i = 0; i < MAX_STATS_QUEUE - n_cut; i++) {
            rtk_stats_queue->offset[i] = rtk_stats_queue->offset[i + n_cut];
        }
        for (i = MAX_STATS_QUEUE - n_cut; i < MAX_STATS_QUEUE; i++) {
            rtk_stats_queue->offset[i] = offsets_cut[i - MAX_STATS_QUEUE + n_cut];
        }
        rtk_stats_queue->length -= n_cut;
    }

}

static void rtk_stats_queue_add(rtk_stats_queue_t *rtk_stats_queue, const rtk_stats_t *rtk_stats, int n_rtk_stats)
{
    int i, offset;
    
    assert( rtk_stats_queue_is_valid(rtk_stats_queue) );
    assert( n_rtk_stats >= 0 );
    
    for (i = 0; i < n_rtk_stats; i++) {
        
        assert( rtk_stats_is_valid(&rtk_stats[i]) );
        
        /* add i-th rtk to the queue */
        if ( rtk_stats_queue->length <= 0 ) {
            offset = rtk_stats_queue->offset[0];
            rtk_stats_queue->length = 1;
        }
        else if ( rtk_stats_queue->length < MAX_STATS_QUEUE ) {
            offset = rtk_stats_queue->offset[rtk_stats_queue->length];
            rtk_stats_queue->length++;
        }
        else { /* rtk_stats_queue->length >= MAX_STATS_QUEUE */
            rtk_stats_queue_cut(rtk_stats_queue, 1);
            offset = rtk_stats_queue->offset[MAX_STATS_QUEUE - 1];
            rtk_stats_queue->length = MAX_STATS_QUEUE;
        }
        *rtk_stats_queue->stats[offset] = rtk_stats[i];
    }
    
}

static rtk_stats_t *rtk_stats_queue_get_pointer(const rtk_stats_queue_t *rtk_stats_queue, int index)
{
    int offset;
    
    assert( rtk_stats_queue_is_valid(rtk_stats_queue) );
    
    if ( !rtk_stats_queue_is_index_valid(rtk_stats_queue, index) ) {
        
        return NULL;
    }
    
    offset = rtk_stats_queue->offset[index];
    
    return rtk_stats_queue->stats[offset];
}

/* rtk_hypothesis */

extern rtk_hypothesis_t *rtk_hypothesis_init(prcopt_t opt)
{
    rtk_hypothesis_t *rtk_hypothesis = malloc(sizeof(rtk_hypothesis_t));
    if ( rtk_hypothesis == NULL ) {
    
        return NULL;
    }
    
    rtk_hypothesis->rtk = rtk_init(&opt);
    if ( rtk_hypothesis->rtk == NULL ) {
    
        free(rtk_hypothesis);
        return NULL;
    }
    
    rtk_hypothesis->stats_history = rtk_stats_queue_init(opt);
    if ( rtk_hypothesis->stats_history == NULL ) {
    
        rtk_free(rtk_hypothesis->rtk);
        free(rtk_hypothesis);
        return NULL;
    }
    
    rtk_hypothesis->solution_quality = -1.0;
    rtk_hypothesis->is_active        = 0;
    
    return rtk_hypothesis;
}

extern int rtk_hypothesis_is_valid(const rtk_hypothesis_t *rtk_hypothesis)
{
    if ( rtk_hypothesis == NULL ) {

        return 0;
    }
    if ( !rtk_is_valid(rtk_hypothesis->rtk) ) {
        
        return 0;
    }
    if ( !rtk_stats_queue_is_valid(rtk_hypothesis->stats_history) ) {
    
        return 0;
    }
    if ( (!rtk_hypothesis->is_active) && (rtk_hypothesis->stats_history->length != 0) ) {
    
        return 0;
    }
    if ( (rtk_hypothesis->is_active) && (rtk_hypothesis->stats_history->length <= 0) ) {
    
        return 0;
    }
    
    return 1;
}

extern void rtk_hypothesis_free(rtk_hypothesis_t *rtk_hypothesis)
{
    assert( rtk_hypothesis_is_valid(rtk_hypothesis) );

    rtk_free(rtk_hypothesis->rtk);
    rtk_stats_queue_free(rtk_hypothesis->stats_history);
    free(rtk_hypothesis);
}

extern void rtk_hypothesis_activate(rtk_hypothesis_t *rtk_hypothesis, const rtk_t *rtk)
{
    rtk_stats_t *rtk_stats;
    
    assert( rtk_hypothesis_is_valid(rtk_hypothesis) );
    assert( !rtk_hypothesis->is_active );
    
    if ( rtk != NULL ) {
        
        assert( rtk_is_valid(rtk) );
        rtk_copy(rtk, rtk_hypothesis->rtk);
        
        rtk_stats = rtk_get_stats(rtk);
        rtk_stats_queue_add(rtk_hypothesis->stats_history, rtk_stats, 1);
        rtk_stats_free(rtk_stats);
    }
    
    rtk_hypothesis->is_active = 1;
    rtk_hypothesis->solution_quality = -1.0;
}

extern void rtk_hypothesis_deactivate(rtk_hypothesis_t *rtk_hypothesis)
{
    assert( rtk_hypothesis_is_valid(rtk_hypothesis) );
    assert( rtk_hypothesis->is_active );
    
    rtk_stats_queue_cut(rtk_hypothesis->stats_history, rtk_hypothesis->stats_history->length);
    rtk_reset(rtk_hypothesis->rtk);
    rtk_hypothesis->is_active = 0;
    rtk_hypothesis->solution_quality = -1.0;
}

extern void rtk_hypothesis_reset(rtk_hypothesis_t *rtk_hypothesis, rtk_t *rtk)
{
    assert( rtk_hypothesis_is_valid(rtk_hypothesis) );
    
    rtk_hypothesis_deactivate(rtk_hypothesis);
    rtk_hypothesis_activate(rtk_hypothesis, rtk);
}

extern rtk_stats_t *rtk_hypothesis_get_stats_pointer(const rtk_hypothesis_t *rtk_hypothesis, int index)
{
    int index_tail;
    
    assert( rtk_hypothesis_is_valid(rtk_hypothesis) );
    assert( rtk_hypothesis->is_active );
    
    /* head of rtk_hypothesis (last in) is a tail of rtk_stats_queue  */
    index_tail = rtk_hypothesis->stats_history->length - 1 - index;
    return rtk_stats_queue_get_pointer(rtk_hypothesis->stats_history, index_tail);
}

extern rtk_stats_t *rtk_hypothesis_get_stats_pointer_last(const rtk_hypothesis_t *rtk_hypothesis)
{
    return rtk_hypothesis_get_stats_pointer(rtk_hypothesis, 0);
}

extern void rtk_hypothesis_copy_stats_history(const rtk_hypothesis_t *rtk_hypothesis_source,
                                              rtk_hypothesis_t *rtk_hypothesis_destination, int index)
{
    int i;
    rtk_stats_t *stats;
    
    assert( rtk_hypothesis_is_valid(rtk_hypothesis_source) );
    assert( rtk_hypothesis_is_valid(rtk_hypothesis_destination) );
    
    /* empty stats_history */
    rtk_stats_queue_cut(rtk_hypothesis_destination->stats_history, rtk_hypothesis_destination->stats_history->length);
    
    for (i = index; i >= 0; i--) {
        
        stats = rtk_hypothesis_get_stats_pointer(rtk_hypothesis_source, i);
        rtk_stats_queue_add(rtk_hypothesis_destination->stats_history, stats, 1);
    }
}

/* rtk_multi_strategy */

extern int rtk_multi_strategy_is_valid(const rtk_multi_strategy_t *rtk_multi_strategy)
{
    if ( rtk_multi_strategy == NULL ) {
    
        return 0;
    }
    
    if ( (rtk_multi_strategy->split == NULL)
      || (rtk_multi_strategy->qualify == NULL)
      || (rtk_multi_strategy->merge == NULL) ) {
    
        return 0;
    }
    
    return 1;
}

/* rtk_input_data */

extern int rtk_input_data_is_valid(const rtk_input_data_t *rtk_input_data)
{
    int n_obsd = rtk_input_data->n_obsd;
    
    if ( rtk_input_data == NULL ) {
        
        return 0;
    }
    if ( rtk_input_data->obsd == NULL ) {
        
        return 0;
    }
    if ( (n_obsd < 0) || (n_obsd > MAXOBS * 2) ) {
        
        return 0;
    }
    if ( rtk_input_data->nav == NULL ) {
        
        return 0;
    }
    
    return 1;
}

/* rtk_multi */

extern rtk_multi_t *rtk_multi_init(prcopt_t opt)
{
    int i, j;
    rtk_multi_t *rtk_multi = malloc(sizeof(rtk_multi_t));
    if ( rtk_multi == NULL ) {
        
        return NULL;
    }
    
    rtk_multi->opt = opt;
    rtk_multi->rtk_out = rtk_init(&opt);
    if ( rtk_multi->rtk_out == NULL ) {
    
        free(rtk_multi);
        return NULL;
    }
    
    for (i = 0; i < MAX_RTK_HYPOTHESES; i++) {
        
        rtk_multi->hypotheses[i] = rtk_hypothesis_init(opt);
        if ( rtk_multi->hypotheses[i] == NULL ) {
            
            for (j = 0; j < i; j++) {
                
                rtk_hypothesis_free(rtk_multi->hypotheses[j]);
            }
            rtk_free(rtk_multi->rtk_out);
            free(rtk_multi);
            return NULL;
        }
    }
    rtk_multi->n_hypotheses = 0;
    rtk_multi->index_main = -1;
    
    return rtk_multi;
}

extern int rtk_multi_is_valid(const rtk_multi_t *rtk_multi)
{
    int i, is_n_hypotheses_in_bounds;
    int index_main;
    int n_hypotheses = 0;
    
    if ( rtk_multi == NULL ) {
        
        return 0;
    }
    
    is_n_hypotheses_in_bounds = (rtk_multi->n_hypotheses >= 0) 
                             && (rtk_multi->n_hypotheses <= MAX_RTK_HYPOTHESES);

    if ( !is_n_hypotheses_in_bounds ) {
    
        return 0;
    }
    if ( !rtk_is_valid(rtk_multi->rtk_out) ) {

        return 0;
    }
    if ( (rtk_multi->index_main < -1) || (rtk_multi->index_main >= MAX_RTK_HYPOTHESES) ) {
        
        return 0;
    }

    for (i = 0; i < MAX_RTK_HYPOTHESES; i++) {

        if ( !rtk_hypothesis_is_valid(rtk_multi->hypotheses[i]) ) {

            return 0;
        }
        if ( rtk_multi->hypotheses[i]->is_active ) {
            
            n_hypotheses++;
        }
    }
    index_main = rtk_multi->index_main;
    if ( (index_main >= 0) && (!rtk_multi->hypotheses[index_main]->is_active) ) {
        
        return 0;
    }
    if ( n_hypotheses != rtk_multi->n_hypotheses ) {

        return 0;
    }

    return 1;
}

extern void rtk_multi_free(rtk_multi_t *rtk_multi)
{
    int i;

    assert( rtk_multi_is_valid(rtk_multi) );
    
    for (i = 0; i < MAX_RTK_HYPOTHESES; i++) {
            
        rtk_hypothesis_free(rtk_multi->hypotheses[i]);
    }

    rtk_free(rtk_multi->rtk_out);
    free(rtk_multi);
}

extern int rtk_multi_add(rtk_multi_t *rtk_multi, rtk_t *rtk)
{
    int i;
    
    assert( rtk_multi_is_valid(rtk_multi) );
    assert( rtk_is_valid(rtk) );
    
    /* try to find a vacancy */
    for (i = 0; i < MAX_RTK_HYPOTHESES; i++) {
    
        if ( !rtk_multi->hypotheses[i]->is_active ) break;
    }
    
    if ( i >= MAX_RTK_HYPOTHESES ) { /* no free space */
    
        return -1;
    }
    
    rtk_hypothesis_activate(rtk_multi->hypotheses[i], rtk);
    rtk_multi->n_hypotheses++;
    
    return i;
}

extern void rtk_multi_exclude(rtk_multi_t *rtk_multi, int index)
{
    int is_index_in_bounds = (index >= 0) && (index < MAX_RTK_HYPOTHESES);
    
    assert( rtk_multi_is_valid(rtk_multi) );
    assert( is_index_in_bounds );
    assert( rtk_multi->hypotheses[index]->is_active );
    
    rtk_hypothesis_deactivate(rtk_multi->hypotheses[index]);
    rtk_multi->n_hypotheses--;
    
    if ( rtk_multi->index_main == index ) {
        
        rtk_multi->index_main = -1;
    }
}

extern void rtk_hypothesis_step(rtk_hypothesis_t *rtk_hypothesis,
                                const rtk_input_data_t *rtk_input_data)
{
    rtk_stats_t *rtk_stats;
    
    assert( rtk_hypothesis_is_valid(rtk_hypothesis) );
    assert( rtk_hypothesis->is_active );
    assert( rtk_input_data_is_valid(rtk_input_data) );
    
    rtkpos(rtk_hypothesis->rtk, rtk_input_data->obsd, rtk_input_data->n_obsd, rtk_input_data->nav);
    
    rtk_stats = rtk_get_stats(rtk_hypothesis->rtk);
    rtk_stats_queue_add(rtk_hypothesis->stats_history, rtk_stats, 1);
    rtk_stats_free(rtk_stats);
}

extern void rtk_multi_estimate_main(rtk_multi_t *rtk_multi,
                                    const rtk_input_data_t *rtk_input_data)
{
    int index_main = rtk_multi->index_main;
    rtk_hypothesis_t *hypothesis_main;

    assert( rtk_multi_is_valid(rtk_multi) );
    assert( rtk_input_data_is_valid(rtk_input_data) );
    assert( index_main >= 0 );
    
    hypothesis_main = rtk_multi->hypotheses[index_main];
    
    if ( !hypothesis_main->is_active ) {
        
        return;
    }
    
    rtk_copy(hypothesis_main->rtk, rtk_multi->rtk_out);
    
    /* set new base position */
    memcpy(rtk_multi->rtk_out->opt.rb, rtk_multi->opt.rb, sizeof(double) * 3);
    memcpy(rtk_multi->rtk_out->rb, rtk_multi->opt.rb, sizeof(double) * 3);
    
    /* turn off expensive option to produce local estimation faster */
    rtk_multi->rtk_out->opt.residual_mode = 0;

    rtkpos(rtk_multi->rtk_out, rtk_input_data->obsd, rtk_input_data->n_obsd, rtk_input_data->nav);
}

static void rtk_multi_step(rtk_multi_t *rtk_multi,
                           const rtk_input_data_t *rtk_input_data)
{
    int i;
    rtk_hypothesis_t *hypothesis;

    for (i = 0; i < MAX_RTK_HYPOTHESES; i++) {
        
        hypothesis = rtk_multi->hypotheses[i];
        if ( !hypothesis->is_active ) continue;

        rtk_hypothesis_step(hypothesis, rtk_input_data);
    }
    
}

static void rtk_multi_update_base_pos(rtk_multi_t * rtk_multi)
{
    int i;
    rtk_hypothesis_t *hypothesis;

    for (i = 0; i < MAX_RTK_HYPOTHESES; i++ ) {
        
        hypothesis = rtk_multi->hypotheses[i];
        if ( !hypothesis->is_active ) continue;
        
        memcpy(hypothesis->rtk->opt.rb, rtk_multi->opt.rb, sizeof(double) * 3);
        memcpy(hypothesis->rtk->rb, rtk_multi->opt.rb, sizeof(double) * 3);
    }
}

extern void rtk_multi_process(rtk_multi_t *rtk_multi,
                              const rtk_multi_strategy_t *rtk_multi_strategy, 
                              const rtk_input_data_t *rtk_input_data)
{   
    unsigned int tick = tickget();
    
    assert( rtk_multi_is_valid(rtk_multi) );
    assert( rtk_multi_strategy_is_valid(rtk_multi_strategy) );
    assert( rtk_input_data_is_valid(rtk_input_data) );
    
    trace(2, "rtk_multi_process\n");
    
    rtk_multi_update_base_pos(rtk_multi);
    
    tick = tickget();
    
    rtk_multi_strategy->split(rtk_multi, rtk_input_data);
    
    trace(2, "rtk_multi_process: split, %d [ms]\n", (int) (tickget() - tick));
    tick = tickget();
    
    rtk_multi_step(rtk_multi, rtk_input_data);
    
    trace(2, "rtk_multi_process: step, %d [ms]\n", (int) (tickget() - tick));
    tick = tickget();

    rtk_multi_strategy->qualify(rtk_multi);
    
    trace(2, "rtk_multi_process: qualify, %d [ms]\n", (int) (tickget() - tick));
    tick = tickget();
    
    rtk_multi_strategy->merge(rtk_multi);
    
    trace(2, "rtk_multi_process: merge, %d [ms]\n", (int) (tickget() - tick));
    tick = tickget();
    
    if ( alter_fix_out_counter > 0 ) {
        
        rtk_multi->rtk_out->sol.ratio = ALTER_FIX_CODE;
        alter_fix_out_counter--;
    }
    if ( large_res_out_counter > 0 ) {
        
        rtk_multi->rtk_out->sol.ratio = LARGE_RES_CODE;
        large_res_out_counter--;
    }
}
