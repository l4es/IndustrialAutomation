#pragma once

#include "Global.h"

/*
template<typename VEC, typename FloatType = double>
class CInterpLagrange
{
	VEC _Points;
	std::vector<FloatType> D;
public:
	CInterpLagrange(VEC& points):_Points(points)
	{
		VEC::size_type size = _Points.size();
		D.resize(size);
		for(VEC::size_type i=0;i<size;i++)
		{
			FloatType d = 1;
			for(VEC::size_type j = 0;j<size;j++)
			{
				ATLTRACE("%.3f : %.3f : %.3f\n", _Points[i].x, _Points[j].x,_Points[i].x - _Points[j].x);
				if(i != j)
					d *= (_Points[i].x - _Points[j].x)*10000;
			}
			d/=10000;
			ATLASSERT(d != 0);
			D[i] = _Points[i].y/d;
		}
	};
	inline FloatType operator()(FloatType x)
	{
		std::vector<FloatType> dX;
		VEC::size_type size = _Points.size();
		dX.resize(size);
		for(VEC::size_type i=0;i<size;i++)
		{
			dX[i] = x - _Points[i].x;
		}

		FloatType y=0;
		for(VEC::size_type i=0;i<size;i++)
		{
			y+= D[i]*dX[i];
		}
		return y;
	};
};
*/

template<typename VEC, typename FloatType> 
FloatType LagrangeIterp(VEC& points, FloatType x, ULONG N)
{
	//Выбираем точки интерполирования
	VEC vec;
	vec.reserve(N);
	typedef VEC::iterator it;
	it right,left;
	for(right=points.begin();right != points.end(); right++)
	{
		if(right->x > x)
			break;
	}

	left = right-1;
	vec.push_back(*left);
	vec.push_back(*right);
	ULONG n=0;
	while(n<(N-2))
	{
		if(left != points.begin())
		{
			left--;
			vec.push_back(*left);
			n++;
		}
		if(right != points.end())
			right++;
		if(left == points.begin()	&&
			right == points.end())
			break;

		if(right != points.end() && n<(N-2))
		{
			vec.push_back(*right);
			n++;
		}
	}

	//Сортируем точки по возрастанию X
	for(int k=1;k>0;)
	{
		k=0;
		for(it i=vec.begin();i!=vec.end()-1;i++)
		{
			if(i->x > (i+1)->x)
			{
				k++;
				FloatType	x = i->x,
							y = i->y;
				i->x = (i+1)->x;
				i->y = (i+1)->y;
				(i+1)->x = x;
				(i+1)->y = y;
			}
		}
	}

	//Строим вектор D
	VEC::size_type size = vec.size();
	vector<FloatType> D;
	D.resize(size);
	for(VEC::size_type i=0;i<size;i++)
	{
		FloatType d = 1;
		for(VEC::size_type j = 0;j<size;j++)
		{
			if(i != j)
				d *= (vec[i].x - vec[j].x) == 0?1:(vec[i].x - vec[j].x);
		}
//		ATLASSERT(d != 0);
		D[i] = d==0?1:d;
	}

	//Строим вектор X
	std::vector<FloatType> dX;
	dX.resize(size);
	for(VEC::size_type i=0;i<size;i++)
	{
		FloatType t = 1;
		for(VEC::size_type j = 0;j<size;j++)
		{
			if(i != j)
				t *= (x - vec[j].x);
		}
		dX[i] = t;
	}

	//Вычисляем значение Y
	FloatType y=0;
	for(VEC::size_type i=0;i<size;i++)
	{
		y+= vec[i].y*(dX[i]/D[i]);
	}
	return y;
}

template<typename VEC, typename FloatType> 
FloatType LinearIterp(VEC& points, FloatType x)
{
	typedef VEC::iterator it;
	it left,right;
	for(it i=points.begin();i!=points.end()-1;i++)
	{
		left = i;
		right = i+1;
		//ATLTRACE("left:%.3f; X:%.3f; right:%.3f\n",left->x,x,right->x);
		if(	(left->x <= x &&
			right->x >= x))
		{
			if(left->x != right->x)
			{
				FloatType y;
				//уравнение прямой
				y = (x - left->x)/(right->x - left->x);
				y *= (right->y - left->y);
				y += left->y;
				return y;
			}
			else
				return (left->y + right->y)/2;
		}
	}
	return 0;
}