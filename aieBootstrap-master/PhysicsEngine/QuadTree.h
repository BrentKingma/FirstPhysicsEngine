#pragma once
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <vector>
#include <Renderer2D.h>

const int NODE_CAPACITY = 4;

struct Node
{
	glm::vec2 m_center;	

	Node(glm::vec2 a_center) : m_center(a_center) {}
};

class QuadTree
{
public:
	QuadTree()
	{
		m_Node = nullptr;
		m_northEast = nullptr;
		m_northWest = nullptr;
		m_southEast = nullptr;
		m_southWest = nullptr;

		m_topLeft = { 0.0f, 0.0f };
		m_bottomRight = { 0.0f, 0.0f };
	}
	QuadTree(glm::vec2 a_topLeft, glm::vec2 a_bottomRight) :m_topLeft(a_topLeft), m_bottomRight(a_bottomRight) 
	{
		m_Node = nullptr;
		m_northEast = nullptr;
		m_northWest = nullptr;
		m_southEast = nullptr;
		m_southWest = nullptr;
	}
	~QuadTree();

	/*void Subdivide()
	{
		glm::vec2 center = m_Node->m_center;
		float halfExtents = m_Node->m_halfExtents;
		m_northWest = new QuadTree(Node({ center.x + (halfExtents * 0.5f), center.y + (halfExtents * 0.5f) }, halfExtents * 0.5f));
		m_northEast = new QuadTree(Node({ center.x - (halfExtents * 0.5f), center.y + (halfExtents * 0.5f) }, halfExtents * 0.5f));
		m_southWest = new QuadTree(Node({ center.x + (halfExtents * 0.5f), center.y - (halfExtents * 0.5f) }, halfExtents * 0.5f));
		m_southEast = new QuadTree(Node({ center.x - (halfExtents * 0.5f), center.y - (halfExtents * 0.5f) }, halfExtents * 0.5f));
	}*/
	void Insert(Node* a_node)
	{
		if (a_node == nullptr)
		{
			return;
		}
		if (!InBoundary(&a_node->m_center))
		{
			return;
		}

		if (glm::abs(m_topLeft.x - m_bottomRight.x) <= 1 && glm::abs(m_topLeft.y - m_bottomRight.y) <= 1)
		{
			if (m_Node == nullptr)
			{
				m_Node = a_node;
			}
			return;
		}

		if ((m_topLeft.x + m_bottomRight.x) * 0.5f >= a_node->m_center.x)
		{
			//Indicates northEast
			if ((m_topLeft.y + m_bottomRight.y) * 0.5f >= a_node->m_center.y)
			{
				if (m_northEast == nullptr)
				{
					m_northEast = new QuadTree({ m_topLeft.x, m_topLeft.y }, { (m_topLeft.x + m_bottomRight.x) * 0.5f, (m_topLeft.y + m_bottomRight.y) * 0.5f });
					m_northEast->Insert(a_node);
				}
			}
			//Indicates southEast
			else
			{
				if (m_southEast == nullptr)
				{
					m_southEast = new QuadTree({ m_topLeft.x, (m_topLeft.y + m_bottomRight.y) * 0.5f }, { (m_topLeft.x + m_bottomRight.x) * 0.5f, m_bottomRight.y });
					m_southEast->Insert(a_node);
				}
			}
		}
		else
		{
			//Indicates northWest
			if ((m_topLeft.y + m_bottomRight.y) / 2 >= a_node->m_center.y)
			{
				if (m_northWest == nullptr)
				{
					m_northWest = new QuadTree({ (m_topLeft.x + m_bottomRight.x) / 2, m_topLeft.y }, { m_bottomRight.x, (m_topLeft.y + m_bottomRight.y) / 2 });
					m_northWest->Insert(a_node);
				}
			}
			//Indicates southWest
			else
			{
				if (m_southWest == nullptr)
				{
					m_southWest = new QuadTree({ (m_topLeft.x + m_bottomRight.x) * 0.5f,	(m_topLeft.y + m_bottomRight.y) * 0.5f }, { m_bottomRight.x, m_bottomRight.y });
					m_southWest->Insert(a_node);
				}
			}
		}

	}
	bool InBoundary(glm::vec2* a_point)
	{
		return (a_point->x >= m_topLeft.x && a_point->x <= m_bottomRight.x && a_point->y <= m_topLeft.y && a_point->y >= m_bottomRight.y);
	}
	Node* Search(glm::vec2 a_point)
	{
		if (!InBoundary(&a_point))
		{
			return nullptr;
		}

		if (m_Node != nullptr)
		{
			return m_Node;
		}

		if ((m_topLeft.x + m_bottomRight.x) * 0.5f >= a_point.x)
		{
			// Indicates northEast
			if ((m_topLeft.y + m_bottomRight.y) * 0.5f >= a_point.y)
			{
				if (m_northEast == nullptr)
				{
					return nullptr;
				}
				return m_northEast->Search(a_point);
			}
			// Indicates southEast
			else
			{
				if (m_southEast == nullptr)
					return nullptr;
				return m_southEast->Search(a_point);
			}
		}
		else
		{
			// Indicates northWest
			if ((m_topLeft.y + m_bottomRight.y) * 0.5f >= a_point.y)
			{
				if (m_northWest == nullptr)
					return nullptr;
				return m_northWest->Search(a_point);
			}

			// Indicates southWest
			else
			{
				if (m_southWest == nullptr)
					return nullptr;
				return m_southWest->Search(a_point);
			}
		}
	}
	void draw(aie::Renderer2D* a_renderer)
	{
		a_renderer->setRenderColour(1, 1, 1);
		a_renderer->drawLine(m_topLeft.x, m_bottomRight.y, m_bottomRight.x, m_bottomRight.y); //Bottom line
		a_renderer->drawLine(m_topLeft.x, m_topLeft.y, m_topLeft.x, m_bottomRight.y); //Left line
		a_renderer->drawLine(m_topLeft.x, m_topLeft.y, m_bottomRight.x, m_topLeft.y); //Top line
		a_renderer->drawLine(m_bottomRight.x, m_topLeft.y, m_bottomRight.x, m_bottomRight.y); // Right line

		if (m_northEast != nullptr)
		{
			m_northEast->draw(a_renderer);
		}
		if (m_northWest != nullptr)
		{
			m_northWest->draw(a_renderer);
		}
		if (m_southWest != nullptr)
		{
			m_southWest->draw(a_renderer);
		}
		if (m_southEast != nullptr)
		{
			m_southEast->draw(a_renderer);
		}
	}
private:	
	//Parent AABB
	Node* m_Node;
	//The two corners
	glm::vec2 m_topLeft;
	glm::vec2 m_bottomRight;
	
	int m_pointNumber = 0;
	//The four children of this parent
	QuadTree* m_northWest;
	QuadTree* m_northEast;
	QuadTree* m_southWest;
	QuadTree* m_southEast;

};

