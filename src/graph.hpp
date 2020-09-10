// This file is part of Heimer.
// Copyright (C) 2018 Jussi Lind <jussi.lind@iki.fi>
//
// Heimer is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// Heimer is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Heimer. If not, see <http://www.gnu.org/licenses/>.

#ifndef GRAPH_HPP
#define GRAPH_HPP

#include "edge.hpp"
#include "node.hpp"

#include <map>
#include <set>

class Node;

class Graph
{
public:
    Graph();

    Graph(const Graph & other) = delete;

    Graph & operator=(const Graph & other) = delete;

    virtual ~Graph();

    using NodeVector = std::vector<NodePtr>;

    using EdgeVector = std::vector<EdgePtr>;

    void clear();

    void addNode(NodePtr node);

    void deleteNode(int index);

    void addEdge(EdgePtr edge);

    void deleteEdge(int index0, int index1);

    bool areDirectlyConnected(NodePtr node0, NodePtr node1);

#ifdef HEIMER_UNIT_TEST
    void addEdge(int node0, int node1);
#endif
    size_t numNodes() const;

    EdgeVector getEdgesFromNode(NodePtr node);

    EdgeVector getEdgesToNode(NodePtr node);

    const EdgeVector & getEdges() const;

    NodePtr getNode(int index);

    const NodeVector & getNodes() const;

    NodeVector getNodesConnectedToNode(NodePtr node);

private:
    NodeVector m_nodes;

    EdgeVector m_edges;

    int m_count = 0;
};

#endif // GRAPH_HPP
