#ifndef MULTIHYPOTHESIS_H
#define MULTIHYPOTHESIS_H

#include "rtklib.h"

#define MAX_RTK_HYPOTHESES 2       /* max number of hypotheses */
#define MAX_STATS_QUEUE    300     /* max number of epochs stored */

#define SQR(x) ((x) * (x))

/* ------------------------------------------------------------------------- */
/* basic types ------------------------------------------------------------- */

typedef struct {
    
    int    solution_status;
    double residuals_carrier[MAXSAT][NFREQ];
    int    carrier_fix_status[MAXSAT][NFREQ];   /* 0: not defined, 1: float, 2: fix, 3: hold */
    double position[3];
    
} rtk_stats_t;

typedef struct {

    int    length;                       /* in [0, MAX_STATS_QUEUE] */
    int    offset[MAX_STATS_QUEUE];      /* i-th element in the queue is stored in rtk[offset[i]] */
    rtk_stats_t *stats[MAX_STATS_QUEUE]; /* rtk structures (queue nodes) */
    int    target_solution_status;      /* SOLQ_FLOAT, SOLQ_FIX, ... */

} rtk_stats_queue_t;

typedef struct {

    rtk_t *rtk;                       /* current rtk state */
    rtk_stats_queue_t *stats_history; /* solution stats for several consequtive epochs */
    double solution_quality;          /* 0.0 is the best, negative value means 'is not defined' */
    int is_active;                    /* is hypothesis active or is not */
    int target_solution_status;
    
} rtk_hypothesis_t;

typedef struct {
    
    int      n_hypotheses;         /* number of active hypotheses */
    rtk_hypothesis_t *hypotheses[MAX_RTK_HYPOTHESES];
    int      index_main;           /* index of the main hypothesis (-1 means is not defined) */
    rtk_t   *rtk_out;              /* resulting rtk solution structure to output */
    prcopt_t opt;                  /* processing options */
    
} rtk_multi_t;

typedef struct {

    obsd_t *obsd;                   /* observation data */
    int     n_obsd;                 /* number of data records */
    nav_t  *nav;                    /* navigation data */
    
} rtk_input_data_t;

typedef void (*rtk_multi_split_fpt)   (rtk_multi_t *, const rtk_input_data_t *);
typedef void (*rtk_multi_qualify_fpt) (rtk_multi_t *);
typedef void (*rtk_multi_merge_fpt)   (rtk_multi_t *);

typedef struct {
    
    rtk_multi_split_fpt   split;
    rtk_multi_qualify_fpt qualify;
    rtk_multi_merge_fpt   merge;
    
} rtk_multi_strategy_t;

/* -------------------------------------------------------------------------- */
/* manipulation functions --------------------------------------------------- */

/* rtk_hypothesis */

extern rtk_hypothesis_t *rtk_hypothesis_init(prcopt_t opt);
extern int rtk_hypothesis_is_valid(const rtk_hypothesis_t *rtk_hypothesis);
extern void rtk_hypothesis_free(rtk_hypothesis_t *rtk_hypothesis);

extern void rtk_hypothesis_activate(rtk_hypothesis_t *rtk_hypothesis, const rtk_t *rtk);
extern void rtk_hypothesis_deactivate(rtk_hypothesis_t *rtk_hypothesis);
extern void rtk_hypothesis_reset(rtk_hypothesis_t *rtk_hypothesis, rtk_t *rtk);
extern rtk_stats_t *rtk_hypothesis_get_stats_pointer(const rtk_hypothesis_t *rtk_hypothesis, int index);
extern rtk_stats_t *rtk_hypothesis_get_stats_pointer_last(const rtk_hypothesis_t *rtk_hypothesis);
extern void rtk_hypothesis_copy_stats_history(const rtk_hypothesis_t *rtk_hypothesis_source,
                                              rtk_hypothesis_t *rtk_hypothesis_destination, int index);
extern void rtk_hypothesis_step(rtk_hypothesis_t *rtk_hypothesis,
                                const rtk_input_data_t *rtk_input_data);

/* rtk_input_data */

extern int rtk_input_data_is_valid(const rtk_input_data_t *rtk_input_data);

/* rtk_multi */

extern rtk_multi_t *rtk_multi_init(prcopt_t opt);
extern int rtk_multi_is_valid(const rtk_multi_t *rtk_multi);
extern void rtk_multi_free(rtk_multi_t *rtk_multi);

extern int rtk_multi_add(rtk_multi_t *rtk_multi, rtk_t *rtk);
extern void rtk_multi_exclude(rtk_multi_t *rtk_multi, int index);

extern void rtk_multi_estimate_main(rtk_multi_t *rtk_multi,
                                    const rtk_input_data_t *rtk_input_data);
extern void rtk_multi_process(rtk_multi_t *rtk_multi,
                              const rtk_multi_strategy_t *rtk_multi_strategy, 
                              const rtk_input_data_t *rtk_input_data);

/* rtk_multi_strategy */

extern int rtk_multi_strategy_is_valid(const rtk_multi_strategy_t *rtk_multi_strategy);

#endif /* #ifndef MULTIHYPOTHESIS_H */
