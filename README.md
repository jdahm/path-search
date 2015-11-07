# Homework 4

## Goal

Return the shortest path through all vertexes. This will be computed
with the following function:

    path = Hamiltonian_path<Comp>(vertex_set)

## Object model

### vertex_path

Paths are referred to by a ordered list of vertex references. This is
loosely what a path is:

    typedef std::deque<vertex_ref_type> path_type

Paths should additionally define:

* `npoint()` : number of points in path
* `length()` : length of path
* `push_back(i)`  : add vertex to path
* `pop_back()` : remove last element in path
* any Comp operator that can be passed to `Hamiltonian_path`

### vertex_set

Container that holds a set of vertexes. Vertexes must be able to be
referenced in some way and thus this object must define:

* `npoint()`
* `distance(i,j)`

constructors:

* `vertex_set(square_symmetric_matrix)`

### square_symmetric_matrix

What it says... Should define:

* `size()` : returns the number of rows
* `operator()(i,j)` : returns entry

Is allowed to do some memory optimizations to reduce storage if it
wants.

### Hamiltonian_path

Returns `path` that satisfies the comparison operator.

Inside the function uses shared-memory parallelism, utilizing
`OMP_NUM_THREADS` number of threads.

The most trivial implementation of this function is

    def Hamiltonian_path(vs)
        # construct a search_tree object starting at vertex 0
        st = search_tree(vs);
        # find the shortest path in st using dfs
        return st.find_path();

This is inherently serial, since `st` exists on a single thread. A
better approach is to use BFS first to populate a number of sub-search
trees then call st.shortest_path() on each of these in parallel.

### Hsearch_tree

This loosely wraps a path object, inducing a Hamiltonian-esque
search-tree through all vertexes. This object must define publically:

* `find_path(path)` : shortest path through all vertexes (use dfs)
* `paths = chop(n, path)` : returns a number of sub-Hsearch trees rooted at level n

### tree

In order to implement `shortest_path` and `chop`, the Hsearch_tree
must have certain private functions to operate on a tree. This should
be the minimal set needed for a non-binary tree:

* `my_index()` : self child number
* `root_size()` : size of the root
* `size()` : total size of the tree
* `dequeue()` : go to parent
* `enqueue(i)` : go to child i
* `num_children()` : returns the number of children in the tree

from these we can implement a number of _nice_ methods for
`Hsearch_tree` to use

* `is_root()` : returns true if the active location is at the root
* `is_leaf()` : returns true if the active location is at a leaf


## Todo

Overall task: Find best path through tree

Sub-task: Find better complete path through tree than bound

Manager: gives work to workers

Worker: uses strategy


Task: `Search`
Find best path through tree
Result: best path
Defines: `done`, `get_more_info`, `split`, `answer`
Constructor: takes `search_tree`, and a bound


`find_Hamiltonian_path`

search collection of tasks (queue, priority queue, etc.)

initialize a `search` at the top of the tree

loop {
try to get lock
when (got lock) {
pop task
release lock
do task
}
}


path: 0 1 6 2 5 3 4 7 weight: 29.9379


* Remove if statements in next_branch() and check about reducing the
logic there
* Switch manager queue to FILO std::stack
* Look into optimizing the split() and manager/queue calls
