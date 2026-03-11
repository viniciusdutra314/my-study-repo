use petgraph::visit::Dfs;
use petgraph::{
    Directed, EdgeType, csr::DefaultIx, data::Build, data::DataMap, graph::IndexType, visit::Bfs,
    visit::Data, visit::EdgeCount, visit::GraphBase, visit::IntoNeighbors, visit::NodeCount,
    visit::NodeIndexable, visit::Visitable,
};
use std::collections::LinkedList;
use std::marker::PhantomData;

struct MyGraph<N = (), E = (), Ty = Directed, Idx = DefaultIx>
where
    Ty: EdgeType,
    Idx: IndexType,
{
    adjlist: Vec<LinkedList<Idx>>,
    node_weights: Vec<N>,
    edges_weights: Vec<E>,
    _marker: PhantomData<Ty>,
}

impl<N, E, Ty, Idx> Default for MyGraph<N, E, Ty, Idx>
where
    Ty: EdgeType,
    Idx: IndexType,
{
    fn default() -> Self {
        Self {
            adjlist: Vec::new(),
            node_weights: Vec::new(),
            edges_weights: Vec::new(),
            _marker: PhantomData,
        }
    }
}

impl<N, E, Ty, Idx> GraphBase for MyGraph<N, E, Ty, Idx>
where
    Idx: IndexType,
    Ty: EdgeType,
{
    type NodeId = Idx;
    type EdgeId = Idx;
}

impl<N, E, Ty, Idx> NodeCount for MyGraph<N, E, Ty, Idx>
where
    Idx: IndexType,
    Ty: EdgeType,
{
    fn node_count(self: &Self) -> usize {
        self.adjlist.len()
    }
}

impl<N, E, Ty, Idx> EdgeCount for MyGraph<N, E, Ty, Idx>
where
    Idx: IndexType,
    Ty: EdgeType,
{
    fn edge_count(self: &Self) -> usize {
        self.edges_weights.len()
    }
}
impl<N, E, Ty, Idx> Build for MyGraph<N, E, Ty, Idx>
where
    Idx: IndexType,
    Ty: EdgeType,
{
    fn add_node(&mut self, weight: Self::NodeWeight) -> Self::NodeId {
        let id = Idx::new(self.adjlist.len());
        self.adjlist.push(LinkedList::new());
        self.node_weights.push(weight);
        id
    }

    fn update_edge(
        &mut self,
        a: Self::NodeId,
        b: Self::NodeId,
        weight: Self::EdgeWeight,
    ) -> Self::EdgeId {
        let mut global_edge_idx = 0;
        for i in 0..a.index() {
            global_edge_idx += self.adjlist[i].len();
        }

        for (pos, &neighbor) in self.adjlist[a.index()].iter().enumerate() {
            if neighbor == b {
                let edge_idx = Idx::new(global_edge_idx + pos);
                self.edges_weights[edge_idx.index()] = weight;
                return edge_idx;
            }
        }

        self.adjlist[a.index()].push_back(b);
        let edge_idx = Idx::new(self.edges_weights.len());
        self.edges_weights.push(weight);
        edge_idx
    }
}

impl<N, E, Ty, Idx> Data for MyGraph<N, E, Ty, Idx>
where
    Idx: IndexType,
    Ty: EdgeType,
{
    type NodeWeight = N;
    type EdgeWeight = E;
}

impl<N, E, Ty, Idx> DataMap for MyGraph<N, E, Ty, Idx>
where
    Idx: IndexType,
    Ty: EdgeType,
{
    fn node_weight(self: &Self, id: Self::NodeId) -> Option<&Self::NodeWeight> {
        self.node_weights.get(id.index())
    }
    fn edge_weight(self: &Self, id: Self::EdgeId) -> Option<&Self::EdgeWeight> {
        self.edges_weights.get(id.index())
    }
}

impl<'a, N, E, Ty, Idx> IntoNeighbors for &'a MyGraph<N, E, Ty, Idx>
where
    Idx: IndexType,
    Ty: EdgeType,
{
    type Neighbors = std::iter::Copied<std::collections::linked_list::Iter<'a, Idx>>;
    fn neighbors(self, a: Self::NodeId) -> Self::Neighbors {
        self.adjlist[a.index()].iter().copied()
    }
}

impl<N, E, Ty, Idx> NodeIndexable for MyGraph<N, E, Ty, Idx>
where
    Idx: IndexType,
    Ty: EdgeType,
{
    fn node_bound(self: &Self) -> usize {
        self.adjlist.len()
    }
    fn to_index(self: &Self, a: Self::NodeId) -> usize {
        a.index()
    }
    fn from_index(self: &Self, i: usize) -> Self::NodeId {
        Self::NodeId::new(i)
    }
}

impl<N, E, Ty, Idx> Visitable for MyGraph<N, E, Ty, Idx>
where
    Idx: IndexType,
    Ty: EdgeType,
{
    type Map = fixedbitset::FixedBitSet;

    fn visit_map(&self) -> Self::Map {
        fixedbitset::FixedBitSet::with_capacity(self.node_count())
    }

    fn reset_map(&self, map: &mut Self::Map) {
        map.clear();
    }
}

fn main() {
    let mut graph = MyGraph::<i32, i32>::default();
    let a = graph.add_node(2);
    let b = graph.add_node(4);
    let c = graph.add_node(8);
    let d = graph.add_node(16);

    graph.update_edge(a, b, 10);
    graph.update_edge(a, c, 20);
    graph.update_edge(b, d, 30);
    graph.update_edge(c, d, 40);
    println!(
        "    /*
     *  (a:2) --10--> (b:4)
     *    |             |
     *   20            30
     *    v             v
     *  (c:8) --40--> (d:16)
     */"
    );
    println!("BFS from (a)");
    let mut bfs = Bfs::new(&graph, a);
    while let Some(node) = bfs.next(&graph) {
        println!("node weight: {}", graph.node_weight(node).unwrap());
    }
    println!("DFS from (a)");
    let mut dfs = Dfs::new(&graph, a);
    while let Some(node) = dfs.next(&graph) {
        println!("node weight: {}", graph.node_weight(node).unwrap());
    }
}
