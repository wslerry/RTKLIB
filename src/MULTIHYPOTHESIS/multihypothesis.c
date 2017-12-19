#include "multihypothesis.h"

/* -------------------------------------------------------------------------- */
/* manipulation functions --------------------------------------------------- */

/* rtk_queue */

static rtk_queue_t *rtk_queue_init(prcopt_t opt)
{
    int i, j;
    rtk_queue_t *rtk_queue = malloc(sizeof(rtk_queue_t));
    if ( rtk_queue == NULL ) {

        return NULL;
    }
    
    rtk_queue->length = 0;

    for (i = 0; i < MAX_RTK_QUEUE; i++) {
        
        rtk_queue->offset[i] = i;
        rtk_queue->rtk[i] = rtk_init(&opt);
        if ( rtk_queue->rtk[i] == NULL ) {
            for (j = 0; j < i; j++) rtk_free(rtk_queue->rtk[j]);
            free(rtk_queue);
            return NULL;
        }
    }
    
    return rtk_queue;
}

static int rtk_queue_is_valid(const rtk_queue_t *rtk_queue)
{
    int i;
    int offsets_check[MAX_RTK_QUEUE] = {0};
    
    if ( rtk_queue == NULL ) {
        
        return 0;
    }
    if ( (rtk_queue->length < 0) || (rtk_queue->length > MAX_RTK_QUEUE) ) {
        
        return 0;
    }

    for (i = 0; i < MAX_RTK_QUEUE; i++) {   
        
        if ( !rtk_is_valid(rtk_queue->rtk[i]) ) {
            
            return 0;
        }
        if ( (rtk_queue->offset[i] < 0) || (rtk_queue->offset[i] >= MAX_RTK_QUEUE) ) {
            
            return 0;
        }
        offsets_check[rtk_queue->offset[i]] = 1;
    }
    
    for (i = 0; i < MAX_RTK_QUEUE; i++) {
        if ( offsets_check[i] == 0 ) {

            return 0;
        }
    }
    
    return 1;
}

static void rtk_queue_free(rtk_queue_t *rtk_queue)
{
    int i;
    assert( rtk_queue_is_valid(rtk_queue) );

    for (i = 0; i < MAX_RTK_QUEUE; i++) {
        if ( rtk_queue->rtk[i] != NULL ) rtk_free(rtk_queue->rtk[i]);
    }
    
    free(rtk_queue);
}

static int rtk_queue_is_index_valid(const rtk_queue_t *rtk_queue, int index)
{
    int is_index_in_bounds;
    assert( rtk_queue_is_valid(rtk_queue) );

    is_index_in_bounds = (index >= 0) && (index < MAX_RTK_QUEUE) && (index < rtk_queue->length);
    if ( !is_index_in_bounds ) {
        
        return 0;
    }
    
    return 1;
}

static void rtk_queue_cut(rtk_queue_t *rtk_queue, int index_cut)
{
    int i;
    int offset_cut[MAX_RTK_QUEUE];
    
    assert( rtk_queue_is_valid(rtk_queue) );
    assert( rtk_queue_is_index_valid(rtk_queue, index_cut) );
    
    if ( index_cut > 0 ) {
        for (i = 0; i < index_cut; i++ ) {
            offset_cut[i] = rtk_queue->offset[i];
        }
        for (i = 0; i < MAX_RTK_QUEUE - index_cut; i++ ) {
            rtk_queue->offset[i] = rtk_queue->offset[i + index_cut];
        }
        for (i = MAX_RTK_QUEUE - index_cut; i < MAX_RTK_QUEUE; i++ ) {
            rtk_queue->offset[i] = offset_cut[i - MAX_RTK_QUEUE + index_cut];
        }
        rtk_queue->length -= index_cut;
    }

}

static void rtk_queue_add(rtk_queue_t *rtk_queue, const rtk_t *rtk, int n_rtk)
{
    int i, offset;
    
    assert( rtk_queue_is_valid(rtk_queue) );
    assert( n_rtk >= 0 );
    
    for (i = 0; i < n_rtk; i++) {

        assert( rtk_is_valid(&rtk[i]) );

        /* add i-th rtk to the queue */
        if ( rtk_queue->length <= 0 ) {
            offset = rtk_queue->offset[0];
            rtk_queue->length = 1;
        }
        else if ( rtk_queue->length < MAX_RTK_QUEUE ) {
            offset = rtk_queue->offset[rtk_queue->length];
            rtk_queue->length++;
        }
        else { /* rtk_queue->length >= MAX_RTK_QUEUE */
            rtk_queue_cut(rtk_queue, 1);
            offset = rtk_queue->offset[MAX_RTK_QUEUE - 1];
            rtk_queue->length = MAX_RTK_QUEUE;
        }
        rtk_copy(&rtk[i], rtk_queue->rtk[offset]);
    }
    
}

static rtk_t *rtk_queue_get_pointer(const rtk_queue_t *rtk_queue, int index_head)
{
    int offset, index_tail;

    assert( rtk_queue_is_valid(rtk_queue) );
    
    index_tail = rtk_queue->length - 1 - index_head;
    assert( (index_tail >= 0) && (index_tail < MAX_RTK_QUEUE) && "index is out of bounds" );
    
    if ( !rtk_queue_is_index_valid(rtk_queue, index_tail) ) {
        
        return NULL;
    }

    offset = rtk_queue->offset[index_tail];

    return rtk_queue->rtk[offset];
}

/* rtk_history */

extern rtk_history_t *rtk_history_init(prcopt_t opt)
{
    rtk_history_t *rtk_history = malloc(sizeof(rtk_history_t));
    if ( rtk_history == NULL ) {

        return NULL;
    }

    rtk_history->target_solution_status = SOLQ_NONE;
    rtk_history->solution_quality = -1.0;
    rtk_history->history = rtk_queue_init(opt);
    if ( rtk_history->history == NULL ) {

        free(rtk_history);
        return NULL;
    }

    return rtk_history;
}

extern int rtk_history_is_valid(const rtk_history_t *rtk_history)
{
    int sol_stat;
    
    if ( rtk_history == NULL ) {

        return 0;
    }
    sol_stat = rtk_history->target_solution_status;
    if ( (sol_stat < 0) || (sol_stat > MAXSOLQ) ) {

        return 0;
    }
    if ( !rtk_queue_is_valid(rtk_history->history) ) {

        return 0;
    }

    return 1;
}

extern void rtk_history_free(rtk_history_t *rtk_history)
{
    assert(rtk_history_is_valid(rtk_history));

    rtk_queue_free(rtk_history->history);
    free(rtk_history);
}

extern void rtk_history_add(rtk_history_t *rtk_history, const rtk_t *rtk)
{
    assert( rtk_history_is_valid(rtk_history) );
    assert( rtk_is_valid(rtk) );

    rtk_queue_add(rtk_history->history, rtk, 1);
}

extern void rtk_history_cut(rtk_history_t *rtk_history, int index_cut)
{
    assert( rtk_history_is_valid(rtk_history) );

    rtk_queue_cut(rtk_history->history, index_cut);
}

extern rtk_t *rtk_history_get_pointer(const rtk_history_t *rtk_history, int index)
{
    assert( rtk_history_is_valid(rtk_history) );

    return rtk_queue_get_pointer(rtk_history->history, index);
}

extern rtk_t *rtk_history_get_pointer_to_last(const rtk_history_t *rtk_history)
{
    assert( rtk_history_is_valid(rtk_history) );

    return rtk_queue_get_pointer(rtk_history->history, 0);
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
    int i;
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
        
        rtk_multi->hypotheses[i] = NULL;
    }
    rtk_multi->n_hypotheses = 0;
    
    return rtk_multi;
}

extern int rtk_multi_is_valid(const rtk_multi_t *rtk_multi)
{
    int i, is_n_hypotheses_in_bounds;
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

    for (i = 0; i < MAX_RTK_HYPOTHESES; i++) {

        if ( rtk_multi->hypotheses[i] == NULL ) continue;
        if ( !rtk_history_is_valid(rtk_multi->hypotheses[i]) ) {

            return 0;
        }
        n_hypotheses++;
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

        if ( rtk_multi->hypotheses[i] != NULL ) {
            
            rtk_history_free(rtk_multi->hypotheses[i]);
        }
    }

    rtk_free(rtk_multi->rtk_out);
    free(rtk_multi);
}

extern int rtk_multi_add(rtk_multi_t *rtk_multi, rtk_history_t *hypothesis)
{
    int i;
    
    assert( rtk_multi_is_valid(rtk_multi) );
    assert( rtk_history_is_valid(hypothesis) );
    
    /* try to find a vacancy */
    for (i = 0; i < MAX_RTK_HYPOTHESES; i++) {

        if ( rtk_multi->hypotheses[i] == NULL ) break;
    }

    if ( i >= MAX_RTK_HYPOTHESES ) { /* no free space */

        return 0;
    }
    
    rtk_multi->hypotheses[i] = hypothesis;
    rtk_multi->n_hypotheses++;

    return 1;
} 

extern int rtk_multi_exclude(rtk_multi_t *rtk_multi, int index)
{
    int is_index_in_bounds = (index >= 0) && (index < MAX_RTK_HYPOTHESES);
    
    assert( rtk_multi_is_valid(rtk_multi) );
    assert( is_index_in_bounds );

    if ( rtk_multi->hypotheses[index] == NULL ) {

        return 0;
    }

    rtk_history_free(rtk_multi->hypotheses[index]);
    rtk_multi->hypotheses[index] = NULL;
    rtk_multi->n_hypotheses--;

    return 1;
}

extern void rtk_multi_estimate(rtk_multi_t *rtk_multi,
                               const rtk_multi_strategy_t *rtk_multi_strategy, 
                               const rtk_input_data_t *rtk_input_data)
{
    int i;
    rtk_t *rtk;
    rtk_t *rtk_last;
    rtk_history_t *hypothesis;
    
    assert( rtk_multi_is_valid(rtk_multi) );
    assert( rtk_multi_strategy_is_valid(rtk_multi_strategy) );
    assert( rtk_input_data_is_valid(rtk_input_data) );

    rtk_multi_strategy->split(rtk_multi, rtk_input_data);
    
    for (i = 0; i < MAX_RTK_HYPOTHESES; i++) {
        
        hypothesis = rtk_multi->hypotheses[i];
        if ( hypothesis == NULL ) continue;

        rtk_last = rtk_history_get_pointer_to_last(hypothesis);
        rtk = rtk_init(&rtk_last->opt);
        rtk_copy(rtk_last, rtk);
        rtkpos(rtk, rtk_input_data->obsd, rtk_input_data->n_obsd, rtk_input_data->nav);
        rtk_history_add(hypothesis, rtk);
        rtk_free(rtk);
    }
    
    rtk_multi_strategy->qualify(rtk_multi);
    
    rtk_multi_strategy->merge(rtk_multi);
}
