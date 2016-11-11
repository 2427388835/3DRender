#include "Rasterizer.h"

void Rasterizer::sort_vertex(Vertex &v1,Vertex &v2,Vertex &v3)
{
	if (v1.position.y>v2.position.y || (v1.position.y==v2.position.y&&v1.position.x>v2.position.x) )
	{
		swap(v1,v2);
	}
	if (v2.position.y>v3.position.y || (v2.position.y==v3.position.y&&v2.position.x>v3.position.x) )
	{
		swap(v2,v3);
	}
	if (v1.position.y>v2.position.y || (v1.position.y==v2.position.y&&v1.position.x>v2.position.x) )
	{
		swap(v1,v2);
	}
}

int yy;
Scanline Rasterizer::generate_scanline(Vertex vl,Vertex vr,int y)
{
	float divide = vr.position.x - vl.position.x;
	int startX = static_cast<int>(vl.position.x + 0.5f);
	int width = static_cast<int>(vr.position.x + 0.5f) - startX;
	Vertex step((vr.position-vl.position)/divide,(vr.color-vl.color)/divide,(vr.u-vl.u)/divide,(vr.v-vl.v)/divide);
	step.inv_w = (vr.inv_w-vl.inv_w)/divide;
	return Scanline(vl,step,startX,y,width);
}

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


void Rasterizer::draw_scanline(Vertex vl,Vertex vr,int y)
{
	Scanline scanline = generate_scanline(vl,vr,y);
	for (int i=0;i<=scanline.width;++i)
	{
		/*AColor color = scanline.v.color/scanline.v.inv_w;
		color.normalize();*/
		AColor color = texture->get_color(scanline.v.u/scanline.v.inv_w,scanline.v.v/scanline.v.inv_w);
		DirectX::instance().drawPixel(scanline.x+i,scanline.y,color);
		scanline.v.add(scanline.step);
	}
}

void Rasterizer::draw_top_flat_triangle(Vertex v1,Vertex v2,Vertex v3)
{
	int startY = v1.position.y + 0.5;
	int endY = v3.position.y + 0.5;
	for (float y=startY;y<endY;++y) 
	{
		float factor = (static_cast<float>(y+0.5)-v1.position.y)/(v3.position.y-v1.position.y);
		Vertex vl = v1.interp(v3,factor);
		Vertex vr = v2.interp(v3,factor);
		draw_scanline(vl,vr,y);
		
	}
}

void Rasterizer::draw_button_flat_triangle(Vertex v1,Vertex v2,Vertex v3)
{
	int startY = v1.position.y + 0.5;
	int endY = v3.position.y + 0.5;
	for (int y=startY;y<endY;++y) 
	{
		float factor = (static_cast<float>(y+0.5)-v1.position.y)/(v3.position.y-v1.position.y);
		Vertex vl = v1.interp(v2,factor);
		Vertex vr = v1.interp(v3,factor);
		draw_scanline(vl,vr,y);
		
	}
}

void Rasterizer::draw_triangle(Vertex v1,Vertex v2,Vertex v3)
{
	sort_vertex(v1,v2,v3);
	if (v1.position.y==v2.position.y)
	{
		draw_top_flat_triangle(v1,v2,v3);
	}
	else if (v2.position.y==v3.position.y)
	{
		draw_button_flat_triangle(v1,v2,v3);
	}
	else
	{
		float factor = (v2.position.y-v1.position.y)/(v3.position.y-v1.position.y);
		Vertex v4 = v1.interp(v3,factor);
		if (v4.position.x<v2.position.x)
		{
			swap(v4,v2);
		}
		draw_button_flat_triangle(v1,v2,v4);
		draw_top_flat_triangle(v2,v4,v3);
	}
}

void Rasterizer::set_texture(string path)
{
	if (texture!=nullptr)
	{
		delete texture;
	}
	texture = new Texture(path);
}