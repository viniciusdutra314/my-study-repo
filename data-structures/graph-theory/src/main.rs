use std::collections::LinkedList;
use std::marker::PhantomData;

struct MyGraph<N, E, Ty, Idx> {
    adjlist: Vec<LinkedList<Idx>>,
    node_weights: Vec<N>,
    edges_weights: Vec<E>,
    edge_count: usize,
    _marker: PhantomData<(N, E, Ty)>,
}

impl<N, E, Ty, Idx> petgraph::visit::GraphBase for MyGraph<N, E, Ty, Idx>
where
    Idx: Copy + PartialEq,
{
    type NodeId = Idx;
    type EdgeId = Idx;
}

impl<N, E, Ty, Idx> petgraph::visit::NodeCount for MyGraph<N, E, Ty, Idx>
where
    Idx: petgraph::graph::IndexType,
{
    fn node_count(self: &Self) -> usize {
        self.adjlist.len()
    }
}

impl<N, E, Ty, Idx> petgraph::visit::EdgeCount for MyGraph<N, E, Ty, Idx>
where
    Idx: petgraph::graph::IndexType,
{
    fn edge_count(self: &Self) -> usize {
        self.edge_count
    }
}
impl<N, E, Ty, Idx> petgraph::data::Build for MyGraph<N, E, Ty, Idx>
where
    Idx: petgraph::graph::IndexType,
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
        self.edge_count += 1;
        edge_idx
    }
}

impl<N, E, Ty, Idx> petgraph::visit::Data for MyGraph<N, E, Ty, Idx>
where
    Idx: petgraph::graph::IndexType,
{
    type NodeWeight = N;
    type EdgeWeight = E;
}
impl<'a, N, E, Ty, Idx> petgraph::visit::IntoNeighbors for &'a MyGraph<N, E, Ty, Idx>
where
    Idx: petgraph::graph::IndexType,
{
    type Neighbors = std::iter::Copied<std::collections::linked_list::Iter<'a, Idx>>;
    fn neighbors(self, a: Self::NodeId) -> Self::Neighbors {
        self.adjlist[a.index()].iter().copied()
    }
}

impl<N, E, Ty, Idx> petgraph::visit::NodeIndexable for MyGraph<N, E, Ty, Idx>
where
    Idx: petgraph::graph::IndexType,
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

impl<N, E, Ty, Idx> petgraph::visit::Visitable for MyGraph<N, E, Ty, Idx>
where
    Idx: petgraph::graph::IndexType,
{
    type Map = fixedbitset::FixedBitSet;

    fn visit_map(&self) -> Self::Map {
        use petgraph::visit::NodeCount;
        fixedbitset::FixedBitSet::with_capacity(self.node_count())
    }

    fn reset_map(&self, map: &mut Self::Map) {
        map.clear();
    }
}

fn main() {
    println!("Hello, world!");
}
