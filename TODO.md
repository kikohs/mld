# Framework
## SuperNode

Add a bool field for root nodes in SuperNodes

- Add field: m_root
method: isRoot()
method: setAsRoot()

## XCoarsener

- Change xselector algorithm
- Flag root nodes node.setAsRoot()
- Reduction factor for minimum number of nodes per layer

## Diffusion

- Create diffuser

# Tests
- XCoarsener test

# Tools
- Add TCLAP for command line parsing
- Create graphParser tool using EdgeTypeLoader CSV format
- Create Coarsener tool
- Create Explorer tool
- Create benchmark tool which compares BFS, Dijkstra with subgraph from MLD

