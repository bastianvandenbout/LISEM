//
// Created by Nezha on 2016/12/15.
//

/**
* REAMME *
* @author: ZJ Jiang (Nezha)
* @github: https://github.com/CallmeNezha/SimpleDBSCAN
* @describe: This is a simple DBSCAN clustering method implement
*/

// Option brute force or use kdtree(by default)
#define BRUTEFORCE false

#ifndef __DBSCAN_H__
#define __DBSCAN_H__

#include <vector>
#include <queue>
#include <set>
#include <memory>
#include <functional>


#ifdef __cplusplus
extern "C" {
#endif

struct kdhyperrect {
    int dim;
    double *min, *max;              /* minimum/maximum coords */
};

struct kdnode {
    double *pos;
    int dir;
    void *data;

    struct kdnode *left, *right;    /* negative/positive side */
};

struct res_node {
    struct kdnode *item;
    double dist_sq;
    struct res_node *next;
};

struct kdtree {
    int dim;
    struct kdnode *root;
    struct kdhyperrect *rect;
    void (*destr)(void*);
};

struct kdres {
    struct kdtree *tree;
    struct res_node *rlist, *riter;
    int size;
};


/* create a kd-tree for "k"-dimensional data */
struct kdtree *kd_create(int k);

/* free the struct kdtree */
void kd_free(struct kdtree *tree);

/* remove all the elements from the tree */
void kd_clear(struct kdtree *tree);

/* if called with non-null 2nd argument, the function provided
 * will be called on data pointers (see kd_insert) when nodes
 * are to be removed from the tree.
 */
void kd_data_destructor(struct kdtree *tree, void (*destr)(void*));

/* insert a node, specifying its position, and optional data */
int kd_insert(struct kdtree *tree, const double *pos, void *data);

/* Find the nearest node from a given point.
 *
 * This function returns a pointer to a result set with at most one element.
 */
struct kdres *kd_nearest(struct kdtree *tree, const double *pos);

/* Find the N nearest nodes from a given point.
 *
 * This function returns a pointer to a result set, with at most N elements,
 * which can be manipulated with the kd_res_* functions.
 * The returned pointer can be null as an indication of an error. Otherwise
 * a valid result set is always returned which may contain 0 or more elements.
 * The result set must be deallocated with kd_res_free after use.
 */
/*
struct kdres *kd_nearest_n(struct kdtree *tree, const double *pos, int num);
struct kdres *kd_nearest_nf(struct kdtree *tree, const float *pos, int num);
struct kdres *kd_nearest_n3(struct kdtree *tree, double x, double y, double z);
struct kdres *kd_nearest_n3f(struct kdtree *tree, float x, float y, float z);
*/

/* Find any nearest nodes from a given point within a range.
 *
 * This function returns a pointer to a result set, which can be manipulated
 * by the kd_res_* functions.
 * The returned pointer can be null as an indication of an error. Otherwise
 * a valid result set is always returned which may contain 0 or more elements.
 * The result set must be deallocated with kd_res_free after use.
 */
struct kdres *kd_nearest_range(struct kdtree *tree, const double *pos, double range);

/* frees a result set returned by kd_nearest_range() */
void kd_res_free(struct kdres *set);

/* returns the size of the result set (in elements) */
int kd_res_size(struct kdres *set);

/* rewinds the result set iterator */
void kd_res_rewind(struct kdres *set);

/* returns non-zero if the set iterator reached the end after the last element */
int kd_res_end(struct kdres *set);

/* advances the result set iterator, returns non-zero on success, zero if
 * there are no more elements in the result set.
 */
int kd_res_next(struct kdres *set);

/* returns the data pointer (can be null) of the current result set item
 * and optionally sets its position to the pointers(s) if not null.
 */
void *kd_res_item(struct kdres *set, double *pos);

/* equivalent to kd_res_item(set, 0) */
void *kd_res_item_data(struct kdres *set);


#ifdef __cplusplus
}
#endif

typedef unsigned int uint;


//! type T must be a vector-like container and MUST SUPPORT operator[] for iteration
//! Float can be float ,double ,int or any other number type but MUST SUPPORT implicitly convert to double type
template<typename T, typename Float>
class DBSCAN final {



    enum ERROR_TYPE {
        SUCCESS = 0
        , FAILED
        , COUNT
    };

    using TVector      = std::vector<T>;
    using DistanceFunc = std::function<Float(const T&, const T&)>;

public:
    DBSCAN() { }
    ~DBSCAN() { }

    /**
    * @describe: Run DBSCAN clustering alogrithm
    * @param: V {std::vector<T>} : data
    * @param: dim {unsigned int} : dimension of T (a vector-like struct)
    * @param: eps {Float} : epsilon or in other words, radian
    * @param: min {unsigned int} : minimal number of points in epsilon radian, then the point is cluster core point
    * @param: disfunc {DistanceFunc} : !!!! only used in bruteforce mode. Distance function recall. Euclidian distance is recommanded, but you can replace it by any metric measurement function
    * @usage: Object.Run() and get the cluster and noise indices from this->Clusters & this->Noise.
    * @pitfall: If you set big eps(search range) and huge density V, then kdtree will be a bottleneck of performance
    * @pitfall: You MUST ensure the data's identicality (TVector* V) during Run(), because DBSCAN just use the reference of data passed in.
    * @TODO: customize kdtree algorithm or rewrite it ,stop further searching when minimal number which indicates cluster core point condition is satisfied
    */
    int Run(TVector* V, const uint dim, const Float eps, const uint min, const DistanceFunc& disfunc = [](const T& t1, const T& t2)->Float { return 0; });


private:
    std::vector<uint> regionQuery(const uint pid) const;
    void              addToCluster(const uint pid, const uint cid);
    void              expandCluster(const uint cid, const std::vector<uint>& neighbors);
    void              addToBorderSet(const uint pid) { this->_borderset.insert(pid); }
    void              addToBorderSet(const std::vector<uint>& pids) { for (uint pid : pids) this->_borderset.insert(pid); }
    bool              isInBorderSet(const uint pid) const { return this->_borderset.end() != this->_borderset.find(pid); }
    void              buildKdtree(const TVector* V);
    void              destroyKdtree();

public:
    std::vector<std::vector<uint>>  Clusters;
    std::vector<uint>               Noise;

private:
    //temporary variables used during computation
    std::vector<bool>   _visited;
    std::vector<bool>   _assigned;
    std::set<uint>      _borderset;
    uint                _datalen;
    uint                _minpts;
    Float               _epsilon;
    uint                _datadim;

    DistanceFunc        _disfunc;
#if !BRUTEFORCE
    kdtree*             _kdtree;
#endif //!BRUTEFORCE

    std::vector<T>*     _data;  //Not owner, just holder, no responsible for deallocate


};

template<typename T, typename Float>
int DBSCAN<T, Float>::Run(
    TVector*                V
    , const uint            dim
    , const Float           eps
    , const uint            min
    , const DistanceFunc&   disfunc
) {

    // Validate
    if (V->size() < 1) return ERROR_TYPE::FAILED;
    if (dim < 1) return ERROR_TYPE::FAILED;
    if (min < 1) return ERROR_TYPE::FAILED;

    // initialization
    this->_datalen = (uint)V->size();
    this->_visited = std::vector<bool>(this->_datalen, false);
    this->_assigned = std::vector<bool>(this->_datalen, false);
    this->Clusters.clear();
    this->Noise.clear();
    this->_minpts = min;
    this->_data = V;
    this->_disfunc = disfunc;
    this->_epsilon = eps;
    this->_datadim = dim;

#if BRUTEFORCE
#else
this->buildKdtree(this->_data);
#endif // !BRUTEFORCE


    for (uint pid = 0; pid < this->_datalen; ++pid) {
        // Check if point forms a cluster
        this->_borderset.clear();
        if (!this->_visited[pid]) {
            this->_visited[pid] = true;

            // Outliner it maybe noise or on the border of one cluster.
            const std::vector<uint> neightbors = this->regionQuery(pid);
            if (neightbors.size() < this->_minpts) {
                continue;
            }
            else {
                uint cid = (uint)this->Clusters.size();
                this->Clusters.push_back(std::vector<uint>());
                // first blood
                this->addToBorderSet(pid);
                this->addToCluster(pid, cid);
                this->expandCluster(cid, neightbors);
            }
        }
    }

    for (uint pid = 0; pid < this->_datalen; ++pid) {
        if (!this->_assigned[pid]) {
            this->Noise.push_back(pid);
        }
    }

#if BRUTEFORCE
#else
    this->destroyKdtree();
#endif // !BRUTEFORCE

    return ERROR_TYPE::SUCCESS;

}




template<typename T, typename Float>
void DBSCAN<T, Float>::destroyKdtree() {
    kd_free(this->_kdtree);
}

template<typename T, typename Float>
void DBSCAN<T, Float>::buildKdtree(const TVector* V)
{
    this->_kdtree = kd_create((int)this->_datadim);
    std::unique_ptr<double[]> v(new double[this->_datadim]);
    for (uint r = 0; r < this->_datalen; ++r) {
        // kdtree only support double type
        for (uint c = 0; c < this->_datadim; ++c) {
            v[c] = (double)(*V)[r][c];
        }
        kd_insert(this->_kdtree, v.get(), (void*)&(*V)[r]);
    }
}

template<typename T, typename Float>
std::vector<uint> DBSCAN<T, Float>::regionQuery(const uint pid) const {

    std::vector<uint> neighbors;

#if BRUTEFORCE //brute force  O(n^2)
    for (uint i = 0; i < this->_data->size(); ++i)
        if (i != pid &&  this->_disfunc((*this->_data)[pid], (*this->_data)[i]) < this->_epsilon)
            neighbors.push_back(i);
#else //kdtree
    std::unique_ptr<double[]> v(new double[this->_datadim]);
    for (uint c = 0; c < this->_datadim; ++c) {
        v[c] = (double)((*this->_data)[pid][c]);
    }

    kdres* presults = kd_nearest_range(this->_kdtree, v.get(), this->_epsilon);
    while (!kd_res_end(presults)) {
        /* get the data and position of the current result item */
        T* pch = (T*)kd_res_item(presults, v.get());
        uint pnpid = (uint)(pch - &(*this->_data)[0]);
        if(pid != pnpid) neighbors.push_back(pnpid);
        /* go to the next entry */
        kd_res_next(presults);
    }
    kd_res_free(presults);

#endif // !BRUTEFORCE

    return neighbors;
}

template<typename T, typename Float>
void DBSCAN<T, Float>::expandCluster(const uint cid, const std::vector<uint>& neighbors) {

    std::queue<uint> border; // it has unvisited , visited unassigned pts. visited assigned will not appear
    for (uint pid : neighbors) border.push(pid);
    this->addToBorderSet(neighbors);

    while(border.size() > 0) {
        const uint pid = border.front(); border.pop();

        if (!this->_visited[pid]) {

            // not been visited, great! , hurry to mark it visited
            this->_visited[pid] = true;
            const std::vector<uint> pidneighbors = this->regionQuery(pid);

            // Core point, the neighbors will be expanded
            if (pidneighbors.size() >= this->_minpts) {
                this->addToCluster(pid, cid);
                for (uint pidnid : pidneighbors) {
                    if (!this->isInBorderSet(pidnid)) {
                        border.push(pidnid);
                        this->addToBorderSet(pidnid);
                    }
                }
            }
        }
    }

}

template<typename T, typename Float>
void DBSCAN<T, Float>::addToCluster(const uint pid, const uint cid) {
    this->Clusters[cid].push_back(pid);
    this->_assigned[pid] = true;
}





#endif //__DBSCAN_H__
