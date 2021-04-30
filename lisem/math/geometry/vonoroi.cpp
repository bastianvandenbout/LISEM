#include "vonoroi.h"



void jcv_diagram_generate( int num_points, const jcv_point* points, const jcv_rect* rect, jcv_diagram* d )
{
    jcv_diagram_generate_useralloc(num_points, points, rect, 0, jcv_alloc_fn, jcv_free_fn, d);
}


void jcv_diagram_generate_useralloc( int num_points, const jcv_point* points, const jcv_rect* rect, void* userallocctx, FJCVAllocFn allocfn, FJCVFreeFn freefn, jcv_diagram* d )
{
    if( d->internal )
        jcv_diagram_free( d );

    int max_num_events = num_points*2; // beachline can have max 2*n-5 parabolas
    size_t sitessize = (size_t)num_points * sizeof(jcv_site);
    size_t memsize = 8u + (size_t)max_num_events * sizeof(void*) + sizeof(jcv_priorityqueue) + sitessize + sizeof(jcv_context_internal);

    char* originalmem = (char*)allocfn(userallocctx, memsize);
    memset(originalmem, 0, memsize);

    // align memory
    char* mem = originalmem + 8 - ( (size_t)(originalmem) & 0x7);

    jcv_context_internal* internal = (jcv_context_internal*)mem;
    mem += sizeof(jcv_context_internal);

    internal->mem    = originalmem;
    internal->memctx = userallocctx;
    internal->alloc  = allocfn;
    internal->free   = freefn;

    internal->beachline_start = jcv_halfedge_new(internal, 0, 0);
    internal->beachline_end = jcv_halfedge_new(internal, 0, 0);

    internal->beachline_start->left     = 0;
    internal->beachline_start->right    = internal->beachline_end;
    internal->beachline_end->left       = internal->beachline_start;
    internal->beachline_end->right  = 0;

    internal->last_inserted = 0;

    internal->sites = (jcv_site*) mem;
    mem += sitessize;

    internal->eventqueue = (jcv_priorityqueue*)mem;
    mem += sizeof(jcv_priorityqueue);

    jcv_cast_align_struct tmp;
    tmp.charp = mem;
    internal->eventmem = tmp.voidpp;

    if( rect == 0 )
    {
        _jcv_calc_bounds(num_points, points, &d->min, &d->max);
        d->min.x -= 10;
        d->min.y -= 10;
        d->max.x += 10;
        d->max.y += 10;
    }
    else
    {
        d->min = rect->min;
        d->max = rect->max;
    }
    internal->min = d->min;
    internal->max = d->max;

    jcv_pq_create(internal->eventqueue, max_num_events, (void**)internal->eventmem);

    jcv_site* sites = internal->sites;

    for( int i = 0; i < num_points; ++i )
    {
        sites[i].p        = points[i];
        sites[i].edges    = 0;
        sites[i].index    = i;
    }

    qsort(sites, (size_t)num_points, sizeof(jcv_site), jcv_point_cmp);

    int offset = 0;
    for (int i = 0; i < num_points; i++)
    {
        const jcv_site* s = &sites[i];
        // Remove duplicates, to avoid anomalies
        if( i > 0 && jcv_point_eq(&s->p, &sites[i - 1].p) )
        {
            offset++;
            continue;
        }
        // Remove points outside of the bounding box to avoid bad edge clipping later on
        if( rect != 0 )
        {
            if( s->p.x < rect->min.x || s->p.x > rect->max.x ||
                s->p.y < rect->min.y || s->p.y > rect->max.y )
            {
                offset++;
                continue;
            }
        }

        sites[i - offset] = sites[i];
    }
    num_points -= offset;

    d->internal = internal;
    d->numsites = num_points;

    internal->numsites      = num_points;
    internal->numsites_sqrt = (int)(JCV_SQRT((jcv_real)num_points));
    internal->currentsite   = 0;

    internal->bottomsite = jcv_nextsite(internal);

    jcv_priorityqueue* pq = internal->eventqueue;
    jcv_site* site = jcv_nextsite(internal);

    int finished = 0;
    while( !finished )
    {
        jcv_point lowest_pq_point;
        if( !jcv_pq_empty(pq) )
        {
            jcv_halfedge* he = (jcv_halfedge*)jcv_pq_top(pq);
            lowest_pq_point.x = he->vertex.x;
            lowest_pq_point.y = he->y;
        }

        if( site != 0 && (jcv_pq_empty(pq) || jcv_point_less(&site->p, &lowest_pq_point) ) )
        {
            jcv_site_event(internal, site);
            site = jcv_nextsite(internal);
        }
        else if( !jcv_pq_empty(pq) )
        {
            jcv_circle_event(internal);
        }
        else
        {
            finished = 1;
        }
    }

    for( jcv_halfedge* he = internal->beachline_start->right; he != internal->beachline_end; he = he->right )
    {
        jcv_finishline(internal, he->edge);
    }

    jcv_fillgaps(d);
}


void jcv_diagram_free( jcv_diagram* d )
{
    jcv_context_internal* internal = d->internal;
    void* memctx = internal->memctx;
    FJCVFreeFn freefn = internal->free;
    while(internal->memblocks)
    {
        jcv_memoryblock* p = internal->memblocks;
        internal->memblocks = internal->memblocks->next;
        freefn( memctx, p );
    }

    freefn( memctx, internal->mem );
}



const jcv_site* jcv_diagram_get_sites( const jcv_diagram* diagram )
{
    return diagram->internal->sites;
}

const jcv_edge* jcv_diagram_get_edges( const jcv_diagram* diagram )
{
    return diagram->internal->edges;
}

const jcv_edge* jcv_diagram_get_next_edge( const jcv_edge* edge )
{
    const jcv_edge* e = edge->next;
    while (e != 0 && jcv_point_eq(&e->pos[0], &e->pos[1])) {
        e = e->next;
    }
    return e;
}

