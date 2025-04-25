#include "Mesh.h"

std::istream& Mesh::LoadPositions(std::istream& in)
{
	size_t numVertices;
	in >> numVertices;

	vertexPositions.clear();
	vertexPositions.reserve(numVertices);

	for (int i = 0; i < numVertices; i++)
	{
		VertexPosition vertex;
		in >> vertex.x >> vertex.y >> vertex.z;
		vertexPositions.push_back(vertex);
	}

	return in;
}

std::istream& Mesh::LoadNormals(std::istream& in)
{
	size_t numVertices;
	in >> numVertices;

	vertexNormals.clear();
	vertexNormals.reserve(numVertices);

	for (int i = 0; i < numVertices; i++)
	{
		unsigned int vertexIndex;
		VertexNormal vertex;
		in >> vertexIndex >> vertex.nx >> vertex.ny >> vertex.nz;
		vertex.positionPtr = &vertexPositions[vertexIndex];

		vertexNormals.push_back(vertex);
	}

	return in;
}

std::istream& Mesh::LoadFaces(std::istream& in)
{
	size_t numFaces;
	in >> numFaces;

	faces.clear();
	faces.reserve(numFaces);

	for (int i = 0; i < numFaces; i++)
	{
		Face face;
		in >> face.vertexIdx1 >> face.vertexIdx2 >> face.vertexIdx3;
		faces.push_back(face);
	}
	return in;
}

std::istream& Mesh::LoadEdges(std::istream& in)
{
	size_t numEdges;
	in >> numEdges;

	edges.clear();
	edges.reserve(numEdges);

	for (int i = 0; i < numEdges; i++)
	{
		Edge edge;
		unsigned int vertexIndex1, vertexIndex2;
		unsigned int faceIndex1, faceIndex2;
		in >> vertexIndex1 >> vertexIndex2;
		in >> faceIndex1 >> faceIndex2;
		edge.vertexIndexPtr1 = &vertexPositions[vertexIndex1];
		edge.vertexIndexPtr2 = &vertexPositions[vertexIndex2];
		edge.faceIndexPtr1 = faces[faceIndex1];
		edge.faceIndexPtr2 = faces[faceIndex2];
		edges.push_back(edge);
	}

	return in;
}
