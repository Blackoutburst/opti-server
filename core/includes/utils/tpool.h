/*
https://nachtimwald.com/2019/04/12/thread-pool-in-c/
https://github.com/user-none/poddown/blob/master/src/tpool.c
*/

#ifndef __TPOOL_H__
#define __TPOOL_H__

#include <stdbool.h>
#include <stddef.h>

/*! \addtogroup thread_pool Thread Pool
 *
 * @{
 */

struct tpool;
typedef struct tpool tpool_t;

/*! Callback function that the pool will call to do work.
 *
 * \param[in,out] arg Argument.
 */
typedef void (*thread_func_t)(void *arg);

/* - - - - */

/*! Create a thread pool.
 *
 * \param[in] num Number of threads the pool should have.
 *                If 0 defaults to 2.
 *
 * \return pool.
 */
tpool_t *tpool_create(size_t num);

/*! Destory a thread pool
 *
 * The pool can be destroyed while there is outstanding work to process.  All
 * outstanding unprocessed work will be discareded. There may be a delay before
 * this function returns because it will block for work that is processing to
 * complete.
 *
 * \param[in,out] tp Thread pool.
 */
void tpool_destroy(tpool_t *tp);

/* - - - - */

/*! Add work to a thread pool.
 *
 * \param[in,out] tp   Thread pool.
 * \param[in]     func Function the pool should call.
 * \param[in,out] arg  Argument to pass to func.
 *
 * \return true if work was added, otherwise false.
 */
bool tpool_add_work(tpool_t *tp, thread_func_t func, void *arg);

/*! Wait for all work in the pool to be completed.
 *
 * \param[in,out] tp Thread pool.
 */
void tpool_wait(tpool_t *tp);

/*! @}
 */

#endif /* __TPOOL_H__ */
