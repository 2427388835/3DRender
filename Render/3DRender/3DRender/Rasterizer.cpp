#include "Rasterizer.h"

/************************************************************************/
/* ��ֵ΢��DDA(Digital Differential Analyzer)�㷨                        */
/************************************************************************/
void Rasterizer::drawline_dda(int x1,int y1,int x2,int y2,AColor color)
{
	int dx = abs(x2 - x1);
	int dy = abs(y2 - y1);
	if (dx>=dy) //��dx=1Ϊ�������������ֶϵ�
	{
		if (x1>x2)
		{
			swap(x1,x2);
			swap(y1,y2);
		}
		float k = static_cast<float>(y2-y1)/(x2-x1);
		float y = y1;
		for (int x=x1;x<=x2;++x)
		{
			DirectX::instance().drawPixel(x,y,color);
			y += k;
		}
	}
	else
	{
		if (y1>y2)
		{
			swap(x1,x2);
			swap(y1,y2);
		}
		float k = static_cast<float>(x2-x1)/(y2-y1);
		float x = x1;
		for (int y=y1;y<=y2;++y)
		{
			DirectX::instance().drawPixel(x,y,color);
			x += k;
		}
	}	
}

/************************************************************************/
/* ����ֱ��(�е㷨)
 * ��ʽ����f(x,y)=(y1-y1)x+(x2-x1)y+x1y2-x2y1=0
 * B*f(x,y)>0Ϊ��,<0Ϊ��,ÿ�����е㣨x+1,y��0.5��������бȽ�
 * �˴�ȡB>0,����ֱ���ж�
 */                                                                     
/************************************************************************/
void Rasterizer::drawline_mid(int x1,int y1,int x2,int y2,AColor color)
{
	int dx = abs(x2 - x1);
	int dy = abs(y2 - y1);
	if (dx>=dy)										//��dx=1Ϊ�������������ֶϵ�
	{
		if (x1>x2)
		{
			swap(x1,x2);
			swap(y1,y2);
		}
		int A = y1-y2;
		int B = x2-x1;
		if(y2>=y1)									//б��[0,1]
		{
			int d = (A<<1) + B;						//f(x+1,y+0.5)*2����������������
			int upIncrement = (A+B)<<1;				//ȡ�ϵ�ʱd������
			int downTncrement = A<<1;				//ȡ�µ�ʱd������
			for (int x=x1,y=y1;x<=x2;++x)
			{
				DirectX::instance().drawPixel(x,y,color);
				if (d<0){							//�е���ֱ���£�ȡ�ϵ�
					d += upIncrement;
					++y;
				}
				else
				{
					d += downTncrement;
				}
			}
		}
		else										//б��[-1,0)
		{
			int d = (A<<1) - B;					
			int upIncrement = A<<1;			
			int downTncrement = (A-B)<<1;					
			for (int x=x1,y=y1;x<=x2;++x)
			{
				DirectX::instance().drawPixel(x,y,color);
				if (d<0){								
					d += upIncrement;
				}
				else
				{
					d += downTncrement;
					--y;
				}
			}
		}	
	}
	else
	{
		if (y1>y2)
		{
			swap(x1,x2);
			swap(y1,y2);
		}
		int A = x1-x2;
		int B = y2-y1;
		if (x2>=x1)
		{
			int d = (A<<1) + B;					//f(x+0.5,y+1)*2����������������,�˴�Ay+Bx+C=0
			int upIncrement = (A+B)<<1;			//ȡ�ϵ�ʱd������
			int downTncrement = A<<1;			//ȡ�µ�ʱd������
			for (int x=x1,y=y1;y<=y2;++y)
			{
				DirectX::instance().drawPixel(x,y,color);
				if (d<0){						//�е���ֱ���£�ȡ�ϵ�
					d += upIncrement;
					++x;
				}
				else
				{
					d += downTncrement;
				}
			}
		}
		else
		{
			int d = (A<<1) - B;					
			int upIncrement = A<<1;			
			int downTncrement = (A-B)<<1;			
			for (int x=x1,y=y1;y<=y2;++y)
			{
				DirectX::instance().drawPixel(x,y,color);
				if (d<0){						
					d += upIncrement;
				}
				else
				{
					d += downTncrement;
					--x;
				}
			}
		}
	}	
}

 /************************************************************************/
/* Bresenham�㷨   
 * ��Ҫͨ��e=d��0.5�жϷ���
*/
/************************************************************************/
void Rasterizer::drawline_bresenham(int x1,int y1,int x2,int y2,AColor color)
{
	int dx = abs(x2 - x1);
	int dy = abs(y2 - y1);
	if (dx>=dy)										//��dx=1Ϊ�������������ֶϵ�
	{
		if (x1>x2)
		{
			swap(x1,x2);
			swap(y1,y2);
		}
		int flag = y2>=y1?1:-1;						//б��[-1,1]
		int k = flag*(dy<<1);
		int e = -dx*flag;
		for (int x=x1,y=y1;x<=x2;++x) 
		{
			DirectX::instance().drawPixel(x,y,color);
			e += k;
			if (flag*e>0)
			{
				y += flag;
				e -= 2*dx*flag;
			}
		}
	}
	else
	{
		if (y1>y2)
		{
			swap(x1,x2);
			swap(y1,y2);
		}
		int flag = x2>x1?1:-1;
		int k = flag*(dx<<1);
		int e = -dy*flag;
		for (int x=x1,y=y1;y<=y2;++y) 
		{
			DirectX::instance().drawPixel(x,y,color);
			e += k;
			if (flag*e>0)
			{
				x += flag;
				e -= 2*dy*flag;
			}
		}
	}
}