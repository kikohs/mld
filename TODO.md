# Framework

## Db scheme
- Add MLG node -> layers
    - name
    - version

- Layer
    - description when coarsening occurs

## Operator
- Big refactoring
- Coarsener factory, Selector and Merger factory
- Unify Single and Multi derived class into 1
- One coarsener with 2 different options:
    - mirroring and second pass
    - firstpass and mirroring and second pass.

## Diffusion
- Create diffuser

## IO
- Export layers
- Export MLD

# Tools
- Create Explorer tool
- Create benchmark tool which compares BFS, Dijkstra with subgraph from MLD

