# OsmRoutingEngine
<p align="center">
  <img width="500" height="350" src="https://www.gopetfriendly.com/blog/wp-content/uploads/2016/03/RT-Map.png">
</p>

OsmRoutingEngine utilizes [contraction hierarchies](https://en.wikipedia.org/wiki/Contraction_hierarchies "Contraction Hierarchies") in order to transform an OpenStreetMap (OSM) file into a graph that can be used for extremely fast routing. Originally proposed 
by Robert Geisberger in 2008, this routing technique is a simple and effective solution to the shortest path problem in large road networks. The contraction hierarchy routing 
algorithm consists of two phases: the pre-processing phase and the query phase.

During the pre-processing phase, we first order all nodes in the graph by importance. We then construct a hierarchy by iteratively contracting the nodes in order of least 
important to most important.  When a node is contracted, it is removed from the graph. If the removed node was on the shortest path of two of its neighbors, we add a shortcut edge 
that connects those two neighbors such that the shortest path between those neighbors is preserved. Otherwise, if there is another path connecting those neighbors that does not 
go through the removed vertex, a shortcut edge is not necessary. We call these paths witness paths, and the process of searching for these paths a witness search.

During the query phase, we run a bidirectional search that only considers increasingly important nodes. We run this search on the overlay graph, the graph that contains all the 
original nodes and edges in addition to the shortcut edges added during the contraction process. This results in much faster routing than traditional shortest path algorithms. 
On very large graphs, this bidirectional search is nearly 1000 times faster than a standard implementation of Dijkstra’s algorithm. Testing has revealed that querying a route 
takes less than one millisecond in most cases.

Note that OsmRoutingEngine is still in development. Bugs and other issues are expected. One can expect new features such as location lookup and python bindings to be added soon.

## References
* [Gesiberger, Robert. “Contraction Hierarchies: Faster and Simpler Hierarchical Routing in Road Networks.” Diploma Thesis at Institut Für Theoretische Informatik Universität Karlsruhe (TH), 1 July 2008.]( http://algo2.iti.kit.edu/schultes/hwy/contract.pdf "Diploma Thesis")
