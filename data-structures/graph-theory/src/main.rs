mod my_graph;
use crate::my_graph::MyGraph;
use petgraph::data::{Build, DataMap};
use petgraph::visit::{Bfs, Dfs};

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
